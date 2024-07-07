#ifndef DOWNWARD_TASK_PROXY_H
#define DOWNWARD_TASK_PROXY_H

#include "downward/operator_id.h"
#include "downward/planning_task.h"
#include "downward/state_id.h"
#include "downward/task_id.h"


#include "downward/algorithms/int_packer.h"
#include "downward/utils/collections.h"
#include "downward/utils/hash.h"
#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/system.h"

#include <cassert>
#include <compare>
#include <cstddef>
#include <iterator>
#include <string>
#include <variant>
#include <vector>

class EffectProxy;
class FactProxy;
class FactsProxy;
class GoalProxy;
class OperatorProxy;
class OperatorsProxy;
class AbstractOperatorsProxy;
class PreconditionProxy;
class StateRegistry;
class PlanningTaskProxy;
class ClassicalTaskProxy;
class VariableProxy;
class VariablesProxy;

class PartialAssignment;
class State;

using PackedStateBin = int_packer::IntPacker::Bin;

/*
  The task interface is divided into two parts: a set of proxy classes
  for accessing task information (ClassicalTaskProxy, OperatorProxy, etc.) and
  task implementations (subclasses of ClassicalTask). Each proxy class
  knows which ClassicalTask it belongs to and uses its methods to retrieve
  information about the task.

  Example code for creating a new task proxy object and accessing its operators:

      std::shared_ptr<ClassicalTask> task = ...;

      // Create proxy to access information
      ClassicalTaskProxy task_proxy(*task);
      for (OperatorProxy op : task_proxy.get_operators())
          std::cout << op.get_name() << endl;

  Since proxy classes only store a reference to the ClassicalTask and
  some indices, they can be copied cheaply.

  For helper functions that work on task related objects, see task_properties.h.
*/

/**
 * \addtogroup proxy
 *  @{
 */

/*
 * Base class for ranges over task object proxies.
 *
 * Inheriting classes can be used in a range-based for loop to iterate over the
 * task object proxies held by the class.
 */
template <typename T>
class ProxyRange {
public:
    class ProxyIterator {
        /* We store a pointer to collection instead of a reference
           because iterators have to be copy assignable. */
        const T* collection;
        std::size_t pos;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = decltype(std::declval<const T>().operator[](0ULL));
        using difference_type = int;
        using pointer = const value_type*;
        using reference = value_type;

        ProxyIterator(const T& collection, std::size_t pos)
            : collection(&collection)
            , pos(pos)
        {
        }

        reference operator*() const { return (*collection)[pos]; }

        value_type operator++(int)
        {
            value_type value(**this);
            ++(*this);
            return value;
        }

        ProxyIterator& operator++()
        {
            ++pos;
            return *this;
        }

        friend bool
        operator==(const ProxyIterator& left, const ProxyIterator& right)
        {
            assert(left.collection == right.collection);
            return left.pos == right.pos;
        }
    };

    /// Checks if the collection is empty.
    bool empty() const { return static_cast<T&>(*this).size() == 0; }

    /// Returns an iterator to the beginning of the collection.
    auto begin() const
    {
        return ProxyIterator(static_cast<const T&>(*this), 0);
    }

    /// Returns an iterator to the beginning of the collection.
    auto end() const
    {
        return ProxyIterator(
            static_cast<const T&>(*this),
            static_cast<const T&>(*this).size());
    }
};

/**
 * @brief The FactProxy class represents a single fact \f$f \in \mathcal{F}\f$
 * of a PlanningTask.
 */
class FactProxy {
    const PlanningTask* task;
    FactPair fact;

public:
    /// Construct the proxy for a given task and a fact of this task given by
    /// variable index and domain value.
    FactProxy(const PlanningTask& task, int var_id, int value);

    /// Construct the proxy for a given task and a fact of this task given by
    /// a FactPair.
    FactProxy(const PlanningTask& task, const FactPair& fact);

    /// Get a proxy to the variable of this fact.
    VariableProxy get_variable() const;

    /// Get the value for the variable of this fact.
    int get_value() const;

    /// Get the FactPair of this fact.
    FactPair get_pair() const;

    /// Get the name of this fact.
    std::string get_name() const;

    // Check if this fact is mutually exclusive with another fact.
    bool is_mutex(const FactProxy& other) const;

    /// Checks if two proxies refer to the same fact.
    friend bool operator==(const FactProxy& left, const FactProxy& right);
};

