#include "probfd/task_proxy.h"

#include "downward/utils/collections.h"
#include "downward/utils/hash.h"
#include "downward/utils/system.h"

#include <cassert>

namespace probfd {

ProbabilisticEffectProxy::ProbabilisticEffectProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index)
    : task(&task)
    , op_index(op_index)
    , outcome_index(outcome_index)
{
}

std::size_t ProbabilisticEffectProxy::size() const
{
    return task->get_num_operator_outcome_effect_facts(op_index, outcome_index);
}

FactProxy ProbabilisticEffectProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());

    return FactProxy(
        *task,
        task->get_operator_outcome_effect_fact(
            op_index,
            outcome_index,
            eff_index));
}

ProbabilisticOutcomeProxy::ProbabilisticOutcomeProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index)
    : task(&task)
    , op_index(op_index)
    , outcome_index(outcome_index)
{
}

ProbabilisticEffectProxy ProbabilisticOutcomeProxy::get_effect() const
{
    return ProbabilisticEffectProxy(*task, op_index, outcome_index);
}

double ProbabilisticOutcomeProxy::get_probability() const
{
    return task->get_operator_outcome_probability(op_index, outcome_index);
}

ProbabilisticOutcomesProxy::ProbabilisticOutcomesProxy(
    const ProbabilisticTask& task,
    int op_index)
    : task(&task)
    , op_index(op_index)
{
}

std::size_t ProbabilisticOutcomesProxy::size() const
{
    return task->get_num_operator_outcomes(op_index);
}

ProbabilisticOutcomeProxy
ProbabilisticOutcomesProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return ProbabilisticOutcomeProxy(*task, op_index, eff_index);
}

ProbabilisticOperatorProxy::ProbabilisticOperatorProxy(
    const ProbabilisticTask& task,
    int index)
    : task(&task)
    , index(index)
{
}

bool operator==(
    const ProbabilisticOperatorProxy& left,
    const ProbabilisticOperatorProxy& right)
{
    assert(left.task == right.task);
    return left.index == right.index;
}

PreconditionProxy ProbabilisticOperatorProxy::get_precondition() const
{
    return PreconditionProxy(*task, index);
}

ProbabilisticOutcomesProxy ProbabilisticOperatorProxy::get_outcomes() const
{
    return ProbabilisticOutcomesProxy(*task, index);
}

double ProbabilisticOperatorProxy::get_cost() const
{
    return task->get_operator_cost(index);
}

std::string ProbabilisticOperatorProxy::get_name() const
{
    return task->get_operator_name(index);
}

int ProbabilisticOperatorProxy::get_id() const
{
    return index;
}

ProbabilisticOperatorsProxy::ProbabilisticOperatorsProxy(
    const ProbabilisticTask& task)
    : task(&task)
{
}

std::size_t ProbabilisticOperatorsProxy::size() const
{
    return task->get_num_operators();
}

ProbabilisticOperatorProxy
ProbabilisticOperatorsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return ProbabilisticOperatorProxy(*task, index);
}

ProbabilisticOperatorProxy
ProbabilisticOperatorsProxy::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

ProbabilisticTaskProxy::ProbabilisticTaskProxy(const ProbabilisticTask& task)
    : PlanningTaskProxy(task)
{
}

ProbabilisticOperatorsProxy ProbabilisticTaskProxy::get_operators() const
{
    return ProbabilisticOperatorsProxy(
        *static_cast<const ProbabilisticTask*>(task));
}

} // namespace probfd
