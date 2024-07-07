#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLING_SEARCH_SIMPLE_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLING_SEARCH_SIMPLE_H

#include "neuralfd/sampling_algorithms/sampling_search_base.h"

#include <vector>

namespace options {
class Options;
}

namespace sampling_algorithm {

class SamplingSearchSimple : public SamplingSearchBase {
protected:
    const bool store_plan_cost;
    const bool store_state;
    const bool store_operator;
    const std::vector<FactPair> relevant_facts;
    const std::string header;

    virtual std::vector<std::string> extract_samples() override;
    virtual std::string construct_header() const;
    virtual std::string sample_file_header() const override;

public:
    explicit SamplingSearchSimple(const options::Options& opts);
    virtual ~SamplingSearchSimple() override = default;
};
} // namespace sampling_algorithm
#endif
