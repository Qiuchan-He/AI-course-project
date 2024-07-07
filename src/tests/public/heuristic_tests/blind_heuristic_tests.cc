#include <gtest/gtest.h>

#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/utils/logging.h"

#include "tests/tasks/blocksworld.h"

#include "tests/utils/heuristic_utils.h"

using namespace blind_search_heuristic;
using namespace tests;

TEST(BlindHeuristicTestsPublic, test_goal_aware)
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

    std::shared_ptr task = create_problem_task(problem, initial_state, goal);
    auto blind_heuristic = std::make_shared<BlindSearchHeuristic>(task);

    ASSERT_EQ(get_initial_state_estimate(*task, *blind_heuristic), 0);
}

TEST(BlindHeuristicTestsPublic, test_non_goal_minimum_cost)
{
    // 4 blocks, cost of picking up block is 100, putting down is 10
    BlocksWorldProblem problem(4, 100, 10);

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

    std::shared_ptr task = create_problem_task(problem, initial_state, goal);
    auto blind_heuristic = std::make_shared<BlindSearchHeuristic>(task);

    ASSERT_EQ(get_initial_state_estimate(*task, *blind_heuristic), 10);
}