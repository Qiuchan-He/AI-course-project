#ifndef NEURALFD_TASK_UTILS_REGRESSION_UTILS_H
#define NEURALFD_TASK_UTILS_REGRESSION_UTILS_H

#include "downward/operator_id.h"
#include "downward/planning_task.h"
#include "downward/state.h"

#include "downward/utils/rng.h"

#include <cassert>
#include <set>
#include <string>
#include <utility>
#include <vector>

/* Special thanks to Jendrik Seipp from whom I got an initial code base for
   regression.*/

class RegressionCondition {
    friend class RegressionConditionProxy;

public:
    FactPair data;

    RegressionCondition(int var, int value);
    ~RegressionCondition() = default;

    bool operator<(const RegressionCondition& other) const
    {
        return data.var < other.data.var ||
               (data.var == other.data.var && data.value < other.data.value);
    }

    bool operator==(const RegressionCondition& other) const
    {
        return data.var == other.data.var && data.value == other.data.value;
    }

    bool operator!=(const RegressionCondition& other) const
    {
        return !(*this == other);
    }

    bool is_satisfied(const PartialAssignment& assignment) const;
};

class RegressionConditionProxy {
    const ClassicalTask* task;
    const RegressionCondition condition;

public:
    RegressionConditionProxy(const ClassicalTask& task, int var_id, int value);
    RegressionConditionProxy(
        const ClassicalTask& task,
        const RegressionCondition& fact);
    ~RegressionConditionProxy() = default;

    VariableProxy get_variable() const
    {
        return VariableProxy(*task, condition.data.var);
    }

    int get_value() const { return condition.data.value; }

    FactPair get_pair() const { return condition.data; }

    RegressionCondition get_condition() const { return condition; }

    std::string get_name() const { return task->get_fact_name(condition.data); }

    bool operator==(const RegressionConditionProxy& other) const
    {
        assert(task == other.task);
        return condition == other.condition;
    }

    bool operator!=(const RegressionConditionProxy& other) const
    {
        return !(*this == other);
    }

    bool is_mutex(const RegressionConditionProxy& other) const
    {
        return task->are_facts_mutex(condition.data, other.condition.data);
    }
};

class ConditionsProxy : public ProxyRange<ConditionsProxy> {
protected:
    const ClassicalTask* task;

public:
    explicit ConditionsProxy(const ClassicalTask& task)
        : task(&task)
    {
    }
    virtual ~ConditionsProxy() = default;

    virtual std::size_t size() const = 0;
    virtual FactProxy operator[](std::size_t index) const = 0;

    bool empty() const { return size() == 0; }

    template <typename T>
    void
    dump_pddl(T& out = utils::g_log, const std::string& separator = "\n") const
    {
        for (FactProxy fact : (*this)) {
            std::string fact_name = fact.get_name();
            if (fact_name != "<none of those>") out << fact_name << separator;
        }
        out << std::flush;
    }

    template <typename T>
    void
    dump_fdr(T& out = utils::g_log, const std::string& separator = "\n") const
    {
        for (FactProxy fact : (*this)) {
            VariableProxy var = fact.get_variable();
            out << "  #" << var.get_id() << " [" << var.get_name() << "] -> "
                << fact.get_value() << separator;
        }
        out << std::flush;
    }
};

class RegressionConditionsProxy : public ConditionsProxy {
    const std::vector<RegressionCondition> conditions;

public:
    RegressionConditionsProxy(
        const ClassicalTask& task,
        const std::vector<RegressionCondition>& conditions)
        : ConditionsProxy(task)
        , conditions(conditions)
    {
    }
    ~RegressionConditionsProxy() = default;

    std::size_t size() const override { return conditions.size(); }

    FactProxy operator[](std::size_t fact_index) const override
    {
        assert(fact_index < size());
        return FactProxy(*task, conditions[fact_index].data);
    }

    RegressionCondition get(std::size_t fact_index) const
    {
        return conditions[fact_index];
    }
};

class RegressionEffect {
    friend class RegressionEffectProxy;

public:
    FactPair data;
    std::vector<FactPair> conditions;

    RegressionEffect(int var, int value);
    ~RegressionEffect() = default;

    bool operator<(const RegressionEffect& other) const
    {
        return data < other.data ||
               (data == other.data && conditions < other.conditions);
    }

    bool operator==(const RegressionEffect& other) const
    {
        return data == other.data && conditions == other.conditions;
    }

    bool operator!=(const RegressionEffect& other) const
    {
        return !(*this == other);
    }
};

