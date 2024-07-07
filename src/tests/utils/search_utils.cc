#include "tests/utils/search_utils.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

#include "downward/open_list_factory.h"

#include <set>

namespace tests {

std::unique_ptr<SearchAlgorithm> create_astar_search_algorithm(
    std::shared_ptr<ClassicalTask> task,
    std::shared_ptr<Evaluator> evaluator)
{
    auto temp = search_common::create_astar_open_list_factory_and_f_eval(
        utils::Verbosity::SILENT,
        evaluator);
    return std::make_unique<eager_search::EagerSearch>(
        task,
        utils::get_silent_log(),
        OperatorCost::NORMAL,
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<int>::max(),
        true,
        temp.first->create_state_open_list(),
        temp.second,
        std::vector<std::shared_ptr<Evaluator>>{},
        std::shared_ptr<Evaluator>());
}

}