#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLING_SEARCH_BASE_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLING_SEARCH_BASE_H

#include "neuralfd/sampling_algorithms/sampling_state_algorithm.h"

#include "downward/options/predefinitions.h"
#include "downward/options/registries.h"

#include "downward/ext/tree.hh"

#include <deque>
#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

class Evaluator;
class Heuristic;

namespace options {
class Options;
struct ParseNode;
using ParseTree = tree<ParseNode>;
} // namespace options

namespace sampling_algorithm {

class SamplingSearchBase : public SamplingStateAlgorithm {
protected:
    // Internal
    const options::ParseTree search_parse_tree;
    options::Registry registry;
    options::Predefinitions predefinitions;
    std::vector<std::string> tmp_ignore_repredefinitions;
    std::unordered_set<std::string> ignore_repredefinitions;

    std::shared_ptr<SearchAlgorithm> algorithm;

    virtual void post_search(std::vector<std::string>& samples);
    virtual std::vector<std::string> extract_samples() = 0;

    virtual std::vector<std::string>
    sample(std::shared_ptr<ClassicalTask> task) override;
    virtual void next_algorithm();

public:
    explicit SamplingSearchBase(const options::Options& opts);
    virtual ~SamplingSearchBase() override = default;

    static void add_sampling_search_base_options(options::OptionParser& parser);
};
} // namespace sampling_algorithm
#endif
