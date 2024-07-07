#include "downward/heuristics/ff_heuristic.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/utils/logging.h"

namespace ff_heuristic {
// construction and destruction
FFHeuristic::FFHeuristic(const Options& opts)
    : FFHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts))
{
}

FFHeuristic::FFHeuristic(
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log)
    : RelaxationHeuristic("hFF", std::move(task), std::move(log))
{
    // TODO Initialize the heuristic, if needed.
    utils::throw_not_implemented();
}

int FFHeuristic::compute_heuristic(const State& state)
{
    if (auto dr_plan = compute_relaxed_plan(state)) {
        return dr_plan->cost;
    }

    return Heuristic::DEAD_END;
}

std::optional<DeleteRelaxedPlan>
FFHeuristic::compute_relaxed_plan(const State& state)
{
    // TODO Return the a delete-relaxed plan paired with its cost or
    // std::nullopt if no delete-relaxed plan exists.
    (void)state;
    utils::throw_not_implemented();
}

static std::shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis("FF heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "yes");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;
    return std::make_shared<FFHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("ff", _parse);
} // namespace ff_heuristic
