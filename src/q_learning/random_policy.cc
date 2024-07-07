#include "q_learning/random_policy.h"

namespace q_learning {

RandomPolicy::RandomPolicy(int seed)
    : rng(seed)
{
}

OperatorID RandomPolicy::choose_next_action(
    const State&,
    std::vector<OperatorID>& applicable_actions,
    const QVFApproximator&)
{
    return *rng.choose(applicable_actions);
}

} // namespace q_learning
