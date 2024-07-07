#ifndef DOWNWARD_STATE_H
#define DOWNWARD_STATE_H

#include "downward/task_proxy.h"

template <class Entry>
class PerStateInformation;
class SearchSpace;

namespace sampling_technique {
class TechniqueIForwardNone;
}

// PartialAssignment is an internal class that should not be used directly.
class PartialAssignment : public ProxyRange<PartialAssignment> {
    friend class State;

protected:
    const PlanningTask* task;
    // if values[i] == -1 => not assigned value.
    mutable std::shared_ptr<std::vector<int>> values;
    explicit PartialAssignment(const PlanningTask& task);

public:
    static constexpr int UNASSIGNED = -1;

    PartialAssignment(const PlanningTask& task, std::vector<int>&& values);
    PartialAssignment(
        const PartialAssignment& assignment,
        std::vector<int>&& values);

    virtual ~PartialAssignment() = default;

    bool assigned(std::size_t var_id) const;
    bool assigned(const VariableProxy& var) const;

    virtual std::size_t size() const;

    std::size_t assigned_size() const;

    virtual FactProxy operator[](std::size_t var_id) const;
    virtual FactProxy operator[](VariableProxy var) const;

    PlanningTaskProxy get_task() const;

    virtual const std::vector<int>& get_unpacked_values() const;
    const std::vector<int>& get_values() const;
    std::vector<FactPair> get_assigned_facts() const;

    PartialAssignment get_partial_successor(OperatorProxy op) const;
    bool violates_mutexes() const;
    std::pair<bool, State>
    get_full_state(bool check_mutexes, utils::RandomNumberGenerator& rng) const;

    friend bool operator==(const PartialAssignment&, const PartialAssignment&);
};

/**
 * @brief Represents a state of a planning task, i.e. a complete variable
 * assignment.
 *
 * The State class contains the individual variable values \f$s(v), v \in V\f$
 * of a state \f$s\f$.
 * Variable values of a state can be accessed individually using operator[].
 * Alternatively, all variable values can be returned as a list.
 *
 * States can be created directly by specifying the complete variable assignment
 * as a list of values. Alternatively, they can be created as successor states
 * of other states as the result of applying an operator's effect.
 *
 * If states need to be associated with their goal distances or other
 * information, states can be registered within a StateRegistry, which gives
 * each State a unique ID. The class PerStateInformation can be used with
 * registered states to associate arbitrary information with them.
 *
 * @see StateRegistry
 * @see PerStateInformation
 *
 * @ingroup planning_tasks
 */
class State final : private PartialAssignment {
    const StateRegistry* registry;
    StateID id;
    const PackedStateBin* buffer;
    const int_packer::IntPacker* state_packer;
    int num_variables;

    template <class Entry>
    friend class PerStateInformation;

    friend PlanningTaskProxy;
    friend StateRegistry;
    friend SearchSpace;
    friend sampling_technique::TechniqueIForwardNone;

    // Construct a registered state with only packed data.
    State(
        const PlanningTask& task,
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer);

    // Construct a registered state with packed and unpacked data.
    State(
        const PlanningTask& task,
        const StateRegistry& registry,
        StateID id,
        const PackedStateBin* buffer,
        std::vector<int>&& values);

    // Access the packed values. Accessing packed values on states that do
    // not have them (unregistered states) is an error.
    const PackedStateBin* get_buffer() const;

    // Generate unpacked data if it is not available yet. Calling the function
    // on a state that already has unpacked data has no effect.
    void unpack() const;

    // Return a pointer to the registry in which this state is registered.
    // If the state is not registered, returns nullptr.
    const StateRegistry* get_registry() const;

public:
    // These functions of the privately inherited base classes are explicitly
    // made accessible.
    using PartialAssignment::begin;
    using PartialAssignment::end;
    using PartialAssignment::get_task;
    using PartialAssignment::get_values;

    /**
     * @brief Construct a state of a planning task that represents the given
     * complete variable assignment.
     *
     * The entry `values[i]` of the input vector corresponds to the value
     * assigned to the variable with index `i`. The list must have the same size
     * as the number of variables of the given planning task.
     */
    State(const PlanningTask& task, std::vector<int> values);

    ~State() final override = default; // mark final

    /// Get the number of variables of this state.
    std::size_t size() const final override;

    /// Get the fact for a variable in this state, given by its index.
    FactProxy operator[](std::size_t var_id) const final override;

    /// Get the fact for a variable in this state.
    FactProxy operator[](VariableProxy var) const final override;

    /**
     * @brief Access the state's variable assignment as a vector.
     *
     * The entry `values[i]` of the vector corresponds to the value assigned
     * to the variable with index `i`.
     */
    const std::vector<int>& get_unpacked_values() const final override;

    /// Return the ID of the state within its associated registry. If the state
    /// is not registered in a registry, return StateID::no_state.
    StateID get_id() const;

    /**
     * @brief Create a successor state by applying the given effect to a
     * state.
     *
     * @see StateRegistry::get_successor
     */
    template <typename Effect>
    State get_unregistered_successor(const Effect& effect_facts) const
    {
        assert(values);
        std::vector<int> new_values = get_unpacked_values();

        for (const auto effect : effect_facts) {
            FactPair effect_fact = effect.get_pair();
            new_values[effect_fact.var] = effect_fact.value;
        }

        return State(*task, std::move(new_values));
    }

    /// Checks if two states are equal.
    friend bool operator==(const State& left, const State& right);
};

namespace utils {
// Internal methods.
void feed(HashState& hash_state, const PartialAssignment& assignment);
void feed(HashState& hash_state, const State& state);
} // namespace utils

#endif // DOWNWARD_STATE_H
