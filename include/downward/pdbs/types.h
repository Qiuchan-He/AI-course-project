#ifndef DOWNWARD_PDBS_TYPES_H
#define DOWNWARD_PDBS_TYPES_H

#include <vector>

/**
 * \addtogroup heuristics
 *  @{
 */

namespace pdbs {
/// A pattern is represented as an ordered list of variables indices
using Pattern = std::vector<int>;
/// A pattern collection is represented as list of patterns
using PatternCollection = std::vector<Pattern>;
} // namespace pdbs

/**
 *  @}
 */

#endif
