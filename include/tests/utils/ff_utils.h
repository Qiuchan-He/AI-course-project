#ifndef FF_UTILS_H
#define FF_UTILS_H

class ClassicalTask;
class OperatorID;

#include <optional>
#include <vector>

namespace ff_heuristic {
class FFHeuristic;
struct DeleteRelaxedPlan;
} // namespace ff_heuristic

namespace tests {

/**
 * @brief Invokes the compute_heuristic_and_relaxed_plan method of the given FF
 * heuristic on the initial state of the given planning task and returns the
 * result.
 *
 * @ingroup utils
 */
std::optional<ff_heuristic::DeleteRelaxedPlan> get_delete_relaxed_plan(
    const ClassicalTask& task,
    ff_heuristic::FFHeuristic& heuristic);

/**
 * @brief Checks if the given operator sequence constitutes a relaxed plan for
 * the given planning task.
 *
 * @ingroup utils
 */
bool verify_relaxed_plan(
    const ClassicalTask& task,
    const std::vector<OperatorID>& relaxed_plan,
    bool reject_superfluous_operators = false);

} // namespace tests

#endif // FF_UTILS_H
