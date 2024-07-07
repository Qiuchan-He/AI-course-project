#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLING_TASKS_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLING_TASKS_H

#include "neuralfd/sampling_algorithms/sampling_algorithm.h"

#include "downward/ext/tree.hh"

#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

namespace options {
class Options;
}

namespace sampling_algorithm {
class SamplingTasks : public SamplingAlgorithm {
protected:
    virtual std::vector<std::string>
    sample(std::shared_ptr<ClassicalTask> task) override;
    virtual std::string sample_file_header() const override;

public:
    explicit SamplingTasks(const options::Options& opts);
    virtual ~SamplingTasks() override = default;
};
} // namespace sampling_algorithm
#endif
