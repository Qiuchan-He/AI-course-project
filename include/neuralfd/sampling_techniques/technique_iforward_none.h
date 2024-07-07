#ifndef NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_IFORWARD_NONE_H
#define NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_IFORWARD_NONE_H

#include "neuralfd/sampling_techniques/sampling_technique.h"

#include "downward/state.h"
#include "downward/task_proxy.h"

#include "downward/utils/distribution.h"

namespace sampling {
class RandomWalkSampler;
}

namespace sampling_technique {
class TechniqueIForwardNone : public SamplingTechnique {
protected:
    std::shared_ptr<utils::DiscreteDistribution> steps;
    const bool deprioritize_undoing_steps;
    const options::ParseTree bias_evaluator_tree;
    const bool bias_probabilistic;
    const double bias_adapt;
    utils::HashMap<PartialAssignment, int> cache;
    std::shared_ptr<Heuristic> bias = nullptr;
    const int bias_reload_frequency;
    int bias_reload_counter;
    std::shared_ptr<sampling::RandomWalkSampler> rws = nullptr;

    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) override;

public:
    explicit TechniqueIForwardNone(const options::Options& opts);
    virtual ~TechniqueIForwardNone() override = default;

    virtual const std::string& get_name() const override;
    const static std::string name;
};
} // namespace sampling_technique
#endif
