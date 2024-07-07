#ifndef DOWNWARD_HEURISTICS_MAX_HEURISTIC_H
#define DOWNWARD_HEURISTICS_MAX_HEURISTIC_H

#include "downward/heuristics/relaxation_heuristic.h"

namespace max_heuristic {

/**
 * @brief The \f$h^{\text{max}}\f$ heuristic.
 *
 * The \f$h^{\text{max}}\f$ heuristic approximates the cost of a state by the
 * cost of the most expensive goal fact in the delete-relaxed problem that has
 * not been achieved yet.
 *
 * For a planning task \f$ \Pi = (V, A, I, G)\f$, the \f$h^{\text{max}}\f$
 * heuristic is formally defined as
 * \f$h^{\text{max}}(s) := h^{\text{max}}(s, G)\f$, where
 * \f$h^{\text{max}}(s, g)\f$ is the point-wise greatest function satisfying
 *
 * \f[
 * h^{\text{max}}(s, g) =
 * \begin{cases}
 *   0 & s \subseteq g \\
 *   \min_{a \in A, g' \in \mathit{add}_a}
 *   c(a) + h^{\text{max}}(s, \mathit{pre}_a) & g = \{g'\}\\
 *   \max_{g' \in g} h^{\text{max}}(s, \{g'\}) & |g| > 1
 * \end{cases}
 * \f]
 *
 * @ingroup heuristics
 */
class HSPMaxHeuristic : public relaxation_heuristic::RelaxationHeuristic {
public:
    explicit HSPMaxHeuristic(const options::Options& opts);
    HSPMaxHeuristic(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log = utils::get_silent_log());

    virtual int compute_heuristic(const State& ancestor_state) override;
};
} // namespace max_heuristic

#endif
