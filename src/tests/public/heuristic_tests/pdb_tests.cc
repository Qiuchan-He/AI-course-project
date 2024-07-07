#include <gtest/gtest.h>

#include "downward/heuristics/pdb_heuristic.h"
#include "downward/pdbs/types.h"

#include "downward/search_algorithm.h"

#include "tests/tasks/blocksworld.h"
#include "tests/tasks/gripper.h"
#include "tests/tasks/nomystery.h"
#include "tests/tasks/sokoban.h"
#include "tests/tasks/visitall.h"

#include "tests/utils/heuristic_utils.h"
#include "tests/utils/search_utils.h"

using namespace pdbs;
using namespace tests;

TEST(PDBHeuristicTestsPublic, test_bw_goal_aware)
{
    // 4 blocks
    BlocksWorldProblem problem(4);

    /**
     * 3
     * 2
     * 1
     * 0
     */
    std::vector<FactPair> initial_state = {
        problem.get_fact_is_hand_empty(true),
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_block(3, 2),
        problem.get_fact_is_clear(0, false),
        problem.get_fact_is_clear(1, false),
        problem.get_fact_is_clear(2, false),
        problem.get_fact_is_clear(3, true)};

    /**
     * 3
     * 2
     * 1
     * 0
     */
    std::vector<FactPair> goal = {
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_block(3, 2)};

    std::vector<int> pattern = {
        problem.get_variable_hand_empty(),
        problem.get_variable_location(0)};

    auto task = create_problem_task(problem, initial_state, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 0);
}

