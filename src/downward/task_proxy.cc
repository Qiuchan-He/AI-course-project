#include "downward/task_proxy.h"
#include "downward/state.h"

using namespace std;

FactProxy::FactProxy(const PlanningTask& task, const FactPair& fact)
    : task(&task)
    , fact(fact)
{
    assert(fact.var >= 0 && fact.var < task.get_num_variables());
    assert(fact.value >= 0 && fact.value < get_variable().get_domain_size());
}

FactProxy::FactProxy(const PlanningTask& task, int var_id, int value)
    : FactProxy(task, FactPair(var_id, value))
{
}

VariableProxy FactProxy::get_variable() const
{
    return VariableProxy(*task, fact.var);
}

int FactProxy::get_value() const
{
    return fact.value;
}

FactPair FactProxy::get_pair() const
{
    return fact;
}

std::string FactProxy::get_name() const
{
    return task->get_fact_name(fact);
}

bool FactProxy::is_mutex(const FactProxy& other) const
{
    return task->are_facts_mutex(fact, other.fact);
}

bool operator==(const FactProxy& left, const FactProxy& right)
{
    assert(left.task == right.task);
    return left.fact == right.fact;
}

FactsProxy::FactsProxyIterator::FactsProxyIterator(
    const PlanningTask& task,
    int var_id,
    int value)
    : task(&task)
    , var_id(var_id)
    , value(value)
{
}

FactProxy FactsProxy::FactsProxyIterator::operator*() const
{
    return FactProxy(*task, var_id, value);
}

FactsProxy::FactsProxyIterator& FactsProxy::FactsProxyIterator::operator++()
{
    assert(var_id < task->get_num_variables());
    int num_facts = task->get_variable_domain_size(var_id);
    assert(value < num_facts);
    ++value;
    if (value == num_facts) {
        ++var_id;
        value = 0;
    }
    return *this;
}

bool operator==(
    const FactsProxy::FactsProxyIterator& left,
    const FactsProxy::FactsProxyIterator& right)
{
    assert(left.task == right.task);
    return left.var_id == right.var_id && left.value == right.value;
}

FactsProxy::FactsProxy(const PlanningTask& task)
    : task(&task)
{
}

FactsProxy::FactsProxyIterator FactsProxy::begin() const
{
    return FactsProxyIterator(*task, 0, 0);
}

FactsProxy::FactsProxyIterator FactsProxy::end() const
{
    return FactsProxyIterator(*task, task->get_num_variables(), 0);
}

int FactsProxy::size() const
{
    if (num_facts == -1) {
        num_facts = 0;
        for (auto it = begin(), e = end(); it != e; ++it)
            ++num_facts;
    }
    return num_facts;
}

VariableProxy::VariableProxy(const PlanningTask& task, int id)
    : task(&task)
    , id(id)
{
}

int VariableProxy::get_id() const
{
    return id;
}

std::string VariableProxy::get_name() const
{
    return task->get_variable_name(id);
}

int VariableProxy::get_domain_size() const
{
    return task->get_variable_domain_size(id);
}

FactProxy VariableProxy::get_fact(int index) const
{
    assert(index < get_domain_size());
    return FactProxy(*task, id, index);
}

bool operator==(const VariableProxy& left, const VariableProxy& right)
{
    assert(left.task == right.task);
    return left.id == right.id;
}

VariablesProxy::VariablesProxy(const PlanningTask& task)
    : task(&task)
{
}

std::size_t VariablesProxy::size() const
{
    return task->get_num_variables();
}

VariableProxy VariablesProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return VariableProxy(*task, index);
}

FactsProxy VariablesProxy::get_facts() const
{
    return FactsProxy(*task);
}

PreconditionProxy::PreconditionProxy(const PlanningTask& task, int op_index)
    : task(&task)
    , op_index(op_index)
{
}

PreconditionProxy::PreconditionProxy(
    const PlanningTask& task,
    OperatorID op_index)
    : task(&task)
    , op_index(op_index.get_index())
{
}

std::size_t PreconditionProxy::size() const
{
    return task->get_num_operator_precondition_facts(op_index);
}

FactProxy PreconditionProxy::operator[](std::size_t fact_index) const
{
    assert(fact_index < size());
    return FactProxy(
        *task,
        task->get_operator_precondition_fact(op_index, fact_index));
}

EffectProxy::EffectProxy(const ClassicalTask& task, int op_index)
    : task(&task)
    , op_index(op_index)
{
}

EffectProxy::EffectProxy(const ClassicalTask& task, OperatorID op_index)
    : task(&task)
    , op_index(op_index.get_index())
{
}

std::size_t EffectProxy::size() const
{
    return task->get_num_operator_effect_facts(op_index);
}

