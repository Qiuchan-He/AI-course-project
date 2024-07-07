#ifndef TASK_UTILS_H
#define TASK_UTILS_H

#include "probfd/task_proxy.h"

#include <memory>

class ClassicalTask;

namespace probfd {
class ProbabilisticTask;
}

namespace tests {

/**
 * @brief Converts a deterministic task to a probabilistic task.
 *
 * @ingroup utils
 */
std::shared_ptr<probfd::ProbabilisticTask>
as_probabilistic_task(std::shared_ptr<ClassicalTask> deterministic_task);

} // namespace tests

#endif // UTILS_H