class RegressionEffectConditionsProxy : public ConditionsProxy {
    const std::vector<FactPair> conditions;

public:
    RegressionEffectConditionsProxy(
        const ClassicalTask& task,
        const std::vector<FactPair>& conditions)
        : ConditionsProxy(task)
        , conditions(conditions)
    {
    }
    ~RegressionEffectConditionsProxy() = default;

    std::size_t size() const override { return conditions.size(); }

    //    bool empty() const {
    //        return size() == 0;
    //    }

    FactProxy operator[](std::size_t fact_index) const override
    {
        assert(fact_index < size());
        return FactProxy(*task, conditions[fact_index]);
    }
};

class RegressionEffectProxy {
    const ClassicalTask* task;
    RegressionEffect effect;

public:
    RegressionEffectProxy(const ClassicalTask& task, int var_id, int value);
    RegressionEffectProxy(
        const ClassicalTask& task,
        const RegressionEffect& fact);
    ~RegressionEffectProxy() = default;

    RegressionEffectConditionsProxy get_conditions() const
    {
        return RegressionEffectConditionsProxy(*task, effect.conditions);
    }

    VariableProxy get_variable() const
    {
        return VariableProxy(*task, effect.data.var);
    }

    int get_value() const { return effect.data.value; }

    FactPair get_pair() const { return effect.data; }

    RegressionEffect get_effect() const { return effect; }

    std::string get_name() const { return task->get_fact_name(effect.data); }

    bool operator==(const RegressionEffectProxy& other) const
    {
        assert(task == other.task);
        return effect == other.effect;
    }

    bool operator!=(const RegressionEffectProxy& other) const
    {
        return !(*this == other);
    }

    bool is_mutex(const RegressionEffectProxy& other) const
    {
        return task->are_facts_mutex(effect.data, other.effect.data);
    }
};

inline bool
does_fire(const RegressionEffect& effect, const PartialAssignment& assignment)
{
    for (FactPair condition_pair : effect.conditions) {
        if (assignment[condition_pair.var].get_value() != condition_pair.value)
            return false;
    }
    return true;
}
inline bool does_fire(
    const RegressionEffectProxy& effect,
    const PartialAssignment& assignment)
{
    for (FactProxy condition : effect.get_conditions()) {
        FactPair condition_pair = condition.get_pair();
        if (assignment[condition_pair.var].get_value() != condition_pair.value)
            return false;
    }
    return true;
}

class RegressionEffectsProxy : public ConditionsProxy {
    const std::vector<RegressionEffect> effects;

public:
    RegressionEffectsProxy(
        const ClassicalTask& task,
        const std::vector<RegressionEffect>& effects)
        : ConditionsProxy(task)
        , effects(effects)
    {
    }
    ~RegressionEffectsProxy() = default;

    std::size_t size() const override { return effects.size(); }

    //    bool empty() const {
    //        return size() == 0;
    //    }

    FactProxy operator[](std::size_t fact_index) const override
    {
        assert(fact_index < size());
        return FactProxy(*task, effects[fact_index].data);
    }

    RegressionEffect get(std::size_t fact_index) const
    {
        return effects[fact_index];
    }
};

class RegressionOperator {
    friend class RegressionOperatorProxy;
    const int original_index;
    const int cost;
    const std::string name;

    std::vector<RegressionCondition> preconditions;
    std::vector<RegressionEffect> effects;
    std::set<int> original_effect_vars;

public:
    explicit RegressionOperator(OperatorProxy& op);
    ~RegressionOperator() = default;

    bool operator==(const RegressionOperator& other) const
    {
        return preconditions == other.preconditions &&
               effects == other.effects &&
               original_effect_vars == other.original_effect_vars &&
               name == other.name && cost == other.cost;
    }

    bool operator!=(const RegressionOperator& other) const
    {
        return !(*this == other);
    }

    int get_original_index() const { return original_index; }

    int get_cost() const { return cost; }

    const std::string& get_name() const { return name; }

    const std::vector<RegressionCondition>& get_precondition() const
    {
        return preconditions;
    }

    const std::vector<RegressionEffect>& get_effect() const { return effects; }

    const std::set<int> get_original_effect_vars() const
    {
        return original_effect_vars;
    }

    bool achieves_subgoal(const PartialAssignment& assignment) const;
    bool is_applicable(const PartialAssignment& assignment) const;

    PartialAssignment
    get_anonym_predecessor(const PartialAssignment& assignment) const
    {
        std::vector<int> new_values = assignment.get_values();
        for (const RegressionEffect& effect : effects) {
            if (does_fire(effect, assignment)) {
                new_values[effect.data.var] = effect.data.value;
            }
        }

        return PartialAssignment(assignment, std::move(new_values));
    }
};

