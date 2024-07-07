#ifndef PROBFD_Q_LEARNING_TRAINABLE_NETWORK_H
#define PROBFD_Q_LEARNING_TRAINABLE_NETWORK_H

#include <memory>
#include <string>
#include <vector>

namespace at {
class Tensor;
}

namespace q_learning {

/**
 * @brief Represents a neural network that can be evaluated for an input and
 * trained.
 *
 * @ingroup q_learning
 */
class TrainableNetwork {
public:
    virtual ~TrainableNetwork() = default;

    /**
     * @brief Evaluates the network on the given input tensor (vector).
     *
     * Each entry in the input vector holds the input value for a neuron in
     * the input layer. The result is a vector of outputs, one for each neuron
     * in the output layer.
     */
    virtual at::Tensor get_output(const at::Tensor& inputs) const = 0;

    /**
     * @brief Produces a copy of the network.
     */
    virtual std::unique_ptr<TrainableNetwork> clone() = 0;

    /**
     * @brief Updates the parameters of the model according to the specified
     * prediction and target values, with respect to the given learning rate.
     */
    virtual void update_parameters(
        at::Tensor prediction,
        at::Tensor target,
        double learning_rate) = 0;
};

} // namespace q_learning

#endif // PROBFD_Q_LEARNING_TRAINABLE_NETWORK_H
