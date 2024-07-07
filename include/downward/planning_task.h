#ifndef DOWNWARD_PLANNING_TASK_H
#define DOWNWARD_PLANNING_TASK_H

#include "downward/operator_id.h"

#include "downward/algorithms/subscriber.h"
#include "downward/utils/hash.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Represents a fact \f$(v, d), v \in V, d \in D(v)\f$ as a pair of
 * variable and variable value index.
 *
 * @ingroup planning_tasks
 */
struct FactPair {
    int var;   ///< The variable index.
    int value; ///< The index of the value of the variable.

    /// Creates a fact from a variable and variable value.
    FactPair(int var, int value)
        : var(var)
        , value(value)
    {
    }

    /// Compares two facts lexicographically. First compares the variables of
    /// both facts, then the values.
    friend auto operator<=>(const FactPair&, const FactPair&) = default;

    /// This special object represents "no such fact". E.g., functions that
    /// search a fact can return "no_fact" when no matching fact is found.
    /// Only compares equal to itself.
    static const FactPair no_fact;
};

/// Prints a fact to an output stream.
std::ostream& operator<<(std::ostream& os, const FactPair& fact_pair);

namespace utils {
inline void feed(HashState& hash_state, const FactPair& fact)
{
    feed(hash_state, fact.var);
    feed(hash_state, fact.value);
}
} // namespace utils

/**
 * @brief The PlanningTask class represents a planning task (classical or
 * probabilistic).
 *
 * A PlanningTask identifies each variable \f$v \in V\f$ with a unique index in
 * \f$\{ 0, ..., |V| - 1 \}\f$. This analogously holds for operators, which are
 * represented by indices in \f$\{ 0, ..., |O| - 1\}\f$, individual goal facts
 * which are indices in \f$\{0, ..., |G| - 1\}\f$, and so on. Knowing the index
 * suffices to query all attributes of a component, like an operator's
 * precondition, or the human-readable name of a variable.
 *
 * This class models all common components of classical and probabilistic
 * planning tasks, like variables, the goal, the operator preconditions etc.,
 * but does not model the (stochatic) operator effect(s) and operator costs.
 * These concepts are added by the corresponding sub-classes ClassicalTask and
 * probfd::ProbabilisticTask of PlanningTask.
 *
 * @warning This class should not be used directly to access individual
 * sub-components of a planning task. To this end, use PlanningTaskProxy.
 *
 * @see PlanningTaskProxy
 * @see ClassicalTask
 * @see probfd::ProbabilisticTask
 *
 * @ingroup planning_tasks
 */
class PlanningTask : public subscriber::SubscriberService<PlanningTask> {
public:
    /**
     * @brief Get the number of variables \f$|V|\f$ of this task.
     */
    virtual int get_num_variables() const = 0;

    /**
     * @brief Get the size of the domain \f$D(v)\f$ of the variable
     * \f$v \in \{ 0, ..., |V| - 1 \}\f$.
     */
    virtual int get_variable_domain_size(int var) const = 0;

    /**
     * @brief Get the number of operators \f$|A|\f$ of the task.
     */
    virtual int get_num_operators() const = 0;

    /**
     * @brief Get the number of precondition facts \f$pre_a\f$ of an operator
     * \f$a\f$ with index in \f$\{ 0, ..., |A|-1\}\f$.
     */
    virtual int get_num_operator_precondition_facts(int op_index) const = 0;

    /**
     * @brief Get the precondition fact at index in \f$\{ 0, ...,
     * |pre_a|-1\}\f$ of an operator \f$a\f$ with index in
     * \f$\{ 0, ..., |A|-1\}\f$.
     */
    virtual FactPair
    get_operator_precondition_fact(int op_index, int fact_index) const = 0;

    /**
     * @brief Get the number of goal facts \f$|G|\f$.
     */
    virtual int get_num_goal_facts() const = 0;

    /**
     * @brief Get the goal fact with index in \f$\{ 0, ..., |G|-1\}\f$.
     */
    virtual FactPair get_goal_fact(int fact_index) const = 0;

    /**
     * @brief Get the initial state as a list of initial variable values for
     * each variable.
     *
     * The value at index i holds the initial value of variable i.
     */
    virtual std::vector<int> get_initial_state_values() const = 0;

    /**
     * @brief Get the human-readable name associated with a variable
     * \f$v \in \{ 0, ..., |V| - 1\}\f$
     */
    virtual std::string get_variable_name(int var) const = 0;

    /**
     * @brief Get the human-readable name associated with a fact.
     */
    virtual std::string get_fact_name(const FactPair& fact) const = 0;

    /**
     * @brief Get the human-readable name associated with an operator with
     * index in \f$\{ 0, ..., |A|-1\}\f$.
     */
    virtual std::string get_operator_name(int op_index) const = 0;

    // Internal method.
    virtual bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const = 0;

    // Internal method.
    virtual bool is_undefined(const FactPair& fact) const = 0;
};

/**
 * @brief The ClassicalTask class represents a classical planning task.
 *
 * This class extends PlanningTask by also associating each operator with a
 * single effect, and an integer cost.
 *
 * @warning This class should not be used directly to access individual
 * sub-components of a classical planning task. To this end, use
 * ClassicalTaskProxy.
 *
 * @note Integer costs are used to avoid floating-point imprecision.
 *
 * @see ClassicalTaskProxy
 *
 * @ingroup planning_tasks
 */
class ClassicalTask : public PlanningTask {
public:
    ClassicalTask() = default;
    virtual ~ClassicalTask() override = default;

    /**
     * @brief Get the cost \f$c(a)\f$ of the operator \f$a\f$ with index
     * in \f$\{ 0, ..., |A|-1\}\f$.
     */
    virtual int get_operator_cost(int op_index) const = 0;

    /**
     * @brief Get the number of effect facts \f$|\mathit{eff}_a|\f$ of the
     * operator \f$a\f$ with index in \f$\{ 0, ..., |A|-1\}\f$.
     */
    virtual int get_num_operator_effect_facts(int op_index) const = 0;

    /**
     * @brief Get an effect fact at index in \f$\{ 0, ...,
     * |\mathit{eff}_a|-1\}\f$ of the operator \f$a\f$ with index in \f$\{
     * 0, ..., |A|-1\}\f$.
     */
    virtual FactPair
    get_operator_effect_fact(int op_index, int fact_index) const = 0;
};

#endif // DOWNWARD_PLANNING_TASK_H
