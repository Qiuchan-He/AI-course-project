#include "q_learning/qvf_approximator_network.h"

#include "q_learning/experience_sample.h"
#include "q_learning/torch_networks.h"
#include "q_learning/trainable_network.h"

#include "downward/operator_id.h"
#include "downward/state.h"
#include "downward/task_proxy.h"

#include <ranges>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include <ATen/core/ivalue.h>

#include <ATen/core/Tensor.h>      // for definition of at::Tensor

#include <ATen/TensorOperators.h>  // for operator overloads (+, *, []) on tensors
#include <ATen/ops/max.h>          // for at::max

#include <torch/types.h>           // makes at::max available as torch::max

#include <torch/csrc/autograd/generated/variable_factories.h> // for torch::tensor

#ifdef WIN32
#pragma warning(pop)
#endif

namespace q_learning {

QVFApproximatorNetwork::QVFApproximatorNetwork(
    PlanningTaskProxy task_proxy,
    std::unique_ptr<TrainableNetwork>&& network,
    int updates_until_copy):state_encoder(task_proxy)
{
    this->prediction_model = network->clone();
    this->target_model = network->clone();
    this->update_steps = updates_until_copy;
    this->count = 0;
     
}

double
QVFApproximatorNetwork::get_qvalue(const State& state, OperatorID op) const
{
       at::Tensor encode_state = state_encoder.get_input_encoding(state);
       at::Tensor q_values = prediction_model->get_output(encode_state);
     
       if(q_values.size(0) > op.get_index())
           return q_values[op.get_index()].item<double>();
       else
           return 0.0;
 
}

double QVFApproximatorNetwork::compute_max_qvalue(const State& state) const
{
        at::Tensor encode_state = state_encoder.get_input_encoding(state);
        at::Tensor qValues = prediction_model->get_output(encode_state);
      
        if(qValues.size(0) == 0)
            return 0.0;
        else
            return torch::max(qValues).item<double>();
    
}

void QVFApproximatorNetwork::update(
    const ExperienceSample& sample,
    double discount_factor,
    double learning_rate)
{   
    at::Tensor encode_state = state_encoder.get_input_encoding(sample.state);
    at::Tensor q_values = prediction_model->get_output(encode_state);
    at::Tensor P = q_values[sample.action.get_index()];

    at::Tensor T = torch::zeros_like(P);
        if (sample.terminal) {
            T = torch::tensor(sample.reward);
        } else {
            at::Tensor encode_state = state_encoder.get_input_encoding(sample.successor);
            at::Tensor qValues = target_model->get_output(encode_state);

            T = torch::tensor(sample.reward) +
                discount_factor * torch::max(qValues);
        }

    prediction_model->update_parameters(P, T, learning_rate);
      
    if (count % update_steps == 0) {
        target_model = prediction_model->clone();
    }
    count++;
  
}

} // namespace q_learning
