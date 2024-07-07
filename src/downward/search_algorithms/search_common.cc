#include "downward/search_algorithms/search_common.h"

#include "downward/open_list_factory.h"
#include "downward/option_parser_util.h"

#include "downward/evaluators/g_evaluator.h"
#include "downward/evaluators/sum_evaluator.h"

#include "downward/open_lists/tiebreaking_open_list.h"

#include <memory>

using namespace std;

namespace search_common {
using GEval = g_evaluator::GEvaluator;
using SumEval = sum_evaluator::SumEvaluator;

pair<shared_ptr<OpenListFactory>, const shared_ptr<Evaluator>>
create_astar_open_list_factory_and_f_eval(const Options& opts)
{
    return create_astar_open_list_factory_and_f_eval(
        opts.get<utils::Verbosity>("verbosity"),
        opts.get<shared_ptr<Evaluator>>("eval"));
}

pair<shared_ptr<OpenListFactory>, const shared_ptr<Evaluator>>
create_astar_open_list_factory_and_f_eval(
    utils::Verbosity verbosity,
    shared_ptr<Evaluator> evaluator)
{
    Options g_evaluator_options;
    g_evaluator_options.set<utils::Verbosity>("verbosity", verbosity);
    shared_ptr<GEval> g = make_shared<GEval>(g_evaluator_options);
    shared_ptr<Evaluator> h = evaluator;
    Options f_evaluator_options;
    f_evaluator_options.set<utils::Verbosity>("verbosity", verbosity);
    f_evaluator_options.set<vector<shared_ptr<Evaluator>>>(
        "evals",
        vector<shared_ptr<Evaluator>>({g, h}));
    shared_ptr<Evaluator> f = make_shared<SumEval>(f_evaluator_options);
    vector<shared_ptr<Evaluator>> evals = {f, h};

    Options options;
    options.set("evals", evals);
    options.set("pref_only", false);
    options.set("unsafe_pruning", false);
    shared_ptr<OpenListFactory> open =
        make_shared<tiebreaking_open_list::TieBreakingOpenListFactory>(options);
    return make_pair(open, f);
}
} // namespace search_common
