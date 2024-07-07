#ifndef NEURALFD_TASKS_MODIFIED_INIT_GOALS_TASK_H
#define NEURALFD_TASKS_MODIFIED_INIT_GOALS_TASK_H

#include "downward/tasks/delegating_task.h"

#include <vector>

namespace extra_tasks {
class ModifiedInitGoalsTask : public tasks::DelegatingTask {
    const std::vector<int> initial_state;
    const std::vector<FactPair> goals;

public:
    ModifiedInitGoalsTask(
        const std::shared_ptr<ClassicalTask>& parent,
        const std::vector<int>&& initial_state,
        const std::vector<FactPair>&& goals);
    virtual ~ModifiedInitGoalsTask() override = default;

    virtual int get_num_goal_facts() const override;
    virtual FactPair get_goal_fact(int index) const override;
    virtual std::vector<int> get_initial_state_values() const override;
};
} // namespace extra_tasks
#endif
