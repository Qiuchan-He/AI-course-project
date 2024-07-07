#ifndef HEURISTIC_UTILS_H
#define HEURISTIC_UTILS_H

class ClassicalTask;
class Heuristic;

namespace tests {

/**
 * @brief Compute the estimate of the given heuristic for the given task's
 * initial state.
 *
 * @ingroup utils
 */
int get_initial_state_estimate(const ClassicalTask& task, Heuristic& heuristic);

} // namespace tests

#endif // TEST_UTILS_H
