#ifndef PROBFD_Q_LEARNING_QVF_APPROXIMATOR_H
#define PROBFD_Q_LEARNING_QVF_APPROXIMATOR_H

#include "downward/state.h"

namespace q_learning {

struct ExperienceSample;

/**
 * @brief This interface represents a Q value function.
 *
 * @ingroup q_learning
 */
class QVFApproximator {
public:
    virtual ~QVFApproximator() = default;

    /**
     * @brief Returns the value \f$Q(s, a)\f$ as represented by this class.
     */
    virtual double get_qvalue(const State&, OperatorID) const = 0;

    /**
     * @brief Returns the value \f$\max_{a \in A(s)} Q(s, a)\f$ as represented
     * by this class.
     */
    virtual double compute_max_qvalue(const State&) const = 0;

    /**
     * @brief Updates the Q value function with the provided experience sample,
     * with respect to the given discount factor and learning rate.
     */
    virtual void update(
        const ExperienceSample& sample,
        double discount_factor,
        double learning_rate) = 0;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_QVF_APPROXIMATOR_H
