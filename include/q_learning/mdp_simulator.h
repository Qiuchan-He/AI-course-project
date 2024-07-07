#ifndef PROBFD_Q_LEARNING_MDP_SIMULATOR_H
#define PROBFD_Q_LEARNING_MDP_SIMULATOR_H

#include "downward/state.h"

#include <vector>

namespace q_learning {

/// A sampled reward and successor state.
struct SampleResult {
    double reward;       ///< The reward.
    State sampled_state; ///< The successor state.
};

/**
 * @brief The environment simulator interface.
 *
 * @ingroup q_learning
 */
class MDPSimulator {
public:
    virtual ~MDPSimulator() = default;

    /// Obtain the initial state from the environment.
    virtual State get_initial_state() = 0;

    /**
     * @brief Generates the applicable actions for a state.
     *
     * @param state The state for which the applicable actions are generated.
     * @param[out] applicable_actions The actions are added to this list.
     */
    virtual void generate_applicable_actions(
        const State& state,
        std::vector<OperatorID>& applicable_actions) = 0;

    /**
     * @brief Samples a successor state and a reward for a given state and
     * action.
     */
    virtual SampleResult
    sample_next_state(const State& state, OperatorID action) = 0;
};

} // namespace q_learning

#endif