#include "downward/heuristic.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/task_utils/task_properties.h"
#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include <cassert>
#include <cstdlib>
#include <limits>

using namespace std;

Heuristic::Heuristic(const Options& opts)
    : Evaluator(opts)
    , heuristic_cache(HEntry(NO_VALUE, true))
    , task(opts.get<shared_ptr<ClassicalTask>>("transform"))
    , task_proxy(*task)
{
}

Heuristic::Heuristic(
    std::string description,
    utils::LogProxy log,
    std::shared_ptr<ClassicalTask> task)
    : Evaluator(std::move(description), log)
    , heuristic_cache(HEntry(NO_VALUE, true))
    , task(std::move(task))
    , task_proxy(*this->task)
{
}

Heuristic::~Heuristic()
{
}

void Heuristic::add_options_to_parser(OptionParser& parser)
{
    add_evaluator_options_to_parser(parser);
    parser.add_option<shared_ptr<ClassicalTask>>(
        "transform",
        "Optional task transformation for the heuristic."
        " Currently, adapt_costs() and no_transform() are available.",
        "no_transform()");
}

std::pair<int, double>
Heuristic::compute_heuristic_and_confidence(const State& state)
{
    return {compute_heuristic(state), DEAD_END};
}

void Heuristic::get_path_dependent_evaluators(std::set<Evaluator*>& /*evals*/)
{
}

EvaluationResult Heuristic::compute_result(EvaluationContext& eval_context)
{
    EvaluationResult result;

    assert(preferred_operators.empty());

    const State& state = eval_context.get_state();
    bool calculate_preferred = eval_context.get_calculate_preferred();

    int heuristic = NO_VALUE;

    if (!calculate_preferred && heuristic_cache[state].h != NO_VALUE &&
        !heuristic_cache[state].dirty) {
        heuristic = heuristic_cache[state].h;
        result.set_count_evaluation(false);
    } else {
        heuristic = compute_heuristic(state);
        heuristic_cache[state] = HEntry(heuristic, false);
        result.set_count_evaluation(true);
    }

    assert(heuristic == DEAD_END || heuristic >= 0);

    if (heuristic == DEAD_END) {
        /*
          It is permissible to mark preferred operators for dead-end
          states (thus allowing a heuristic to mark them on-the-fly
          before knowing the final result), but if it turns out we
          have a dead end, we don't want to actually report any
          preferred operators.
        */
        preferred_operators.clear();
        heuristic = EvaluationResult::INFTY;
    }

#ifndef NDEBUG
    ClassicalTaskProxy global_task_proxy =
        static_cast<ClassicalTaskProxy>(state.get_task());
    OperatorsProxy global_operators = global_task_proxy.get_operators();
    if (heuristic != EvaluationResult::INFTY) {
        for (OperatorID op_id : preferred_operators)
            assert(
                task_properties::is_applicable(global_operators[op_id], state));
    }
#endif

    result.set_evaluator_value(heuristic);
    result.set_preferred_operators(preferred_operators.pop_as_vector());
    assert(preferred_operators.empty());

    return result;
}

bool Heuristic::is_estimate_cached(const State& state) const
{
    return heuristic_cache[state].h != NO_VALUE;
}

int Heuristic::get_cached_estimate(const State& state) const
{
    assert(is_estimate_cached(state));
    return heuristic_cache[state].h;
}

static PluginTypePlugin<Heuristic> _type_plugin(
    "Heuristic",
    "An evaluator specification is either a newly created evaluator "
    "instance or an evaluator that has been defined previously. "
    "This page describes how one can specify a new evaluator instance. "
    "For re-using evaluators, see OptionSyntax#Evaluator_Predefinitions.\n\n"
    "If the evaluator is a heuristic, "
    "definitions of //properties// in the descriptions below:\n\n"
    " * **admissible:** h(s) <= h*(s) for all states s\n"
    " * **consistent:** h(s) <= c(s, s') + h(s') for all states s "
    "connected to states s' by an action with cost c(s, s')\n"
    " * **safe:** h(s) = infinity is only true for states "
    "with h*(s) = infinity\n"
    " * **preferred operators:** this heuristic identifies "
    "preferred operators ");