TEST(PDBHeuristicTestsPublic, test_bw_single_state)
{
    // 4 blocks
    BlocksWorldProblem problem(4);

    /**
     * 1 2
     * 0 3
     */
    std::vector<FactPair> initial_state = {
        problem.get_fact_is_hand_empty(true),
        problem.get_fact_location_on_block(0, 3),
        problem.get_fact_location_on_block(1, 2),
        problem.get_fact_location_on_table(2),
        problem.get_fact_location_on_table(3),
        problem.get_fact_is_clear(0, false),
        problem.get_fact_is_clear(1, true),
        problem.get_fact_is_clear(2, true),
        problem.get_fact_is_clear(3, false)};

    /**
     * 3
     * 2
     * 1
     * 0
     */
    std::vector<FactPair> goal = {
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_block(3, 2)};

    std::vector<int> pattern = {
        problem.get_variable_hand_empty(),
        problem.get_variable_location(2),
        problem.get_variable_location(3)};

    auto task = create_problem_task(problem, initial_state, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
}

TEST(PDBHeuristicTestsPublic, test_bw_single_state_reeval)
{
    // 4 blocks
    BlocksWorldProblem problem(4);

    /**
     * 1 2
     * 0 3
     */
    std::vector<FactPair> initial_state = {
        problem.get_fact_is_hand_empty(true),
        problem.get_fact_location_on_block(0, 3),
        problem.get_fact_location_on_block(1, 2),
        problem.get_fact_location_on_table(2),
        problem.get_fact_location_on_table(3),
        problem.get_fact_is_clear(0, false),
        problem.get_fact_is_clear(1, true),
        problem.get_fact_is_clear(2, true),
        problem.get_fact_is_clear(3, false)};

    /**
     * 3
     * 2
     * 1
     * 0
     */
    std::vector<FactPair> goal = {
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_block(3, 2)};

    std::vector<int> pattern = {
        problem.get_variable_hand_empty(),
        problem.get_variable_location(2),
        problem.get_variable_location(3)};

    auto task = create_problem_task(problem, initial_state, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
}

TEST(PDBHeuristicTestsPublic, test_bw_astar)
{
    // 4 blocks
    BlocksWorldProblem problem(6);

    /**
     * 2
     * 1 4
     * 0 3 5
     */
    std::vector<FactPair> initial_state(
        {problem.get_fact_is_hand_empty(true),
         problem.get_fact_location_on_table(0),
         problem.get_fact_location_on_block(1, 0),
         problem.get_fact_location_on_block(2, 1),
         problem.get_fact_location_on_table(3),
         problem.get_fact_location_on_block(4, 3),
         problem.get_fact_location_on_table(5),
         problem.get_fact_is_clear(0, false),
         problem.get_fact_is_clear(1, false),
         problem.get_fact_is_clear(2, true),
         problem.get_fact_is_clear(3, false),
         problem.get_fact_is_clear(4, true),
         problem.get_fact_is_clear(5, true)});

    /**
     * 0
     * 4
     * 3
     * 5
     * 2
     * 1
     */
    std::vector<FactPair> goal(
        {problem.get_fact_location_on_block(0, 4),
         problem.get_fact_location_on_table(1),
         problem.get_fact_location_on_block(2, 1),
         problem.get_fact_location_on_block(3, 5),
         problem.get_fact_location_on_block(4, 3),
         problem.get_fact_location_on_block(5, 2)});

    std::vector<int> pattern = {
        problem.get_variable_hand_empty(),
        problem.get_variable_location(0)};

    auto task = create_problem_task(problem, initial_state, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);
    auto algorithm = create_astar_search_algorithm(task, hpdb);

    algorithm->search();

    ASSERT_TRUE(algorithm->found_solution());

    const std::vector<OperatorID> expected_plan = {
        problem.get_operator_id_pick_from_block(2, 1),
        problem.get_operator_id_put_on_table(2),
        problem.get_operator_id_pick_from_block(1, 0),
        problem.get_operator_id_put_on_table(1),
        problem.get_operator_id_pick_from_table(2),
        problem.get_operator_id_put_on_block(2, 1),
        problem.get_operator_id_pick_from_block(4, 3),
        problem.get_operator_id_put_on_table(4),
        problem.get_operator_id_pick_from_table(5),
        problem.get_operator_id_put_on_block(5, 2),
        problem.get_operator_id_pick_from_table(3),
        problem.get_operator_id_put_on_block(3, 5),
        problem.get_operator_id_pick_from_table(4),
        problem.get_operator_id_put_on_block(4, 3),
        problem.get_operator_id_pick_from_table(0),
        problem.get_operator_id_put_on_block(0, 4)};

    ASSERT_EQ(algorithm->get_plan(), expected_plan);
}

TEST(PDBHeuristicTestsPublic, test_gripper_goal_aware)
{
    // 2 rooms, 2 balls
    GripperProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_room(0),
         problem.get_fact_carry_left_none(),
         problem.get_fact_carry_right_none(),
         problem.get_fact_ball_at_room(0, 1),
         problem.get_fact_ball_at_room(1, 1)});

    std::vector<FactPair> goal(
        {problem.get_fact_ball_at_room(0, 1),
         problem.get_fact_ball_at_room(1, 1)});

    std::vector<int> pattern = {
        problem.get_variable_ball_at(0),
        problem.get_variable_ball_at(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 0);
}

TEST(PDBHeuristicTestsPublic, test_gripper_single_state)
{
    // 2 rooms, 2 balls
    GripperProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_room(0),
         problem.get_fact_carry_left_none(),
         problem.get_fact_carry_right_none(),
         problem.get_fact_ball_at_room(0, 0),
         problem.get_fact_ball_at_room(1, 0)});

    std::vector<FactPair> goal(
        {problem.get_fact_ball_at_room(0, 1),
         problem.get_fact_ball_at_room(1, 1)});

    std::vector<int> pattern = {
        problem.get_variable_ball_at(0),
        problem.get_variable_ball_at(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 2);
}

TEST(PDBHeuristicTestsPublic, test_gripper_single_state_reeval)
{
    // 2 rooms, 2 balls
    GripperProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_room(0),
         problem.get_fact_carry_left_none(),
         problem.get_fact_carry_right_none(),
         problem.get_fact_ball_at_room(0, 0),
         problem.get_fact_ball_at_room(1, 0)});

    std::vector<FactPair> goal(
        {problem.get_fact_ball_at_room(0, 1),
         problem.get_fact_ball_at_room(1, 1)});

    std::vector<int> pattern = {
        problem.get_variable_ball_at(0),
        problem.get_variable_ball_at(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 2);
    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 2);
}

