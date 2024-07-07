#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include "downward/task_proxy.h"

#include <memory>
#include <string_view>

class ClassicalTask;
class OperatorID;

namespace probfd {
class ProbabilisticTask;
} // namespace probfd

namespace tests {

/**
 * @brief Reads a task from a .sas file with the given filename. The file must
 * be in the resources/problems directory of the project.
 *
 * By default, the run script of the planner (fast_downward.py) emits the
 * translated plannig task file for a given pddl problem via a file named
 * 'output.sas'. A different name can be specified via the --sas-file option.
 * Example:
 * `./fast-downward.py --sas-file sokoban_p01.sas domains/sokoban/p01.pddl`
 *
 * @ingroup utils
 */
std::unique_ptr<ClassicalTask>
read_task_from_file(std::string_view task_filename);

/**
 * @brief Reads a plan for the specified task from a .plan file with the given
 * filename. The file must be in the resources/plans directory of the project.
 *
 * By default, the run script of the planner (fast_downward.py) emits the plan
 * for the search problem if found via a file named 'sas_plan'. A different name
 * can be specified via the --plan-file option:
 * `./fast-downward.py --plan-file <filename> domains/sokoban/p01.pddl
 * --search ...`.
 *
 * @ingroup utils
 */
std::vector<OperatorID> read_plan_from_file(
    ClassicalTaskProxy task_proxy,
    std::string_view plan_filename);

} // namespace tests

#endif // INPUT_UTILS_H
