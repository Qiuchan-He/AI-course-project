#ifndef Q_LEARNING_TORCH_NETWORKS_H
#define Q_LEARNING_TORCH_NETWORKS_H

#include "q_learning/trainable_network.h"

#include <filesystem>
#include <memory>

namespace q_learning {

std::unique_ptr<TrainableNetwork> create_fully_connected_torch_network(
    int input_layer_neurons,
    int hidden_layers,
    int hidden_layer_neurons,
    int out_layer_neurons,
    int random_seed);

std::unique_ptr<TrainableNetwork>
load_torch_network_from_file(std::string filepath);

std::unique_ptr<TrainableNetwork>
load_torch_network_from_file(std::filesystem::path filepath);

} // namespace q_learning

#endif // Q_LEARNING_TORCH_NETWORKS_H
