#ifndef PROBFD_Q_LEARNING_REPLAY_BUFFER_EXPERIENCE_SAMPLER_H
#define PROBFD_Q_LEARNING_REPLAY_BUFFER_EXPERIENCE_SAMPLER_H

#include "q_learning/experience_sampler.h"

#include "downward/utils/rng.h"

#include <queue>
#include <memory>

namespace q_learning {

class Policy;
class MDPSimulator;

/**
 * @brief Represents a sampler that extends an ordinary on-policy sampler with a
 * replay buffer.
 *
 * This sampler also does a step-by-step exploration of a policy in the same
 * way as OnPolicyExperienceSampler, but stores the experiences made during
 * exploration in an internal replay buffer. The replay buffer is limited
 * to a size of \f$S\f$ samples. If the replay buffer is full when a new sample
 * is generated during exploration, the oldest sample is ejected and the new
 * one added. To report the next sample to the user, the sampler draws uniformly
 * from the replay buffer.
 *
 * @ingroup q_learning
 */
class ReplayBufferExperienceSampler : public ExperienceSampler {
public:
    std::shared_ptr<MDPSimulator> simulator;
    std::shared_ptr<Policy> policy;
    size_t num_episodes;
    size_t max_expansions;
    size_t max_buffer_size;
    size_t current_episode;
    size_t current_step;
    std::optional<State> next_state;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    std::queue<ExperienceSample> replay_buffer;


    ReplayBufferExperienceSampler(
        std::shared_ptr<MDPSimulator> simulator,
        std::shared_ptr<Policy> policy,
        size_t num_episodes,
        size_t max_expansions,
        size_t max_buffer_size,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    std::optional<ExperienceSample>
    sample_experience(QVFApproximator& qvf_approximator) override;
    std::optional<ExperienceSample> on_policy_sample(QVFApproximator& qvf_approximator);
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_REPLAY_BUFFER_EXPERIENCE_SAMPLER_H
