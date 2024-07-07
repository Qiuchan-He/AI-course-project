#ifndef DOWNWARD_ALGORITHMS_MAX_CLIQUES_H
#define DOWNWARD_ALGORITHMS_MAX_CLIQUES_H

#include <set>
#include <vector>

namespace max_cliques {
/**
 * @brief Computes the set of maximal cliques in an undirected graph.
 *
 * The input graph is specified via one set of connected nodes for each graph
 * node, numbered from 0 to N - 1, where N is the number of sets. E.g. the input
 * [ {1, 2}, {0, 2}, {0, 1}, {} ] corresponds to a graph with N=4 nodes,
 * numbered 0, 1, 2 and 3. The node 0 is connected to both 1 and 2, the node 1
 * is connected to 0 and 2, node 2 is connected to 0 and 1, and node 3 is
 * isolated (no edges).
 *
 * 0 -- 1
 * |  /
 * | /
 * 2    3
 *
 * Returns the set of maximal cliques, where each maximal clique is a set of
 * nodes. In this case, {{0, 1, 2}, {3}}.
 *
 * @ingroup downward
 */
extern std::set<std::set<int>>
compute_max_cliques(const std::vector<std::set<int>>& graph);
}

#endif
