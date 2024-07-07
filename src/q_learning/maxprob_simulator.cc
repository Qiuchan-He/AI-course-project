#include "q_learning/maxprob_simulator.h"

#include "downward/task_utils/task_properties.h"

using namespace probfd;

namespace q_learning {

MaxProbSimulator::MaxProbSimulator(
    ProbabilisticTaskProxy task_proxy,
    int rng_seed)
    : task_proxy(task_proxy)
    , rng(rng_seed)
    , state_registry(task_proxy)
    , successor_generator(task_proxy)
{
}

State MaxProbSimulator::get_initial_state()
{
    return state_registry.get_initial_state();
}

State MaxProbSimulator::get_successor(
    const State& state,
    ProbabilisticOutcomeProxy outcome)
{
    return state_registry.get_successor_state(state, outcome.get_effect());
}

void MaxProbSimulator::generate_applicable_actions(
    const State& state,
    std::vector<OperatorID>& applicable_actions)
{
    if (!::task_properties::is_goal_state(task_proxy, state)) {
        successor_generator.generate_applicable_ops(state, applicable_actions);
    }
}

SampleResult
MaxProbSimulator::sample_next_state(const State& state, OperatorID op_id)
{
    const auto op = task_proxy.get_operators()[op_id];

    double d = rng.random();

    double total_prob = 0;

    for (const auto outcome : op.get_outcomes()) {
        total_prob += outcome.get_probability();
        if (d <= total_prob) {
            auto successor =
                state_registry.get_successor_state(state, outcome.get_effect());
            return SampleResult{
                ::task_properties::is_goal_state(task_proxy, successor) ? 1.0 : 0.0,
                successor};
        }
    }

    abort();
}

} // namespace q_learning
