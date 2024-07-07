#include "downward/search_algorithm.h"

#include "downward/evaluation_context.h"
#include "downward/evaluator.h"
#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/algorithms/ordered_set.h"
#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"
#include "downward/tasks/root_task.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;
using utils::ExitCode;

static successor_generator::SuccessorGenerator& get_successor_generator(
    const ClassicalTaskProxy& task_proxy,
    utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Building successor generator..." << flush;
    }
    int peak_memory_before = utils::get_peak_memory_in_kb();
    utils::Timer successor_generator_timer;
    successor_generator::SuccessorGenerator& successor_generator =
        successor_generator::g_successor_generators[task_proxy];
    successor_generator_timer.stop();
    if (log.is_at_least_normal()) {
        log << "done!" << endl;
    }
    int peak_memory_after = utils::get_peak_memory_in_kb();
    int memory_diff = peak_memory_after - peak_memory_before;
    if (log.is_at_least_normal()) {
        log << "peak memory difference for successor generator creation: "
            << memory_diff << " KB" << endl
            << "time for successor generation creation: "
            << successor_generator_timer << endl;
    }
    return successor_generator;
}

SearchAlgorithm::SearchAlgorithm(const Options& opts)
    : SearchAlgorithm(
          opts.get<shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts),
          opts.get<OperatorCost>("cost_type"),
          opts.get<double>("max_time"),
          opts.get<int>("bound"))
{
}

SearchAlgorithm::SearchAlgorithm(
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log,
    OperatorCost cost_type,
    double max_time,
    int bound)
    : status(IN_PROGRESS)
    , solution_found(false)
    , task(task)
    , task_proxy(*task)
    , log(log)
    , state_registry(task_proxy)
    , successor_generator(get_successor_generator(task_proxy, this->log))
    , search_space(state_registry, this->log)
    , statistics(this->log)
    , cost_type(cost_type)
    , is_unit_cost(task_properties::is_unit_cost(task_proxy))
    , max_time(max_time)
{
    if (bound < 0) {
        cerr << "error: negative cost bound " << bound << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    this->bound = bound;
    if (log.is_at_least_normal()) {
        task_properties::print_variable_statistics(task_proxy, log);
    }
}

SearchAlgorithm::~SearchAlgorithm()
{
}

bool SearchAlgorithm::found_solution() const
{
    return solution_found;
}

SearchStatus SearchAlgorithm::get_status() const
{
    return status;
}

const Plan& SearchAlgorithm::get_plan() const
{
    assert(solution_found);
    return plan;
}

void SearchAlgorithm::set_plan(const Plan& p)
{
    solution_found = true;
    plan = p;
}

const State SearchAlgorithm::get_goal_state() const
{
    assert(solution_found);
    return state_registry.lookup_state(goal_id);
}

const StateRegistry& SearchAlgorithm::get_state_registry() const
{
    return state_registry;
}

const SearchSpace& SearchAlgorithm::get_search_space() const
{
    return search_space;
}

const ClassicalTaskProxy& SearchAlgorithm::get_task_proxy() const
{
    return task_proxy;
}

void SearchAlgorithm::search()
{
    initialize();
    assert(!timer);
    timer = std::make_unique<utils::CountdownTimer>(max_time);
    while (status == IN_PROGRESS) {
        status = step();
        if (timer->is_expired()) {
            if (log.is_at_least_normal())
                log << "Time limit reached. Abort search." << endl;
            status = TIMEOUT;
            break;
        }
    }
    // TODO: Revise when and which search times are logged.
    if (log.is_at_least_normal())
        log << "Actual search time: " << timer->get_elapsed_time() << endl;
}

bool SearchAlgorithm::check_goal_and_set_plan(const State& state)
{
    if (task_properties::is_goal_state(task_proxy, state)) {
        if (log.is_at_least_normal()) log << "Solution found!" << endl;
        goal_id = state.get_id();
        Plan plan;
        search_space.trace_path(state, plan);
        set_plan(plan);
        return true;
    }
    return false;
}

void SearchAlgorithm::save_plan_if_necessary()
{
    if (found_solution()) {
        plan_manager.save_plan(get_plan(), task_proxy);
    }
}

int SearchAlgorithm::get_adjusted_cost(const OperatorProxy& op) const
{
    return get_adjusted_action_cost(op, cost_type, is_unit_cost);
}

double SearchAlgorithm::get_max_time()
{
    return max_time;
}

void SearchAlgorithm::reduce_max_time(double new_max_time)
{
    if (timer) {
        cerr << "The time limit of a search cannot be reduced after it was"
                "started"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    } else if (new_max_time > max_time) {
        cerr << "You cannot increase the time limit of a search!" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    } else {
        max_time = new_max_time;
    }
}

void SearchAlgorithm::add_options_to_parser(OptionParser& parser)
{
    ::add_cost_type_option_to_parser(parser);
    parser.add_option<int>(
        "bound",
        "exclusive depth bound on g-values. Cutoffs are always performed "
        "according to "
        "the real cost, regardless of the cost_type parameter",
        "infinity");
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds the search is allowed to run for. The "
        "timeout is only checked after each complete search step "
        "(usually a node expansion), so the actual runtime can be arbitrarily "
        "longer. Therefore, this parameter should not be used for "
        "time-limiting "
        "experiments. Timed-out searches are treated as failed searches, "
        "just like incomplete search algorithms that exhaust their search "
        "space.",
        "infinity");
    parser.add_option<shared_ptr<ClassicalTask>>(
        "transform",
        "Optional task transformation for the search algorithm."
        " Currently, adapt_costs(), sampling_transform(), and no_transform() "
        "are "
        "available.",
        "no_transform()");
    utils::add_log_options_to_parser(parser);
}

/* Method doesn't belong here because it's only useful for certain derived
   classes.
   TODO: Figure out where it belongs and move it there. */
void SearchAlgorithm::add_succ_order_options(OptionParser& parser)
{
    vector<string> options;
    parser.add_option<bool>(
        "randomize_successors",
        "randomize the order in which successors are generated",
        "false");
    parser.add_option<bool>(
        "preferred_successors_first",
        "consider preferred operators first",
        "false");
    parser.document_note(
        "Successor ordering",
        "When using randomize_successors=true and "
        "preferred_successors_first=true, randomization happens before "
        "preferred operators are moved to the front.");
    utils::add_rng_options(parser);
}

void print_initial_evaluator_values(const EvaluationContext& eval_context)
{
    eval_context.get_cache().for_each_evaluator_result(
        [](const Evaluator* eval, const EvaluationResult& result) {
            eval->report_value_for_initial_state(result);
        });
}

static PluginTypePlugin<SearchAlgorithm> _type_plugin(
    "SearchAlgorithm",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");

void collect_preferred_operators(
    EvaluationContext& eval_context,
    Evaluator* preferred_operator_evaluator,
    ordered_set::OrderedSet<OperatorID>& preferred_operators)
{
    if (!eval_context.is_evaluator_value_infinite(
            preferred_operator_evaluator)) {
        for (OperatorID op_id : eval_context.get_preferred_operators(
                 preferred_operator_evaluator)) {
            preferred_operators.insert(op_id);
        }
    }
}
