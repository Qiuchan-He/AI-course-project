#ifndef PROBFD_Q_LEARNING_EXPERIENCE_SAMPLER_H
#define PROBFD_Q_LEARNING_EXPERIENCE_SAMPLER_H

#include "q_learning/experience_sample.h"

#include <optional>

namespace q_learning {

class QVFApproximator;

/**
 * @brief This interface represents a sampling method that samples a stream of
 * experiences \f$(s, a, r, s')\f$ of the environment.
 *
 * @ingroup q_learning
 */
class ExperienceSampler {
public:
    virtual ~ExperienceSampler() = default;

    /**
     * @brief Sample the next experience. If no more experiences are generated,
     * returns std::nullopt.
     */
    virtual std::optional<ExperienceSample>
    sample_experience(QVFApproximator& qvf_approximator) = 0;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_EXPERIENCE_SAMPLER_H
