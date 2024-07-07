#include <gtest/gtest.h>

#include <ATen/core/ivalue.h>
#include <torch/jit.h>
#include <torch/script.h>

#include <filesystem>
#include <regex>
#include <source_location>

#include "tests/utils/input_utils.h"

static const std::filesystem::path DATA_PATH =
    std::filesystem::path(std::source_location::current().file_name())
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
        .append("resources")
        .append("policies");

namespace {

struct SLTestData {
    std::string regex;
    int input_width;
    int output_width;
};

class ModelSanityCheck : public testing::TestWithParam<SLTestData> {};
} // namespace

TEST_P(ModelSanityCheck, verify_models)
{
    const auto& param = GetParam();
    const auto& raw_regex = param.regex;
    const int input_width = param.input_width;
    const int output_width = param.output_width;

    std::regex regex(raw_regex, std::regex_constants::ECMAScript);

    int count = 0;

    for (auto& entry : std::filesystem::directory_iterator(DATA_PATH)) {
        if (!entry.is_regular_file()) continue;
        auto path = entry.path();
        if (path.extension() != ".pt" ||
            !std::regex_match(path.stem().string(), regex))
            continue;

        auto model = torch::jit::load(path.string());
        auto parameters = model.parameters();

        ASSERT_GT(parameters.size(), 0)
            << "Model " << path << " does not have trainable parameters!";
        auto first_param = *parameters.begin();

        ASSERT_EQ(first_param.dim(), 2)
            << "First parameter of model " << path
            << " is not a weight matrix of a linear layer!";

        ASSERT_EQ(first_param.size(1), input_width)
            << "Input layer of model " << path
            << " does not have the correct shape!";

        std::vector<torch::jit::IValue> inputs = {torch::zeros({input_width})};
        at::Tensor output = model.forward(inputs).toTensor();

        ASSERT_EQ(output.dim(), 1)
            << "Output of model " << path << " is not a vector!";

        ASSERT_EQ(output.size(0), output_width)
            << "Output layer of model " << path
            << " does not have the correct shape!";

        auto accessor = output.accessor<float, 1>();

        float total = 0.0f;

        for (int i = 0; i != output.size(0); ++i) {
            float val = accessor[i];
            ASSERT_TRUE(0.0 <= val && val <= 1.0)
                << "Output of model " << path
                << " is not a probability vector!\nValue at index " << i
                << " was not in range [0, 1]!";
            total += val;
        }

        ASSERT_NEAR(total, 1.0f, 0.01)
            << "Output vector probabilities of model " << path
            << " do not sum up to one!";

        if (++count == 5) break;
    }

    ASSERT_TRUE(count > 0) << "No models found matching naming convention!";
}

INSTANTIATE_TEST_SUITE_P(
    SLTestsPublic,
    ModelSanityCheck,
    testing::Values(
        SLTestData{"task1(_[a-zA-Z0-9]+)*", 36, 50},
        SLTestData{"task2(_[a-zA-Z0-9]+)*", 49, 72},
        SLTestData{"task3(_[a-zA-Z0-9]+)*", 64, 98},
        SLTestData{"task4(_[a-zA-Z0-9]+)*", 81, 128},
        SLTestData{"task5(_[a-zA-Z0-9]+)*", 100, 162}));