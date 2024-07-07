#ifndef DOWNWARD_TASKS_DELEGATING_TASK_H
#define DOWNWARD_TASKS_DELEGATING_TASK_H

#include "downward/planning_task.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tasks {
/*
  Task transformation that delegates all calls to the corresponding methods of
  the parent task. You should inherit from this class instead of ClassicalTask
  if you need specialized behavior for only some of the methods.
*/
class DelegatingTask : public ClassicalTask {
protected:
    const std::shared_ptr<ClassicalTask> parent;

public:
    explicit DelegatingTask(const std::shared_ptr<ClassicalTask>& parent);
    virtual ~DelegatingTask() override = default;

    virtual int get_num_variables() const override;
    virtual std::string get_variable_name(int var) const override;
    virtual int get_variable_domain_size(int var) const override;
    virtual std::string get_fact_name(const FactPair& fact) const override;
    virtual bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    virtual int get_operator_cost(int index) const override;
    virtual std::string get_operator_name(int index) const override;
    virtual int get_num_operators() const override;
    virtual int get_num_operator_precondition_facts(int index) const override;
    virtual FactPair
    get_operator_precondition_fact(int op_index, int fact_index) const override;
    virtual int get_num_operator_effect_facts(int op_index) const override;
    virtual FactPair
    get_operator_effect_fact(int op_index, int eff_index) const override;

    virtual int convert_operator_index_to_parent(int index) const
    {
        return index;
    }

    virtual int get_num_goal_facts() const override;
    virtual FactPair get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;

    virtual bool is_undefined(const FactPair& fact) const override;

    virtual void convert_state_values_from_parent(std::vector<int>&) const {}
};
} // namespace tasks

#endif
