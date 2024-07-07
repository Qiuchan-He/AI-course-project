#ifndef NEURALFD_NEURAL_NETWORKS_TORCH_NETWORK_H
#define NEURALFD_NEURAL_NETWORKS_TORCH_NETWORK_H

#include "neuralfd/neural_networks/abstract_network.h"

#include "downward/option_parser.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4702)
#endif
// HACK: This only exists because of incomplete type errors within torchlib.
#include <ATen/core/ivalue.h>
#include <torch/script.h>
#ifdef WIN32
#pragma warning(pop)
#endif

#include <string>
#include <vector>

namespace neural_networks {
/* Base class for all networks which use torch. */
class TorchNetwork : public AbstractNetwork {
protected:
    const std::shared_ptr<ClassicalTask> task;
    ClassicalTaskProxy task_proxy;

    /* Path to the stored model */
    const std::string path;

    /* Torch model */
    torch::jit::script::Module module;

    /* Flag if network is initialized */
    bool is_initialized = false;

    /*
     * Returns the required input tensors for an forward pass of the given
     * state
     * @param state state for which the tensor is constructed
     * @return tensor a tensor for a forward pass
     */
    virtual std::vector<at::Tensor> get_input_tensors(const State& state) = 0;
    /*
     * Parse the raw output of the network and process it.
     * @param count Number of predictions of the network to extract (
     * by default all)
     */
    virtual void parse_output(const torch::jit::IValue& output) = 0;
    virtual void clear_output() = 0;

    virtual void initialize() override;
    virtual void evaluate(const State& state) override;
    virtual void evaluate(const std::vector<State>& states) override;

public:
    explicit TorchNetwork(const Options& opts);
    TorchNetwork(const TorchNetwork& orig) = delete;
    virtual ~TorchNetwork() override;

    /*
     * Add the parser options for the TorchNetwork.
     * @param parser OptionsParser to which the options shall be added.
     */
    static void add_options_to_parser(options::OptionParser& parser);
};
} // namespace neural_networks
#endif /* NEURAL_NETWORKS_TORCH_NETWORK_H */
