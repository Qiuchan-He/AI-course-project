#ifndef DOWNWARD_PDBS_PDB_HEURISTIC_H
#define DOWNWARD_PDBS_PDB_HEURISTIC_H

#include "downward/heuristic.h"
#include "downward/pdbs/syntactic_projection.h"
#include "downward/pdbs/types.h"

namespace options {
class Options;
}

namespace pdbs {

class PatternDatabase;

/**
 * @brief The pattern database (PDB) heuristic \f$h^P\f$ for a pattern \f$P\f$.
 *
 * The PDB heuristic returns the cost-to-goal of the corresponding abstract
 * state for a projection of the planning task, where some variables are
 * ignored.
 *
 * For a given planning task \f$ \Pi = (V, A, I, G)\f$ and pattern \f$P\f$, the
 * projection heuristic / PDB heuristic \f$h_{\Pi}^P\f$ is defined by
 * \f$h_{\Pi}^P(s) := h_{\Pi|_P}^{*}(s|_P)\f$,
 * where \f$s|_P\f$ is the projection of \f$s\f$ to \f$P\f$ and \f$ \Pi|_P\f$ is
 * the planning task with variables \f$P\f$ obtained from \f$ \Pi\f$ by
 * deleting all occurences of variables not in \f$P\f$ in preconditions,
 * effects and the goal.
 *
 * @ingroup heuristics
 */
class PDBHeuristic : public Heuristic {
public:
    /*
      Important: It is assumed that the pattern (passed via Options) is
      sorted, and contains no duplicates.
    */
    PDBHeuristic(const options::Options& opts);

    /**
     * @brief Constructs a PDB heuristic for a given planning task, pattern and
     * log.
     */
    PDBHeuristic(
        std::shared_ptr<ClassicalTask> task,
        Pattern pattern,
        utils::LogProxy log = utils::get_silent_log());

    int compute_heuristic(const State& ancestor_state) override;
    bool is_abstract_goal_state(const SyntacticProjection projection, const State& state);
    bool  is_abstract_operation_applicable(const std::vector<FactPair> fact_pairs, const State& state);
    int find_successor(const State& state, const std::vector<FactPair> fact_pairs);
    std::vector<int> values;
    SyntacticProjection *projection;
    Pattern pattern;
};
} // namespace pdbs

#endif
