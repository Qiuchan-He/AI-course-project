#ifndef PROBFD_Q_LEARNING_RANDOM_POLICY_H
#define PROBFD_Q_LEARNING_RANDOM_POLICY_H

#include "q_learning/policy.h"

#include "downward/utils/rng.h"

#include <memory>

namespace q_learning {

class RandomPolicy : public Policy {
    utils::RandomNumberGenerator rng;

public:
    explicit RandomPolicy(int seed);

    OperatorID choose_next_action(
        const State& state,
        std::vector<OperatorID>& applicable_actions,
        const QVFApproximator& qvf_approximator) override;
};

} // namespace q_learning

#endif