#ifndef DOWNWARD_SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define DOWNWARD_SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include "downward/open_list.h"
#include "downward/search_algorithm.h"

#include <memory>
#include <vector>

class Evaluator;

namespace options {
class OptionParser;
class Options;
} // namespace options

namespace eager_search {
class EagerSearch : public SearchAlgorithm {
    const bool reopen_closed_nodes;

    std::unique_ptr<StateOpenList> open_list;
    std::shared_ptr<Evaluator> f_evaluator;

    std::vector<Evaluator*> path_dependent_evaluators;
    std::vector<std::shared_ptr<Evaluator>> preferred_operator_evaluators;
    std::shared_ptr<Evaluator> lazy_evaluator;

    void start_f_value_statistics(EvaluationContext& eval_context);
    void update_f_value_statistics(EvaluationContext& eval_context);
    void reward_progress();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit EagerSearch(const options::Options& opts);
    EagerSearch(
        std::shared_ptr<ClassicalTask> task,
        utils::LogProxy log,
        OperatorCost cost_type,
        double max_time,
        int bound,
        bool reopen_closed,
        std::unique_ptr<StateOpenList> open_list,
        std::shared_ptr<Evaluator> f_eval,
        std::vector<std::shared_ptr<Evaluator>> preferred,
        std::shared_ptr<Evaluator> lazy_evaluator);
    virtual ~EagerSearch() = default;

    virtual void print_statistics() const override;

    void dump_search_space() const;
};

extern void add_options_to_parser(options::OptionParser& parser);
} // namespace eager_search

#endif
