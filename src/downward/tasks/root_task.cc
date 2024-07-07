#include "downward/tasks/root_task.h"

#include "downward/option_parser.h"
#include "downward/plugin.h"
#include "downward/state_registry.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

using namespace std;
using utils::ExitCode;

namespace tasks {
static const auto PRE_FILE_VERSION = "3";
static const auto PRE_FILE_PROB_VERSION = "3P";
shared_ptr<ClassicalTask> g_root_task = nullptr;

struct ExplicitVariable {
    int domain_size;
    string name;
    vector<string> fact_names;
    int axiom_layer;
    int axiom_default_value;

    explicit ExplicitVariable(istream& in);
};

struct ExplicitEffect {
    FactPair fact;
    vector<FactPair> conditions;

    ExplicitEffect(int var, int value, vector<FactPair>&& conditions);
};

struct ExplicitOperator {
    vector<FactPair> preconditions;
    vector<ExplicitEffect> effects;
    int cost;
    string name;
    bool is_an_axiom;

    void read_pre_post(istream& in);
    ExplicitOperator(istream& in, bool is_an_axiom, bool use_metric);
};

class RootTask : public ClassicalTask {
    vector<ExplicitVariable> variables;
    // TODO: think about using hash sets here.
    vector<vector<set<FactPair>>> mutexes;
    vector<ExplicitOperator> operators;
    vector<ExplicitOperator> axioms;
    vector<int> initial_state_values;
    vector<FactPair> goals;

    const ExplicitVariable& get_variable(int var) const;
    const ExplicitEffect&
    get_effect(int op_id, int effect_id, bool is_axiom) const;
    const ExplicitOperator&
    get_operator_or_axiom(int index, bool is_axiom) const;

public:
    explicit RootTask(istream& in);

    virtual int get_num_variables() const override;
    virtual string get_variable_name(int var) const override;
    virtual int get_variable_domain_size(int var) const override;
    virtual string get_fact_name(const FactPair& fact) const override;
    virtual bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    virtual int get_num_operators() const override;

    virtual int get_operator_cost(int index) const override;
    virtual string get_operator_name(int index) const override;
    virtual int get_num_operator_precondition_facts(int index) const override;
    virtual FactPair
    get_operator_precondition_fact(int op_index, int fact_index) const override;
    virtual int get_num_operator_effect_facts(int op_index) const override;
    virtual FactPair
    get_operator_effect_fact(int op_index, int eff_index) const override;

    virtual int get_num_goal_facts() const override;
    virtual FactPair get_goal_fact(int index) const override;

    virtual vector<int> get_initial_state_values() const override;

