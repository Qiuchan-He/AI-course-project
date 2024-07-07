#ifndef DOWNWARD_SEARCH_ALGORITHMS_SEARCH_COMMON_H
#define DOWNWARD_SEARCH_ALGORITHMS_SEARCH_COMMON_H

/*
  This module contains functions for creating open list factories used
  by the search algorithms.

  TODO: Think about where this code should ideally live. One possible
  ordering principle: avoid unnecessary plug-in dependencies.

  This code currently depends on multiple different open list
  implementations, so it would be good if it would not be a dependency
  of search algorithms that don't need all these open lists. Under this
  maxim, EHC should not depend on this file. A logical solution might
  be to move every creation function that only uses one particular
  open list type into the header for this open list type, and leave
  this file for cases that require more complex set-up and are common
  to eager and lazy search.
*/

#include "downward/utils/logging.h"

#include <memory>

class Evaluator;
class OpenListFactory;

namespace options {
class Options;
}

namespace search_common {

/*
  Create open list factory and f_evaluator (used for displaying progress
  statistics) for A* search.

  The resulting open list factory produces a tie-breaking open list
  ordered primarily on g + h and secondarily on h. Uses "eval" from
  the passed-in Options object as the h evaluator.
*/
extern std::pair<std::shared_ptr<OpenListFactory>, const std::shared_ptr<Evaluator>>
create_astar_open_list_factory_and_f_eval(const options::Options &opts);

extern std::
    pair<std::shared_ptr<OpenListFactory>, const std::shared_ptr<Evaluator>>
    create_astar_open_list_factory_and_f_eval(
        utils::Verbosity verbosity,
        std::shared_ptr<Evaluator> evaluator);
} // namespace search_common

#endif
