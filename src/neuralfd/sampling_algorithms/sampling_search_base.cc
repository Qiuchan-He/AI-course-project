#include "neuralfd/sampling_algorithms/sampling_search_base.h"

#include "downward/evaluation_context.h"
#include "downward/evaluator.h"
#include "downward/heuristic.h"
#include "downward/plugin.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>

using namespace std;

namespace sampling_algorithm {

static options::ParseTree
prepare_search_parse_tree(const std::string& unparsed_config)
{
    options::ParseTree pt = options::generate_parse_tree(unparsed_config);
    return subtree(pt, options::first_child_of_root(pt));
}

SamplingSearchBase::SamplingSearchBase(const options::Options& opts)
    : SamplingStateAlgorithm(opts)
    , search_parse_tree(prepare_search_parse_tree(opts.get_unparsed_config()))
    , registry(*opts.get_registry())
    , predefinitions(*opts.get_predefinitions())
    , tmp_ignore_repredefinitions(
          opts.get_list<string>("ignore_repredefinitions"))
{
    ignore_repredefinitions = std::unordered_set<string>(
        tmp_ignore_repredefinitions.begin(),
        tmp_ignore_repredefinitions.end());
}

void SamplingSearchBase::next_algorithm()
{
    sampling_algorithm::paths.clear();
    registry.handle_all_repredefinition(
        predefinitions,
        ignore_repredefinitions);

    options::OptionParser algorithm_parser(
        search_parse_tree,
        registry,
        predefinitions,
        false);
    algorithm = algorithm_parser.start_parsing<shared_ptr<SearchAlgorithm>>();
    if (algorithm->get_max_time() > timer->get_remaining_time()) {
        algorithm->reduce_max_time(timer->get_remaining_time());
    }
}

std::vector<std::string>
SamplingSearchBase::sample(std::shared_ptr<ClassicalTask> task)
{
    utils::g_log << "." << flush;
    sampling_technique::modified_task = task;
    next_algorithm();
    algorithm->search();
    vector<string> samples;
    if (algorithm->found_solution()) {
        samples = extract_samples();
        post_search(samples);
    }
    return samples;
}

void SamplingSearchBase::post_search(vector<string>& /*samples*/)
{
}

void SamplingSearchBase::add_sampling_search_base_options(
    options::OptionParser& parser)
{
    parser.add_option<shared_ptr<SearchAlgorithm>>(
        "search",
        "Search algorithm to use for sampling");
    parser.add_list_option<string>(
        "ignore_repredefinitions",
        "List of predefined object types NOT to redefine for every "
        "search",
        "[]");
}
} // namespace sampling_algorithm
