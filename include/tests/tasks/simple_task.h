#ifndef TASKS_SIMPLE_TASK_H
#define TASKS_SIMPLE_TASK_H

#include "downward/planning_task.h"

#include <algorithm>
#include <string>
#include <vector>

namespace tests {

/**
 * @brief Represents a classical planning problem, but without an initial state
 * and goal.
 */
class ClassicalPlanningProblem {
protected:
    struct VariableInfo {
        std::string name;
        int domain_size;
        std::vector<std::string> fact_names;

        VariableInfo() = default;

        VariableInfo(
            std::string name,
            int domain_size,
            std::vector<std::string> fact_names)
            : name(std::move(name))
            , domain_size(domain_size)
            , fact_names(std::move(fact_names))
        {
        }
    };

    struct OperatorInfo {
        std::string name;
        int cost;
        std::vector<FactPair> precondition;
        std::vector<FactPair> effect;

        OperatorInfo() = default;

        OperatorInfo(
            std::string name,
            int cost,
            std::vector<FactPair> precondition,
            std::vector<FactPair> effect)
            : name(std::move(name))
            , cost(cost)
            , precondition(std::move(precondition))
            , effect(std::move(effect))
        {
            std::ranges::sort(precondition);
            std::ranges::sort(effect);
        }
    };

    std::vector<VariableInfo> variable_infos;
    std::vector<OperatorInfo> operators;

public:
    ClassicalPlanningProblem() = default;
    ClassicalPlanningProblem(int num_variables, int num_operators);

    int get_num_variables() const;
    std::string get_variable_name(int var) const;
    int get_variable_domain_size(int var) const;
    std::string get_fact_name(const FactPair& fact) const;

    int get_num_operators() const;
    std::string get_operator_name(int index) const;
    int get_operator_cost(int index) const;

    int get_num_operator_precondition_facts(int op_index) const;
    FactPair get_operator_precondition_fact(int op_index, int fact_index) const;

    int get_num_operator_effect_facts(int op_index) const;
    FactPair get_operator_effect_fact(int op_index, int eff_index) const;

    bool verify_partial_assignment(const std::vector<FactPair>& facts) const;
    bool verify_complete_assignment(const std::vector<FactPair>& facts) const;
};

/**
 * @brief Construct a ClassicalTask for a given ClassicalPlanningProblem,
 * by adding an initial state and goal.
 */
std::shared_ptr<ClassicalTask> create_problem_task(
    const ClassicalPlanningProblem& problem,
    const std::vector<FactPair>& initial,
    std::vector<FactPair> goal);

// Utility function that aborts the program with an error message.
[[noreturn]] void abort_with_error_msg(std::string errormsg);

} // namespace tests

#endif // TASKS_SIMPLE_TASK_H
