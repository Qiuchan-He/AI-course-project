#ifndef NEURALFD_TASK_UTILS_PREDECESSOR_GENERATOR_H
#define NEURALFD_TASK_UTILS_PREDECESSOR_GENERATOR_H

#include "neuralfd/task_utils/operator_generator_factory.h"
#include "neuralfd/task_utils/regression_task_proxy.h"

#include "downward/per_task_information.h"

#include <memory>
#include <vector>

class OperatorID;
class PartialAssignment;
class ClassicalTaskProxy;

namespace predecessor_generator {

class PredecessorGenerator {
    std::unique_ptr<operator_generator::GeneratorBase> root;
    const RegressionOperatorsProxy ops;

public:
    explicit PredecessorGenerator(const ClassicalTaskProxy& task_proxy);
    explicit PredecessorGenerator(
        const RegressionTaskProxy& regression_task_proxy);

    ~PredecessorGenerator();

    void generate_applicable_ops(
        const PartialAssignment& assignment,
        std::vector<OperatorID>& applicable_ops) const;
};

extern PerTaskInformation<PredecessorGenerator> g_predecessor_generators;
} // namespace predecessor_generator

#endif
