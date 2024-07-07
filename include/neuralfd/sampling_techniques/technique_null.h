#ifndef NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_NULL_H
#define NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_NULL_H

#include "neuralfd/sampling_techniques/sampling_technique.h"

namespace sampling_technique {

class TechniqueNull : public SamplingTechnique {
protected:
    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) override;

public:
    TechniqueNull();

    virtual ~TechniqueNull() override = default;

    virtual const std::string& get_name() const override;

    const static std::string name;
};
} // namespace sampling_technique
#endif
