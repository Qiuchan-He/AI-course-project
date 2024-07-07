#ifndef PROBFD_TASK_PROXY_H
#define PROBFD_TASK_PROXY_H

#include "probfd/probabilistic_task.h"

#include "downward/task_proxy.h"

#include <cstddef>
#include <string>

namespace probfd {

class ProbabilisticEffectProxy;
class ProbabilisticOperatorProxy;
class ProbabilisticOperatorsProxy;
class ProbabilisticOutcomeProxy;
class ProbabilisticOutcomesProxy;
class ProbabilisticTaskProxy;

/**
 * \addtogroup prob_proxy
 *  @{
 */

/**
 * @brief The ProbabilisticEffectProxy class represents a stochastic effect
 * \f$\mathit{eff^i}_a\f$ with index \f$i\f$ of an operator \f$a \in A\f$ of a
 * ProbabilisticTask.
 *
 * This class can be used like a range of effect facts (FactProxy). Example:
 * ```
 * ProbabilisticOutcomeProxy outcome = ...;
 *
 * for (FactProxy& effect_fact : outcome.get_effect()) {
 *     // Do something with the effect fact...
 * }
 * ```
 */
class ProbabilisticEffectProxy : public ProxyRange<ProbabilisticEffectProxy> {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;

public:
    ProbabilisticEffectProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index);

    /// Returns the number of effect facts.
    std::size_t size() const;

    /// Get an effect fact by its index.
    FactProxy operator[](std::size_t eff_index) const;
};

/**
 * @brief The ProbabilisticOutcomeProxy class represents a single stochastic
 * outcome of an operator \f$a \in A\f$ of a ProbabilisticTask.
 */
class ProbabilisticOutcomeProxy {
    const ProbabilisticTask* task;
    int op_index;
    int outcome_index;

public:
    ProbabilisticOutcomeProxy(
        const ProbabilisticTask& task,
        int op_index,
        int outcome_index);

    /// Get the probabilistic effect of this outcome.
    ProbabilisticEffectProxy get_effect() const;

    /// Get the probability of this outcome.
    double get_probability() const;
};

/**
 * @brief The ProbabilisticOutcomesProxy class represents the set of stochastic
 * outcomes of an operator \f$a \in A\f$ of a ProbabilisticTask.
 *
 * This class can be used like a range of stochastic outcomes
 * (ProbabilisticOutcomeProxy). Example:
 * ```
 * ProbabilisticOperatorProxy op = ...;
 *
 * for (ProbabilisticOutcomeProxy& outcome : op.get_outcomes()) {
 *     // Do something with the outcome...
 * }
 * ```
 */
class ProbabilisticOutcomesProxy
    : public ProxyRange<ProbabilisticOutcomesProxy> {
    const ProbabilisticTask* task;
    int op_index;

public:
    ProbabilisticOutcomesProxy(const ProbabilisticTask& task, int op_index);

    /// Returns the number of outcomes of the probabilistic operator.
    std::size_t size() const;

    /// Get a specific outcome by its index.
    ProbabilisticOutcomeProxy operator[](std::size_t eff_index) const;
};

/**
 * @brief The ProbabilisticOperatorProxy class represents an operator \f$a
 * \in A\f$ of a ProbabilisticTask.
 */
class ProbabilisticOperatorProxy {
    const ProbabilisticTask* task;
    int index;

public:
    ProbabilisticOperatorProxy(const ProbabilisticTask& task, int index);

    /// Get the precondition \f$pre_a\f$ of the operator.
    PreconditionProxy get_precondition() const;

    /// Get the outcomes of the operator.
    ProbabilisticOutcomesProxy get_outcomes() const;

    /// Get the cost \f$c(a)\f$ of the operator.
    double get_cost() const;

    /// Get the name of the operator.
    std::string get_name() const;

    /// Get the operator's index in the set \f$\{ 0, ..., |A| - 1 \}\f$.
    int get_id() const;

    /// Checks if two proxies reference the same probabilistic operator.
    friend bool operator==(
        const ProbabilisticOperatorProxy&,
        const ProbabilisticOperatorProxy&);
};

/**
 * @brief The ProbabilisticOperatorsProxy class represents the set of operators
 * \f$A\f$ of a ProbabilisticTask.
 *
 * This class can be used like a range of probabilistic operators
 * (ProbabilisticOperatorProxy). Example:
 * ```
 * ProbabilisticTaskProxy proxy = ...;
 *
 * for (ProbabilisticOperator& op : proxy.get_operators()) {
 *     // Do something with the operator...
 * }
 * ```
 */
class ProbabilisticOperatorsProxy
    : public ProxyRange<ProbabilisticOperatorsProxy> {
    const ProbabilisticTask* task;

public:
    /// Construct a proxy object representing this task's probabilistic
    /// operators.
    explicit ProbabilisticOperatorsProxy(const ProbabilisticTask& task);

    /// Get the number of probabilistic operators \f$|A|\f$.
    std::size_t size() const;

    /// Get an operator by its index in \f$\{ 0, ..., |A| - 1\}\f$
    ProbabilisticOperatorProxy operator[](std::size_t index) const;

    /// Get an operator by OperatorID, representing an index in
    /// \f$\{ 0, ..., |A| - 1\}\f$
    ProbabilisticOperatorProxy operator[](OperatorID id) const;
};

/**
 * @brief The ProbabilisticTaskProxy class is used to traverse the components of
 * a ProbabilisticTask.
 *
 * Usage example:
 * ```
 * bool is_really_stochastic(ProbabilisticTask& task) {
 *     // Create a proxy to traverse the components
 *     ProbabilisticTaskProxy task_proxy(task);
 *
 *     // Iterate over all operators
 *     for (OperatorProxy op : task_proxy.get_operators()) {
 *         // Access the outcomes of the operator
 *         ProbabilisticOutcomesProxy outs = op.get_outcomes();
 *         if (outs.size() > 1) return true;
 *     }
 *
 *     return false;
 * }
 * ```
 */
class ProbabilisticTaskProxy : public PlanningTaskProxy {
public:
    explicit ProbabilisticTaskProxy(const ProbabilisticTask& task);

    /// Get the set of operators \f$O\f$ of the probabilistic planning task.
    ProbabilisticOperatorsProxy get_operators() const;
};

/** @}*/

} // namespace probfd

#endif
