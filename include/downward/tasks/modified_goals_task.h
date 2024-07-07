#ifndef DOWNWARD_TASKS_MODIFIED_GOALS_TASK_H
#define DOWNWARD_TASKS_MODIFIED_GOALS_TASK_H

#include "downward/tasks/delegating_task.h"

#include <vector>

namespace extra_tasks {
class ModifiedGoalsTask : public tasks::DelegatingTask {
    const std::vector<FactPair> goals;

public:
    ModifiedGoalsTask(
        const std::shared_ptr<ClassicalTask>& parent,
        std::vector<FactPair>&& goals);
    ~ModifiedGoalsTask() = default;

    virtual int get_num_goal_facts() const override;
    virtual FactPair get_goal_fact(int index) const override;
};
} // namespace extra_tasks

#endif