class RegressionOperatorProxy {
    const ClassicalTask* task;
    const std::shared_ptr<std::vector<RegressionOperator>> ops;
    const size_t index;

public:
    RegressionOperatorProxy(
        const ClassicalTask& task,
        const std::shared_ptr<std::vector<RegressionOperator>>& ops,
        const size_t index)
        : task(&task)
        , ops(ops)
        , index(index)
    {
    }
    ~RegressionOperatorProxy() = default;

    bool operator==(const RegressionOperatorProxy& other) const
    {
        assert(task == other.task);
        return ops.get()[index] == other.ops.get()[index];
    }

    bool operator!=(const RegressionOperatorProxy& other) const
    {
        return !(*this == other);
    }

    RegressionConditionsProxy get_precondition() const
    {
        return RegressionConditionsProxy(*task, (*ops)[index].preconditions);
    }

    RegressionEffectsProxy get_effect() const
    {
        return RegressionEffectsProxy(*task, (*ops)[index].effects);
    }

    std::set<int> get_original_effect_vars() const
    {
        return (*ops)[index].original_effect_vars;
    }

    int get_cost() const { return (*ops)[index].cost; }

    int get_id() const { return (*ops)[index].original_index; }

    std::string get_name() const
    {
        return "Regression" + task->get_operator_name(get_id());
    }

    bool achieves_subgoal(const PartialAssignment& assignment) const
    {
        return (*ops)[index].achieves_subgoal(assignment);
    }

    bool is_applicable(const PartialAssignment& assignment) const
    {
        return (*ops)[index].is_applicable(assignment);
    }

    PartialAssignment
    get_anonym_predecessor(const PartialAssignment& assignment)
    {
        return (*ops)[index].get_anonym_predecessor(assignment);
    }
};

class RegressionOperatorsProxy : public ProxyRange<RegressionOperatorsProxy> {
    const ClassicalTask& task;
    const std::shared_ptr<std::vector<RegressionOperator>> ops;

public:
    RegressionOperatorsProxy(
        const ClassicalTask& task,
        const std::shared_ptr<std::vector<RegressionOperator>> ops)
        : task(task)
        , ops(ops)
    {
    }
    ~RegressionOperatorsProxy() = default;

    std::size_t size() const { return ops->size(); }

    bool empty() const { return size() == 0; }

    RegressionOperatorProxy operator[](std::size_t op_index) const
    {
        assert(op_index < size());
        return RegressionOperatorProxy(task, ops, op_index);
    }
};

class RegressionTaskProxy : public ClassicalTaskProxy {
    const std::shared_ptr<std::vector<RegressionOperator>> operators;

public:
    explicit RegressionTaskProxy(const ClassicalTask& task);
    ~RegressionTaskProxy() = default;

    RegressionOperatorsProxy get_regression_operators() const
    {
        return RegressionOperatorsProxy(
            *static_cast<const ClassicalTask*>(task),
            operators);
    }

    PartialAssignment get_goal_assignment() const
    {
        GoalProxy gp = GoalProxy(*task);
        std::vector<int> values(
            task->get_num_variables(),
            PartialAssignment::UNASSIGNED);

        for (FactProxy goal : gp) {
            int var_id = goal.get_variable().get_id();
            int value = goal.get_value();
            assert(utils::in_bounds(var_id, values));
            values[var_id] = value;
        }
        return PartialAssignment(*task, std::move(values));
    }

    RegressionOperatorProxy
    get_regression_operator(const std::size_t index) const
    {
        assert(index < operators->size());
        return RegressionOperatorProxy(
            *static_cast<const ClassicalTask*>(task),
            operators,
            index);
    }

    RegressionOperatorProxy get_regression_operator(const OperatorID& id) const
    {
        assert(utils::in_bounds(id.get_index(), *operators));
        assert(
            (*operators)[id.get_index()].get_original_index() ==
            id.get_index());
        return RegressionOperatorProxy(
            *static_cast<const ClassicalTask*>(task),
            operators,
            id.get_index());
    }
};

namespace task_properties {
inline bool
is_applicable(RegressionOperatorProxy op, const PartialAssignment& assignment)
{
    for (FactProxy precondition : op.get_precondition()) {
        if (assignment.assigned(precondition.get_variable()) &&
            assignment[precondition.get_variable()] != precondition)
            return false;
    }
    return true;
}
} // namespace task_properties

#endif
