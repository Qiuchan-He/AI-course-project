#ifndef PROBFD_Q_LEARNING_POLICY_H
#define PROBFD_Q_LEARNING_POLICY_H

#include "downward/task_proxy.h"

#include <vector>

namespace q_learning {

class QVFApproximator;

/**
 * @brief This interface represents a probabilistic policy that depends on the
 * current Q value function of the Q-Learning algorithm.
 *
 * @ingroup q_learning
 */
class Policy {
public:
    virtual ~Policy() = default;

    /**
     * @brief Picks an action among the given set of applicable actions for the
     * input state.
     *
     * The decision depends on the supplied Q value function.
     */
    virtual OperatorID choose_next_action(
        const State& state,
        std::vector<OperatorID>& applicable_actions,
        const QVFApproximator& qvf_approximator) = 0;
};

} // namespace q_learning

#endif