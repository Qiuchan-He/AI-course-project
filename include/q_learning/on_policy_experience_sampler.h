#ifndef PROBFD_Q_LEARNING_ON_POLICY_EXPERIENCE_SAMPLER_H
#define PROBFD_Q_LEARNING_ON_POLICY_EXPERIENCE_SAMPLER_H

#include "q_learning/experience_sampler.h"

#include <memory>

namespace q_learning {

class Policy;
class MDPSimulator;

/**
 * @brief Represents a sampler that executes a policy step-by-step and returns
 * the experiences made.
 *
 * The sampler has two parameters, the exploration length \f$M\f$ and the
 * maximum number of episodes \f$N\f$. The policy is executed for \f$M\f$
 * steps or until a terminal state is reached, at which point the episode ends.
 * The next episode restarts from the initial state. After exactly \f$N\f$
 * episodes are completed, the sampler stops producing samples.
 *
 * @ingroup q_learning
 */
class OnPolicyExperienceSampler : public ExperienceSampler {
public:

    std::shared_ptr<MDPSimulator> simulator;
    std::shared_ptr<Policy> policy;
    size_t num_episodes;
    size_t max_expansions;
    size_t current_step;
    size_t current_episode;
    std::optional<State> next_state;

    OnPolicyExperienceSampler(
        std::shared_ptr<MDPSimulator> simulator,
        std::shared_ptr<Policy> policy,
        size_t num_episodes,
        size_t max_expansions);

    std::optional<ExperienceSample>
    sample_experience(QVFApproximator& qvf_approximator) override;
   
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_ON_POLICY_EXPERIENCE_SAMPLER_H
