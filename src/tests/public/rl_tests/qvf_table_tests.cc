
#include <gtest/gtest.h>

#include "tests/utils/heuristic_utils.h"

#include "tests/tasks/blocksworld.h"
#include "tests/tasks/gripper.h"
#include "tests/tasks/nomystery.h"
#include "tests/tasks/simple_task.h"
#include "tests/tasks/sokoban.h"
#include "tests/tasks/visitall.h"

#include "q_learning/experience_sample.h"
#include "q_learning/experience_sampler.h"
#include "q_learning/q_learning.h"
#include "q_learning/qvf_approximator_table.h"

#include "downward/state_registry.h"
#include "downward/utils/rng.h"

#include <ranges>

using namespace probfd;
using namespace q_learning;
using namespace tests;

/**
 * The following tests construct experiences from a plan and check if the
 * updates to the table-based QVF produce the correct predictions. Updates
 * happen in reverse order, i.e. from the end of the path to the initial state,
 * such that the Q-values propagate upwards with the updates.
 */

TEST(QVFTableTestsPublic, test_update_bw)
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

    ClassicalTaskProxy proxy(*task);
    OperatorsProxy operators = proxy.get_operators();
    StateRegistry state_registry(proxy);

    std::vector<std::pair<OperatorID, double>> trace{
        {bw.get_operator_id_pick_from_table(0), 0.0},
        {bw.get_operator_id_put_on_block(0, 1), 100.0}};

    std::vector<ExperienceSample> experiences;

    State state = state_registry.get_initial_state();
    for (auto [op, reward] : trace) {
        State successor = state_registry.get_successor_state(
            state,
            operators[op].get_effect());
        experiences.emplace_back(state, op, reward, successor, false);
        state = successor;
    }

    experiences.back().terminal = true;

    // Update in reverse order
    std::ranges::reverse(experiences);
    std::vector<double> expected_qvalues{50.0, 12.5};

    QValueTable qvf;

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.5, 0.5);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_EQ(post_update_q_value, expected);
    }
}

TEST(QVFTableTestsPublic, test_update_sokoban)
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
         sokoban.get_fact_box_at(1, 1, 0),
         sokoban.get_fact_box_at_goal(0, false),
         sokoban.get_fact_box_at_goal(1, false)},
        {sokoban.get_fact_box_at_goal(0, true),
         sokoban.get_fact_box_at_goal(1, true)});

    ClassicalTaskProxy proxy(*task);
    OperatorsProxy operators = proxy.get_operators();
    StateRegistry state_registry(proxy);

    std::vector<std::pair<OperatorID, double>> trace{
        {sokoban.get_operator_push_up_id(0, 2, 0), 10.0},
        {sokoban.get_operator_move_down_id(2, 1), 0.0},
        {sokoban.get_operator_push_left_id(1, 2, 0), 10.0}};

    std::vector<ExperienceSample> experiences;

    State state = state_registry.get_initial_state();
    for (auto [op, reward] : trace) {
        State successor = state_registry.get_successor_state(
            state,
            operators[op].get_effect());

        experiences.emplace_back(state, op, reward, successor, false);
        state = successor;
    }

    experiences.back().terminal = true;

    // Update in reverse order
    std::ranges::reverse(experiences);
    std::vector<double> expected_qvalues{5, 1.25, 5.3125};

    QValueTable qvf;

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.5, 0.5);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_EQ(post_update_q_value, expected);
    }
}

