#ifndef NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_UNIFORM_NONE_H
#define NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_UNIFORM_NONE_H

#include "neuralfd/sampling_techniques/sampling_technique.h"

namespace sampling_technique {

class TechniqueUniformNone : public SamplingTechnique {
protected:
    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) override;

public:
    explicit TechniqueUniformNone(const options::Options& opts);
    virtual ~TechniqueUniformNone() override = default;

    virtual const std::string& get_name() const override;
    const static std::string name;
};

} // namespace sampling_technique
#endif
