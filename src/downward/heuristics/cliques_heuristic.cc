#include "downward/heuristics/cliques_heuristic.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/utils/logging.h"

#include <memory>

namespace pdbs {

CliquesHeuristic::CliquesHeuristic(const Options& opts)
    : CliquesHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          opts.get_list<Pattern>("patterns"),
          utils::get_log_from_options(opts))
{
}

CliquesHeuristic::CliquesHeuristic(
    std::shared_ptr<ClassicalTask> task,
    PatternCollection pattern_collection,
    utils::LogProxy log)
    : Heuristic("cpdbs", log, task)
{
    // TODO construct the pattern database lookup tables for each pattern here.
    // Afterwards, compute the maximal additive pattern subcollections (maximum
    // cliques in the pattern compatibility graph).
    //
    // NOTE: An implementation of a max cliques algorithm can be found in
    // algorithms/max_cliques.h.
    (void)pattern_collection;
}

int CliquesHeuristic::compute_heuristic(const State& state)
{
    // TODO return the cliques heuristic value for state.
    // To this end, lookup the cost-to-goal values of the abstract states
    // corresponding to state for each pattern. Afterwards, compute the additive
    // heuristic values for each pattern clique, and return the maximum over
    // these values.
    (void)state;
    utils::throw_not_implemented();
}

static std::shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Cliques",
        "The cliques heuristic is calculated as follows. "
        "For a given pattern collection C, the value of the "
        "cliques heuristic function is the maximum over all "
        "maximal additive subsets A in C, where the value for one subset "
        "S in A is the sum of the heuristic values for all patterns in S "
        "for a given state.");
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    parser.add_list_option<Pattern>(
        "patterns",
        "The pattern collection.",
        "[[]]");

    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;
    return std::make_shared<CliquesHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("cpdbs", _parse, "heuristics_pdb");
} // namespace pdbs
