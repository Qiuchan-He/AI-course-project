#ifndef DOWNWARD_SEARCH_ALGORITHM_H
#define DOWNWARD_SEARCH_ALGORITHM_H

#include "downward/operator_cost.h"
#include "downward/operator_id.h"
#include "downward/plan_manager.h"
#include "downward/search_progress.h"
#include "downward/search_space.h"
#include "downward/search_statistics.h"
#include "downward/state_registry.h"
#include "downward/task_proxy.h"

#include "downward/utils/logging.h"

#include <vector>

namespace options {
class OptionParser;
class Options;
} // namespace options

namespace ordered_set {
template <typename T>
class OrderedSet;
}

namespace successor_generator {
class SuccessorGenerator;
}

namespace utils {
class CountdownTimer;
} // namespace utils

enum SearchStatus { IN_PROGRESS, TIMEOUT, FAILED, SOLVED };

class SearchAlgorithm {
    SearchStatus status;
    Plan plan;
    StateID goal_id = StateID::no_state;

protected:
    bool solution_found;

    // Hold a reference to the task implementation and pass it to objects that
    // need it.
    const std::shared_ptr<ClassicalTask> task;
    // Use task_proxy to access task information.
    ClassicalTaskProxy task_proxy;

    mutable utils::LogProxy log;
    PlanManager plan_manager;
    StateRegistry state_registry;
    const successor_generator::SuccessorGenerator& successor_generator;
    SearchSpace search_space;
    SearchProgress search_progress;
    SearchStatistics statistics;
    int bound;
    OperatorCost cost_type;
    bool is_unit_cost;
    double max_time;

    std::unique_ptr<utils::CountdownTimer> timer;

    virtual void initialize() {}
    virtual SearchStatus step() = 0;

    void set_plan(const Plan& plan);
    bool check_goal_and_set_plan(const State& state);
    int get_adjusted_cost(const OperatorProxy& op) const;

public:
    SearchAlgorithm(const options::Options& opts);
    SearchAlgorithm(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log,
        OperatorCost cost_type,
        double max_time,
        int bound);
    virtual ~SearchAlgorithm();
    virtual void print_statistics() const = 0;
    virtual void save_plan_if_necessary();
    bool found_solution() const;
    SearchStatus get_status() const;
    const Plan& get_plan() const;
    const State get_goal_state() const;
    const StateRegistry& get_state_registry() const;
    const SearchSpace& get_search_space() const;
    const ClassicalTaskProxy& get_task_proxy() const;
    void search();
    const SearchStatistics& get_statistics() const { return statistics; }
    void set_bound(int b) { bound = b; }
    int get_bound() { return bound; }
    PlanManager& get_plan_manager() { return plan_manager; }
    double get_max_time();
    void reduce_max_time(double new_max_time);

    /* The following three methods should become functions as they
       do not require access to private/protected class members. */
    static void add_options_to_parser(options::OptionParser& parser);
    static void add_succ_order_options(options::OptionParser& parser);
};

/*
  Print evaluator values of all evaluators evaluated in the evaluation context.
*/
extern void
print_initial_evaluator_values(const EvaluationContext& eval_context);

extern void collect_preferred_operators(
    EvaluationContext& eval_context,
    Evaluator* preferred_operator_evaluator,
    ordered_set::OrderedSet<OperatorID>& preferred_operators);

#endif
