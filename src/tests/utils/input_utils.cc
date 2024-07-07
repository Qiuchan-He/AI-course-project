#include "tests/utils/input_utils.h"

#include "downward/tasks/root_task.h"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <source_location>

using namespace std;

namespace tests {

static const filesystem::path DATA_PATH =
    filesystem::path(std::source_location::current().file_name())
        .parent_path()
        .parent_path()
        .parent_path()
        .parent_path()
        .append("resources/");

static const filesystem::path PROBLEMS_PATH = DATA_PATH / "problems/";
static const filesystem::path PLANS_PATH = DATA_PATH / "plans/";

std::unique_ptr<ClassicalTask>
read_task_from_file(std::string_view task_filename)
{
    std::fstream problem_file(PROBLEMS_PATH / task_filename);
    return tasks::read_task_from_sas(problem_file);
}

std::vector<OperatorID> read_plan_from_file(
    ClassicalTaskProxy task_proxy,
    std::string_view plan_filename)
{
    std::vector<OperatorID> plan;

    std::fstream file(PLANS_PATH / plan_filename);

    for (std::string line; std::getline(file, line) && line[0] != ';';) {
        std::string operator_name = line.substr(1, line.size() - 2);

        for (OperatorProxy op : task_proxy.get_operators()) {
            if (op.get_name() == operator_name) {
                plan.push_back(OperatorID(op.get_id()));
            }
        }
    }

    return plan;
}
}