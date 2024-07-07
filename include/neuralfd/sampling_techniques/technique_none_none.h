#ifndef NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_NONE_NONE_H
#define NEURALFD_SAMPLING_TECHNIQUES_TECHNIQUE_NONE_NONE_H

#include "neuralfd/sampling_techniques/sampling_technique.h"

namespace sampling_technique {

class TechniqueNoneNone : public SamplingTechnique {
protected:
    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) override;

public:
    explicit TechniqueNoneNone(const options::Options& opts);
    TechniqueNoneNone(int count, bool check_mutexes, bool check_solvable);
    virtual ~TechniqueNoneNone() override = default;

    virtual const std::string& get_name() const override;
    const static std::string name;
};
} // namespace sampling_technique
#endif
