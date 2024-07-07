#include "neuralfd/sampling_algorithms/sampling_algorithm.h"
#include "neuralfd/sampling_algorithms/sampling_search.h"
#include "neuralfd/sampling_algorithms/sampling_tasks.h"
#include "neuralfd/sampling_algorithms/sampling_v.h"

#include "downward/search_algorithms/search_common.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"

using namespace std;

namespace plugin_sampling {
static shared_ptr<SearchAlgorithm> _parse_sampling_search(OptionParser& parser)
{
    parser.document_synopsis("Sampling Search Manager", "");

    sampling_algorithm::SamplingSearchBase::add_sampling_search_base_options(
        parser);
    sampling_algorithm::SamplingSearch::add_sampling_search_options(parser);
    sampling_algorithm::SamplingAlgorithm::add_sampling_options(parser);
    sampling_algorithm::SamplingStateAlgorithm::add_sampling_state_options(
        parser,
        "fields",
        "pddl",
        ";",
        "\t");
    SearchAlgorithm::add_options_to_parser(parser);

    Options opts = parser.parse();
    shared_ptr<sampling_algorithm::SamplingSearch> algorithm;
    if (!parser.dry_run()) {
        algorithm = make_shared<sampling_algorithm::SamplingSearch>(opts);
    }

    return algorithm;
}

static Plugin<SearchAlgorithm>
    _plugin_search1("sampling", _parse_sampling_search);
static Plugin<SearchAlgorithm>
    _plugin_search2("sampling_search", _parse_sampling_search);

static shared_ptr<SearchAlgorithm> _parse_sampling_v(OptionParser& parser)
{
    parser.document_synopsis("Sampling Search Manager", "");

    sampling_algorithm::SamplingV::add_sampling_v_options(parser);
    sampling_algorithm::SamplingAlgorithm::add_sampling_options(parser);
    sampling_algorithm::SamplingStateAlgorithm::add_sampling_state_options(
        parser,
        "csv",
        "fdr",
        ";",
        ";");
    SearchAlgorithm::add_options_to_parser(parser);

    Options opts = parser.parse();

    shared_ptr<sampling_algorithm::SamplingV> algorithm;
    if (!parser.dry_run()) {
        algorithm = make_shared<sampling_algorithm::SamplingV>(opts);
    }

    return algorithm;
}

static Plugin<SearchAlgorithm> _plugin_q("sampling_v", _parse_sampling_v);

static shared_ptr<SearchAlgorithm> _parse_sampling_tasks(OptionParser& parser)
{
    parser.document_synopsis("Sampling Task Manager", "");

    sampling_algorithm::SamplingAlgorithm::add_sampling_options(parser);
    SearchAlgorithm::add_options_to_parser(parser);
    // Overwrite default values of the following two options
    parser.add_option<int>(
        "sample_cache_size",
        "If more than sample_cache_size samples are cached, then the entries "
        "are written to disk and the cache is emptied. When sampling "
        "finishes, all remaining cached samples are written to disk. If "
        "running out of memory, the current cache is lost.",
        "1");
    parser.add_option<bool>(
        "iterate_sample_files",
        "Every time the cache is emptied, the output file name is the "
        "specified file name plus an appended increasing index. If not set, "
        "then every time the cache is full, the samples will be appended to "
        "the output file.",
        "true");

    Options opts = parser.parse();

    shared_ptr<sampling_algorithm::SamplingTasks> algorithm;
    if (!parser.dry_run()) {
        algorithm = make_shared<sampling_algorithm::SamplingTasks>(opts);
    }

    return algorithm;
}

static Plugin<SearchAlgorithm>
    _plugin_tasks1("generator", _parse_sampling_tasks);
static Plugin<SearchAlgorithm>
    _plugin_tasks2("sampling_tasks", _parse_sampling_tasks);

// static shared_ptr<SearchAlgorithm> _parse_sampling_overlap(OptionParser
// &parser)
// {
//     parser.document_synopsis("Sampling Overlap Manager", "");
//
//
//     overlap_search::OverlapSearch::add_overlap_options(parser);
//     SearchAlgorithm::add_options_to_parser(parser);
//
//     Options opts = parser.parse();
//     shared_ptr<overlap_search::OverlapSearch> algorithm;
//     if (!parser.dry_run()) {
//         algorithm = make_shared<overlap_search::OverlapSearch>(opts);
//     }
//     return algorithm;
// }

// static Plugin<SearchAlgorithm> _plugin_overlap("overlap_search",
// _parse_sampling_overlap);
} // namespace plugin_sampling
