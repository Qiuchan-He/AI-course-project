#ifndef NEURALFD_SEARCH_ALGORITHMS_SAMPLING_STATE_ALGORITHM_H
#define NEURALFD_SEARCH_ALGORITHMS_SAMPLING_STATE_ALGORITHM_H

#include "neuralfd/sampling_algorithms/sampling_algorithm.h"

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
using Trajectory = std::vector<StateID>;
using Plan = std::vector<OperatorID>;

class Path {
protected:
    Trajectory trajectory;
    Plan plan;

public:
    explicit Path(const StateID& start);
    Path(const Path&) = delete;
    ~Path() = default;

    void add(const OperatorID& op, const StateID& next);

    const Trajectory& get_trajectory() const;
    const Plan& get_plan() const;
};
extern std::vector<Path> paths;

enum SampleFormat { CSV, FIELDS };

enum StateFormat {
    PDDL,
    FDR,
};

class SamplingStateAlgorithm : public SamplingAlgorithm {
protected:
    const bool skip_undefined_facts;

    const SampleFormat sample_format;
    const StateFormat state_format;
    const std::string field_separator;
    const std::string state_separator;

    virtual std::string sample_file_header() const override;

    void
    convert_and_push_state(std::ostringstream& oss, const State& state) const;
    void
    convert_and_push_goal(std::ostringstream& oss, const ClassicalTask& task)
        const;

public:
    explicit SamplingStateAlgorithm(const options::Options& opts);
    virtual ~SamplingStateAlgorithm() override = default;

    static void add_sampling_state_options(
        options::OptionParser& parser,
        const std::string& default_sample_format,
        const std::string& default_state_format,
        const std::string& default_field_separator,
        const std::string& default_state_separator);
};
} // namespace sampling_algorithm
#endif
