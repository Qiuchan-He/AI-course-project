#ifndef PROBFD_Q_LEARNING_EPSILON_GREEDY_POLICY_H
#define PROBFD_Q_LEARNING_EPSILON_GREEDY_POLICY_H

#include "q_learning/policy.h"

#include "downward/utils/rng.h"

#include <memory>

namespace q_learning {

class EpsilonGreedyPolicy : public Policy {
    utils::RandomNumberGenerator rng;
    const double epsilon;

public:
    EpsilonGreedyPolicy(int rng_seed, double epsilon);

    OperatorID choose_next_action(
        const State& state,
        std::vector<OperatorID>& applicable_actions,
        const QVFApproximator& qvf) override;
};

} // namespace q_learning

#endif