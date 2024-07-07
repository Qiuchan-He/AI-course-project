#include "downward/task_utils/task_properties.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;
using utils::ExitCode;

namespace task_properties {

static bool is_strips_fact(const std::string& fact_name)
{
    return fact_name != "<none of those>" &&
           fact_name.rfind("NegatedAtom", 0) == std::string::npos;
}

static bool is_strips_fact(const ClassicalTask* task, const FactPair& fact_pair)
{
    return is_strips_fact(task->get_fact_name(fact_pair));
}

vector<FactPair> get_strips_fact_pairs(const ClassicalTask* task)
{
    std::vector<FactPair> facts;
    for (int var = 0; var < task->get_num_variables(); ++var) {
        for (int val = 0; val < task->get_variable_domain_size(var); ++val) {
            facts.emplace_back(var, val);
            if (!task_properties::is_strips_fact(task, facts.back())) {
                facts.pop_back();
            }
        }
    }
    return facts;
}

bool is_unit_cost(const ClassicalTaskProxy& task)
{
    for (OperatorProxy op : task.get_operators()) {
        if (op.get_cost() != 1) return false;
    }
    return true;
}

double get_average_operator_cost(const ClassicalTaskProxy& task_proxy)
{
    double average_operator_cost = 0;
    for (OperatorProxy op : task_proxy.get_operators()) {
        average_operator_cost += op.get_cost();
    }
    average_operator_cost /= task_proxy.get_abstract_operators().size();
    return average_operator_cost;
}

int get_num_facts(const PlanningTaskProxy& task_proxy)
{
    int num_facts = 0;
    for (VariableProxy var : task_proxy.get_variables())
        num_facts += var.get_domain_size();
    return num_facts;
}

int get_num_total_effects(const ClassicalTaskProxy& task_proxy)
{
    int num_effects = 0;
    for (OperatorProxy op : task_proxy.get_operators())
        num_effects += op.get_effect().size();
    return num_effects;
}

void print_variable_statistics(
    const PlanningTaskProxy& task_proxy,
    utils::LogProxy& log)
{
    const int_packer::IntPacker& state_packer = g_state_packers[task_proxy];

    int num_facts = 0;
    VariablesProxy variables = task_proxy.get_variables();
    for (VariableProxy var : variables)
        num_facts += var.get_domain_size();

    log << "Variables: " << variables.size() << endl;
    log << "FactPairs: " << num_facts << endl;
    log << "Bytes per state: "
        << state_packer.get_num_bins() * sizeof(int_packer::IntPacker::Bin)
        << endl;
}

PerTaskInformation<int_packer::IntPacker>
    g_state_packers([](const PlanningTaskProxy& task_proxy) {
        VariablesProxy variables = task_proxy.get_variables();
        vector<int> variable_ranges;
        variable_ranges.reserve(variables.size());
        for (VariableProxy var : variables) {
            variable_ranges.push_back(var.get_domain_size());
        }
        return std::make_unique<int_packer::IntPacker>(variable_ranges);
    });
} // namespace task_properties
