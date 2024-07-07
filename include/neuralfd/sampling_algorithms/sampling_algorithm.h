#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLING_ALGORITHM_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLING_ALGORITHM_H

#include "neuralfd/sampling_algorithms/sample_cache.h"

#include "neuralfd/sampling_techniques/sampling_technique.h"

#include "downward/search_algorithm.h"

#include "downward/utils/hash.h"

#include "downward/ext/tree.hh"

#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

namespace utils {
class RandomNumberGenerator;
}

namespace sampling_algorithm {
/* Use this magic word if you want to ensure that the file is correctly encoded
 * e.g. after compressing it. Add it into your sample_file_header*/
extern const std::string SAMPLE_FILE_MAGIC_WORD;

class SamplingAlgorithm : public SearchAlgorithm {
protected:
    const bool shuffle_sampling_techniques;
    const std::vector<std::shared_ptr<sampling_technique::SamplingTechnique>>
        sampling_techniques;
    std::vector<std::shared_ptr<sampling_technique::SamplingTechnique>>::
        const_iterator current_technique;
    SampleCacheManager sample_cache_manager;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    virtual void initialize() override;
    virtual void update_current_technique();
    virtual SearchStatus step() override;
    virtual std::vector<std::string>
    sample(std::shared_ptr<ClassicalTask> task) = 0;

public:
    explicit SamplingAlgorithm(const options::Options& opts);
    virtual ~SamplingAlgorithm() = default;

    virtual std::string sample_file_header() const = 0;
    virtual void print_statistics() const override;
    virtual void save_plan_if_necessary() override;

    static void add_sampling_options(options::OptionParser& parser);
};
} // namespace sampling_algorithm
#endif
