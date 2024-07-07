#ifndef Q_LEARNING_UTILS_H
#define Q_LEARNING_UTILS_H

#include "q_learning/experience_sample.h"
#include "q_learning/experience_sampler.h"
#include "q_learning/qvf_approximator.h"

#include "probfd/task_proxy.h"

#include <iostream>
#include <optional>
#include <vector>

namespace q_learning {
struct ExperienceSample;
class TrainableNetwork;
} // namespace q_learning

namespace tests {

/**
 * @brief Dummy implementation of ExperienceSampler that receives an explicit
 * list of experience samples to reproduce.
 *
 * @ingroup utils
 */
class ExperienceSequenceSampler : public q_learning::ExperienceSampler {
    std::vector<q_learning::ExperienceSample>::const_iterator current;
    std::vector<q_learning::ExperienceSample>::const_iterator end;

public:
    explicit ExperienceSequenceSampler(
        const std::vector<q_learning::ExperienceSample>& samples);

    // Returns the next experience sample in the list, or std::nullopt if the
    // all samples have been returned.
    std::optional<q_learning::ExperienceSample>
    sample_experience(q_learning::QVFApproximator&) override;
};

/**
 * @brief Dummy implementation of QVFApproximator that stores the parameters
 * it receives on every update call, to be inspected afterwards.
 *
 * @ingroup utils
 */
class DummyQVF : public q_learning::QVFApproximator {
    std::vector<q_learning::ExperienceSample> experiences;
    std::vector<double> discount_factors;
    std::vector<double> learning_rates;

    // Not needed. Will abort if called.
    double get_qvalue(const State&, OperatorID) const override;
    double compute_max_qvalue(const State&) const override;

public:
    // Stores the parameters for later inspection.
    void update(
        const q_learning::ExperienceSample& sample,
        double discount_factor,
        double learning_rate) override;

    // Returns the experiences received on every update call so far.
    const std::vector<q_learning::ExperienceSample>& get_experiences() const;

    // Returns the discount factors on every update call so far.
    const std::vector<double>& get_discount_factors() const;

    // Returns the learning rates received on every update call so far.
    const std::vector<double>& get_learning_rates() const;
};

/**
 * @brief Dumps all experience sample produced by this sampler (with the given
 * QVF approximator as a parameter) to an output stream.
 *
 * @see read_experience_samples_from_file
 *
 * @ingroup utils
 */
void dump_all_samples(
    q_learning::ExperienceSampler& sampler,
    q_learning::QVFApproximator& qvf,
    probfd::ProbabilisticTaskProxy task_proxy,
    std::ostream& out);

/**
 * @brief Reads experience samples from a .samples file with the given filename.
 * The file must be in the resources/experience_samples directory of the
 * project.
 *
 * @see dump_all_samples
 *
 * @ingroup utils
 */
std::vector<q_learning::ExperienceSample> read_experience_samples_from_file(
    probfd::ProbabilisticTask& task,
    std::string_view samples_filename);

/**
 * @brief Reads a Torch .pt model file with the given filename. The file must be
 * in the `resources/models` directory of the project.
 *
 * @ingroup utils
 */
std::unique_ptr<q_learning::TrainableNetwork>
read_torch_model_from_file(std::string_view model_filename);
}

#endif // Q_LEARNING_UTILS_H
