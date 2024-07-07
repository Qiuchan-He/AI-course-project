#ifndef PROBFD_Q_LEARNING_QVF_APPROXIMATOR_TABLE_H
#define PROBFD_Q_LEARNING_QVF_APPROXIMATOR_TABLE_H

#include "q_learning/qvf_approximator.h"
#include "downward/state.h"

namespace q_learning {

/**
 * @brief Implements a table-based Q value function consisting of
 * \f$|S|^{|A|}\f$ entries in the worst case.
 *
 * @ingroup q_learning
 */
class QValueTable : public QVFApproximator {
public:
    /**
     * @brief Construct the Q value function that maps each state-action pair to
     * zero.
     */
    

    QValueTable();

    double get_qvalue(const State& state, OperatorID op) const override;
    double compute_max_qvalue(const State& state) const override;

    /**
     * @copybrief QVFApproximator::update
     *
     * Given an experience sample \f$(s, a, r, s')\f$, discount factor
     * \f$\gamma\f$ and learning rate \f$\alpha\f$, applies the temporal
     * difference update
     *
     * \f[
     *     Q(s, a) \gets
     *     \begin{cases}
     *         Q(s, a) + \alpha (r - Q(s, a))
     *             & s' \text{ is terminal}\\
     *         Q(s, a) + \alpha (r + \gamma \max_{a' \in A(s)} Q(s', a') -
     *         Q(s, a))
     *             & \text{otherwise}
     *     \end{cases}
     * \f]
     */
    void update(
        const ExperienceSample& sample,
        double discount_factor,
        double learning_rate) override;
        
    std::vector<std::pair<State, OperatorID>> state_action;
    std::vector<double>q_values;
};

} // namespace q_learning

#endif
