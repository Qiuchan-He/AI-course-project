
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
#include "q_learning/qvf_approximator_network.h"
#include "q_learning/torch_networks.h"
#include "q_learning/trainable_network.h"

#include "downward/utils/rng.h"

#include "downward/state_registry.h"

#include <ranges>

using namespace probfd;
using namespace q_learning;
using namespace tests;

// The best random seed.
static constexpr int SEED = 42;

/**
 * The following tests construct experiences from a plan and check if the
 * updates to the network parameters result in the correct predictions. Updates
 * happen in reverse order, i.e. from the end of the path to the initial state,
 * such that the Q-values propagate upwards with the updates.
 */

TEST(QVFNetworkTestsPublic, test_update_bw_no_hidden_layers)
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
    std::vector<double> expected_qvalues{
        1.2000000476837158,
        0.013680001720786095};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            0,
            0,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_bw_one_hidden_layer_width_20)
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
    std::vector<double> expected_qvalues{
        3.2102468013763428,
        0.11164642870426178};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            1,
            20,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_sokoban_no_hidden_layers)
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
    std::vector<double> expected_qvalues{
        0.30000004172325134,
        0.0085500013083219528,
        0.30684000253677368};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            0,
            0,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_sokoban_one_hidden_layer_width_20)
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
    std::vector<double> expected_qvalues{
        0.52787375450134277,
        0.012119797058403492,
        0.29831549525260925};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            1,
            20,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_visitall_no_hidden_layers)
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
    std::vector<double> expected_qvalues{
        12,
        12.136800765991211,
        0.13835954666137695,
        0.11529961973428726,
        12.252275466918945};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            0,
            0,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_visitall_one_hidden_layer_width_20)
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
    std::vector<double> expected_qvalues{
        43.098712921142578,
        34.435192108154297,
        0.27824965119361877,
        0.36191153526306152,
        50.547046661376953};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            1,
            20,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_gripper_no_hidden_layers)
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
        0.12000001221895218,
        0.12136800587177277,
        0.0013835952850058675,
        0.0011529962066560984,
        0.00069179770071059465,
        -0.059544004499912262,
        0.00069179770071059465,
        0.00069179770071059465,
        0.00045600003795698285};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            0,
            0,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_gripper_one_hidden_layer_width_20)
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
        0.42364579439163208,
        0.34457963705062866,
        0.011298437602818012,
        0.0072636902332305908,
        0.00087577581871300936,
        -0.12297964841127396,
        0.0013175832573324442,
        0.0010935827158391476,
        0.0005207701469771564};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            1,
            20,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_nomystery_no_hidden_layers)
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
        1,
        -0.00050000014016404748,
        2.0076000690460205,
        0.0090722013264894485,
        0.015257760882377625,
        -0.0023711198009550571,
        0.011443320661783218,
        -0.0023711198009550571,
        -0.0061855604872107506};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            0,
            0,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}

TEST(QVFNetworkTestsPublic, test_update_nomystery_one_hidden_layer_20)
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
        1.283903956413269,
        0.0031112856231629848,
        1.9340195655822754,
        0.0080261081457138062,
        0.023205159232020378,
        0.0079060774296522141,
        0.049288216978311539,
        0.021181268617510796,
        0.0019942019134759903};

    FactsProxy facts(*task);

    QVFApproximatorNetwork qvf(
        proxy,
        q_learning::create_fully_connected_torch_network(
            facts.size(),
            1,
            20,
            proxy.get_operators().size(),
            SEED));

    for (size_t i = 0; i != experiences.size(); ++i) {
        const auto& [experience, expected] =
            std::tie(experiences[i], expected_qvalues[i]);
        qvf.update(experience, 0.95, 0.001);
        double post_update_q_value =
            qvf.get_qvalue(experience.state, experience.action);
        EXPECT_NEAR(post_update_q_value, expected, 0.0001);
    }
}