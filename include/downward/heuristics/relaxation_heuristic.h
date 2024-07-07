#ifndef DOWNWARD_HEURISTICS_RELAXATION_HEURISTIC_H
#define DOWNWARD_HEURISTICS_RELAXATION_HEURISTIC_H

#include "downward/heuristic.h"

namespace relaxation_heuristic {

class RelaxationHeuristic : public Heuristic {
public:
    explicit RelaxationHeuristic(const options::Options& options);

    RelaxationHeuristic(
        std::string description,
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log);

    virtual bool dead_ends_are_reliable() const override { return true; }
};
} // namespace relaxation_heuristic

#endif
