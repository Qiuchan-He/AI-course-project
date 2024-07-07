#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/utils/logging.h"

namespace goal_count_heuristic {
GoalCountHeuristic::GoalCountHeuristic(const Options& opts)
    : GoalCountHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts))
{
}

GoalCountHeuristic::GoalCountHeuristic(
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log)
    : Heuristic("goal_counting", log, task)
{
    // TODO Initialize the heuristic, if needed.
}

int GoalCountHeuristic::compute_heuristic(const State& state)
{
    // TODO Return number of missing goal facts.

     GoalProxy goal = task_proxy.get_goal();

     int h = 0;
     for (int i = 0; i < goal.size(); i++) {
         // Do something with the goal fact...
         FactProxy goal_fact = goal[i];
         int index = goal_fact.get_variable().get_id();
            if(!(state[index].get_value() == goal_fact.get_value())){
                h++;
        }
    }
    return h;
}

static std::shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis("Goal count heuristic", "");
    parser.document_language_support("action costs", "ignored by design");
    parser.document_property("admissible", "no");
    parser.document_property("consistent", "no");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;
    return std::make_shared<GoalCountHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("goalcount", _parse);
} // namespace goal_count_heuristic
