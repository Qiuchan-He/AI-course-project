#include "tests/utils/task_utils.h"

#include "probfd/probabilistic_task.h"

#include "downward/planning_task.h"

#include "downward/utils/system.h"

using namespace probfd;

namespace tests {

namespace {

class ProbabilisticWrapperTask : public ProbabilisticTask {
    std::shared_ptr<ClassicalTask> deterministic_task;

public:
    explicit ProbabilisticWrapperTask(
        std::shared_ptr<ClassicalTask> deterministic_task)
        : deterministic_task(std::move(deterministic_task))
    {
    }

    int get_num_variables() const override
    {
        return deterministic_task->get_num_variables();
    }

    std::string get_variable_name(int var) const override
    {
        return deterministic_task->get_variable_name(var);
    }

    int get_variable_domain_size(int var) const override
    {
        return deterministic_task->get_variable_domain_size(var);
    }

    std::string get_fact_name(const FactPair& fact) const override
    {
        return deterministic_task->get_fact_name(fact);
    }

    bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const override
    {
        return deterministic_task->are_facts_mutex(fact1, fact2);
    }

    std::string get_operator_name(int index) const override
    {
        return deterministic_task->get_operator_name(index);
    }

    int get_num_operators() const override
    {
        return deterministic_task->get_num_operators();
    }

    int get_num_operator_precondition_facts(int index) const override
    {
        return deterministic_task->get_num_operator_precondition_facts(index);
    }

    FactPair
    get_operator_precondition_fact(int op_index, int fact_index) const override
    {
        return deterministic_task->get_operator_precondition_fact(
            op_index,
            fact_index);
    }

    int get_num_goal_facts() const override
    {
        return deterministic_task->get_num_goal_facts();
    }

    FactPair get_goal_fact(int index) const override
    {
        return deterministic_task->get_goal_fact(index);
    }

    std::vector<int> get_initial_state_values() const override
    {
        return deterministic_task->get_initial_state_values();
    }

    bool is_undefined(const FactPair& fact) const override
    {
        return deterministic_task->is_undefined(fact);
    }

    double get_operator_cost(int op_index) const override
    {
        return static_cast<double>(
            deterministic_task->get_operator_cost(op_index));
    }

    int get_num_operator_outcomes(int) const override { return 1; }

    double get_operator_outcome_probability(int, int) const override
    {
        return 1.0;
    }

    int get_num_operator_outcome_effect_facts(int op_index, int) const override
    {
        return deterministic_task->get_num_operator_effect_facts(op_index);
    }

    FactPair get_operator_outcome_effect_fact(int op_index, int, int eff_index)
        const override
    {
        return deterministic_task->get_operator_effect_fact(
            op_index,
            eff_index);
    }
};

} // namespace

std::shared_ptr<ProbabilisticTask>
as_probabilistic_task(std::shared_ptr<ClassicalTask> deterministic_task)
{
    return std::make_shared<ProbabilisticWrapperTask>(
        std::move(deterministic_task));
}

}