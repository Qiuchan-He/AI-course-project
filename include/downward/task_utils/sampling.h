#ifndef DOWNWARD_TASK_UTILS_SAMPLING_H
#define DOWNWARD_TASK_UTILS_SAMPLING_H

#include "downward/state.h"

#include <functional>
#include <memory>
#include <vector>

class PartialAssignment;
class State;

namespace successor_generator {
class SuccessorGenerator;
}

namespace utils {
class RandomNumberGenerator;
}

using DeadEndDetector = std::function<bool(State&)>;
using PartialDeadEndDetector = std::function<bool(PartialAssignment&)>;
using ValidStateDetector = std::function<bool(PartialAssignment&)>;
using PartialAssignmentBias = std::function<int(PartialAssignment&)>;
using StateBias = std::function<int(State&)>;

namespace sampling {
/*
  Sample states with random walks.
*/
class RandomWalkSampler {
    const OperatorsProxy operators;
    const std::unique_ptr<successor_generator::SuccessorGenerator>
        successor_generator;
    const State initial_state;
    const double average_operator_costs;
    utils::RandomNumberGenerator& rng;

public:
    RandomWalkSampler(
        const ClassicalTaskProxy& task_proxy,
        utils::RandomNumberGenerator& rng);
    ~RandomWalkSampler();

    /*
      Perform a single random walk and return the last visited state.

      The walk length is taken from a binomial distribution centered around the
      estimated plan length, which is computed as the ratio of the h value of
      the initial state divided by the average operator costs. Whenever a dead
      end is detected or a state has no successors, restart from the initial
      state. The function 'is_dead_end' should return whether a given state is
      a dead end. If omitted, no dead end detection is performed. The 'init_h'
      value should be an estimate of the solution cost.
    */
    State sample_state(
        int init_h,
        const DeadEndDetector& is_dead_end = [](const State&) {
            return false;
        }) const;

    State sample_state_length(
        const State& init_state,
        int length,
        const DeadEndDetector& is_dead_end = [](const State&) { return false; },
        bool deprioritize_undoing_steps = false,
        const StateBias* bias = nullptr,
        bool probabilistic_bias = true,
        double adapt_bias = -1) const;
};

} // namespace sampling

#endif