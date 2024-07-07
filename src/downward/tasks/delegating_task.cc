#include "downward/tasks/delegating_task.h"

using namespace std;

namespace tasks {
DelegatingTask::DelegatingTask(const shared_ptr<ClassicalTask>& parent)
    : parent(parent)
{
}

int DelegatingTask::get_num_variables() const
{
    return parent->get_num_variables();
}

string DelegatingTask::get_variable_name(int var) const
{
    return parent->get_variable_name(var);
}

int DelegatingTask::get_variable_domain_size(int var) const
{
    return parent->get_variable_domain_size(var);
}

string DelegatingTask::get_fact_name(const FactPair& fact) const
{
    return parent->get_fact_name(fact);
}

bool DelegatingTask::are_facts_mutex(
    const FactPair& fact1,
    const FactPair& fact2) const
{
    return parent->are_facts_mutex(fact1, fact2);
}

int DelegatingTask::get_operator_cost(int index) const
{
    return parent->get_operator_cost(index);
}

string DelegatingTask::get_operator_name(int index) const
{
    return parent->get_operator_name(index);
}

int DelegatingTask::get_num_operators() const
{
    return parent->get_num_operators();
}

int DelegatingTask::get_num_operator_precondition_facts(int index) const
{
    return parent->get_num_operator_precondition_facts(index);
}

FactPair
DelegatingTask::get_operator_precondition_fact(int op_index, int fact_index)
    const
{
    return parent->get_operator_precondition_fact(op_index, fact_index);
}

int DelegatingTask::get_num_operator_effect_facts(int op_index) const
{
    return parent->get_num_operator_effect_facts(op_index);
}

FactPair
DelegatingTask::get_operator_effect_fact(int op_index, int eff_index) const
{
    return parent->get_operator_effect_fact(op_index, eff_index);
}

int DelegatingTask::get_num_goal_facts() const
{
    return parent->get_num_goal_facts();
}

FactPair DelegatingTask::get_goal_fact(int index) const
{
    return parent->get_goal_fact(index);
}

vector<int> DelegatingTask::get_initial_state_values() const
{
    return parent->get_initial_state_values();
}

bool DelegatingTask::is_undefined(const FactPair& fact) const
{
    return parent->is_undefined(fact);
}

} // namespace tasks
