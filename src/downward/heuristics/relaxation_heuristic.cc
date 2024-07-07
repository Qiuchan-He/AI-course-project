#include "downward/heuristics/relaxation_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/options/options.h"

namespace relaxation_heuristic {

// construction and destruction
RelaxationHeuristic::RelaxationHeuristic(const options::Options& opts)
    : RelaxationHeuristic(
          opts.get_unparsed_config(),
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts))
{
}

RelaxationHeuristic::RelaxationHeuristic(
    std::string description,
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log)
    : Heuristic(std::move(description), log, task)
{
    // TODO Initialize the heuristic, if needed.
}

} // namespace relaxation_heuristic
