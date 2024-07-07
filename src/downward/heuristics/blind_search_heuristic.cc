#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"




namespace blind_search_heuristic {

BlindSearchHeuristic::BlindSearchHeuristic(const Options& opts)
    : BlindSearchHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts))
{
}

BlindSearchHeuristic::BlindSearchHeuristic(
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log)
    : Heuristic("blind", log, std::move(task))
{
    // TODO Initialize the heuristic, if needed.
}

int BlindSearchHeuristic::compute_heuristic(const State& state)
{
   GoalProxy goal = task_proxy.get_goal();
//    task_properties::

   bool is_goal = true;
   for (int i = 0; i<goal.size(); i++){
       // Do something with the goal fact...
         FactProxy goal_fact = goal[i];
         int index = goal_fact.get_variable().get_id();
            if(!(state[index].get_value() == goal_fact.get_value())){
                is_goal = false;
                break;   
        }
    }
    if(is_goal){
         return 0;
    }
    int min = 100000;
    OperatorsProxy operators = task_proxy.get_operators();
    for(OperatorProxy op : operators){
        int cost = op.get_cost();
        if(cost < min){
            min = cost;
        }
    }
    return min;
}

static std::shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Blind heuristic",
        "Returns cost of cheapest action for non-goal states, 0 for goal "
        "states");
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;
    return std::make_shared<BlindSearchHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("blind", _parse);
}// namespace blind_search_heuristic