    virtual bool is_undefined(const FactPair& fact) const override;
};

static void
check_fact(const FactPair& fact, const vector<ExplicitVariable>& variables)
{
    if (!utils::in_bounds(fact.var, variables)) {
        cerr << "Invalid variable id: " << fact.var << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    if (fact.value < 0 || fact.value >= variables[fact.var].domain_size) {
        cerr << "Invalid value for variable " << fact.var << ": " << fact.value
             << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

static void check_facts(
    const vector<FactPair>& facts,
    const vector<ExplicitVariable>& variables)
{
    for (FactPair fact : facts) {
        check_fact(fact, variables);
    }
}

static void check_facts(
    const ExplicitOperator& action,
    const vector<ExplicitVariable>& variables)
{
    check_facts(action.preconditions, variables);
    for (const ExplicitEffect& eff : action.effects) {
        check_fact(eff.fact, variables);
        check_facts(eff.conditions, variables);
    }
}

static void check_magic(istream& in, const string& magic)
{
    string word;
    in >> word;
    if (word != magic) {
        cerr << "Failed to match magic word '" << magic << "'." << endl
             << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the planner "
                 << "on a translator output file from " << endl
                 << "an older version." << endl;
        }
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

static vector<FactPair> read_facts(istream& in)
{
    int count;
    in >> count;
    vector<FactPair> conditions;
    conditions.reserve(count);
    for (int i = 0; i < count; ++i) {
        FactPair condition = FactPair::no_fact;
        in >> condition.var >> condition.value;
        conditions.push_back(condition);
    }
    return conditions;
}

ExplicitVariable::ExplicitVariable(istream& in)
{
    check_magic(in, "begin_variable");
    in >> name;
    in >> axiom_layer;
    in >> domain_size;
    in >> ws;
    fact_names.resize(domain_size);
    for (int i = 0; i < domain_size; ++i)
        getline(in, fact_names[i]);
    check_magic(in, "end_variable");
}

ExplicitEffect::ExplicitEffect(
    int var,
    int value,
    vector<FactPair>&& conditions)
    : fact(var, value)
    , conditions(std::move(conditions))
{
}

void ExplicitOperator::read_pre_post(istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, value_pre, value_post;
    in >> var >> value_pre >> value_post;
    if (value_pre != -1) {
        preconditions.emplace_back(var, value_pre);
    }
    effects.emplace_back(var, value_post, std::move(conditions));
}

ExplicitOperator::ExplicitOperator(
    istream& in,
    bool is_an_axiom,
    bool use_metric)
    : is_an_axiom(is_an_axiom)
{
    if (!is_an_axiom) {
        check_magic(in, "begin_operator");
        in >> ws;
        getline(in, name);
        preconditions = read_facts(in);
        int count;
        in >> count;
        effects.reserve(count);
        for (int i = 0; i < count; ++i) {
            read_pre_post(in);
        }

        int op_cost;
        in >> op_cost;
        cost = use_metric ? op_cost : 1;
        check_magic(in, "end_operator");
    } else {
        name = "<axiom>";
        cost = 0;
        check_magic(in, "begin_rule");
        read_pre_post(in);
        check_magic(in, "end_rule");
    }
    assert(cost >= 0);
}

static void read_and_verify_version(istream& in)
{
    std::string version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_VERSION && version != PRE_FILE_PROB_VERSION) {
        cerr << "Expected translator output file version " << PRE_FILE_VERSION
             << ", got " << version << "." << endl
             << "Exiting." << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

static bool read_metric(istream& in)
{
    bool use_metric;
    check_magic(in, "begin_metric");
    in >> use_metric;
    check_magic(in, "end_metric");
    return use_metric;
}

static vector<ExplicitVariable> read_variables(istream& in)
{
    int count;
    in >> count;
    vector<ExplicitVariable> variables;
    variables.reserve(count);
    for (int i = 0; i < count; ++i) {
        variables.emplace_back(in);
    }
    return variables;
}

static vector<vector<set<FactPair>>>
read_mutexes(istream& in, const vector<ExplicitVariable>& variables)
{
    vector<vector<set<FactPair>>> inconsistent_facts(variables.size());
    for (size_t i = 0; i < variables.size(); ++i)
        inconsistent_facts[i].resize(variables[i].domain_size);

    int num_mutex_groups;
    in >> num_mutex_groups;

    /*
      NOTE: Mutex groups can overlap, in which case the same mutex
      should not be represented multiple times. The current
      representation takes care of that automatically by using sets.
      If we ever change this representation, this is something to be
      aware of.
    */
    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        vector<FactPair> invariant_group;
        invariant_group.reserve(num_facts);
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
            invariant_group.emplace_back(var, value);
        }
        check_magic(in, "end_mutex_group");
        for (const FactPair& fact1 : invariant_group) {
            for (const FactPair& fact2 : invariant_group) {
                if (fact1.var != fact2.var) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the translator (at least
                       with default settings) removes mutex groups
                       that contain *only* redundant mutexes, but it
                       can of course generate mutex groups which lead
                       to *some* redundant mutexes, where some but not
                       all facts talk about the same variable. */
                    inconsistent_facts[fact1.var][fact1.value].insert(fact2);
                }
            }
        }
    }
    return inconsistent_facts;
}

static vector<FactPair> read_goal(istream& in)
{
    check_magic(in, "begin_goal");
    vector<FactPair> goals = read_facts(in);
    check_magic(in, "end_goal");
    if (goals.empty()) {
        cerr << "Task has no goal condition!" << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    return goals;
}

static vector<ExplicitOperator> read_actions(
    istream& in,
    bool is_axiom,
    bool use_metric,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitOperator> actions;
    actions.reserve(count);
    for (int i = 0; i < count; ++i) {
        actions.emplace_back(in, is_axiom, use_metric);
        check_facts(actions.back(), variables);
    }
    return actions;
}

RootTask::RootTask(istream& in)
{
    read_and_verify_version(in);
    bool use_metric = read_metric(in);
    variables = read_variables(in);
    int num_variables = variables.size();

    mutexes = read_mutexes(in, variables);

    initial_state_values.resize(num_variables);
    check_magic(in, "begin_state");
    for (int i = 0; i < num_variables; ++i) {
        in >> initial_state_values[i];
    }
    check_magic(in, "end_state");

    for (int i = 0; i < num_variables; ++i) {
        variables[i].axiom_default_value = initial_state_values[i];
    }

    goals = read_goal(in);
    check_facts(goals, variables);
    operators = read_actions(in, false, use_metric, variables);
    axioms = read_actions(in, true, use_metric, variables);
}

const ExplicitVariable& RootTask::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables));
    return variables[var];
}

const ExplicitEffect&
RootTask::get_effect(int op_id, int effect_id, bool is_axiom) const
{
    const ExplicitOperator& op = get_operator_or_axiom(op_id, is_axiom);
    assert(utils::in_bounds(effect_id, op.effects));
    return op.effects[effect_id];
}

const ExplicitOperator&
RootTask::get_operator_or_axiom(int index, bool is_axiom) const
{
    if (is_axiom) {
        assert(utils::in_bounds(index, axioms));
        return axioms[index];
    } else {
        assert(utils::in_bounds(index, operators));
        return operators[index];
    }
}

int RootTask::get_num_variables() const
{
    return variables.size();
}

string RootTask::get_variable_name(int var) const
{
    return get_variable(var).name;
}

int RootTask::get_variable_domain_size(int var) const
{
    return get_variable(var).domain_size;
}

string RootTask::get_fact_name(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
    return get_variable(fact.var).fact_names[fact.value];
}

bool RootTask::are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
    const
{
    if (fact1.var == fact2.var) {
        // Same variable: mutex iff different value.
        return fact1.value != fact2.value;
    }
    assert(utils::in_bounds(fact1.var, mutexes));
    assert(utils::in_bounds(fact1.value, mutexes[fact1.var]));
    return bool(mutexes[fact1.var][fact1.value].count(fact2));
}

int RootTask::get_num_operators() const
{
    return operators.size();
}

int RootTask::get_operator_cost(int index) const
{
    return get_operator_or_axiom(index, false).cost;
}

string RootTask::get_operator_name(int index) const
{
    return get_operator_or_axiom(index, false).name;
}

int RootTask::get_num_operator_precondition_facts(int index) const
{
    return get_operator_or_axiom(index, false).preconditions.size();
}

FactPair
RootTask::get_operator_precondition_fact(int op_index, int fact_index) const
{
    const ExplicitOperator& op = get_operator_or_axiom(op_index, false);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int RootTask::get_num_operator_effect_facts(int op_index) const
{
    return get_operator_or_axiom(op_index, false).effects.size();
}

FactPair RootTask::get_operator_effect_fact(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index, false).fact;
}

int RootTask::get_num_goal_facts() const
{
    return goals.size();
}

FactPair RootTask::get_goal_fact(int index) const
{
    assert(utils::in_bounds(index, goals));
    return goals[index];
}

vector<int> RootTask::get_initial_state_values() const
{
    return initial_state_values;
}

bool RootTask::is_undefined(const FactPair& /*fact*/) const
{
    return false;
}

std::unique_ptr<ClassicalTask> read_task_from_sas(std::istream& in)
{
    return make_unique<RootTask>(in);
}

void read_root_task(istream& in)
{
    assert(!g_root_task);
    g_root_task = read_task_from_sas(in);
}

static shared_ptr<ClassicalTask> _parse(OptionParser& parser)
{
    if (parser.dry_run())
        return nullptr;
    else
        return g_root_task;
}

static Plugin<ClassicalTask> _plugin("no_transform", _parse);
} // namespace tasks
