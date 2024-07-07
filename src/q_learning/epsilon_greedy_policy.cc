#include "q_learning/epsilon_greedy_policy.h"

#include "q_learning/fp_compare.h"
#include "q_learning/qvf_approximator.h"

#include "downward/utils/rng.h"

using namespace probfd;

namespace q_learning {

EpsilonGreedyPolicy::EpsilonGreedyPolicy(int rng_seed, double epsilon)
    : rng(rng_seed)
    , epsilon(epsilon)
{
}

OperatorID EpsilonGreedyPolicy::choose_next_action(
    const State& state,
    std::vector<OperatorID>& applicable_actions,
    const QVFApproximator& qvf_approximator)
{
    std::vector<OperatorID> greedy;
    std::vector<OperatorID> non_greedy;

    const double best = qvf_approximator.compute_max_qvalue(state);

    for (OperatorID op : applicable_actions) {
        if (is_approx_equal(qvf_approximator.get_qvalue(state, op), best)) {
            greedy.push_back(op);
        } else {
            non_greedy.push_back(op);
        }
    }

    assert(!greedy.empty());

    if (!non_greedy.empty() && rng.random() < epsilon) {
        return *rng.choose(non_greedy);
    }

    return *rng.choose(greedy);
}

} // namespace q_learning
