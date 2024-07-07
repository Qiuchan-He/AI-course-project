#include "downward/pdbs/syntactic_projection.h"

#include "downward/utils/collections.h"

#include "downward/task_proxy.h"

#include <unordered_map>

namespace pdbs {

SyntacticProjection::SyntacticProjection(
    const ClassicalTask& parent_task,
    const Pattern& pattern)
{
    ClassicalTaskProxy proxy(parent_task);

    VariablesProxy vars = proxy.get_variables();

    std::unordered_map<int, int> global2local;

    auto parent_init_values = parent_task.get_initial_state_values();

    int idx = 0;
    multipliers.push_back(1);
    for (int var : pattern) {
        global2local[var] = idx++;
        auto& var_fact_names = fact_names.emplace_back();

        int domain_size = vars[var].get_domain_size();
        multipliers.push_back(multipliers.back() * domain_size);

        for (int i = 0; i != domain_size; ++i) {
            var_fact_names.push_back(
                parent_task.get_fact_name(FactPair(var, i)));
        }

        ExplicitVariable& new_var = variables.emplace_back();
        new_var.name = vars[var].get_name();
        new_var.domain_size = domain_size;

        initial_state_values.push_back(parent_init_values[var]);
    }

    for (OperatorProxy op : proxy.get_operators()) {
        ExplicitOperator& new_op = operators.emplace_back();
        new_op.name = op.get_name();
        new_op.cost = op.get_cost();

        for (FactProxy fact : op.get_precondition()) {
            if (auto it = global2local.find(fact.get_variable().get_id());
                it != global2local.end()) {
                new_op.precondition.emplace_back(it->second, fact.get_value());
            }
        }

        for (FactProxy fact : op.get_effect()) {
            if (auto it = global2local.find(fact.get_variable().get_id());
                it != global2local.end()) {
                new_op.effect.emplace_back(it->second, fact.get_value());
            }
        }
    }

    for (FactProxy fact : proxy.get_goal()) {
        if (auto it = global2local.find(fact.get_variable().get_id());
            it != global2local.end()) {
            goal_facts.emplace_back(it->second, fact.get_value());
        }
    }
}

int SyntacticProjection::get_num_variables() const
{
    return variables.size();
}

std::string SyntacticProjection::get_variable_name(int var) const
{
    return variables[var].name;
}

int SyntacticProjection::get_variable_domain_size(int var) const
{
    return variables[var].domain_size;
}

std::string SyntacticProjection::get_fact_name(const FactPair& fact) const
{
    return fact_names[fact.var][fact.value];
}

int SyntacticProjection::get_num_operators() const
{
    return operators.size();
}

std::string SyntacticProjection::get_operator_name(int op_index) const
{
    return operators[op_index].name;
}

int SyntacticProjection::get_operator_cost(int op_index) const
{
    return operators[op_index].cost;
}

int SyntacticProjection::get_num_operator_precondition_facts(int op_index) const
{
    return operators[op_index].precondition.size();
}

FactPair SyntacticProjection::get_operator_precondition_fact(
    int op_index,
    int fact_index) const
{
    return operators[op_index].precondition[fact_index];
}

int SyntacticProjection::get_num_operator_effect_facts(int op_index) const
{
    return operators[op_index].effect.size();
}

FactPair
SyntacticProjection::get_operator_effect_fact(int op_index, int eff_index) const
{
    return operators[op_index].effect[eff_index];
}

int SyntacticProjection::get_num_goal_facts() const
{
    return goal_facts.size();
}

FactPair SyntacticProjection::get_goal_fact(int index) const
{
    return goal_facts[index];
}

std::vector<int> SyntacticProjection::get_initial_state_values() const
{
    return initial_state_values;
}

bool SyntacticProjection::are_facts_mutex(const FactPair&, const FactPair&)
    const
{
    abort();
}

bool SyntacticProjection::is_undefined(const FactPair&) const
{
    abort();
}

int SyntacticProjection::compute_index(const State& state) const
{
    int index = 0;
    for (size_t i = 0; i != state.size(); ++i) {
        index += multipliers[i] * state[i].get_value();
    }
    return index;
}

int SyntacticProjection::compute_index(const std::vector<int>& state) const
{
    int index = 0;
    for (size_t i = 0; i != state.size(); ++i) {
        index += multipliers[i] * state[i];
    }
    return index;
}

State SyntacticProjection::get_state_for_index(int index) const
{
    std::vector<int> values(multipliers.size() - 1, 0);
    for (size_t i = 0; i != multipliers.size() - 1; ++i) {
        values[i] = (index / multipliers[i]) % variables[i].domain_size;
    }
    return State(*this, std::move(values));
}

size_t SyntacticProjection::get_num_states() const
{
    return multipliers.back();
}

} // namespace pdbs