FactProxy EffectProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return FactProxy(
        *task,
        task->get_operator_effect_fact(op_index, eff_index));
}

OperatorProxy::OperatorProxy(const ClassicalTask& task, int index)
    : task(&task)
    , index(index)
{
}

OperatorProxy::OperatorProxy(const ClassicalTask& task, OperatorID index)
    : task(&task)
    , index(index.get_index())
{
}

PreconditionProxy OperatorProxy::get_precondition() const
{
    return PreconditionProxy(*task, index);
}

EffectProxy OperatorProxy::get_effect() const
{
    return EffectProxy(*task, index);
}

int OperatorProxy::get_cost() const
{
    return task->get_operator_cost(index);
}

std::string OperatorProxy::get_name() const
{
    return task->get_operator_name(index);
}

int OperatorProxy::get_id() const
{
    return index;
}

bool operator==(const OperatorProxy& left, const OperatorProxy& right)
{
    assert(left.task == right.task);
    return left.index == right.index;
}

AbstractOperatorProxy::AbstractOperatorProxy(
    const PlanningTask& task,
    int index)
    : task(&task)
    , index(index)
{
}

AbstractOperatorProxy::AbstractOperatorProxy(
    const PlanningTask& task,
    OperatorID index)
    : task(&task)
    , index(index.get_index())
{
}

PreconditionProxy AbstractOperatorProxy::get_precondition() const
{
    return PreconditionProxy(*task, index);
}

std::string AbstractOperatorProxy::get_name() const
{
    return task->get_operator_name(index);
}

int AbstractOperatorProxy::get_id() const
{
    return index;
}

bool operator==(
    const AbstractOperatorProxy& left,
    const AbstractOperatorProxy& right)
{
    assert(left.task == right.task);
    return left.index == right.index;
}

OperatorsProxy::OperatorsProxy(const ClassicalTask& task)
    : task(&task)
{
}

std::size_t OperatorsProxy::size() const
{
    return task->get_num_operators();
}

OperatorProxy OperatorsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return OperatorProxy(*task, index);
}

OperatorProxy OperatorsProxy::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

AbstractOperatorsProxy::AbstractOperatorsProxy(const PlanningTask& task)
    : task(&task)
{
}

std::size_t AbstractOperatorsProxy::size() const
{
    return task->get_num_operators();
}

AbstractOperatorProxy
AbstractOperatorsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return AbstractOperatorProxy(*task, index);
}

AbstractOperatorProxy AbstractOperatorsProxy::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

GoalProxy::GoalProxy(const PlanningTask& task)
    : task(&task)
{
}

std::size_t GoalProxy::size() const
{
    return task->get_num_goal_facts();
}

FactProxy GoalProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return FactProxy(*task, task->get_goal_fact(index));
}

PlanningTaskProxy::operator ClassicalTaskProxy() const
{
    return ClassicalTaskProxy(*static_cast<const ClassicalTask*>(task));
}

State PlanningTaskProxy::create_state(
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer) const
{
    return State(*task, registry, id, buffer);
}

State PlanningTaskProxy::create_state(
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer,
    std::vector<int>&& state_values) const
{
    return State(*task, registry, id, buffer, std::move(state_values));
}

PlanningTaskProxy::PlanningTaskProxy(const PlanningTask& task)
    : task(&task)
{
}

void PlanningTaskProxy::subscribe_to_task_destruction(
    subscriber::Subscriber<PlanningTask>* subscriber) const
{
    task->subscribe(subscriber);
}

TaskID PlanningTaskProxy::get_id() const
{
    return TaskID(task);
}

VariablesProxy PlanningTaskProxy::get_variables() const
{
    return VariablesProxy(*task);
}

AbstractOperatorsProxy PlanningTaskProxy::get_abstract_operators() const
{
    return AbstractOperatorsProxy(*task);
}

GoalProxy PlanningTaskProxy::get_goal() const
{
    return GoalProxy(*task);
}

State PlanningTaskProxy::create_state(std::vector<int>&& state_values) const
{
    return State(*task, std::move(state_values));
}

State PlanningTaskProxy::get_initial_state() const
{
    return create_state(task->get_initial_state_values());
}

ClassicalTaskProxy::ClassicalTaskProxy(const ClassicalTask& task)
    : PlanningTaskProxy(task)
{
}

const ClassicalTask* ClassicalTaskProxy::get_task() const
{
    return static_cast<const ClassicalTask*>(task);
}

OperatorsProxy ClassicalTaskProxy::get_operators() const
{
    return OperatorsProxy(*static_cast<const ClassicalTask*>(task));
}

pair<bool, State> PlanningTaskProxy::convert_to_full_state(
    PartialAssignment& assignment,
    bool check_mutexes,
    utils::RandomNumberGenerator& rng) const
{
    return assignment.get_full_state(check_mutexes, rng);
}