/**
 * @brief The FactsProxy class represents the set of facts
 * \f$\mathcal{F} = \{ (v, d) \mid v \in V, d \in D(v) \}\f$ of a PlanningTask.
 *
 * This class can be used like a range of facts (FactProxy). Example:
 * ```
 * FactsProxy all_facts(task);
 *
 * for (FactProxy& fact : all_facts) {
 *     // Do something with the fact...
 * }
 * ```
 *
 * The facts are ordered lexicographically, primarily by increasing variable ID,
 * and secondarily in order of increasing variable value.
 */
class FactsProxy {
    const PlanningTask* task;
    mutable int num_facts = -1; // Cache the result

public:
    class FactsProxyIterator {
        const PlanningTask* task;
        int var_id;
        int value;

        friend class FactsProxy;

        FactsProxyIterator(const PlanningTask& task, int var_id, int value);

    public:
        FactProxy operator*() const;

        FactsProxyIterator& operator++();

        friend bool operator==(
            const FactsProxyIterator& left,
            const FactsProxyIterator& right);
    };

    /// Construct a proxy for the facts of this task.
    explicit FactsProxy(const PlanningTask& task);

    /// Returns an input iterator to the beginning of the fact range.
    FactsProxyIterator begin() const;

    /// Returns an input iterator to the end of the fact range.
    FactsProxyIterator end() const;

    /// Returns the number of facts.
    int size() const;
};

/**
 * @brief The VariableProxy class represents a single variable \f$v \in V\f$
 * of a PlanningTask.
 */
class VariableProxy {
    const PlanningTask* task;
    int id;

public:
    /// Constructs a variable proxy from the given task and a variable.
    VariableProxy(const PlanningTask& task, int variable_id);

    /// Get the ID of the variable, i.e. its index in
    /// \f$\{0, ..., |V| - 1]\}\f$.
    int get_id() const;

    /// Get the human-readable name associated with the variable.
    std::string get_name() const;

    /// Get the size \f$|D(v)|\f$ of this variable's domain.
    int get_domain_size() const;

    /// Get a fact for a domain value in \f$\{0, ..., |D(v)| - 1\}\f$ for this
    /// variable.
    FactProxy get_fact(int value) const;

    // Checks if two variables proxies denote the same variable.
    friend bool
    operator==(const VariableProxy& left, const VariableProxy& right);
};

/**
 * @brief The VariablesProxy class represents the set of variables \f$V\f$
 * of a PlanningTask.
 *
 * This class can be used like a range of variables (VariableProxy):
 * ```
 * VariablesProxy variables = task_proxy.get_variables();
 *
 * for (VariableProxy& var : variables) {
 *     // Do something with the variable...
 * }
 * ```
 */
class VariablesProxy : public ProxyRange<VariablesProxy> {
    const PlanningTask* task;

public:
    /// Constructs the proxy object for the variables of this task.
    explicit VariablesProxy(const PlanningTask& task);

    /// Get the number of variables \f$|V|\f$ of the task.
    std::size_t size() const;

    /// Get a variable by its index in \f$\{ 0, ..., |V|-1\}\f$.
    VariableProxy operator[](std::size_t index) const;

    /// Get the set of all facts of the task.
    FactsProxy get_facts() const;
};

/**
 * @brief The PreconditionProxy class represents the precondition
 * \f$\mathit{pre}_a\f$ of an operator \f$a \in A\f$ of a PlanningTask.
 *
 * This class can be used like a range of precondition facts (FactProxy):
 * ```
 * OperatorProxy op = ...;
 *
 * for (FactProxy& effect_fact : op.get_precondition()) {
 *     // Do something with the precondition fact...
 * }
 * ```
 */
class PreconditionProxy : public ProxyRange<PreconditionProxy> {
    const PlanningTask* task;
    int op_index;

public:
    /// Constructs the proxy object for the given task and operator given by
    /// index.
    PreconditionProxy(const PlanningTask& task, int op_index);

    /// Constructs the proxy object for the given task and operator given by
    /// index.
    PreconditionProxy(const PlanningTask& task, OperatorID op_index);

    /// Get the number of precondition facts \f$|\mathit{pre}_a|\f$ of the
    /// operator.
    std::size_t size() const;

    /// Get a precondition fact by its index in
    /// \f$\{0, ..., |\mathit{pre}_a|\}\f$
    FactProxy operator[](std::size_t fact_index) const;
};

