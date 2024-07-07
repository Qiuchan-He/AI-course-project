#ifndef DOWNWARD_HEURISTICS_BLIND_SEARCH_HEURISTIC_H
#define DOWNWARD_HEURISTICS_BLIND_SEARCH_HEURISTIC_H

#include "downward/heuristic.h"

namespace blind_search_heuristic {

/**
 * @brief The blind heuristic \f$h^{0}\f$.
 *
 * This heuristic returns the value 0 for goal states and the minimum action
 * cost for non-goal states.
 *
 * Formally, for a planning task \f$ \Pi = (V, A, I, G)\f$, the blind heuristic
 * \f$h^0\f$ is defined by
 *
 * \f[
 * h^0(s) =
 * \begin{cases}
 *   0 & s \models G \\
 *   \min_{a \in A} c(a) & \text{otherwise}
 * \end{cases}
 * \f]
 *
 *  @ingroup heuristics
 */ 
class BlindSearchHeuristic : public Heuristic {
public:
    BlindSearchHeuristic(const options::Options& opts);
    BlindSearchHeuristic(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log = utils::get_silent_log());

    virtual int compute_heuristic(const State& ancestor_state) override;
};
} // namespace blind_search_heuristic

#endif // DOWNWARD_HEURISTICS_BLIND_SEARCH_HEURISTIC_H
