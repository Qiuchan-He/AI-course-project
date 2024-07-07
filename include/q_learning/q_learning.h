#ifndef PROBFD_Q_LEARNING_Q_LEARNING_H
#define PROBFD_Q_LEARNING_Q_LEARNING_H

namespace q_learning {

class ExperienceSampler;
class QVFApproximator;

/**
 * @brief The Q-learning algorithm.
 *
 * @ingroup q_learning
 */
void run_q_learning(
    ExperienceSampler& sampler,
    QVFApproximator& qvf,
    double discount_factor,
    double learning_rate);

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_Q_LEARNING_H
