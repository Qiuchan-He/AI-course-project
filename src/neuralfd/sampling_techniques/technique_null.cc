#include "neuralfd/sampling_techniques/technique_null.h"

using namespace std;

namespace sampling_technique {
const std::string TechniqueNull::name = "null";

const string& TechniqueNull::get_name() const
{
    return name;
}

TechniqueNull::TechniqueNull()
    : SamplingTechnique(0, false, false, nullptr)
{
}

std::shared_ptr<ClassicalTask> TechniqueNull::create_next(
    shared_ptr<ClassicalTask> /*seed_task*/,
    const ClassicalTaskProxy&)
{
    return nullptr;
}

} // namespace sampling_technique