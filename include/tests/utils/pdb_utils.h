#ifndef UTILS_PDB_UTILS_H
#define UTILS_PDB_UTILS_H

#include "downward/pdbs/types.h"

#include "downward/task_proxy.h"

namespace pdbs {
class PDBHeuristic;
class CliquesHeuristic;
} // namespace pdbs

namespace tests {

/**
 * @brief Obtains a pattern for the input task that has a projection not larger
 * than the specified limit of abstract states by running a greedy algorithm.
 *
 * The greedy algorithm works as follows. First, sort the variables of the task
 * with respect to the following lexicographic order:
 * 1. Goal variables before non-goal variables
 * 2. Variables with smaller domain size before variables with larger domain
 *    size
 *
 * In this order, add variables greedily to the pattern until the abstraction
 * size would exceed the specified limit or no variables are left, then return
 * the pattern.
 *
 * @ingroup utils
 */
pdbs::Pattern get_greedy_pattern(
    ClassicalTaskProxy task_proxy,
    int abstract_state_limit = 10000);

/**
 * @brief Obtains the set of patterns \f$\{ \{v\} \mid v \in V[G] \}\f$
 * consisting of all singleton patterns of goal variables.
 *
 * @ingroup utils
 */
pdbs::PatternCollection get_atomic_patterns(ClassicalTaskProxy task_proxy);

} // namespace tests

#endif // UTILS_PDB_UTILS_H
