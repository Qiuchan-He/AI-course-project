#include "q_learning/torch_networks.h"

#include <exception>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

// HACK: This only exists because of incomplete type errors within torchlib.
#include <ATen/core/ivalue.h>

#include <torch/jit.h>
#include <torch/script.h>

#include <torch/optim/sgd.h>

#include <torch/nn/init.h>
#include <torch/nn/modules/activation.h>
#include <torch/nn/modules/container/sequential.h>
#include <torch/nn/modules/linear.h>
#include <torch/nn/modules/loss.h>

#ifdef WIN32
#pragma warning(pop)
#endif

namespace q_learning {

class TorchTrainableNetwork : public TrainableNetwork {
public:
    void update_parameters(
        at::Tensor prediction,
        at::Tensor target,
        double learning_rate) override;

private:
    /// Returns the parameters of the model to pass them to an optimizer.
    virtual std::vector<at::Tensor> get_parameters() = 0;
};

void TorchTrainableNetwork::update_parameters(
    at::Tensor prediction,
    at::Tensor target,
    double learning_rate)
{
    // Must be scalars!
    assert(prediction.sizes().size() == 0);
    assert(target.sizes().size() == 0);

    // Calculate loss
    torch::nn::MSELoss loss_fn;
    at::Tensor loss = loss_fn(prediction, target);

    // Do one step of gradient descent
    torch::optim::SGD optimizer(
        this->get_parameters(),
        torch::optim::SGDOptions(learning_rate));
    optimizer.zero_grad();
    loss.backward();
    optimizer.step();
}

class TorchFullyConnectedNetwork : public TorchTrainableNetwork {
    mutable torch::nn::Sequential model;

public:
    // Constructs the network from the given parameters.
    explicit TorchFullyConnectedNetwork(
        int input_layer_neurons,
        int hidden_layers,
        int hidden_layer_neurons,
        int out_layer_neurons,
        int random_seed);

    TorchFullyConnectedNetwork(const TorchFullyConnectedNetwork&);

    ~TorchFullyConnectedNetwork() override = default;

    at::Tensor get_output(const at::Tensor& input) const override;

    std::unique_ptr<TrainableNetwork> clone() override;

private:
    std::vector<at::Tensor> get_parameters() override;
};

static torch::nn::Sequential construct_fully_connected_ReLU(
    int input_layer_neurons,
    int hidden_layers,
    int hidden_layer_neurons,
    int out_layer_neurons,
    int random_seed)
{
    torch::manual_seed(random_seed);

    torch::nn::Sequential model;

    if (hidden_layers == 0) {
        torch::nn::Linear linear(input_layer_neurons, out_layer_neurons);
        torch::nn::init::constant_(linear->weight, 0);
        torch::nn::init::constant_(linear->bias, 0);
        model->push_back(linear);
        return model;
    }

    torch::nn::Linear input(input_layer_neurons, hidden_layer_neurons);
    model->push_back(input);
    torch::nn::init::kaiming_normal_(
        input->weight,
        0.0,
        torch::kFanIn,
        torch::kReLU);
    torch::nn::init::constant_(input->bias, 0);

    for (int i = 0; i != hidden_layers; ++i) {
        torch::nn::Linear linear(hidden_layer_neurons, hidden_layer_neurons);
        torch::nn::init::kaiming_normal_(
            linear->weight,
            0.0,
            torch::kFanIn,
            torch::kReLU);
        torch::nn::init::constant_(linear->bias, 0);
        model->push_back(linear);
        model->push_back(torch::nn::ReLU());
    }

    torch::nn::Linear last(hidden_layer_neurons, out_layer_neurons);
    torch::nn::init::constant_(last->weight, 0);
    torch::nn::init::constant_(last->bias, 0);
    model->push_back(last);

    return model;
}

TorchFullyConnectedNetwork::TorchFullyConnectedNetwork(
    int input_layer_neurons,
    int hidden_layers,
    int hidden_layer_neurons,
    int out_layer_neurons,
    int random_seed)
    : model(construct_fully_connected_ReLU(
          input_layer_neurons,
          hidden_layers,
          hidden_layer_neurons,
          out_layer_neurons,
          random_seed))
{
}

TorchFullyConnectedNetwork::TorchFullyConnectedNetwork(
    const TorchFullyConnectedNetwork& other)
    : model(std::dynamic_pointer_cast<torch::nn::SequentialImpl>(
          other.model->clone()))
{
}

at::Tensor
TorchFullyConnectedNetwork::get_output(const at::Tensor& inputs) const
{
    return model->forward(inputs);
}

std::unique_ptr<TrainableNetwork> TorchFullyConnectedNetwork::clone()
{
    return std::make_unique<TorchFullyConnectedNetwork>(*this);
}

std::vector<at::Tensor> TorchFullyConnectedNetwork::get_parameters()
{
    return model->parameters();
}

class TorchScriptNetwork : public TorchTrainableNetwork {
    mutable torch::jit::script::Module model;

public:
    // Constructs the network by reading it from a model file.
    explicit TorchScriptNetwork(std::string filepath);

    // Constructs the network by reading it from a model file.
    explicit TorchScriptNetwork(std::filesystem::path filepath);

    TorchScriptNetwork(const TorchScriptNetwork&);

    ~TorchScriptNetwork() = default;

    at::Tensor get_output(const at::Tensor& inputs) const override;

    std::unique_ptr<TrainableNetwork> clone() override;

private:
    std::vector<at::Tensor> get_parameters() override;
};

TorchScriptNetwork::TorchScriptNetwork(std::string filepath)
    : model(torch::jit::load(filepath))
{
}

TorchScriptNetwork::TorchScriptNetwork(std::filesystem::path filepath)
    : model(torch::jit::load(filepath.string()))
{
}

TorchScriptNetwork::TorchScriptNetwork(const TorchScriptNetwork& other)
    : model(other.model.clone())
{
}

at::Tensor TorchScriptNetwork::get_output(const at::Tensor& input) const
{
    return model.forward({input}).toTensor();
}

std::vector<at::Tensor> TorchScriptNetwork::get_parameters()
{
    std::vector<at::Tensor> params;

    for (auto val : model.parameters()) {
        params.push_back(val);
    }

    return params;
}

std::unique_ptr<TrainableNetwork> TorchScriptNetwork::clone()
{
    return std::make_unique<TorchScriptNetwork>(*this);
}

std::unique_ptr<TrainableNetwork> create_fully_connected_torch_network(
    int input_layer_neurons,
    int hidden_layers,
    int hidden_layer_neurons,
    int out_layer_neurons,
    int random_seed)
{
    return std::make_unique<TorchFullyConnectedNetwork>(
        input_layer_neurons,
        hidden_layers,
        hidden_layer_neurons,
        out_layer_neurons,
        random_seed);
}

std::unique_ptr<TrainableNetwork>
load_torch_network_from_file(std::string filepath)
{
    try {
        return std::make_unique<TorchScriptNetwork>(std::move(filepath));
    } catch (const at::Error& error) {
        throw std::runtime_error(error.msg());
    }
}

std::unique_ptr<TrainableNetwork>
load_torch_network_from_file(std::filesystem::path filepath)
{
    try {
        return std::make_unique<TorchScriptNetwork>(std::move(filepath));
    } catch (const at::Error& error) {
        throw std::runtime_error(error.msg());
    }
}

} // namespace q_learning