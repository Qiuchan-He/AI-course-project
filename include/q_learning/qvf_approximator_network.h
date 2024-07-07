#ifndef PROBFD_Q_LEARNING_QVF_APPROXIMATOR_NETWORK_H
#define PROBFD_Q_LEARNING_QVF_APPROXIMATOR_NETWORK_H

#include "q_learning/qvf_approximator.h"
#include "q_learning/state_encoder.h"
#include "q_learning/trainable_network.h"

#include "downward/task_proxy.h"

#include <filesystem>
#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace q_learning {

/**
 * @brief Implements a double Q network with a prediction and target network.
 *
 * The prediction and target network initially have the same weights. On each
 * update call, only the parameters of the prediction network are updated. Every
 * \f$n\f$ updates, the weights of the prediction network are copied over to
 * the target network so that they are again identical.
 *
 * @ingroup q_learning
 */
class QVFApproximatorNetwork : public QVFApproximator {
public:
    /**
     * @brief Constructs a double Q network.
     *
     * @param task_proxy The planning task for which it is created. Must be
     * known to know how to encode a state as input to the network.
     * @param network The initial prediction network, which initially has the
     * same parameters as the target network.
     * @param updates_until_copy The number \f$n\f$ of successive updates
     * needed until the weights of the prediction model are copied over to the
     * target model.
     */
    QVFApproximatorNetwork(
        PlanningTaskProxy task_proxy,
        std::unique_ptr<TrainableNetwork>&& network,
        int updates_until_copy = 1);

    double get_qvalue(const State& state, OperatorID op) const override;
    double compute_max_qvalue(const State& state) const override;

    /**
     * @copybrief QVFApproximator::update
     *
     * Given an experience sample \f$(s, a, r, s')\f$, discount factor
     * \f$\gamma\f$ and learning rate \f$\alpha\f$, updates the parameters of
     * the prediction network with respect to the prediction and target values
     *
     * \f[
     *     P = Q(s, a) \quad \text{ and } \quad
     *     T =
     *     \begin{cases}
     *         r & s' \text{ is terminal}\\
     *         r + \gamma \max_{a' \in A(s)} Q(s', a') & \text{otherwise}
     *     \end{cases}
     * \f]
     */
    void update(
        const ExperienceSample& sample,
        double discount_factor,
        double learning_rate) override;

    StateEncoder state_encoder;
    std::unique_ptr<TrainableNetwork> prediction_model;
    std::unique_ptr<TrainableNetwork> target_model;
    int update_steps;
    int count;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_QVF_APPROXIMATOR_NETWORK_H
