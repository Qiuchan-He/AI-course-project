#ifndef DOWNWARD_EVALUATORS_MAX_EVALUATOR_H
#define DOWNWARD_EVALUATORS_MAX_EVALUATOR_H

#include "downward/evaluators/combining_evaluator.h"

#include <vector>

namespace options {
class Options;
}

namespace max_evaluator {
class MaxEvaluator : public combining_evaluator::CombiningEvaluator {
protected:
    virtual int combine_values(const std::vector<int>& values) override;

public:
    explicit MaxEvaluator(const options::Options& opts);
    virtual ~MaxEvaluator() override;
};
} // namespace max_evaluator

#endif
