#ifndef PROBFD_Q_LEARNING_MAXPROB_SIMULATOR_H
#define PROBFD_Q_LEARNING_MAXPROB_SIMULATOR_H

#include "q_learning/mdp_simulator.h"

#include "probfd/task_proxy.h"

#include "downward/utils/rng.h"

#include "downward/state_registry.h"

#include "downward/task_utils/successor_generator.h"

namespace q_learning {

class MaxProbSimulator : public MDPSimulator {
    probfd::ProbabilisticTaskProxy task_proxy;

    utils::RandomNumberGenerator rng;
    StateRegistry state_registry;
    successor_generator::SuccessorGenerator successor_generator;

public:
    MaxProbSimulator(probfd::ProbabilisticTaskProxy task_proxy, int rng_seed);

    State get_successor(
        const State& state,
        probfd::ProbabilisticOutcomeProxy outcome);

    State get_initial_state() override;

    void generate_applicable_actions(
        const State& state,
        std::vector<OperatorID>& applicable_actions) override;

    SampleResult
    sample_next_state(const State& state, OperatorID op_id) override;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_MAXPROB_SIMULATOR_H
