#ifndef DOWNWARD_TASKS_COST_ADAPTED_TASK_H
#define DOWNWARD_TASKS_COST_ADAPTED_TASK_H

#include "downward/tasks/delegating_task.h"

#include "downward/operator_cost.h"

namespace options {
class Options;
}

namespace tasks {
/*
  Task transformation that changes operator costs. If the parent task assigns
  costs 'c' to an operator, its adjusted costs, depending on the value of the
  cost_type option, are:

    NORMAL:  c
    ONE:     1
    PLUSONE: 1, if all operators have cost 1 in the parent task, else c + 1
*/
class CostAdaptedTask : public DelegatingTask {
    const OperatorCost cost_type;
    const bool parent_is_unit_cost;

public:
    CostAdaptedTask(
        const std::shared_ptr<ClassicalTask>& parent,
        OperatorCost cost_type);
    virtual ~CostAdaptedTask() override = default;

    virtual int get_operator_cost(int index) const override;
};
} // namespace tasks

#endif
