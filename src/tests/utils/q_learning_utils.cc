
#include "tests/utils/q_learning_utils.h"

#include "tests/utils/task_utils.h"

#include "q_learning/experience_sample.h"
#include "q_learning/torch_networks.h"

#include <filesystem>
#include <fstream>
#include <ranges>

using namespace probfd;
using namespace q_learning;

using namespace std;

namespace tests {

static const filesystem::path DATA_PATH =
    filesystem::path(std::source_location::current().file_name())
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
        .append("resources/");

static const filesystem::path EXPERIENCE_SAMPLES_PATH =
    DATA_PATH / "experience_samples/";
static const filesystem::path MODELS_PATH = DATA_PATH / "models/";

ExperienceSequenceSampler::ExperienceSequenceSampler(
    const std::vector<ExperienceSample>& samples)
    : current(samples.begin())
    , end(samples.end())
{
}

std::optional<ExperienceSample>
ExperienceSequenceSampler::sample_experience(QVFApproximator&)
{
    if (current != end) {
        return *current++;
    }

    return std::nullopt;
}

double DummyQVF::get_qvalue(const State&, OperatorID) const
{
    abort();
}

double DummyQVF::compute_max_qvalue(const State&) const
{
    abort();
}

void DummyQVF::update(
    const ExperienceSample& sample,
    double discount_factor,
    double learning_rate)
{
    experiences.emplace_back(sample);
    discount_factors.emplace_back(discount_factor);
    learning_rates.emplace_back(learning_rate);
}

const std::vector<ExperienceSample>& DummyQVF::get_experiences() const
{
    return experiences;
}

const std::vector<double>& DummyQVF::get_discount_factors() const
{
    return discount_factors;
}

const std::vector<double>& DummyQVF::get_learning_rates() const
{
    return learning_rates;
}

void dump_all_samples(
    ExperienceSampler& sampler,
    QVFApproximator& qvf,
    ProbabilisticTaskProxy task_proxy,
    std::ostream& out)
{
    if (std::optional experience = sampler.sample_experience(qvf)) {
        experience->dump(task_proxy, out);
    }

    while (std::optional experience = sampler.sample_experience(qvf)) {
        out << "\n";
        experience->dump(task_proxy, out);
    }
}

std::vector<ExperienceSample> read_experience_samples_from_file(
    probfd::ProbabilisticTask& task,
    std::string_view samples_filename)
{
    std::vector<ExperienceSample> samples;

    std::fstream in(EXPERIENCE_SAMPLES_PATH / samples_filename);

    in >> std::ws;

    for (in >> std::ws; !in.eof(); in >> std::ws) {
        samples.emplace_back(task, in);
    }

    return samples;
}

std::unique_ptr<TrainableNetwork>
read_torch_model_from_file(std::string_view model_filename)
{
    return load_torch_network_from_file(MODELS_PATH / model_filename);
}
}