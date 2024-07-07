#include <gtest/gtest.h>

#include "downward/heuristics/ff_heuristic.h"

#include "downward/search_algorithm.h"

#include "tests/tasks/blocksworld.h"
#include "tests/tasks/gripper.h"
#include "tests/tasks/nomystery.h"
#include "tests/tasks/sokoban.h"
#include "tests/tasks/visitall.h"

#include "tests/utils/ff_utils.h"
#include "tests/utils/heuristic_utils.h"

using namespace ff_heuristic;
using namespace tests;

TEST(HFFHeuristicTestsPublic, test_bw_goal_aware)
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

    auto task = create_problem_task(problem, initial_state, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(plan.has_value());
    ASSERT_EQ(plan->cost, 0);
    ASSERT_TRUE(plan->operators.empty());
}

TEST(HFFHeuristicTestsPublic, test_bw_single_state)
{
    // 4 blocks
    BlocksWorldProblem problem(4);

    /**
     * 2
     * 1
     * 0 3
     */
    std::vector<FactPair> initial_state = {
        problem.get_fact_is_hand_empty(true),
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_table(3),
        problem.get_fact_is_clear(0, false),
        problem.get_fact_is_clear(1, false),
        problem.get_fact_is_clear(2, true),
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

    auto task = create_problem_task(problem, initial_state, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));
}

TEST(HFFHeuristicTestsPublic, test_bw_single_state_reeval)
{
    // 4 blocks
    BlocksWorldProblem problem(4);

    /**
     * 2
     * 1
     * 0 3
     */
    std::vector<FactPair> initial_state = {
        problem.get_fact_is_hand_empty(true),
        problem.get_fact_location_on_table(0),
        problem.get_fact_location_on_block(1, 0),
        problem.get_fact_location_on_block(2, 1),
        problem.get_fact_location_on_table(3),
        problem.get_fact_is_clear(0, false),
        problem.get_fact_is_clear(1, false),
        problem.get_fact_is_clear(2, true),
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

    auto task = create_problem_task(problem, initial_state, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));

    auto plan2 = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan2.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan2->operators));
}

TEST(HFFHeuristicTestsPublic, test_gripper_goal_aware)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(plan.has_value());
    ASSERT_EQ(plan->cost, 0);
    ASSERT_TRUE(plan->operators.empty());
}

TEST(HFFHeuristicTestsPublic, test_gripper_single_state)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));
}

TEST(HFFHeuristicTestsPublic, test_gripper_single_state_reeval)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));

    auto plan2 = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan2.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan2->operators));
}

TEST(HFFHeuristicTestsPublic, test_visitall_goal_aware)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(plan.has_value());
    ASSERT_EQ(plan->cost, 0);
    ASSERT_TRUE(plan->operators.empty());
}

TEST(HFFHeuristicTestsPublic, test_visitall_single_state)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));
}

TEST(HFFHeuristicTestsPublic, test_visitall_single_state_reeval)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));

    auto plan2 = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan2.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan2->operators));
}

TEST(HFFHeuristicTestsPublic, test_sokoban_goal_aware)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(plan.has_value());
    ASSERT_EQ(plan->cost, 0);
    ASSERT_TRUE(plan->operators.empty());
}

TEST(HFFHeuristicTestsPublic, test_sokoban_dead_end)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(!plan.has_value());
}

TEST(HFFHeuristicTestsPublic, test_sokoban_single_state)
{
    const SokobanGrid playarea = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', 'G', 'G'}
    };

    // 2 boxes
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(1, 0),
         problem.get_fact_box_at(0, 1, 1),
         problem.get_fact_box_at(1, 2, 1),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_box_at_goal(1, false),
         problem.get_fact_square_clear(0, 0, true),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(1, 0, false),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, false),
         problem.get_fact_square_clear(2, 2, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));
}

TEST(HFFHeuristicTestsPublic, test_sokoban_single_state_reeval)
{
    const SokobanGrid playarea = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', 'G', 'G'}
    };

    // 2 boxes
    SokobanProblem problem(playarea, 2);

    std::vector<FactPair> initial(
        {problem.get_fact_player_at(1, 0),
         problem.get_fact_box_at(0, 1, 1),
         problem.get_fact_box_at(1, 2, 1),
         problem.get_fact_box_at_goal(0, false),
         problem.get_fact_box_at_goal(1, false),
         problem.get_fact_square_clear(0, 0, true),
         problem.get_fact_square_clear(0, 1, true),
         problem.get_fact_square_clear(0, 2, true),
         problem.get_fact_square_clear(1, 0, false),
         problem.get_fact_square_clear(1, 1, false),
         problem.get_fact_square_clear(1, 2, true),
         problem.get_fact_square_clear(2, 0, true),
         problem.get_fact_square_clear(2, 1, false),
         problem.get_fact_square_clear(2, 2, true)});

    std::vector<FactPair> goal(
        {problem.get_fact_box_at_goal(0, true),
         problem.get_fact_box_at_goal(1, true)});

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));

    auto plan2 = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan2.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan2->operators));
}

TEST(HFFHeuristicTestsPublic, test_nomystery_goal_aware)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(plan.has_value());
    ASSERT_EQ(plan->cost, 0);
    ASSERT_TRUE(plan->operators.empty());
}

TEST(HFFHeuristicTestsPublic, test_nomystery_dead_end)
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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);
    ASSERT_TRUE(!plan.has_value());
}

TEST(HFFHeuristicTestsPublic, test_nomystery_single_state)
{
    // 4 -- 5 -- 6
    // |  /
    // | /
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 2},
        {2, 1},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 4},
        {5, 6},
        {6, 5}
    };

    // 7 locations, 3 packages, truck capacity 2
    NoMysteryProblem problem(7, 3, 2, roadmap);

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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));
}

TEST(HFFHeuristicTestsPublic, test_nomystery_single_state_reeval)
{
    // 4 -- 5 -- 6
    // |  /
    // | /
    // 0 -- 1 -- 2   3 (disconnected)
    std::set<RoadMapEdge> roadmap = {
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 2},
        {2, 1},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 4},
        {5, 6},
        {6, 5}
    };

    // 7 locations, 3 packages, truck capacity 2
    NoMysteryProblem problem(7, 3, 2, roadmap);

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

    auto task = create_problem_task(problem, initial, goal);
    auto hff = std::make_shared<FFHeuristic>(task);

    auto plan = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan->operators));

    auto plan2 = get_delete_relaxed_plan(*task, *hff);

    ASSERT_TRUE(plan2.has_value());
    ASSERT_TRUE(verify_relaxed_plan(*task, plan2->operators));
}