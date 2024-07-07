#ifndef DOWNWARD_HEURISTICS_FF_HEURISTIC_H
#define DOWNWARD_HEURISTICS_FF_HEURISTIC_H

#include "downward/heuristics/max_heuristic.h"
#include "downward/operator_id.h"

#include <optional>
#include <vector>

namespace ff_heuristic {

/**
 * @brief Represents a delete-relaxed plan, paired with its cost.
 *
 * @ingroup heuristics
 */
struct DeleteRelaxedPlan {
    /// The operators of the plan.
    std::vector<OperatorID> operators; 

    /// The cost of the relaxed plan.
    int cost; 
};

/**
 * @brief The \f$h^{\text{FF}}\f$ heuristic.
 *
 * The \f$h^{\text{FF}}\f$ heuristic returns the cost of a delete-relaxed plan.
 * There are many methods to obtain a delete-relaxed plan, so this heuristic
 * has no unique definition. However, the typical approach is to extract
 * a delete-relaxed plan from a relaxed planning graph via a best-supporter
 * function like \f$h^{\text{max}}\f$ or \f$h^{\text{add}}\f$.
 *
 * @ingroup heuristics
 */
class FFHeuristic : public relaxation_heuristic::RelaxationHeuristic {
public:
    explicit FFHeuristic(const options::Options& opts);
    FFHeuristic(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log = utils::get_silent_log());

    int compute_heuristic(const State& ancestor_state) override;

    /**
     * @brief Returns a delete-relaxed plan paired with its cost, if existent,
     * and std::nullopt otherwise.
     * 
     * @note Implementations are allowed to be non-deterministic, i.e., to
     * return different delete-relaxed plans when called multiple times on the
     * same state.
     * 
     * @see DeleteRelaxedPlan
     */
    std::optional<DeleteRelaxedPlan>
    compute_relaxed_plan(const State& ancestor_state);
};
} // namespace ff_heuristic

#endif // DOWNWARD_HEURISTICS_FF_HEURISTIC_H
