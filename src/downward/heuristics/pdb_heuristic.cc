#include "downward/heuristics/pdb_heuristic.h"
#include "downward/pdbs/syntactic_projection.h"
#include "downward/task_utils/task_properties.h"
#include "downward/option_parser.h"
#include "downward/plugin.h"

#include <limits>
#include <memory>

using namespace std;

namespace pdbs {

PDBHeuristic::PDBHeuristic(const Options& opts)
    : PDBHeuristic(
          opts.get<std::shared_ptr<ClassicalTask>>("transform"),
          opts.get_list<int>("pattern"),
          utils::get_log_from_options(opts))
{
}

bool PDBHeuristic::is_abstract_goal_state(const SyntacticProjection projection, const State& state){
    for (int i = 0; i < projection.get_num_goal_facts(); i++) {
        if (state[projection.get_goal_fact(i).var].get_pair() != projection.get_goal_fact(i))      return false;
    }
    return true;
}

bool  PDBHeuristic::is_abstract_operation_applicable(const std::vector<FactPair> fact_pairs, const State& state){
    for (int i = 0; i < fact_pairs.size(); i++) {
        if (state[fact_pairs[i].var].get_value() != fact_pairs[i].value)      return false;
    }
    return true;
}

int PDBHeuristic::find_successor(const State& state, const std::vector<FactPair> fact_pairs){
    std::vector<int> values;
    int i;
    int j = 0;
    for (i = 0; i<state.size()&&j<fact_pairs.size(); i++){
        if(state[i].get_pair().var !=fact_pairs[j].var){
            values.push_back(state[i].get_value());
        }
        else{
            values.push_back(fact_pairs[j].value);
            j++;
        }
    }
    if(i<state.size()){
        for(;i<state.size();i++){
            values.push_back(state[i].get_value());
        }
    }
    int index = projection->compute_index(values);
    return index;

}
PDBHeuristic::PDBHeuristic(
    std::shared_ptr<ClassicalTask> task,
    Pattern pattern,
    utils::LogProxy log)
    : Heuristic("pdb", log, task)
{
    std::vector<State> A_States;
    std::vector<bool> visited;
    this->pattern = pattern;
    this->projection = new SyntacticProjection(*task, pattern);

    for (int i = 0; i < projection->get_num_states(); i++) {
        State state = projection->get_state_for_index(i);
        A_States.push_back(state);
        if(is_abstract_goal_state(*projection,state)){
            values.push_back(0);
           visited.push_back(true);
        }
        else{
            values.push_back(1000000);
            visited.push_back(false);
        }
    }


    struct edge{
        int to;
        edge* next;
        int cost;
    };

    struct head{
        int from;
        edge* next;
    };
    std::vector<head> heads;
    for (int i = 0; i < projection->get_num_states(); i++) {
        head tmp;
        tmp.from = i;
        tmp.next = nullptr;
        heads.push_back(tmp);
    }
   
    //construct graph
    for (int i = 0; i < projection->get_num_states(); i++) {
       State state = A_States[i];
       if(is_abstract_goal_state(*projection,state)) continue;
       for (int j = 0; j< projection->get_num_operators(); j++){
            std::vector<FactPair> factpairs;
            for(int k = 0; k < projection->	get_num_operator_precondition_facts (j); k++){
                factpairs.push_back(projection->get_operator_precondition_fact(j,k));
            }
           if(is_abstract_operation_applicable(factpairs, state)){
               //to find successor
               std::vector<FactPair> effct_factpairs;
                for(int l = 0; l < projection->	get_num_operator_effect_facts (j); l++){
                      effct_factpairs.push_back(projection->get_operator_effect_fact(j,l));
                }
               int succ_index = find_successor(state, effct_factpairs);
               if(succ_index == i)
                    continue;
               edge* p = heads[succ_index].next;
               edge* tmp = new edge;
               tmp->to = i;
               tmp->cost = projection->get_operator_cost(j);
               tmp->next = nullptr;
                
                if(p == nullptr){
                    heads[succ_index].next = tmp;
                }
                else{
                    bool add = true;
                    while(p->next != nullptr){
                        if(p->to == i){
                            add = false;
                            break;
                        }
                        p = p->next;
                    }
                    if (add&&p->to != i){ 
                        p->next = tmp;
                    }
                       
                }
               
                effct_factpairs.clear();
           }
           factpairs.clear();
       }
    }
   

    //run Dijsktra
     
    std::vector<int> goal_index;
    for(int i = 0; i < values.size(); i++){
        if(values[i] == 0){
            goal_index.push_back(i);
        }
    }

    for (int i = 0; i < goal_index.size(); i++) {
        visited[goal_index[i]] = false;
        
        while(true){
            int min_index = -1;
            int min = INT_MAX;
            for(int k = 0; k < values.size(); k++){
                if(!visited[k] && values[k] < min){
                    min = values[k];
                    min_index = k;
                }
            }
            if(min_index == -1) 
            break;

            visited[min_index] = true;
            edge* p = heads[min_index].next;
            while(p!=nullptr){
                if(!visited[p->to] && values[p->to] > min + p->cost){
                    values[p->to] = min + p->cost;
                }
                p = p->next;
            }
    }

    for(int k = 0; k < visited.size(); k++){
       visited[k] = false;
    }
    for(int k = 0; k < goal_index.size(); k++){
        visited[goal_index[k]] = true;
    }

    }

}

int PDBHeuristic::compute_heuristic(const State& state)
{
    // TODO return the PDB heuristic value for state, i.e. lookup the
    // cost-to-goal value of the abstract state corresponding to state.
   
    std::vector<int> factpair_values;
    for (int i = 0; i < pattern.size();i++){
        int tmp = state[pattern[i]].get_value();
        factpair_values.push_back(tmp);
    }
    int index = projection->compute_index(factpair_values);
    
    if(values[index]==1000000){
        return -1;
    }
    return values[index];

}


static shared_ptr<Heuristic> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Pattern database heuristic",
        "The pattern database heuristic, with respect to a single pattern.");
    parser.document_language_support("action costs", "supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    parser.add_list_option<int>(
        "pattern",
        "the pattern for the projection",
        "[]");
    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PDBHeuristic>(opts);
}

static Plugin<Evaluator> _plugin("pdb", _parse, "heuristics_pdb");
} // namespace pdbs