TEST(QVFTableTestsPublic, test_update_visitall)
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

    ClassicalTaskProxy proxy(*task);
    OperatorsProxy operators = proxy.get_operators();
    StateRegistry state_registry(proxy);

    std::vector<std::pair<OperatorID, double>> trace{
        {visitall.get_operator_move_right_id(0, 0), 1000.0},
        {visitall.get_operator_move_left_id(1, 0), 0.0},
        {visitall.get_operator_move_right_id(0, 0), 0.0},
        {visitall.get_operator_move_up_id(1, 0), 1000.0},
        {visitall.get_operator_move_left_id(1, 1), 1000.0}};

    std::vector<ExperienceSample> experiences;

    State state = state_registry.get_initial_state();
    for (auto [op, reward] : trace) {
        State successor = state_registry.get_successor_state(
            state,
            operators[op].get_effect());

        experiences.emplace_back(state, op, reward, successor, false);
        state = successor;
    }

    experiences.back().terminal = true;

    // Update in reverse order
    std::ranges::reverse(experiences);
    std::vector<double> expected_qvalues{500, 625, 156.25, 39.0625, 656.25};

    QValueTable qvf;

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.5, 0.5);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_EQ(post_update_q_value, expected);
    }
}

TEST(QVFTableTestsPublic, test_update_gripper)
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

    ClassicalTaskProxy proxy(*task);
    OperatorsProxy operators = proxy.get_operators();
    StateRegistry state_registry(proxy);

    std::vector<std::pair<OperatorID, double>> trace{
        {gripper.get_operator_move_id(1, 2), 0.0},
        {gripper.get_operator_pick_left_id(1, 2), 0.0},
        {gripper.get_operator_move_id(2, 0), 0.0},
        {gripper.get_operator_drop_left_id(1, 0), -5.0},
        {gripper.get_operator_pick_left_id(1, 0), 0.0},
        {gripper.get_operator_pick_right_id(0, 0), 0.0},
        {gripper.get_operator_move_id(0, 1), 0.0},
        {gripper.get_operator_drop_left_id(1, 1), 10.0},
        {gripper.get_operator_drop_right_id(0, 1), 10.0}};

    std::vector<ExperienceSample> experiences;

    State state = state_registry.get_initial_state();
    for (auto [op, reward] : trace) {
        State successor = state_registry.get_successor_state(
            state,
            operators[op].get_effect());

        experiences.emplace_back(state, op, reward, successor, false);
        state = successor;
    }

    experiences.back().terminal = true;

    // Update in reverse order
    std::ranges::reverse(experiences);
    std::vector<double> expected_qvalues{
        5,
        6.25,
        1.5625,
        0.390625,
        0.09765625,
        -2.4755859375,
        0.09765625,
        0.0244140625,
        0.006103515625
    };

    QValueTable qvf;

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.5, 0.5);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_EQ(post_update_q_value, expected);
    }
}

TEST(QVFTableTestsPublic, test_update_nomystery)
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

    ClassicalTaskProxy proxy(*task);
    OperatorsProxy operators = proxy.get_operators();
    StateRegistry state_registry(proxy);

    std::vector<std::pair<OperatorID, double>> trace{
        {nomystery.get_operator_drive_id(4, 0), -1.0},
        {nomystery.get_operator_drive_id(0, 1), -1.0},
        {nomystery.get_operator_load_package_id(1, 0, 0), 0.0},
        {nomystery.get_operator_drive_id(1, 2), -1.0},
        {nomystery.get_operator_load_package_id(2, 1, 1), 0.0},
        {nomystery.get_operator_drive_id(2, 1), -1.0},
        {nomystery.get_operator_unload_package_id(1, 1, 2), 200.0},
        {nomystery.get_operator_drive_id(1, 5), -1.0},
        {nomystery.get_operator_unload_package_id(5, 0, 1), 100.0},
    };

    std::vector<ExperienceSample> experiences;

    State state = state_registry.get_initial_state();
    for (auto [op, reward] : trace) {
        State successor = state_registry.get_successor_state(
            state,
            operators[op].get_effect());

        experiences.emplace_back(state, op, reward, successor, false);
        state = successor;
    }

    experiences.back().terminal = true;

    // Update in reverse order
    std::ranges::reverse(experiences);
    std::vector<double> expected_qvalues{
        50,
        12,
        103,
        25.25,
        6.3125,
        1.078125,
        0.26953125,
        -0.4326171875,
        -0.5};

    QValueTable qvf;

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.5, 0.5);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_EQ(post_update_q_value, expected);
    }
}