/**
 * @brief The EffectProxy class represents the effect \f$\mathit{eff}_a\f$ of
 * an operator \f$a \in A\f$ of a ClassicalTask.
 *
 * This class can be used like a range of effect facts (FactProxy). Example:
 * ```
 * OperatorProxy op = ...;
 *
 * for (FactProxy& effect_fact : op.get_effect()) {
 *     // Do something with the effect fact...
 * }
 * ```
 */
class EffectProxy : public ProxyRange<EffectProxy> {
    const ClassicalTask* task;
    int op_index;

public:
    /// Constructs the proxy object for the given task and task operator index.
    EffectProxy(const ClassicalTask& task, int op_index);

    /// Constructs the proxy object for the given task and task operator index.
    EffectProxy(const ClassicalTask& task, OperatorID op_index);

    /// Get the number of effect facts \f$\mathit{eff}_a\f$ of the operator.
    std::size_t size() const;

    /// Get an effect fact by its index in \f$\{0, ..., |\mathit{eff}_a|\}\f$.
    FactProxy operator[](std::size_t eff_index) const;
};

/**
 * @brief The OperatorProxy class represents an operator \f$a \in A\f$
 * of a ClassicalTask.
 */
class OperatorProxy {
    const ClassicalTask* task;
    int index;

public:
    /// Constructs the proxy object for the given task and task operator index.
    OperatorProxy(const ClassicalTask& task, int index);

    /// Constructs the proxy object for the given task and task operator index.
    OperatorProxy(const ClassicalTask& task, OperatorID index);

    /// Get the precondition \f$pre_a\f$ of the operator.
    PreconditionProxy get_precondition() const;

    /// Get the effect \f$\mathit{eff}_a\f$ of the operator.
    EffectProxy get_effect() const;

    /// Get the cost \f$c(a)\f$ of the operator.
    int get_cost() const;

    /// Get the name of the operator.
    std::string get_name() const;

    /// Get the operator's index in the set \f$\{ 0, ..., |A| - 1 \}\f$.
    int get_id() const;

    /// Checks if two proxies represent the same operator.
    friend bool
    operator==(const OperatorProxy& left, const OperatorProxy& right);
};

/**
 * @brief The OperatorsProxy class represents the set of operators \f$A\f$ of
 * a ClassicalTask.
 *
 * This class can be used like a range of operators (OperatorProxy):
 * ```
 * OperatorsProxy operators = task_proxy.get_operators();
 *
 * for (OperatorProxy& op : operators) {
 *     // Do something with the operator...
 * }
 * ```
 * The operators are not ordered in any particular way, but the order always
 * remains the same.
 */
class OperatorsProxy : public ProxyRange<OperatorsProxy> {
    const ClassicalTask* task;

public:
    /// Construct a proxy object representing this task's operators.
    explicit OperatorsProxy(const ClassicalTask& task);

    /// Get the number of operators \f$|A|\f$.
    std::size_t size() const;

    /// Get an operator by its index in \f$\{ 0, ..., |A| - 1\}\f$
    OperatorProxy operator[](std::size_t index) const;

    /// Get an operator by OperatorID, representing an index in
    /// \f$\{ 0, ..., |A| - 1\}\f$
    OperatorProxy operator[](OperatorID id) const;
};

/**
 * @brief The GoalProxy class represents the goal \f$G\f$ of a PlanningTask.
 *
 * This class can be used like a range of goal facts (FactProxy). Example:
 * ```
 * GoalProxy goal = task_proxy.get_goal();
 *
 * for (FactProxy& goal_fact : goal) {
 *     // Do something with the goal fact...
 * }
 * ```
 */
class GoalProxy : public ProxyRange<GoalProxy> {
    const PlanningTask* task;

public:
    /// Construct a proxy object representing this task's goal.
    explicit GoalProxy(const PlanningTask& task);

    /// Get the number of goal facts \f$|G|\f$.
    std::size_t size() const;

    /// Get a goal fact by its index in \f$\{ 0, ..., |G| - 1 \}\f$.
    FactProxy operator[](std::size_t index) const;
};

/**
 * @brief The AbstractOperatorProxy class represents an operator \f$a \in
 * A\f$ of a PlanningTask.
 *
 * @note This class only provides access to attributes of an operator which
 * are shared between classical and probabilistic planning tasks. It does not
 * provide access to the (stochastic) effect(s) and costs of an operator, since
 * these concepts differ between classical and probabilistic planning task
 * operators and may not exist.
 */
