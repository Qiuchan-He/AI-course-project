#ifndef NEURALFD_SEARCH_ALGORITHMS_POLICY_WALK_H
#define NEURALFD_SEARCH_ALGORITHMS_POLICY_WALK_H

#include "downward/evaluation_context.h"
#include "downward/heuristic.h"
#include "downward/open_list.h"
#include "downward/search_algorithm.h"

#include <utility>
#include <vector>

namespace options {
class Options;
}

namespace utils {
class RandomNumberGenerator;
}

class Policy;

namespace search_algorithms {

enum OperatorSelection { First, Best, Probability };

OperatorSelection get_operator_selection(std::string selection);

/*
  Policy Search, following a given Policy by naively choosing the
  (first of all) most probable operator(s)
*/
class PolicyWalk : public SearchAlgorithm {
    std::shared_ptr<Policy> policy;
    std::vector<std::shared_ptr<Evaluator>> dead_end_evaluators;
    const int trajectory_limit;
    const bool reopen;
    const OperatorSelection op_select;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    State current_state;
    int trajectory_length = 0;

    bool is_dead_end(EvaluationContext& eval_context);

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit PolicyWalk(const options::Options& opts);
    virtual ~PolicyWalk() override;

    virtual void print_statistics() const override;
};
} // namespace search_algorithms

#endif
