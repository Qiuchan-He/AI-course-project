#include "tests/tasks/simple_task.h"

#include <set>

namespace tests {

namespace {
bool in_domain(int val, int high)
{
    return val >= 0 && val < high;
}

class SimpleTask : public ClassicalTask {
    const ClassicalPlanningProblem& var_space;
    std::vector<int> initial_state;
    std::vector<FactPair> goal;

public:
    SimpleTask(
        const ClassicalPlanningProblem& var_space,
        std::vector<int> initial,
        std::vector<FactPair> goal)
        : var_space(var_space)
        , initial_state(std::move(initial))
        , goal(std::move(goal))
    {
    }

    ~SimpleTask() override = default;

    int get_num_variables() const override
    {
        return var_space.get_num_variables();
    }

    std::string get_variable_name(int var) const override
    {
        return var_space.get_variable_name(var);
    }

    int get_variable_domain_size(int var) const override
    {
        return var_space.get_variable_domain_size(var);
    }

    std::string get_fact_name(const FactPair& fact) const override
    {
        return var_space.get_fact_name(fact);
    }

    int get_num_operators() const override
    {
        return var_space.get_num_operators();
    }

    int get_operator_cost(int index) const override
    {
        return var_space.get_operator_cost(index);
    }

    std::string get_operator_name(int index) const override
    {
        return var_space.get_operator_name(index);
    }

    int get_num_operator_precondition_facts(int index) const override
    {
        return var_space.get_num_operator_precondition_facts(index);
    }

    FactPair
    get_operator_precondition_fact(int index, int fact_index) const override
    {
        return var_space.get_operator_precondition_fact(index, fact_index);
    }

    int get_num_operator_effect_facts(int index) const override
    {
        return var_space.get_num_operator_effect_facts(index);
    }

    FactPair get_operator_effect_fact(int index, int fact_index) const override
    {
        return var_space.get_operator_effect_fact(index, fact_index);
    }

    int get_num_goal_facts() const override { return goal.size(); }

    FactPair get_goal_fact(int index) const override { return goal[index]; }

    std::vector<int> get_initial_state_values() const override
    {
        return initial_state;
    }

    // Default implementations for inapplicable parts of the interface

    bool are_facts_mutex(const FactPair&, const FactPair&) const override
    {
        return false;
    }

    bool is_undefined(const FactPair&) const override { return false; }
};

} // namespace

ClassicalPlanningProblem::ClassicalPlanningProblem(int num_variables, int num_operators)
    : variable_infos(num_variables)
    , operators(num_operators)
{
}

int ClassicalPlanningProblem::get_num_variables() const
{
    return variable_infos.size();
}

std::string ClassicalPlanningProblem::get_variable_name(int var) const
{
    return variable_infos[var].name;
}

int ClassicalPlanningProblem::get_variable_domain_size(int var) const
{
    return variable_infos[var].domain_size;
}

std::string ClassicalPlanningProblem::get_fact_name(const FactPair& fact) const
{
    return variable_infos[fact.var].fact_names[fact.value];
}

int ClassicalPlanningProblem::get_num_operators() const
{
    return operators.size();
}

std::string ClassicalPlanningProblem::get_operator_name(int index) const
{
    return operators[index].name;
}

int ClassicalPlanningProblem::get_operator_cost(int index) const
{
    return operators[index].cost;
}

int ClassicalPlanningProblem::get_num_operator_precondition_facts(int op_index) const
{
    return operators[op_index].precondition.size();
}

FactPair
ClassicalPlanningProblem::get_operator_precondition_fact(int op_index, int fact_index) const
{
    return operators[op_index].precondition[fact_index];
}

int ClassicalPlanningProblem::get_num_operator_effect_facts(int op_index) const
{
    return operators[op_index].effect.size();
}

FactPair
ClassicalPlanningProblem::get_operator_effect_fact(int op_index, int eff_index) const
{
    return operators[op_index].effect[eff_index];
}

bool ClassicalPlanningProblem::verify_partial_assignment(
    const std::vector<FactPair>& facts) const
{
    std::set<int> vars;

    for (const auto& [var, val] : facts) {
        if (!vars.insert(var).second ||
            !in_domain(val, get_variable_domain_size(var)))
            return false;
    }

    return true;
}

bool ClassicalPlanningProblem::verify_complete_assignment(
    const std::vector<FactPair>& facts) const
{
    return facts.size() == static_cast<size_t>(get_num_variables()) &&
           verify_partial_assignment(facts);
}

std::shared_ptr<ClassicalTask> create_problem_task(
    const ClassicalPlanningProblem& problem,
    const std::vector<FactPair>& initial,
    std::vector<FactPair> goal)
{
    if (!problem.verify_complete_assignment(initial)) {
        abort_with_error_msg(
            "Initial state is not a complete variable assignment!");
    }

    if (!problem.verify_partial_assignment(initial)) {
        abort_with_error_msg(
            "Initial state is not a partial variable assignment!");
    }

    std::vector<int> state(initial.size());
    for (const auto& [var, val] : initial) {
        state[var] = val;
    }

    return std::make_shared<SimpleTask>(
        problem,
        std::move(state),
        std::move(goal));
}

void abort_with_error_msg(std::string errormsg)
{
    std::cerr << errormsg << std::endl;
    abort();
}

} // namespace tests