class AbstractOperatorProxy {
    const PlanningTask* task;
    int index;

public:
    /// Constructs the proxy object for the given task and operator index.
    AbstractOperatorProxy(const PlanningTask& task, int index);

    /// Constructs the proxy object for the given task and operator index.
    AbstractOperatorProxy(const PlanningTask& task, OperatorID index);

    /// Get the precondition \f$pre_a\f$ of the operator.
    PreconditionProxy get_precondition() const;

    /// Get the human-readable name associated with the operator.
    std::string get_name() const;

    /// Get the operator's index in the set \f$\{ 0, ..., |A| - 1 \}\f$.
    int get_id() const;

    /// Checks if two proxies represent the same operator.
    friend bool operator==(
        const AbstractOperatorProxy& left,
        const AbstractOperatorProxy& right);
};

/**
 * @brief The AbstractOperatorProxy class represents the set of operators
 * \f$A\f$ of a PlanningTask.
 */
class AbstractOperatorsProxy : public ProxyRange<AbstractOperatorsProxy> {
    const PlanningTask* task;

public:
    explicit AbstractOperatorsProxy(const PlanningTask& task);

    std::size_t size() const;

    AbstractOperatorProxy operator[](std::size_t index) const;
    AbstractOperatorProxy operator[](OperatorID id) const;
};

/**
 * @brief The PlanningTaskProxy class is used to access the components of a
 * PlanningTask.
 */
class PlanningTaskProxy {
protected:
    const PlanningTask* task;

    friend StateRegistry;

    // This method is meant to be called only by the state registry.
    State create_state(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer) const;

    // This method is meant to be called only by the state registry.
    State create_state(
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer,
        std::vector<int>&& state_values) const;

public:
    /// Constructs the proxy object so that it can be used to access
    /// sub-components of the given input task.
    explicit PlanningTaskProxy(const PlanningTask& task);

    /// Get the set of variables \f$V\f$ of the planning task.
    VariablesProxy get_variables() const;

    // Get the set of abstract operators \f$O\f$ of the planning task.
    AbstractOperatorsProxy get_abstract_operators() const;

    /// Creates the initial state \f$I\f$ of the planinng task.
    State get_initial_state() const;

    /// Get the goal \f$G\f$ of the planning task.
    GoalProxy get_goal() const;

    /// Creates a state of this planning task with the specified values.
    State create_state(std::vector<int>&& state_values) const;

    // Casts this proxy object to a ClassicalTaskProxy object by casting the
    // planning task it refers to to ClassicalTask. Invoking such a cast
    // results in undefined behaviour if the underlying task is not a
    // ClassicalTask!
    explicit operator ClassicalTaskProxy() const;

    // Internal method.
    std::pair<bool, State> convert_to_full_state(
        PartialAssignment& assignment,
        bool check_mutexes,
        utils::RandomNumberGenerator& rng) const;

    // Internal method.
    void subscribe_to_task_destruction(
        subscriber::Subscriber<PlanningTask>* subscriber) const;

    // Internal method.
    TaskID get_id() const;
};

/**
 * @brief The ClassicalTaskProxy class is used to traverse the components of a
 * ClassicalTask.
 *
 * Usage example:
 * ```
 * bool has_zero_cost_operator(ClassicalTask& task) {
 *     // Create a proxy to traverse the components
 *     ClassicalTaskProxy task_proxy(task);
 *
 *     // Iterate over all operators
 *     for (OperatorProxy op : task_proxy.get_operators()) {
 *         // Access the cost of the operator
 *         int cost = op.get_cost();
 *         if (cost == 0) return true;
 *     }
 *
 *     return false;
 * }
 * ```
 */
class ClassicalTaskProxy : public PlanningTaskProxy {
public:
    /// Constructs the proxy object so that it can be used to access
    /// sub-components of the given input planning task.
    explicit ClassicalTaskProxy(const ClassicalTask& task);

    // Get the planning task referenced by the proxy object.
    const ClassicalTask* get_task() const;

    /// Get the set of operators \f$O\f$ of the classical planning task.
    OperatorsProxy get_operators() const;
};

/** @}*/

#endif // DOWNWARD_TASK_PROXY_H