TEST(PDBHeuristicTestsPublic, test_gripper_astar)
{
    // 2 rooms, 2 balls
    GripperProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_room(0),
         problem.get_fact_carry_left_none(),
         problem.get_fact_carry_right_none(),
         problem.get_fact_ball_at_room(0, 0),
         problem.get_fact_ball_at_room(1, 0)});

    std::vector<FactPair> goal(
        {problem.get_fact_ball_at_room(0, 1),
         problem.get_fact_ball_at_room(1, 1)});

    std::vector<int> pattern = {
        problem.get_variable_ball_at(0),
        problem.get_variable_ball_at(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);
    auto algorithm = create_astar_search_algorithm(task, hpdb);

    algorithm->search();

    ASSERT_TRUE(algorithm->found_solution());
    ASSERT_EQ(algorithm->get_statistics().get_reopened(), 0);
    ASSERT_EQ(algorithm->get_statistics().get_expanded(), 19);

    const std::vector<OperatorID> expected_plan = {
        problem.get_operator_pick_left_id(0, 0),
        problem.get_operator_pick_right_id(1, 0),
        problem.get_operator_move_id(0, 1),
        problem.get_operator_drop_left_id(0, 1),
        problem.get_operator_drop_right_id(1, 1)};

    ASSERT_EQ(algorithm->get_plan(), expected_plan);
}

TEST(PDBHeuristicTestsPublic, test_visitall_goal_aware)
{
    // 2 rooms, 2 balls
    VisitAllProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_square(0, 0),
         problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, true),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, true),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<int> pattern = {
        problem.get_variable_robot_at(),
        problem.get_variable_square_visited(1, 1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 0);
}

TEST(PDBHeuristicTestsPublic, test_visitall_single_state)
{
    // 2 rooms, 2 balls
    VisitAllProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_square(0, 0),
         problem.get_fact_square_visited(0, 0, false),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, false),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, true),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<int> pattern = {
        problem.get_variable_robot_at(),
        problem.get_variable_square_visited(1, 0),
        problem.get_variable_square_visited(1, 1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 1);
}

TEST(PDBHeuristicTestsPublic, test_visitall_single_state_reeval)
{
    // 2 rooms, 2 balls
    VisitAllProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_square(0, 0),
         problem.get_fact_square_visited(0, 0, false),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, false),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, true),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<int> pattern = {
        problem.get_variable_robot_at(),
        problem.get_variable_square_visited(1, 0),
        problem.get_variable_square_visited(1, 1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 1);
    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 1);
}

TEST(PDBHeuristicTestsPublic, test_visitall_astar)
{
    // 2 rooms, 2 balls
    VisitAllProblem problem(2, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_robot_at_square(0, 0),
         problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, false),
         problem.get_fact_square_visited(1, 0, false),
         problem.get_fact_square_visited(1, 1, false)});

    std::vector<FactPair> goal(
        {problem.get_fact_square_visited(0, 0, true),
         problem.get_fact_square_visited(0, 1, true),
         problem.get_fact_square_visited(1, 0, true),
         problem.get_fact_square_visited(1, 1, true)});

    std::vector<int> pattern = {
        problem.get_variable_robot_at(),
        problem.get_variable_square_visited(1, 1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);
    auto algorithm = create_astar_search_algorithm(task, hpdb);

    algorithm->search();

    ASSERT_TRUE(algorithm->found_solution());
    ASSERT_EQ(algorithm->get_statistics().get_reopened(), 0);
    ASSERT_EQ(algorithm->get_statistics().get_expanded(), 7);

    const std::vector<OperatorID> expected_plan = {
        problem.get_operator_move_up_id(0, 0),
        problem.get_operator_move_right_id(0, 1),
        problem.get_operator_move_down_id(1, 1)};

    ASSERT_EQ(algorithm->get_plan(), expected_plan);
}

