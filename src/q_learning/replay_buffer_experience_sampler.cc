#include "q_learning/replay_buffer_experience_sampler.h"

#include "q_learning/mdp_simulator.h"
#include "q_learning/policy.h"
#include "q_learning/qvf_approximator.h"
#include "q_learning/experience_sample.h"

#include "downward/utils/rng.h"
#include  <queue>


namespace q_learning {

std::optional<ExperienceSample> ReplayBufferExperienceSampler:: on_policy_sample(QVFApproximator& qvf_approximator){
    if(this->current_episode >= this->num_episodes){
        return std::nullopt;
    }

    std::optional<State> current_state =std::nullopt;
    if(this->next_state.has_value())
        current_state = this->next_state.value();
    else
        current_state = this->simulator->get_initial_state();
    
   
    std::vector<OperatorID> applicable_operators;
    this->simulator->generate_applicable_actions(current_state.value(), applicable_operators);
    OperatorID next_action = this->policy->choose_next_action(current_state.value(), applicable_operators,qvf_approximator);
    
    SampleResult sample_result = this->simulator->sample_next_state(current_state.value(), next_action);

   ExperienceSample current_sample(
        current_state.value(),
        next_action,
        sample_result.reward,
        sample_result.sampled_state,
        false);
        
    std::vector<OperatorID> terminal_actions;
    this->simulator->generate_applicable_actions(sample_result.sampled_state, terminal_actions);
    if(terminal_actions.size() == 0 )
        current_sample.terminal = true;

    this->current_step += 1;

    if(this->current_step >= this->max_expansions || current_sample.terminal){
        this->current_episode += 1;
        this->current_step = 0;
        this->next_state = std::nullopt;
    }else{
        this->next_state = sample_result.sampled_state;
    }
    
    return current_sample;
}


ReplayBufferExperienceSampler::ReplayBufferExperienceSampler(
    std::shared_ptr<MDPSimulator> simulator,
    std::shared_ptr<Policy> policy,
    size_t num_episodes,
    size_t max_expansions,
    size_t max_buffer_size,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
{
    
    this->simulator = simulator;
    this->policy = policy;
    this->num_episodes = num_episodes;
    this->max_expansions = max_expansions;
    this->max_buffer_size = max_buffer_size;
    this->rng = rng;
    this->current_episode = 0;
    this->current_step = 0;
    this->next_state = std::nullopt; 
    this->replay_buffer = std::queue<ExperienceSample>();
}

std::optional<ExperienceSample>
ReplayBufferExperienceSampler::sample_experience(
    QVFApproximator& qvf_approximator)
{
   std::optional<ExperienceSample> sample = on_policy_sample(qvf_approximator);

   std::queue<ExperienceSample> temp;

   

   if (sample.has_value()) {
        for (int i = 0; i < this->replay_buffer.size(); i++) {
            if (this->replay_buffer.front() == sample.value()) {
                this->replay_buffer.pop();
            } else {
                temp.push(this->replay_buffer.front());
                this->replay_buffer.pop();
            }
           }
           this->replay_buffer = temp;
           if(this->replay_buffer.size() == this->max_buffer_size)
               this->replay_buffer.pop();
           this->replay_buffer.push(sample.value());
       }else
            return std::nullopt;

    
    int  index = this->rng->random(this->replay_buffer.size());

    temp = this->replay_buffer;
    for (int i = 0; i < index; i++) {
        temp.pop();
    }

    return temp.front();
    
 }
} // namespace q_learning
