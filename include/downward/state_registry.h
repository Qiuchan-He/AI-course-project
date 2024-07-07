#ifndef DOWNWARD_STATE_REGISTRY_H
#define DOWNWARD_STATE_REGISTRY_H

#include "downward/planning_task.h"
#include "downward/state.h"
#include "downward/state_id.h"

#include "downward/algorithms/int_hash_set.h"
#include "downward/algorithms/int_packer.h"
#include "downward/algorithms/segmented_vector.h"
#include "downward/algorithms/subscriber.h"
#include "downward/utils/hash.h"

#include <ranges>
#include <set>

using PackedStateBin = int_packer::IntPacker::Bin;

/**
 * @brief The StateRegistry class handles a collection of states identified with
 * with consecutive integer IDs.
 *
 * Each state is identified by a unique ID. Duplicate checking is performed so
 * that generating the same state twice results in the same ID.
 *
 * States and StateIDs from different state registries must not be mixed.
 * Registered states, unlike unregistered ones, may be used with
 * PerStateInformation.
 *
 * @see PerStateInformation
 *
 * @ingroup downward
 */
class StateRegistry : public subscriber::SubscriberService<StateRegistry> {
    friend class State;

    struct StateIDSemanticHash {
        const segmented_vector::SegmentedArrayVector<PackedStateBin>&
            state_data_pool;
        int state_size;
        StateIDSemanticHash(
            const segmented_vector::SegmentedArrayVector<PackedStateBin>&
                state_data_pool,
            int state_size)
            : state_data_pool(state_data_pool)
            , state_size(state_size)
        {
        }

        int_hash_set::HashType operator()(int id) const
        {
            const PackedStateBin* data = state_data_pool[id];
            utils::HashState hash_state;
            for (int i = 0; i < state_size; ++i) {
                hash_state.feed(data[i]);
            }
            return hash_state.get_hash32();
        }
    };

    struct StateIDSemanticEqual {
        const segmented_vector::SegmentedArrayVector<PackedStateBin>&
            state_data_pool;
        int state_size;
        StateIDSemanticEqual(
            const segmented_vector::SegmentedArrayVector<PackedStateBin>&
                state_data_pool,
            int state_size)
            : state_data_pool(state_data_pool)
            , state_size(state_size)
        {
        }

        bool operator()(int lhs, int rhs) const
        {
            const PackedStateBin* lhs_data = state_data_pool[lhs];
            const PackedStateBin* rhs_data = state_data_pool[rhs];
            return std::equal(lhs_data, lhs_data + state_size, rhs_data);
        }
    };

    /*
      Hash set of StateIDs used to detect states that are already registered in
      this registry and find their IDs. States are compared/hashed semantically,
      i.e. the actual state data is compared, not the memory location.
    */
    using StateIDSet =
        int_hash_set::IntHashSet<StateIDSemanticHash, StateIDSemanticEqual>;

    PlanningTaskProxy task_proxy;
    const int_packer::IntPacker& state_packer;
    const int num_variables;

    segmented_vector::SegmentedArrayVector<PackedStateBin> state_data_pool;
    StateIDSet registered_states;

    std::unique_ptr<State> cached_initial_state;

    StateID insert_id_or_pop_state();
    int get_bins_per_state() const;

    const int_packer::IntPacker& get_state_packer() const
    {
        return state_packer;
    }

    class const_iterator {
        /*
          We intentionally omit parts of the forward iterator concept
          (e.g. default construction, copy assignment, post-increment)
          to reduce boilerplate. Supported compilers may complain about
          this, in which case we will add the missing methods.
        */

        friend class StateRegistry;
#ifndef NDEBUG
        const StateRegistry* registry;
#endif
        StateID pos;

        const_iterator(const StateRegistry& registry, size_t start)
#ifndef NDEBUG
            : registry(&registry)
            , pos(start)
#else
            : pos(start)
#endif
        {
#ifdef NDEBUG
            (void)registry;
#endif
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = StateID;
        using pointer = const StateID*;
        using reference = const StateID&;
        using difference_type = void;

        const_iterator& operator++()
        {
            ++pos.value;
            return *this;
        }

        friend bool
        operator==(const const_iterator& lhs, const const_iterator& rhs)
        {
            assert(&lhs.registry == &rhs.registry);
            return lhs.pos == rhs.pos;
        }

        StateID operator*() { return pos; }

        StateID* operator->() { return &pos; }
    };

public:
    /// Create a state registry for a planning task.
    explicit StateRegistry(const PlanningTaskProxy& task_proxy);

    // Get the task for which this state registry was created.
    const PlanningTaskProxy& get_task_proxy() const { return task_proxy; }

    // Get the number of variables of the planning task for which this state
    // registry was created.
    int get_num_variables() const { return num_variables; }

    /**
     * @brief Returns the state that was registered with the given ID.
     *
     * @warning The ID must refer to a state in this registry. Do not mix IDs
     * from different registries.
     */
    State lookup_state(StateID id) const;

    /**
     * @brief Returns a reference to the initial state and registers it if this
     * was not done before.
     */
    const State& get_initial_state();

    /// Inserts a state with the given values in the registry and returns it.
    State insert_state(std::vector<int>&& state);

    /**
     * @brief Create a registered successor state by applying the given effect
     * to a state.
     *
     * @see StateRegistry::get_successor
     */
    template <typename Effect>
    State
    get_successor_state(const State& predecessor, const Effect& effect_facts)
    {
        state_data_pool.push_back(predecessor.get_buffer());
        PackedStateBin* buffer = state_data_pool[state_data_pool.size() - 1];

        for (FactProxy effect_fact : effect_facts) {
            FactPair effect_pair = effect_fact.get_pair();
            state_packer.set(buffer, effect_pair.var, effect_pair.value);
        }
        ::StateID id = insert_id_or_pop_state();
        return task_proxy.create_state(*this, id, state_data_pool[id.value]);
    }

    /// Returns the number of distinct states registered so far.
    size_t size() const { return registered_states.size(); }

    // Returns the size of a state in memory in bytes.
    int get_state_size_in_bytes() const;

    // Print the number of registered states and some internal statistics.
    void print_statistics(utils::LogProxy& log) const;

    const_iterator begin() const { return const_iterator(*this, 0); }

    const_iterator end() const { return const_iterator(*this, size()); }
};

#endif // DOWNWARD_STATE_REGISTRY_H