TEST(PDBHeuristicTestsPublic, test_sokoban_goal_aware)
{
    const SokobanGrid playarea = {
        {' ', ' ', ' '},
        {' ', 'G', ' '},
        {' ', 'G', ' '}
    };

    // 1 box
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(0, 0),
         problem.get_fact_box_at(0, 1, 1),
         problem.get_fact_box_at(1, 1, 2),
         problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true),
         problem.get_fact_square_clear(0, 0, false),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(1, 0, true),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, false),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, true),
         problem.get_fact_square_clear(2, 2, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    std::vector<int> pattern = {
        problem.get_variable_player_at(),
        problem.get_variable_box_at(0),
        problem.get_variable_box_at_goal(0)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 0);
}

TEST(PDBHeuristicTestsPublic, test_sokoban_dead_end)
{
    const SokobanGrid playarea = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', 'G', ' '}
    };

    // 1 box
    SokobanProblem problem(playarea, 1);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(0, 0),
         problem.get_fact_box_at(0, 2, 2),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_square_clear(0, 0, false),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(1, 0, true),
         problem.get_fact_square_clear(1, 1, true),
         problem.get_fact_square_clear(1, 2, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, true),
         problem.get_fact_square_clear(2, 2, false)});

    std::vector<FactPair> goal({problem.get_fact_box_at_goal(0, true)});

    std::vector<int> pattern = {
        problem.get_variable_box_at(0),
        problem.get_variable_box_at_goal(0)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), Heuristic::DEAD_END);
}

