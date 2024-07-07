#ifndef DOWNWARD_HEURISTICS_CLIQUES_HEURISTIC_H
#define DOWNWARD_HEURISTICS_CLIQUES_HEURISTIC_H

#include "downward/heuristic.h"

#include "downward/pdbs/types.h"

namespace options {
class OptionParser;
}

namespace pdbs {
class PatternCollectionGenerator;

/**
 * @brief The cliques heuristic \f$h^{\mathcal{C}}\f$ for a pattern collection
 * \f$\mathcal{C}\f$.
 *
 * The cliques heuristic returns the best additive heuristic value among
 * orthogonal PDB heuristics for a given pattern collection, where two PDBs are
 * orthogonal if there is no action that has an effect on variables of both
 * patterns.
 *
 * For a given planning task \f$ \Pi = (V, A, I, G)\f$ and pattern collection
 * \f$\mathcal{C}\f$, let \f$cliques(C)\f$ be the set of maximal cliques in
 * the combinability graph of \f$\mathcal{C}\f$. Then the cliques heuristic
 * \f$h^{\mathcal{C}\f$ is defined by
 * \f[
 *   h^{\mathcal{C}(s) := \max_{D \in cliques(C)} \sum_{P \in D} h^{P}(s).
 * \f]
 *
 * @ingroup heuristics
 */
class CliquesHeuristic : public Heuristic {
public:
    explicit CliquesHeuristic(const options::Options& opts);
    CliquesHeuristic(
        std::shared_ptr<ClassicalTask> task,
        PatternCollection patterns,
        utils::LogProxy log = utils::get_silent_log());

    virtual int compute_heuristic(const State& ancestor_state) override;
};

} // namespace pdbs

#endif // DOWNWARD_HEURISTICS_CLIQUES_HEURISTIC_H
