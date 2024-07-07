#include "downward/task_utils/sampling.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"

#include <cmath>
#include <limits>
#include <queue>

using namespace std;

namespace sampling {
template <typename S>
bool sample_next_state(
    S& current_state,
    S& previous_state,
    S& pre_previous_state,
    double& current_bias,
    const successor_generator::SuccessorGenerator& generator,
    const function<S(const S&, const OperatorID&)>& construct_candidate,
    utils::RandomNumberGenerator& rng,
    bool deprioritize_undoing_steps,
    const function<int(S&)>* bias,
    bool probabilistic_bias,
    double adapt_bias,
    const function<bool(S&)>* is_dead_end = nullptr,
    const function<bool(S&)>* is_valid_state = nullptr)
{

    pre_previous_state = std::move(previous_state);
    previous_state = std::move(current_state);
    vector<OperatorID> applicable_operators;
    generator.generate_applicable_ops(previous_state, applicable_operators);
    // If there are no applicable operators, do not walk further.
    if (applicable_operators.empty()) {
        current_state = std::move(previous_state);
        return false;
    } else {
        bool found_successor = false;
        vector<S> candidate_states;
        vector<double> candidate_bias;
        double bias_helper = 0;
        bool found_non_reversing_state = false;

        int idx_op = 0;
        while (!applicable_operators.empty()) {
            // Generate successor candidate
            if (bias == nullptr) {
                idx_op = rng.random(applicable_operators.size());
            }
            S candidate_state = construct_candidate(
                previous_state,
                applicable_operators[idx_op]);
            applicable_operators.erase(applicable_operators.begin() + idx_op);
            if ((is_valid_state != nullptr &&
                 !(*is_valid_state)(candidate_state)) ||
                (is_dead_end != nullptr && (*is_dead_end)(candidate_state))) {
                continue;
            }
            found_successor = true;

            // manage depriorization of reversing states
            bool non_reversing_state =
                (!deprioritize_undoing_steps ||
                 candidate_state != pre_previous_state);
            if (found_non_reversing_state && !non_reversing_state) {
                continue;
            }
            if (non_reversing_state && !found_non_reversing_state) {
                candidate_states.clear();
                candidate_bias.clear();
                bias_helper = 0;
                found_non_reversing_state = true;
            }

            // manage bias
            double new_candidate_bias =
                (bias == nullptr) ? 1 : (*bias)(candidate_state);

            // !probabilistic_bias === max bias
            if (!probabilistic_bias && new_candidate_bias > bias_helper) {
                candidate_states.clear();
                candidate_bias.clear();
                bias_helper = 0;
            }
            candidate_states.push_back(std::move(candidate_state));
            candidate_bias.push_back(new_candidate_bias);
            bias_helper = (probabilistic_bias)
                              ? (bias_helper + new_candidate_bias)
                              : max(bias_helper, new_candidate_bias);

            if (non_reversing_state && bias == nullptr) {
                break;
            }
        }
        if (!found_successor) {
            current_state = std::move(previous_state);
            return false;
        }

        if (candidate_states.size() == 1 || bias_helper == 0) {
            current_state = std::move(candidate_states.back());
            current_bias = candidate_bias.back();
        } else if (probabilistic_bias) {
            size_t candidate_index = -1;
            if (adapt_bias > 0) {
                vector<double> adapted_bias(candidate_bias.size());
                transform(
                    candidate_bias.begin(),
                    candidate_bias.end(),
                    adapted_bias.begin(),
                    [&](double b) {
                        return pow(adapt_bias, (b - current_bias));
                    });
                candidate_index = rng.weighted_choose_index(adapted_bias);
            } else {
                candidate_index = rng.weighted_choose_index(candidate_bias);
            }
            current_state =
                std::move(*(candidate_states.begin() + candidate_index));
            current_bias = candidate_bias[candidate_index];
        } else {
            int idx = rng.random(candidate_states.size());
            current_state = std::move(candidate_states[idx]);
            current_bias = candidate_bias[idx];
        }
        return true;
    }
}

template <typename S>
S sample_with_random_walk(
    const S& state,
    int length,
    const successor_generator::SuccessorGenerator& generator,
    const function<S(const S&, const OperatorID&)>& construct_candidate,
    utils::RandomNumberGenerator& rng,
    bool deprioritize_undoing_steps,
    const function<int(S&)>* bias,
    bool probabilistic_bias,
    double adapt_bias,
    const function<bool(S&)>* is_dead_end = nullptr,
    const function<bool(S&)>* is_valid_state = nullptr)
{
    // Sample one state with a random walk of length length.
    S current_state(state);
    S candidate_state(current_state);
    S previous_state(current_state);
    S pre_previous_state(current_state);
    double initial_bias = (bias == nullptr) ? 1 : (*bias)(current_state);
    double current_bias = initial_bias;

    for (int j = 0; j < length; ++j) {
        if (!sample_next_state(
                current_state,
                previous_state,
                pre_previous_state,
                current_bias,
                generator,
                construct_candidate,
                rng,
                deprioritize_undoing_steps,
                bias,
                probabilistic_bias,
                adapt_bias,
                is_dead_end,
                is_valid_state)) {
            if (is_dead_end != nullptr && (*is_dead_end)(current_state)) {
                current_state = S(state);
                current_bias = initial_bias;
            } else {
                break;
            }
        }
    }
    // The last state of the random walk is used as a sample.
    return current_state;
}

static State sample_state_with_random_forward_walk(
    const OperatorsProxy& operators,
    const successor_generator::SuccessorGenerator& successor_generator,
    const State& initial_state,
    int length,
    utils::RandomNumberGenerator& rng,
    const DeadEndDetector& is_dead_end,
    bool deprioritize_undoing_steps = false,
    const StateBias* bias = nullptr,
    bool probabilistic_bias = false,
    double adapt_bias = -1)
{

    function<State(const State&, const OperatorID&)> construct_candidate =
        [&](const State& s, const OperatorID& op_id) -> State {
        OperatorProxy op_proxy = operators[op_id];
        assert(task_properties::is_applicable(op_proxy, s));
        return s.get_unregistered_successor(op_proxy.get_effect());
    };

    return sample_with_random_walk<State>(
        initial_state,
        length,
        successor_generator,
        construct_candidate,
        rng,
        deprioritize_undoing_steps,
        bias,
        probabilistic_bias,
        adapt_bias,
        &is_dead_end);
}

static State sample_state_with_random_forward_walk(
    const OperatorsProxy& operators,
    const State& initial_state,
    const successor_generator::SuccessorGenerator& successor_generator,
    int init_h,
    double average_operator_cost,
    utils::RandomNumberGenerator& rng,
    const DeadEndDetector& is_dead_end)
{
    assert(init_h != numeric_limits<int>::max());
    int n;
    if (init_h == 0) {
        n = 10;
    } else {
        /*
          Convert heuristic value into an approximate number of actions
          (does nothing on unit-cost problems).
          average_operator_cost cannot equal 0, as in this case, all operators
          must have costs of 0 and in this case the if-clause triggers.
        */
        assert(average_operator_cost != 0);
        int solution_steps_estimate =
            int(lround(init_h / average_operator_cost));
        n = 4 * solution_steps_estimate;
    }
    double p = 0.5;
    /* The expected walk length is np = 2 * estimated number of solution steps.
       (We multiply by 2 because the heuristic is underestimating.) */

    // Calculate length of random walk according to a binomial distribution.
    int length = 0;
    for (int j = 0; j < n; ++j) {
        double random = rng.random(); // [0..1)
        if (random < p) ++length;
    }

    // Sample one state with a random walk of length length.
    return sample_state_with_random_forward_walk(
        operators,
        successor_generator,
        initial_state,
        length,
        rng,
        is_dead_end);
}

RandomWalkSampler::RandomWalkSampler(
    const ClassicalTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng)
    : operators(task_proxy.get_operators())
    , successor_generator(
          std::make_unique<successor_generator::SuccessorGenerator>(task_proxy))
    , initial_state(task_proxy.get_initial_state())
    , average_operator_costs(
          task_properties::get_average_operator_cost(task_proxy))
    , rng(rng)
{
}

RandomWalkSampler::~RandomWalkSampler()
{
}

State RandomWalkSampler::sample_state(
    int init_h,
    const DeadEndDetector& is_dead_end) const
{
    return sample_state_with_random_forward_walk(
        operators,
        initial_state,
        *successor_generator,
        init_h,
        average_operator_costs,
        rng,
        is_dead_end);
}

State RandomWalkSampler::sample_state_length(
    const State& init_state,
    int length,
    const DeadEndDetector& is_dead_end,
    bool deprioritize_undoing_steps,
    const StateBias* bias,
    bool probabilistic_bias,
    double adapt_bias) const
{
    return sample_state_with_random_forward_walk(
        operators,
        *successor_generator,
        init_state,
        length,
        rng,
        is_dead_end,
        deprioritize_undoing_steps,
        bias,
        probabilistic_bias,
        adapt_bias);
}

} // namespace sampling