TEST(PDBHeuristicTestsPublic, test_sokoban_single_state)
{
    const SokobanGrid playarea = {
        {' ', 'G', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', 'G', ' '}
    };

    // 2 boxes
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(0, 0),
         problem.get_fact_box_at(0, 1, 1),
         problem.get_fact_box_at(1, 1, 2),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_box_at_goal(1, false),
         problem.get_fact_square_clear(0, 0, false),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(0, 3, true),
         problem.get_fact_square_clear(1, 0, true),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, false),
         problem.get_fact_square_clear(1, 3, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, true),
         problem.get_fact_square_clear(2, 2, true),
         problem.get_fact_square_clear(2, 3, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    std::vector<int> pattern = {
        problem.get_variable_player_at(),
        problem.get_variable_box_at(0),
        problem.get_variable_box_at_goal(0),
        problem.get_variable_box_at(1),
        problem.get_variable_box_at_goal(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
}

TEST(PDBHeuristicTestsPublic, test_sokoban_single_state_reeval)
{
    const SokobanGrid playarea = {
        {' ', 'G', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', 'G', ' '}
    };

    // 2 boxes
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(0, 0),
         problem.get_fact_box_at(0, 1, 1),
         problem.get_fact_box_at(1, 1, 2),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_box_at_goal(1, false),
         problem.get_fact_square_clear(0, 0, false),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(0, 3, true),
         problem.get_fact_square_clear(1, 0, true),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, false),
         problem.get_fact_square_clear(1, 3, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, true),
         problem.get_fact_square_clear(2, 2, true),
         problem.get_fact_square_clear(2, 3, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    std::vector<int> pattern = {
        problem.get_variable_player_at(),
        problem.get_variable_box_at(0),
        problem.get_variable_box_at_goal(0),
        problem.get_variable_box_at(1),
        problem.get_variable_box_at_goal(1)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 4);
}

TEST(PDBHeuristicTestsPublic, test_sokoban_astar)
{
    const SokobanGrid playarea = {
        {' ', ' ', ' '},
        {' ', ' ', 'G'},
        {' ', ' ', 'G'}};

    // 2 boxes
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(0, 0),
         problem.get_fact_box_at(0, 1, 0),
         problem.get_fact_box_at(1, 1, 1),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_box_at_goal(1, false),
         problem.get_fact_square_clear(0, 0, false),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(1, 0, false),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, true),
         problem.get_fact_square_clear(2, 2, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    std::vector<int> pattern = {
        problem.get_variable_player_at(),
        problem.get_variable_box_at(0),
        problem.get_variable_box_at_goal(0)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);
    auto algorithm = create_astar_search_algorithm(task, hpdb);

    algorithm->search();

    ASSERT_TRUE(algorithm->found_solution());
    ASSERT_EQ(algorithm->get_statistics().get_reopened(), 0);
    ASSERT_EQ(algorithm->get_statistics().get_expanded(), 13);

    const std::vector<OperatorID> expected_plan = {
        problem.get_operator_push_right_id(0, 0, 0),
        problem.get_operator_move_left_id(1, 0),
        problem.get_operator_move_up_id(0, 0),
        problem.get_operator_push_right_id(1, 0, 1)};

    ASSERT_EQ(algorithm->get_plan(), expected_plan);
}

TEST(PDBHeuristicTestsPublic, test_nomystery_goal_aware)
{
    // 0 -- 1 -- 2 -- 3
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {1, 0},
        {1, 2},
        {2, 1},
        {2, 3},
        {3, 2}
    };

    // 4 locations, 2 packages, truck capacity 2
    NoMysteryProblem problem(4, 2, 2, roadmap);

    // Package P1 at L1 and P2 at L2, truck starts at L0.
    std::vector<FactPair> initial(
        {problem.get_fact_truck_at_location(0),
         problem.get_fact_packages_loaded(0),
         problem.get_fact_package_at_location(0, 1),
         problem.get_fact_package_at_location(1, 2)});

    // Package P1 at L1 and P2 at L2
    std::vector<FactPair> goal(
        {problem.get_fact_package_at_location(0, 1),
         problem.get_fact_package_at_location(1, 2)});

    pdbs::Pattern pattern = {
        problem.get_variable_truck_at(),
        problem.get_variable_package_at(0)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 0);
}

TEST(PDBHeuristicTestsPublic, test_nomystery_dead_end)
{
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {1, 0},
        {1, 2},
        {2, 1}
    };

    // 4 locations, 2 packages, truck capacity 2
    NoMysteryProblem problem(4, 2, 2, roadmap);

    // Package P1 at L2 and P2 at L3, truck starts at L0.
    std::vector<FactPair> initial(
        {problem.get_fact_truck_at_location(0),
         problem.get_fact_packages_loaded(0),
         problem.get_fact_package_at_location(0, 3),
         problem.get_fact_package_at_location(1, 3)});

    // Package P1 at L0 and P2 at L0
    std::vector<FactPair> goal(
        {problem.get_fact_package_at_location(0, 0),
         problem.get_fact_package_at_location(1, 0)});

    pdbs::Pattern pattern = {
        problem.get_variable_truck_at(),
        problem.get_variable_package_at(0)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), Heuristic::DEAD_END);
}

TEST(PDBHeuristicTestsPublic, test_nomystery_single_state)
{
    // 4 -- 5 -- 6
    // |  /    /
    // | /    /
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 2},
        {1, 6},
        {2, 1},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 4},
        {5, 6},
        {6, 1},
        {6, 5}
    };

    // 7 locations, 3 packages, truck capacity 2
    NoMysteryProblem problem(7, 3, 1, roadmap);

    // Package P1 at L1, P2 at L4, P3 at L5, truck starts at L0.
    std::vector<FactPair> initial(
        {problem.get_fact_truck_at_location(0),
         problem.get_fact_packages_loaded(0),
         problem.get_fact_package_at_location(0, 1),
         problem.get_fact_package_at_location(1, 4),
         problem.get_fact_package_at_location(2, 5)});

    // Package P1 at L2, P2 and P3 at L6
    std::vector<FactPair> goal(
        {problem.get_fact_package_at_location(0, 2),
         problem.get_fact_package_at_location(1, 6),
         problem.get_fact_package_at_location(2, 6)});

    // Ignore the limited capacity
    pdbs::Pattern pattern = {
        problem.get_variable_truck_at(),
        problem.get_variable_package_at(0),
        problem.get_variable_package_at(1),
        problem.get_variable_package_at(2)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 11);
}

