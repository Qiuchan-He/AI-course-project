#include "neuralfd/task_utils/predecessor_generator.h"

#include "neuralfd/task_utils/operator_generator_internals.h"
#include "neuralfd/task_utils/predecessor_generator_factory.h"
#include "neuralfd/task_utils/regression_task_proxy.h"

#include "downward/task_proxy.h"
#include "downward/task_utils/task_properties.h"

using namespace std;

namespace predecessor_generator {
PredecessorGenerator::PredecessorGenerator(
    const RegressionTaskProxy& regression_task_proxy)
    : root(PredecessorGeneratorFactory(regression_task_proxy).create())
    , ops(regression_task_proxy.get_regression_operators())
{
}

PredecessorGenerator::PredecessorGenerator(const ClassicalTaskProxy& task_proxy)
    : PredecessorGenerator(RegressionTaskProxy(*task_proxy.get_task()))
{
}

PredecessorGenerator::~PredecessorGenerator() = default;

void PredecessorGenerator::generate_applicable_ops(
    const PartialAssignment& assignment,
    vector<OperatorID>& applicable_ops) const
{
    root->generate_applicable_ops(
        assignment.get_unpacked_values(),
        applicable_ops);
    applicable_ops.erase(
        std::remove_if(
            applicable_ops.begin(),
            applicable_ops.end(),
            [&](const OperatorID& op_id) {
                return !ops[op_id.get_index()].achieves_subgoal(assignment);
            }),
        applicable_ops.end());
}

PerTaskInformation<PredecessorGenerator>
    g_predecessor_generators([](const PlanningTaskProxy& task_proxy) {
        return std::make_unique<PredecessorGenerator>(
            static_cast<ClassicalTaskProxy>(task_proxy));
    });
} // namespace predecessor_generator
