#ifndef DOWNWARD_PER_STATE_INFORMATION_H
#define DOWNWARD_PER_STATE_INFORMATION_H

#include "downward/state_registry.h"

#include "downward/algorithms/segmented_vector.h"
#include "downward/algorithms/subscriber.h"
#include "downward/utils/collections.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>

/**
 * @brief A mapping from (registered) states to some type of information.
 *
 * This class behaves somewhat like a hash map from states to objects of type
 * `Entry`. However, lookup of unknown states is supported and leads to
 * insertion of a default value (similar to the defaultdict class in Python).
 *
 * For example, search algorithms can use it to associate goal distances with
 * each state, in which case `Entry` = `int`.
 *
 * This class may only be used with states registered in a StateRegistry, as it
 * requires each State to be associated with an ID.
 *
 * @tparam Entry - The type of the information associated with the states.
 *
 * @see StateRegistry
 *
 * @ingroup downward
 */
template <class Entry>
class PerStateInformation : public subscriber::Subscriber<StateRegistry> {
    const Entry default_value;
    using EntryVectorMap = std::unordered_map<
        const StateRegistry*,
        std::unique_ptr<segmented_vector::SegmentedVector<Entry>>>;
    EntryVectorMap entries_by_registry;

    mutable const StateRegistry* cached_registry;
    mutable segmented_vector::SegmentedVector<Entry>* cached_entries;

    /*
      Returns the SegmentedVector associated with the given StateRegistry.
      If no vector is associated with this registry yet, an empty one is
      created. Both the registry and the returned vector are cached to speed up
      consecutive calls with the same registry.
    */
    segmented_vector::SegmentedVector<Entry>*
    get_entries(const StateRegistry* registry)
    {
        if (cached_registry != registry) {
            cached_registry = registry;
            auto it = entries_by_registry.find(registry);
            if (it == entries_by_registry.end()) {
                cached_entries = new segmented_vector::SegmentedVector<Entry>();
                entries_by_registry.emplace(registry, cached_entries);
                registry->subscribe(this);
            } else {
                cached_entries = it->second.get();
            }
        }
        assert(
            cached_registry == registry &&
            cached_entries == entries_by_registry[registry].get());
        return cached_entries;
    }

    /*
      Returns the SegmentedVector associated with the given StateRegistry.
      Returns nullptr, if no vector is associated with this registry yet.
      Otherwise, both the registry and the returned vector are cached to speed
      up consecutive calls with the same registry.
    */
    const segmented_vector::SegmentedVector<Entry>*
    get_entries(const StateRegistry* registry) const
    {
        if (cached_registry != registry) {
            const auto it = entries_by_registry.find(registry);
            if (it == entries_by_registry.end()) {
                return nullptr;
            } else {
                cached_registry = registry;
                cached_entries = it->second.get();
            }
        }
        assert(cached_registry == registry);
        return cached_entries;
    }

public:
    /**
     * @brief Create a PerStateInformation object with the given default entry
     * for unknown states.
     */
    explicit PerStateInformation(const Entry& default_value_ = Entry())
        : default_value(default_value_)
        , cached_registry(nullptr)
        , cached_entries(nullptr)
    {
    }

    // May not be copied.
    PerStateInformation(const PerStateInformation<Entry>&) = delete;
    PerStateInformation& operator=(const PerStateInformation<Entry>&) = delete;

    /**
     * @brief Look up the information associated with a State.
     *
     * @warning The State must be registered with a StateRegistry.
     */
    Entry& operator[](const State& state)
    {
        const StateRegistry* registry = state.get_registry();
        if (!registry) {
            std::cerr << "Tried to access per-state information with an "
                      << "unregistered state." << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
        segmented_vector::SegmentedVector<Entry>* entries =
            get_entries(registry);
        int state_id = state.get_id().value;
        assert(state.get_id() != StateID::no_state);
        size_t virtual_size = registry->size();
        assert(utils::in_bounds(state_id, *registry));
        if (entries->size() < virtual_size) {
            entries->resize(virtual_size, default_value);
        }
        return (*entries)[state_id];
    }

    /**
     * @brief Look up the information associated with a State.
     *
     * @warning The state must be registered with a StateRegistry.
     */
    const Entry& operator[](const State& state) const
    {
        const StateRegistry* registry = state.get_registry();
        if (!registry) {
            std::cerr << "Tried to access per-state information with an "
                      << "unregistered state." << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
        const segmented_vector::SegmentedVector<Entry>* entries =
            get_entries(registry);
        if (!entries) {
            return default_value;
        }
        int state_id = state.get_id().value;
        assert(state.get_id() != StateID::no_state);
        assert(utils::in_bounds(state_id, *registry));
        int num_entries = entries->size();
        if (state_id >= num_entries) {
            return default_value;
        }
        return (*entries)[state_id];
    }

    virtual void
    notify_service_destroyed(const StateRegistry* registry) override
    {
        entries_by_registry.erase(registry);
        if (registry == cached_registry) {
            cached_registry = nullptr;
            cached_entries = nullptr;
        }
    }
};

#endif
