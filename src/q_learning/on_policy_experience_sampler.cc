#include "q_learning/on_policy_experience_sampler.h"

#include "q_learning/mdp_simulator.h"
#include "q_learning/policy.h"
#include "q_learning/qvf_approximator.h"
#include "q_learning/experience_sample.h"
#include "downward/task_utils/task_properties.h"
#include "downward/state_id.h"

namespace q_learning {

OnPolicyExperienceSampler::OnPolicyExperienceSampler(
    std::shared_ptr<MDPSimulator> simulator,
    std::shared_ptr<Policy> policy,
    size_t num_episodes,
    size_t max_expansions)
{
    this->simulator = simulator;
    this->policy = policy;
    this->num_episodes = num_episodes;
    this->max_expansions = max_expansions;
    this->current_episode = 0;
    this->current_step = 0;
    this->next_state = std::nullopt;
}


std::optional<ExperienceSample>
OnPolicyExperienceSampler::sample_experience(QVFApproximator& qvf_approximator)
{

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

} // namespace q_learning
