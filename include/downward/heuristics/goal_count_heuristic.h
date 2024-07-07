#ifndef DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_H
#define DOWNWARD_HEURISTICS_GOAL_COUNT_HEURISTIC_H

#include "downward/heuristic.h"

namespace goal_count_heuristic {

/**
 * @brief The goal-counting heuristic \f$h^{\text{GC}}\f$.
 *
 * This heuristic returns the number of missing goal facts in the current state.
 *
 * Formally, for a planning task \f$ \Pi = (V, A, I, G)\f$, the blind heuristic
 * \f$h^{\text{GC}}\f$ is defined by
 *
 * \f[
 * h^{\text{GC}} = |\{v \in G \mid G(v) \neq s(v) \}|
 * \f]
 *
 * @ingroup heuristics
 */
class GoalCountHeuristic : public Heuristic {
public:
    explicit GoalCountHeuristic(const options::Options& opts);
    explicit GoalCountHeuristic(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log = utils::get_silent_log());

    virtual int compute_heuristic(const State& ancestor_state) override;
};
} // namespace goal_count_heuristic

#endif
