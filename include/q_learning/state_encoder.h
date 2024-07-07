#ifndef PROBFD_Q_LEARNING_STATE_ENCODER_H
#define PROBFD_Q_LEARNING_STATE_ENCODER_H

#include "downward/task_proxy.h"

#include <vector>

namespace at {
class Tensor;
}

namespace q_learning {

/**
 * @brief Encodes a state into a vector of 0/1 values, to be fed to a machine
 * learning model.
 *
 * If \f$\mathcal{F}\f$ is the set of facts of the task, then the encoding for
 * \f$s\f$ is \f$x_s := (x_f)_{f \in \mathcal{F}}\f$ with \f$x_f := 1\f$ if
 * \f$f \in s\f$ and \f$x_f := 0\f$ otherwise.
 *
 * For this encoding, facts are ordered lexicographically, primarily in
 * increasing order of variable index, secondarily in increasing order of
 * variable value.
 *
 * @ingroup q_learning
 */
class StateEncoder {
    const std::vector<int> facts_until;

    int get_index(FactPair fact) const;

public:
    /**
     * @brief Constructs a state encoder for a given planning task.
     */
    explicit StateEncoder(PlanningTaskProxy task_proxy);

    /**
     * @brief Encodes a state into a vector of 0/1 values.
     */
    at::Tensor get_input_encoding(const State& state) const;

    /**
     * @brief Returns the number of facts of the planning task for which the
     * encoder was constructed.
     */
    int num_facts() const;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_STATE_ENCODER_H
