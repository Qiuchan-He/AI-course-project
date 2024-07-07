#ifndef DOWNWARD_STATE_ID_H
#define DOWNWARD_STATE_ID_H

#include <compare>
#include <iostream>

/**
 * @brief The StateID class identifies states within a StateRegistry.
 *
 * If the registry is known, the ID is sufficient to look up the state, which
 * is why IDs are intended for long term storage (e.g. in open lists).
 * Internally, a StateID is just an integer.
 *
 * @see StateRegistry
 *
 * @ingroup downward
 */
class StateID {
    friend class StateRegistry;
    friend std::ostream &operator<<(std::ostream &os, StateID id);
    template <typename>
    friend class PerStateInformation;

    int value;
    explicit StateID(int value_)
        : value(value_) {
    }

public:
    static const StateID no_state;

    friend auto operator<=>(const StateID&, const StateID&) = default;
};


#endif // DOWNWARD_STATE_ID_H
