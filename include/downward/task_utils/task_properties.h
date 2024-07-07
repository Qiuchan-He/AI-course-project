#ifndef DOWNWARD_TASK_UTILS_TASK_PROPERTIES_H
#define DOWNWARD_TASK_UTILS_TASK_PROPERTIES_H

#include "downward/per_task_information.h"
#include "downward/state.h"

#include "downward/algorithms/int_packer.h"

namespace task_properties {
inline bool is_applicable(const OperatorProxy& op, const State& state)
{
    for (FactProxy precondition : op.get_precondition()) {
        if (state[precondition.get_variable()] != precondition) return false;
    }
    return true;
}

inline bool is_goal_state(const PlanningTaskProxy& task, const State& state)
{
    for (FactProxy goal : task.get_goal()) {
        if (state[goal.get_variable()] != goal) return false;
    }
    return true;
}

inline bool is_goal_assignment(
    const ClassicalTaskProxy& task,
    const PartialAssignment& partial_assignment)
{
    for (FactProxy goal : task.get_goal()) {
        if (partial_assignment[goal.get_variable()] != goal) return false;
    }
    return true;
}

extern std::vector<FactPair> get_strips_fact_pairs(const ClassicalTask* task);

/*
  Return true iff all operators have cost 1.

  Runtime: O(n), where n is the number of operators.
*/
extern bool is_unit_cost(const ClassicalTaskProxy& task);

extern double get_average_operator_cost(const ClassicalTaskProxy& task_proxy);

/*
  Return the number of facts of the task.
  Runtime: O(n), where n is the number of state variables.
*/
extern int get_num_facts(const PlanningTaskProxy& task_proxy);

/*
  Return the total number of effects of the task.
  Runtime: O(n), where n is the number of operators.
*/
extern int get_num_total_effects(const ClassicalTaskProxy& task_proxy);

template <class FactProxyCollection>
std::vector<FactPair> get_fact_pairs(const FactProxyCollection& facts)
{
    std::vector<FactPair> fact_pairs;
    fact_pairs.reserve(facts.size());
    for (FactProxy fact : facts) {
        fact_pairs.push_back(fact.get_pair());
    }
    return fact_pairs;
}

extern void print_variable_statistics(
    const PlanningTaskProxy& task_proxy,
    utils::LogProxy& log);

template <typename T = utils::LogProxy>
void dump_pddl(
    const State& state,
    T& stream = utils::g_log,
    const std::string& separator = "\n",
    bool skip_undefined = false)
{
    for (FactProxy fact : state) {
        std::string fact_name = fact.get_name();
        if (skip_undefined && fact_name == "<undefined>") {
            continue;
        }
        if (fact_name != "<none of those>") stream << fact_name << separator;
    }
    stream << std::flush;
}

template <typename T = utils::LogProxy>
void dump_fdr(
    const State& state,
    T& stream = utils::g_log,
    const std::string& separator = "\n")
{
    for (FactProxy fact : state) {
        VariableProxy var = fact.get_variable();
        stream << "  #" << var.get_id() << " [" << var.get_name() << "] -> "
               << fact.get_value() << separator;
    }
    stream << std::flush;
}

template <typename T = utils::LogProxy>
void dump_pddl(
    const PartialAssignment& state,
    T& stream = utils::g_log,
    const std::string& separator = "\n")
{
    for (unsigned int var = 0; var < state.size(); ++var) {
        if (state.assigned(var)) {
            FactProxy fact = state[var];
            std::string fact_name = fact.get_name();
            if (fact_name != "<none of those>")
                stream << fact_name << separator;
        }
    }
    stream << std::flush;
}

template <typename T = utils::LogProxy>
void dump_fdr(
    const PartialAssignment& state,
    T& stream = utils::g_log,
    const std::string& separator = "\n")
{
    for (unsigned int var = 0; var < state.size(); ++var) {
        if (state.assigned(var)) {
            FactProxy fact = state[var];
            VariableProxy var = fact.get_variable();
            stream << "  #" << var.get_id() << " [" << var.get_name() << "] -> "
                   << fact.get_value() << separator;
        }
    }
    stream << std::flush;
}

template <typename T = utils::LogProxy>
void dump_goals(
    const GoalProxy& goals,
    T& stream = utils::g_log,
    const std::string& separator = "\n")
{
    stream << "Goal conditions:" << separator;
    for (FactProxy goal : goals) {
        stream << "  " << goal.get_variable().get_name() << ": "
               << goal.get_value() << separator;
    }
    stream << std::flush;
}

template <typename T = utils::LogProxy>
void dump_task(
    const ClassicalTaskProxy& task_proxy,
    T& stream = utils::g_log,
    const std::string& separator = "\n")
{
    OperatorsProxy operators = task_proxy.get_operators();
    int min_action_cost = std::numeric_limits<int>::max();
    int max_action_cost = 0;
    for (OperatorProxy op : operators) {
        min_action_cost = std::min(min_action_cost, op.get_cost());
        max_action_cost = std::max(max_action_cost, op.get_cost());
    }
    stream << "Min action cost: " << min_action_cost << separator;
    stream << "Max action cost: " << max_action_cost << separator;

    VariablesProxy variables = task_proxy.get_variables();
    stream << "Variables (" << variables.size() << "):" << separator;
    for (VariableProxy var : variables) {
        stream << "  " << var.get_name() << " (range " << var.get_domain_size()
               << ")" << separator;
        for (int val = 0; val < var.get_domain_size(); ++val) {
            stream << "    " << val << ": " << var.get_fact(val).get_name()
                   << separator;
        }
    }
    State initial_state = task_proxy.get_initial_state();
    stream << "Initial state (PDDL):" << separator;
    dump_pddl(initial_state, stream, separator);
    stream << "Initial state (FDR):" << separator;
    dump_fdr(initial_state, stream, separator);
    dump_goals(task_proxy.get_goal(), stream, separator);
    stream << std::flush;
}

extern PerTaskInformation<int_packer::IntPacker> g_state_packers;
} // namespace task_properties

#endif