TEST(PDBHeuristicTestsPublic, test_nomystery_single_state_reeval)
{
    // 4 -- 5 -- 6
    // |  /    /
    // | /    /
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 2},
        {1, 6},
        {2, 1},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 4},
        {5, 6},
        {6, 1},
        {6, 5}
    };

    // 7 locations, 3 packages, truck capacity 1
    NoMysteryProblem problem(7, 3, 1, roadmap);

    // Package P1 at L1, P2 at L4, P3 at L5, truck starts at L0.
    std::vector<FactPair> initial(
        {problem.get_fact_truck_at_location(0),
         problem.get_fact_packages_loaded(0),
         problem.get_fact_package_at_location(0, 1),
         problem.get_fact_package_at_location(1, 4),
         problem.get_fact_package_at_location(2, 5)});

    // Package P1 at L2, P2 and P3 at L6
    std::vector<FactPair> goal(
        {problem.get_fact_package_at_location(0, 2),
         problem.get_fact_package_at_location(1, 6),
         problem.get_fact_package_at_location(2, 6)});

    // Ignore the limited capacity
    pdbs::Pattern pattern = {
        problem.get_variable_truck_at(),
        problem.get_variable_package_at(0),
        problem.get_variable_package_at(1),
        problem.get_variable_package_at(2)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);

    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 11);
    ASSERT_EQ(get_initial_state_estimate(*task, *hpdb), 11);
}

TEST(PDBHeuristicTestsPublic, test_nomystery_astar)
{
    // 4 -- 5 -- 6
    // |  /    /
    // | /    /
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 2},
        {1, 6},
        {2, 1},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 4},
        {5, 6},
        {6, 1},
        {6, 5}
    };

    // 7 locations, 3 packages, truck capacity 1
    NoMysteryProblem problem(7, 3, 1, roadmap);

    // Package P1 at L1, P2 at L4, P3 at L5, truck starts at L0.
    std::vector<FactPair> initial(
        {problem.get_fact_truck_at_location(0),
         problem.get_fact_packages_loaded(0),
         problem.get_fact_package_at_location(0, 1),
         problem.get_fact_package_at_location(1, 4),
         problem.get_fact_package_at_location(2, 5)});

    // Package P1 at L2, P2 and P3 at L6
    std::vector<FactPair> goal(
        {problem.get_fact_package_at_location(0, 2),
         problem.get_fact_package_at_location(1, 6),
         problem.get_fact_package_at_location(2, 6)});

    pdbs::Pattern pattern = {
        problem.get_variable_package_at(0),
        problem.get_variable_package_at(1),
        problem.get_variable_package_at(2)};

    auto task = create_problem_task(problem, initial, goal);
    auto hpdb = std::make_shared<PDBHeuristic>(task, pattern);
    auto algorithm = create_astar_search_algorithm(task, hpdb);

    algorithm->search();

    ASSERT_TRUE(algorithm->found_solution());
    ASSERT_EQ(algorithm->get_statistics().get_reopened(), 0);
    ASSERT_EQ(algorithm->get_statistics().get_expanded(), 205);

    const std::vector<OperatorID> expected_plan = {
        problem.get_operator_drive_id(0, 4),
        problem.get_operator_load_package_id(4, 1, 0),
        problem.get_operator_drive_id(4, 5),
        problem.get_operator_drive_id(5, 6),
        problem.get_operator_unload_package_id(6, 1, 1),
        problem.get_operator_drive_id(6, 5),
        problem.get_operator_load_package_id(5, 2, 0),
        problem.get_operator_drive_id(5, 6),
        problem.get_operator_unload_package_id(6, 2, 1),
        problem.get_operator_drive_id(6, 1),
        problem.get_operator_load_package_id(1, 0, 0),
        problem.get_operator_drive_id(1, 2),
        problem.get_operator_unload_package_id(2, 0, 1)};

    ASSERT_EQ(algorithm->get_plan(), expected_plan);
}