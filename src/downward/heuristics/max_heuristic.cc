#include "downward/heuristics/max_heuristic.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/utils/logging.h"

namespace max_heuristic {

HSPMaxHeuristic::HSPMaxHeuristic(const Options& opts)
    : HSPMaxHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          utils::get_log_from_options(opts))
{
}

HSPMaxHeuristic::HSPMaxHeuristic(
    std::shared_ptr<ClassicalTask> task,
    utils::LogProxy log)
    : RelaxationHeuristic("hmax", std::move(task), std::move(log))
{
    // TODO Initialize the heuristic, if needed.
}

int HSPMaxHeuristic::compute_heuristic(const State& state)
{
    //initialize T table
    std::vector<FactProxy> facts;
    std::vector<int> factValues;
    FactsProxy all_facts(*task);

    for (FactProxy fact : all_facts) {
        facts.push_back(fact);
        VariableProxy fact_var = fact.get_variable();
        if (state[fact_var] == fact) {
            factValues.push_back(0);
        } else {
            factValues.push_back(1000000);
        }
    }    

    //update T table
    bool flag = true;
    while (flag) {
        //flag to check if the fact values did not change
        flag = false;
        OperatorsProxy operators = task_proxy.get_operators();
            for (OperatorProxy op : operators) {
                PreconditionProxy pre_fact = op.get_precondition();
                bool is_applicable = true;
                int max_pre_fact_value = 0;
                for(FactProxy fact : pre_fact){
                    int fact_index = std::find(facts.begin(), facts.end(), fact) - facts.begin();
                    int fact_value = factValues[fact_index];
                    if (fact_value == 1000000) {
                        is_applicable = false;
                        break;
                    }else{
                        if (max_pre_fact_value < fact_value) {
                            max_pre_fact_value = fact_value;
                        }
                    }
                }

                if (is_applicable) {
                    int op_cost = op.get_cost();
                    EffectProxy eff_fact = op.get_effect();
                    for(FactProxy fact : eff_fact){
                        int fact_index = std::find(facts.begin(), facts.end(), fact) - facts.begin();
                        if (max_pre_fact_value + op_cost < factValues[fact_index]) {
                            factValues[fact_index] = max_pre_fact_value + op_cost;
                            flag = true;
                        }
                    }
                }
        }
    }
    
    
    //get heuristic value of the state
    GoalProxy goal = task_proxy.get_goal();
    int cost = 0;
    int goal_size = goal.size();

    if(goal_size == 0){
        return 0;
    }else if (goal_size == 1) {
        auto it = std::find(facts.begin(), facts.end(), goal[0]) - facts.begin();
        return factValues[it] == 1000000 ? DEAD_END : factValues[it];
    }else{
        for(FactProxy goal_fact : goal){
            auto it = std::find(facts.begin(), facts.end(), goal_fact) - facts.begin();
            int goalValue = factValues[it];
            if (cost < goalValue) {
                cost = goalValue;
            }
        }
    }
    return cost == 1000000 ? DEAD_END : cost;
}

static std::shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis("Max heuristic", "");
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;
    return std::make_shared<HSPMaxHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("hmax", _parse);
} // namespace max_heuristic
