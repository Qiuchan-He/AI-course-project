#ifndef SEARCH_UTILS_H
#define SEARCH_UTILS_H

#include <memory>

class ClassicalTask;
class Evaluator;
class SearchAlgorithm;

namespace tests {

/**
 * @brief Creates an A* search algorithm with suppressed log output.
 *
 * @ingroup utils
 */
std::unique_ptr<SearchAlgorithm> create_astar_search_algorithm(
    std::shared_ptr<ClassicalTask> task,
    std::shared_ptr<Evaluator> evaluator);

} // namespace tests

#endif // SEARCH_UTILS_H
