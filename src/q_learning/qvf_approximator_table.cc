#include "q_learning/qvf_approximator_table.h"

#include "downward/operator_id.h"
#include "downward/state.h"
#include "downward/utils/system.h"
#include "q_learning/experience_sample.h"

namespace q_learning {

QValueTable::QValueTable()
{
   
}

double QValueTable::get_qvalue(const State& state, OperatorID op) const
{   
    std::pair<State, OperatorID> key = std::make_pair(state, op);
    for(int i = 0; i < state_action.size(); i++){
        if(state_action[i] == key){
            return q_values[i];
        }
    }
    return 0.0;
}

double QValueTable::compute_max_qvalue(const State& state) const
{
    double max_qvalue = 0.0;
    for(int i = 0; i < state_action.size(); i++){
        if(state_action[i].first == state){
            if(q_values[i] > max_qvalue){
                max_qvalue = q_values[i];
            }
        }
    }
    return max_qvalue;
}

void QValueTable::update(
    const ExperienceSample& sample,
    double discount_factor,
    double learning_rate)
{       
        double r = sample.reward;
        double q_value = get_qvalue(sample.state, sample.action);
        double new_q_value = 0.0;

        if (sample.terminal) {
            new_q_value = q_value + learning_rate * (r - q_value);
        } else {
            double max_next_q_value = compute_max_qvalue(sample.successor);
            new_q_value = q_value + learning_rate * (r + discount_factor * max_next_q_value - q_value);
        }
        
        std::pair<State, OperatorID> key = std::make_pair(sample.state, sample.action);
        for(int i = 0; i < state_action.size(); i++){
            if(state_action[i] == key){
                q_values[i] = new_q_value;
                return;
            }
        }
        state_action.push_back(key);
        q_values.push_back(new_q_value);

       
    }

}// namespace q_learning
