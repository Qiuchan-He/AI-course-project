
#include <gtest/gtest.h>

#include "tests/utils/heuristic_utils.h"
#include "tests/utils/input_utils.h"
#include "tests/utils/operator_reward_pair.h"
#include "tests/utils/task_utils.h"
#include "tests/utils/q_learning_utils.h"

#include "tests/tasks/blocksworld.h"
#include "tests/tasks/gripper.h"
#include "tests/tasks/nomystery.h"
#include "tests/tasks/simple_task.h"
#include "tests/tasks/sokoban.h"
#include "tests/tasks/visitall.h"

#include "q_learning/experience_sample.h"
#include "q_learning/experience_sampler.h"
#include "q_learning/q_learning.h"
#include "q_learning/qvf_approximator.h"

#include "downward/utils/rng.h"

#include <ranges>

using namespace std::ranges;

using namespace probfd;
using namespace q_learning;
using namespace tests;

TEST(QLearningTestsPublic, test_run_bw)
{
    BlocksWorldProblem bw(2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        bw,
        {bw.get_fact_is_hand_empty(true),
         bw.get_fact_is_clear(0, true),
         bw.get_fact_is_clear(1, true),
         bw.get_fact_location_on_table(0),
         bw.get_fact_location_on_table(1)},
        {bw.get_fact_location_on_block(0, 1),
         bw.get_fact_location_on_table(1)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    // Read experiences from the specified file.
    auto experiences = read_experience_samples_from_file(
        *wrapped,
        "bw_one_simulation_on_policy.samples");

    // Check if the experiences are passed to our dummy unchanged and in the
    // correct order by the implementation.
    ExperienceSequenceSampler sampler(experiences);
    DummyQVF qvf;
    run_q_learning(sampler, qvf, 0.90, 0.01);

    EXPECT_EQ(qvf.get_experiences(), experiences);
    EXPECT_TRUE(
        all_of(qvf.get_discount_factors(), [=](auto v) { return v == 0.90; }));
    EXPECT_TRUE(
        all_of(qvf.get_learning_rates(), [=](auto v) { return v == 0.01; }));
}

TEST(QLearningTestsPublic, test_run_sokoban)
{
    SokobanGrid grid{
        {' ', ' ', 'G'},
        {'G', ' ', ' '},
        {' ', ' ', ' '},
    };

    SokobanProblem sokoban(grid, 2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        sokoban,
        {sokoban.get_fact_square_clear(0, 0, true),
         sokoban.get_fact_square_clear(1, 0, false),
         sokoban.get_fact_square_clear(2, 0, false),
         sokoban.get_fact_square_clear(0, 1, true),
         sokoban.get_fact_square_clear(1, 1, true),
         sokoban.get_fact_square_clear(2, 1, false),
         sokoban.get_fact_square_clear(0, 2, true),
         sokoban.get_fact_square_clear(1, 2, true),
         sokoban.get_fact_square_clear(2, 2, true),
         sokoban.get_fact_player_at(2, 0),
         sokoban.get_fact_box_at(0, 2, 1),
         sokoban.get_fact_box_at(1, 1, 0),
         sokoban.get_fact_box_at_goal(0, false),
         sokoban.get_fact_box_at_goal(1, false)},
        {sokoban.get_fact_box_at_goal(0, true),
         sokoban.get_fact_box_at_goal(1, true)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    // Read experiences from the specified file.
    auto experiences = read_experience_samples_from_file(
        *wrapped,
        "sokoban_one_simulation_on_policy.samples");

    // Check if the experiences are passed to our dummy unchanged and in the
    // correct order by the implementation.
    ExperienceSequenceSampler sampler(experiences);
    DummyQVF qvf;
    run_q_learning(sampler, qvf, 0.90, 0.01);

    EXPECT_EQ(qvf.get_experiences(), experiences);

    EXPECT_TRUE(
        all_of(qvf.get_discount_factors(), [=](auto v) { return v == 0.90; }));
    EXPECT_TRUE(
        all_of(qvf.get_learning_rates(), [=](auto v) { return v == 0.01; }));
}

TEST(QLearningTestsPublic, test_run_visitall)
{
    VisitAllProblem visitall(2, 2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        visitall,
        {visitall.get_fact_square_visited(0, 0, true),
         visitall.get_fact_square_visited(0, 1, false),
         visitall.get_fact_square_visited(1, 0, false),
         visitall.get_fact_square_visited(1, 1, false),
         visitall.get_fact_robot_at_square(0, 0)},
        {visitall.get_fact_square_visited(0, 0, true),
         visitall.get_fact_square_visited(0, 1, true),
         visitall.get_fact_square_visited(1, 0, true),
         visitall.get_fact_square_visited(1, 1, true)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    // Read experiences from the specified file.
    auto experiences = read_experience_samples_from_file(
        *wrapped,
        "visitall_one_simulation_on_policy.samples");

    // Check if the experiences are passed to our dummy unchanged and in the
    // correct order by the implementation.
    ExperienceSequenceSampler sampler(experiences);
    DummyQVF qvf;
    run_q_learning(sampler, qvf, 0.90, 0.01);
    EXPECT_EQ(qvf.get_experiences(), experiences);
    EXPECT_TRUE(
        all_of(qvf.get_discount_factors(), [=](auto v) { return v == 0.90; }));
    EXPECT_TRUE(
        all_of(qvf.get_learning_rates(), [=](auto v) { return v == 0.01; }));
}

TEST(QLearningTestsPublic, test_run_gripper)
{
    GripperProblem gripper(3, 2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        gripper,
        {gripper.get_fact_ball_at_room(0, 0),
         gripper.get_fact_ball_at_room(1, 2),
         gripper.get_fact_carry_left_none(),
         gripper.get_fact_carry_right_none(),
         gripper.get_fact_robot_at_room(1)},
        {gripper.get_fact_ball_at_room(0, 1),
         gripper.get_fact_ball_at_room(1, 1)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    // Read experiences from the specified file.
    auto experiences = read_experience_samples_from_file(
        *wrapped,
        "gripper_one_simulation_on_policy.samples");

    // Check if the experiences are passed to our dummy unchanged and in the
    // correct order by the implementation.
    ExperienceSequenceSampler sampler(experiences);
    DummyQVF qvf;
    run_q_learning(sampler, qvf, 0.90, 0.01);

    EXPECT_EQ(qvf.get_experiences(), experiences);
    EXPECT_TRUE(
        all_of(qvf.get_discount_factors(), [=](auto v) { return v == 0.90; }));
    EXPECT_TRUE(
        all_of(qvf.get_learning_rates(), [=](auto v) { return v == 0.01; }));
}

TEST(QLearningTestsPublic, test_run_nomystery)
{
    // 3 -- 4 -- 5
    // |  /    /
    // | /    /
    // 0 -- 1 -- 2
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 3},
        {0, 4},
        {1, 0},
        {1, 2},
        {1, 5},
        {2, 1},
        {3, 0},
        {3, 4},
        {4, 0},
        {4, 3},
        {4, 5},
        {5, 1},
        {5, 4}};
    NoMysteryProblem nomystery(6, 2, 2, std::move(roadmap));
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        nomystery,
        {nomystery.get_fact_truck_at_location(4),
         nomystery.get_fact_package_at_location(0, 1),
         nomystery.get_fact_package_at_location(1, 2),
         nomystery.get_fact_packages_loaded(0)},
        {nomystery.get_fact_package_at_location(0, 5),
         nomystery.get_fact_package_at_location(1, 1)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    // Read experiences from the specified file.
    auto experiences = read_experience_samples_from_file(
        *wrapped,
        "nomystery_one_simulation_on_policy.samples");

    // Check if the experiences are passed to our dummy unchanged and in the
    // correct order by the implementation.
    ExperienceSequenceSampler sampler(experiences);
    DummyQVF qvf;
    run_q_learning(sampler, qvf, 0.90, 0.01);

    EXPECT_EQ(qvf.get_experiences(), experiences);
    EXPECT_TRUE(
        all_of(qvf.get_discount_factors(), [=](auto v) { return v == 0.90; }));
    EXPECT_TRUE(
        all_of(qvf.get_learning_rates(), [=](auto v) { return v == 0.01; }));
}