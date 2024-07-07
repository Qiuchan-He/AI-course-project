#ifndef NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_GBACKWARD_NONE_H
#define NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_GBACKWARD_NONE_H

#include "neuralfd/sampling_techniques/sampling_technique.h"

#include "downward/state.h"
#include "downward/task_proxy.h"

#include "downward/utils/distribution.h"

class StateRegistry;
class RegressionTaskProxy;
namespace sampling {
class RandomRegressionWalkSampler;
}

namespace sampling_technique {
class TechniqueGBackwardNone : public SamplingTechnique {
protected:
    std::shared_ptr<utils::DiscreteDistribution> steps;
    const bool wrap_partial_assignment;
    const bool deprioritize_undoing_steps;
    const bool is_valid_walk;
    const options::ParseTree bias_evaluator_tree;
    const bool bias_probabilistic;
    const double bias_adapt;
    utils::HashMap<PartialAssignment, int> cache;
    std::shared_ptr<Heuristic> bias = nullptr;
    const int bias_reload_frequency;
    int bias_reload_counter;
    std::shared_ptr<StateRegistry> state_registry = nullptr;
    std::shared_ptr<ClassicalTask> last_partial_wrap_task = nullptr;
    std::shared_ptr<RegressionTaskProxy> regression_task_proxy = nullptr;
    std::shared_ptr<sampling::RandomRegressionWalkSampler> rrws = nullptr;

    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) override;

    virtual void do_upgrade_parameters() override;

public:
    explicit TechniqueGBackwardNone(const options::Options& opts);
    virtual ~TechniqueGBackwardNone() override = default;

    virtual void
    dump_upgradable_parameters(std::ostream& stream) const override;

    virtual const std::string& get_name() const override;
    const static std::string name;
};
} // namespace sampling_technique
#endif
