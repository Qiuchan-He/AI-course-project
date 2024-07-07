#include "tests/utils/ff_utils.h"

#include "downward/task_proxy.h"

#include "downward/heuristics/ff_heuristic.h"

#include <set>

namespace tests {

namespace {
template <typename Facts>
static bool is_subset_of(const Facts& left, const std::set<FactPair>& right)
{
    for (FactProxy precondition : left) {
        if (!right.contains(precondition.get_pair())) return false;
    }
    return true;
}
} // namespace

std::optional<ff_heuristic::DeleteRelaxedPlan>
get_delete_relaxed_plan(
    const ClassicalTask& task,
    ff_heuristic::FFHeuristic& heuristic)
{
    ClassicalTaskProxy task_proxy(task);
    return heuristic.compute_relaxed_plan(task_proxy.get_initial_state());
}

bool verify_relaxed_plan(
    const ClassicalTask& task,
    const std::vector<OperatorID>& relaxed_plan,
    bool reject_superfluous_operators)
{
    ClassicalTaskProxy task_proxy(task);
    OperatorsProxy operators = task_proxy.get_operators();

    State initial_state = task_proxy.get_initial_state();

    std::set<FactPair> facts;

    for (const FactProxy& fact : initial_state) {
        facts.insert(fact.get_pair());
    }

    for (OperatorID op_id : relaxed_plan) {
        if (reject_superfluous_operators &&
            is_subset_of(task_proxy.get_goal(), facts))
            return false;

        OperatorProxy op = operators[op_id];

        if (!is_subset_of(op.get_precondition(), facts)) return false;

        for (auto effect_fact : op.get_effect()) {
            facts.insert(effect_fact.get_pair());
        }
    }

    return is_subset_of(task_proxy.get_goal(), facts);
}

}