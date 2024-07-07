#ifndef NEURALFD_TASKS_PARTIAL_STATE_WRAPPER_TASK_H
#define NEURALFD_TASKS_PARTIAL_STATE_WRAPPER_TASK_H

#include "downward/tasks/delegating_task.h"

#include <vector>

namespace extra_tasks {
class PartialStateWrapperTask : public tasks::DelegatingTask {
public:
    explicit PartialStateWrapperTask(
        const std::shared_ptr<ClassicalTask>& parent);
    virtual ~PartialStateWrapperTask() = default;

    virtual int get_variable_domain_size(int var) const override;
    virtual std::string get_fact_name(const FactPair& fact) const override;
    virtual bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    virtual bool is_undefined(const FactPair& fact) const override;
};
} // namespace extra_tasks
#endif
