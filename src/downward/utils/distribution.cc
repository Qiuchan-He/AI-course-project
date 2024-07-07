#include "downward/utils/distribution.h"

#include "downward/plugin.h"

#include <memory>

using namespace std;
namespace utils {
template <typename T>
Distribution<T>::Distribution(std::shared_ptr<RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

template <typename T>
Distribution<T>::Distribution(const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

template <typename T>
void Distribution<T>::upgrade_parameters()
{
}

UniformIntDistribution::UniformIntDistribution(
    int min,
    int max,
    float upgrade_min,
    float upgrade_max,
    std::shared_ptr<RandomNumberGenerator> rng)
    : DiscreteDistribution(std::move(rng))
    , min(min)
    , max(max)
    , upgrade_min(upgrade_min)
    , upgrade_max(upgrade_max)
    , dist(std::uniform_int_distribution<int>(min, max - 1))
{
    if (max <= min) {
        cerr << "Random distribution bounds may not overlap" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

UniformIntDistribution::UniformIntDistribution(const options::Options& opts)
    : DiscreteDistribution(opts)
    , min(opts.get<int>("min"))
    , max(opts.get<int>("max"))
    , upgrade_min(static_cast<float>(opts.get<double>("upgrade_min")))
    , upgrade_max(static_cast<float>(opts.get<double>("upgrade_max")))
    , dist(std::uniform_int_distribution<int>(min, max - 1))
{
}

UniformIntDistribution::~UniformIntDistribution()
{
}

int UniformIntDistribution::next()
{
    return static_cast<int>(rng->get_uniform(dist));
}

void UniformIntDistribution::dump_parameters(std::ostream& stream)
{
    stream << "min=" << min << ",max=" << max << ",";
}

void UniformIntDistribution::upgrade_parameters()
{
    min *= static_cast<int>(upgrade_min);
    max *= static_cast<int>(upgrade_max);
    assert(max >= min);
    dist = uniform_int_distribution<int>(min, max);
}

static void add_general_parser_options(options::OptionParser& parser)
{
    parser.add_option<int>(
        "random_seed",
        "Set to -1 (default) to use the global random number generator. "
        "Set to any other value to use a local random number generator with "
        "the given seed.",
        "-1",
        options::Bounds("-1", "infinity"));
}

static shared_ptr<DiscreteDistribution>
_parse_uniform_int_distribution(options::OptionParser& parser)
{
    parser.add_option<int>(
        "min",
        "Minimum value (included) to return from the distribution");
    parser.add_option<int>(
        "max",
        "Maximum value (excluded) to return from the distribution");
    parser.add_option<double>(
        "upgrade_min",
        "constant to multiple min with at each upgrade. Use 0 to disable",
        "0");
    parser.add_option<double>(
        "upgrade_max",
        "constant to multiple max with at each upgrade. Use 0 to disable",
        "0");
    add_general_parser_options(parser);

    options::Options opts = parser.parse();

    shared_ptr<UniformIntDistribution> distribution;
    if (!parser.dry_run()) {
        distribution = make_shared<UniformIntDistribution>(opts);
    }
    return distribution;
}

static Plugin<DiscreteDistribution> _plugin_uniform_int_distribution(
    "uniform_int_dist",
    _parse_uniform_int_distribution);

static PluginTypePlugin<DiscreteDistribution>
    _type_plugin("DiscreteDistribution", "Kinds of Discrete distributions");

} // namespace utils
