#include <gtest/gtest.h>

#include "tests/tasks/blocksworld.h"
#include "tests/tasks/gripper.h"
#include "tests/tasks/nomystery.h"
#include "tests/tasks/sokoban.h"
#include "tests/tasks/visitall.h"

#include "tests/utils/input_utils.h"
#include "tests/utils/operator_reward_pair.h"
#include "tests/utils/q_learning_utils.h"
#include "tests/utils/task_utils.h"

#include "q_learning/experience_sample.h"
#include "q_learning/maxprob_simulator.h"
#include "q_learning/on_policy_experience_sampler.h"
#include "q_learning/random_policy.h"

#include "downward/utils/rng.h"

#include "downward/task_utils/task_properties.h"

#include <ranges>

using namespace probfd;
using namespace q_learning;
using namespace tests;

using namespace std;
using namespace std::ranges;

TEST(OnPolicyExperienceSamplerTestsPublic, test_bw_one_simulation)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 3495;
    static constexpr int POLICY_SEED = 4067;
    static constexpr int SIMULATIONS = 1;
    static constexpr int EXPLORATIONS = 5;

    // Define a Blocksworld task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "bw_one_simulation_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_bw_multiple_simulations)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 42;
    static constexpr int POLICY_SEED = 1337;
    static constexpr int SIMULATIONS = 2;
    static constexpr int EXPLORATIONS = 5;

    // Define a Blocksworld task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "bw_multiple_simulations_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_sokoban_one_simulation)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 3495;
    static constexpr int POLICY_SEED = 4067;
    static constexpr int SIMULATIONS = 1;
    static constexpr int EXPLORATIONS = 5;

    // Define a Sokoban task
    SokobanGrid grid{
        {' ', ' ', 'G'},
        {'G', ' ', ' '},
        {' ', ' ', ' '},
    };

    SokobanProblem sokoban(grid, 2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        sokoban,
        {sokoban.get_fact_square_clear(0, 0, true),
         sokoban.get_fact_square_clear(1, 0, true),
         sokoban.get_fact_square_clear(2, 0, false),
         sokoban.get_fact_square_clear(0, 1, true),
         sokoban.get_fact_square_clear(1, 1, false),
         sokoban.get_fact_square_clear(2, 1, false),
         sokoban.get_fact_square_clear(0, 2, true),
         sokoban.get_fact_square_clear(1, 2, true),
         sokoban.get_fact_square_clear(2, 2, true),
         sokoban.get_fact_player_at(2, 0),
         sokoban.get_fact_box_at(0, 2, 1),
         sokoban.get_fact_box_at(1, 1, 1),
         sokoban.get_fact_box_at_goal(0, false),
         sokoban.get_fact_box_at_goal(1, false)},
        {sokoban.get_fact_box_at_goal(0, true),
         sokoban.get_fact_box_at_goal(1, true)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "sokoban_one_simulation_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_sokoban_multiple_simulations)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 42;
    static constexpr int POLICY_SEED = 1337;
    static constexpr int SIMULATIONS = 2;
    static constexpr int EXPLORATIONS = 5;

    // Define a Sokoban task
    SokobanGrid grid{
        {' ', ' ', 'G'},
        {'G', ' ', ' '},
        {' ', ' ', ' '},
    };

    SokobanProblem sokoban(grid, 2);
    std::shared_ptr<ClassicalTask> task = create_problem_task(
        sokoban,
        {sokoban.get_fact_square_clear(0, 0, true),
         sokoban.get_fact_square_clear(1, 0, true),
         sokoban.get_fact_square_clear(2, 0, false),
         sokoban.get_fact_square_clear(0, 1, true),
         sokoban.get_fact_square_clear(1, 1, false),
         sokoban.get_fact_square_clear(2, 1, false),
         sokoban.get_fact_square_clear(0, 2, true),
         sokoban.get_fact_square_clear(1, 2, true),
         sokoban.get_fact_square_clear(2, 2, true),
         sokoban.get_fact_player_at(2, 0),
         sokoban.get_fact_box_at(0, 2, 1),
         sokoban.get_fact_box_at(1, 1, 1),
         sokoban.get_fact_box_at_goal(0, false),
         sokoban.get_fact_box_at_goal(1, false)},
        {sokoban.get_fact_box_at_goal(0, true),
         sokoban.get_fact_box_at_goal(1, true)});

    std::shared_ptr<ProbabilisticTask> wrapped = as_probabilistic_task(task);

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        ""
        "sokoban_multiple_simulations_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_visitall_one_simulation)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 3495;
    static constexpr int POLICY_SEED = 4067;
    static constexpr int SIMULATIONS = 1;
    static constexpr int EXPLORATIONS = 5;

    // Define a VisitAll task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "visitall_one_simulation_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_visitall_multiple_simulations)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 42;
    static constexpr int POLICY_SEED = 1337;
    static constexpr int SIMULATIONS = 2;
    static constexpr int EXPLORATIONS = 5;

    // Define a VisitAll task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "visitall_multiple_simulations_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_gripper_one_simulation)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 3495;
    static constexpr int POLICY_SEED = 4067;
    static constexpr int SIMULATIONS = 1;
    static constexpr int EXPLORATIONS = 5;

    // Define a Gripper task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "gripper_one_simulation_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_gripper_multiple_simulations)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 42;
    static constexpr int POLICY_SEED = 1337;
    static constexpr int SIMULATIONS = 2;
    static constexpr int EXPLORATIONS = 5;

    // Define a Gripper task
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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "gripper_multiple_simulations_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_nomystery_one_simulation)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 3495;
    static constexpr int POLICY_SEED = 4067;
    static constexpr int SIMULATIONS = 1;
    static constexpr int EXPLORATIONS = 5;

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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "nomystery_one_simulation_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}

TEST(OnPolicyExperienceSamplerTestsPublic, test_nomystery_multiple_simulations)
{
    // Test parameters
    static constexpr int SIMULATOR_SEED = 42;
    static constexpr int POLICY_SEED = 1337;
    static constexpr int SIMULATIONS = 2;
    static constexpr int EXPLORATIONS = 5;

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

    ProbabilisticTaskProxy proxy(*wrapped);

    // Create a MaxProb simulator and a uniformly random policy to use for the
    // sampler.
    auto simulator = std::make_shared<MaxProbSimulator>(proxy, SIMULATOR_SEED);
    auto policy = std::make_shared<RandomPolicy>(POLICY_SEED);

    // The QVF is unused by the policy.
    DummyQVF qvf;

    // Now, create the sampler.
    OnPolicyExperienceSampler sampler(
        simulator,
        policy,
        SIMULATIONS,
        EXPLORATIONS);

    // The expected experience samples are read from the specified file.
    auto samples = read_experience_samples_from_file(
        *wrapped,
        "nomystery_multiple_simulations_on_policy.samples");

    for (const auto& sample : samples) {
        std::optional experience = sampler.sample_experience(qvf);
        ASSERT_TRUE(experience.has_value());
        EXPECT_EQ(*experience, sample);
    }

    // No more experiences should be generated.
    ASSERT_TRUE(!sampler.sample_experience(qvf).has_value());
}