#include "tests/utils/heuristic_utils.h"

#include "downward/heuristic.h"
#include "downward/task_proxy.h"

namespace tests {

int get_initial_state_estimate(const ClassicalTask& task, Heuristic& heuristic)
{
    ClassicalTaskProxy task_proxy(task);
    return heuristic.compute_heuristic(task_proxy.get_initial_state());
}

}