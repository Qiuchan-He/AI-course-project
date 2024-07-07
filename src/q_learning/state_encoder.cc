#include "q_learning/state_encoder.h"

#include "downward/state.h"

#include <vector>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include <ATen/core/ivalue.h>
#include <ATen/core/Tensor.h>
#include <torch/csrc/autograd/generated/variable_factories.h>

#ifdef WIN32
#pragma warning(pop)
#endif

namespace q_learning {

static std::vector<int> get_fact_index_vector(PlanningTaskProxy task_proxy)
{
    auto variables = task_proxy.get_variables();

    std::vector<int> facts_until;

    facts_until.reserve(variables.size() + 1);

    facts_until.push_back(0);
    for (auto var : variables) {
        facts_until.push_back(facts_until.back() + var.get_domain_size());
    }

    return facts_until;
}

StateEncoder::StateEncoder(PlanningTaskProxy task_proxy)
    : facts_until(get_fact_index_vector(task_proxy))
{
}

int StateEncoder::get_index(FactPair fact) const
{
    return facts_until[fact.var] + fact.value;
}

int StateEncoder::num_facts() const
{
    return facts_until.back();
}

at::Tensor StateEncoder::get_input_encoding(const State& state) const
{
    std::vector<double> inputs(num_facts(), 0.0);

    // Set input values
    for (const auto& fact : state) {
        inputs[get_index(fact.get_pair())] = 1;
    }

    return torch::tensor(inputs);
}

} // namespace q_learning
