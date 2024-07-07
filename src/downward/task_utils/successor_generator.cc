#include "downward/task_utils/successor_generator.h"

#include "downward/task_utils/successor_generator_factory.h"
#include "downward/task_utils/successor_generator_internals.h"

#include "downward/planning_task.h"
#include "downward/state.h"

using namespace std;

namespace successor_generator {
SuccessorGenerator::SuccessorGenerator(const PlanningTaskProxy& task_proxy)
    : root(SuccessorGeneratorFactory(task_proxy).create())
{
}

SuccessorGenerator::~SuccessorGenerator() = default;

void SuccessorGenerator::generate_applicable_ops(
    const State& state,
    vector<OperatorID>& applicable_ops) const
{
    root->generate_applicable_ops(state.get_unpacked_values(), applicable_ops);
}

PerTaskInformation<SuccessorGenerator> g_successor_generators;
} // namespace successor_generator
