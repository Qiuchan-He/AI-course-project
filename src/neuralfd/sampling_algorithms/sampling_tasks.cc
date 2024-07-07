#include "neuralfd/sampling_algorithms/sampling_tasks.h"

#include "downward/task_utils/successor_generator.h"

#include <memory>
#include <string>

using namespace std;

namespace sampling_algorithm {
namespace {
string get_sas(const ClassicalTask& task)
{
    ostringstream sas;
    bool use_metric = false;
    for (int idx_op = 0; idx_op < task.get_num_operators(); ++idx_op) {
        if (task.get_operator_cost(idx_op) != 1) {
            use_metric = true;
            break;
        }
    }

    // SAS Head
    sas << "begin_version" << endl
        << "3" << endl
        << "end_version" << endl
        << "begin_metric" << endl
        << use_metric << endl
        << "end_metric" << endl;

    // Variable section
    sas << task.get_num_variables() << endl;
    for (int var = 0; var < task.get_num_variables(); ++var) {
        sas << "begin_variable" << endl
            << task.get_variable_name(var) << endl
            << -1 << endl
            << task.get_variable_domain_size(var) << endl;
        for (int value = 0; value < task.get_variable_domain_size(var);
             ++value) {
            sas << task.get_fact_name(FactPair(var, value)) << endl;
        }
        sas << "end_variable" << endl;
    }

    // Mutex section
    sas << "INVALID" << endl << "MUTEX" << endl << "SECTION" << endl;

    // Initial state section
    sas << "begin_state" << endl;
    for (int value : task.get_initial_state_values()) {
        sas << value << endl;
    }
    sas << "end_state" << endl;

    // Goal Section
    sas << "begin_goal" << endl << task.get_num_goal_facts() << endl;
    for (int idx_goal = 0; idx_goal < task.get_num_goal_facts(); ++idx_goal) {
        const FactPair goal_fact = task.get_goal_fact(idx_goal);
        sas << goal_fact.var << " " << goal_fact.value << endl;
    }
    sas << "end_goal" << endl;

    // Operator section
    sas << task.get_num_operators() << endl;
    for (int idx_op = 0; idx_op < task.get_num_operators(); ++idx_op) {
        sas << "begin_operator" << endl
            << task.get_operator_name(idx_op) << endl;
        // Preconditions
        sas << task.get_num_operator_precondition_facts(idx_op) << endl;
        for (int idx_pre = 0;
             idx_pre < task.get_num_operator_precondition_facts(idx_op);
             ++idx_pre) {
            FactPair pre = task.get_operator_precondition_fact(idx_op, idx_pre);
            sas << pre.var << " " << pre.value << endl;
        }
        // Operator effects
        sas << task.get_num_operator_effect_facts(idx_op) << endl;
        for (int idx_eff = 0;
             idx_eff < task.get_num_operator_effect_facts(idx_op);
             ++idx_eff) {
            // Operator effect conditions
            sas << 0;
            // Operator precondition, variable, new value
            FactPair eff = task.get_operator_effect_fact(idx_op, idx_eff);
            sas << eff.var << " " << -1 << " " << eff.value << endl;
        }
        sas << task.get_operator_cost(idx_op) << endl << "end_operator" << endl;
    }

    // Axiom section
    sas << 0 << endl;

    return sas.str();
}
} // namespace

SamplingTasks::SamplingTasks(const options::Options& opts)
    : SamplingAlgorithm(opts)
{
}

string SamplingTasks::sample_file_header() const
{
    return "";
}

vector<string> SamplingTasks::sample(shared_ptr<ClassicalTask> task)
{
    return vector<string>{get_sas(*task)};
}

} // namespace sampling_algorithm
