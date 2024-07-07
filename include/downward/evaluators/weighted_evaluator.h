#ifndef DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_H
#define DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_H

#include "downward/evaluator.h"

#include <memory>

namespace options {
class Options;
}

namespace weighted_evaluator {
class WeightedEvaluator : public Evaluator {
    std::shared_ptr<Evaluator> evaluator;
    int w;

public:
    explicit WeightedEvaluator(const options::Options& opts);
    virtual ~WeightedEvaluator() override;

    virtual bool dead_ends_are_reliable() const override;
    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;
    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override;
};
} // namespace weighted_evaluator

#endif
