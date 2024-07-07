#include "downward/state.h"
#include "downward/state_registry.h"

#include <algorithm>
#include <iostream>
#include <ranges>

using namespace std;

static bool contains_mutex_with_variable(
    const PlanningTask* task,
    size_t var,
    const vector<int>& values,
    size_t first_var2_index = 0)
{
    assert(utils::in_bounds(var, values));
    if (values[var] == PartialAssignment::UNASSIGNED) {
        return false;
    }
    FactPair fp(var, values[var]);

    for (size_t var2 = first_var2_index; var2 < values.size(); ++var2) {
        assert(utils::in_bounds(var2, values));
        if (var2 == var || values[var] == PartialAssignment::UNASSIGNED) {
            continue;
        }
        FactPair fp2(var2, values[var2]);
        if (task->are_facts_mutex(fp, fp2)) {
            return true;
        }
    }
    return false;
}

static bool contains_mutex(const PlanningTask* task, const vector<int>& values)
{
    for (size_t var = 0; var < values.size(); ++var) {
        if (contains_mutex_with_variable(task, var, values, var + 1)) {
            return true;
        }
    }
    return false;
}

/*
  Replace values[var] with non-mutex value. Return true iff such a
  non-mutex value could be found.
 */
static bool replace_with_non_mutex_value(
    const PlanningTask* task,
    vector<int>& values,
    const int idx_var,
    utils::RandomNumberGenerator& rng)
{
    utils::in_bounds(idx_var, values);
    int old_value = values[idx_var];
    vector<int> domain(task->get_variable_domain_size(idx_var));
    iota(domain.begin(), domain.end(), 0);
    rng.shuffle(domain);
    for (int new_value : domain) {
        values[idx_var] = new_value;
        if (!contains_mutex_with_variable(task, idx_var, values)) {
            return true;
        }
    }
    values[idx_var] = old_value;
    return false;
}

static const int MAX_TRIES_EXTEND = 10000;
static bool replace_dont_cares_with_non_mutex_values(
    const PlanningTask* task,
    vector<int>& values,
    utils::RandomNumberGenerator& rng)
{
    assert(values.size() == (size_t)task->get_num_variables());
    vector<int> vars_order(task->get_num_variables());
    iota(vars_order.begin(), vars_order.end(), 0);

    for (int round = 0; round < MAX_TRIES_EXTEND; ++round) {
        bool invalid = false;
        rng.shuffle(vars_order);
        vector<int> new_values = values;

        for (int idx_var : vars_order) {
            if (new_values[idx_var] == PartialAssignment::UNASSIGNED) {
                if (!replace_with_non_mutex_value(
                        task,
                        new_values,
                        idx_var,
                        rng)) {
                    invalid = true;
                    break;
                }
            }
        }
        if (!invalid) {
            values = new_values;
            return true;
        }
    }
    return false;
}

PartialAssignment::PartialAssignment(const PlanningTask& task)
    : task(&task)
    , values(nullptr)
{
}

PartialAssignment::PartialAssignment(
    const PlanningTask& task,
    vector<int>&& values)
    : task(&task)
    , values(make_shared<vector<int>>(std::move(values)))
{
    assert(static_cast<int>(this->values->size()) == task.get_num_variables());
}

PartialAssignment::PartialAssignment(
    const PartialAssignment& assignment,
    vector<int>&& values)
    : task(assignment.task)
    , values(make_shared<vector<int>>(std::move(values)))
{
    assert(static_cast<int>(this->values->size()) == task->get_num_variables());
}

bool PartialAssignment::assigned(std::size_t var_id) const
{
    assert(var_id < size());
    return (*values)[var_id] != PartialAssignment::UNASSIGNED;
}

bool PartialAssignment::assigned(const VariableProxy& var) const
{
    return (*values)[var.get_id()] != PartialAssignment::UNASSIGNED;
}

bool operator==(const PartialAssignment& left, const PartialAssignment& right)
{
    assert(left.task == right.task);
    return *left.values == *right.values;
}

std::size_t PartialAssignment::size() const
{
    return task->get_num_variables();
}

std::size_t PartialAssignment::assigned_size() const
{
    std::size_t size = 0;
    for (std::size_t var = 0; var < (*this).size(); ++var) {
        if ((*this).assigned(var)) {
            ++size;
        }
    }
    return size;
}

FactProxy PartialAssignment::operator[](std::size_t var_id) const
{
    assert(var_id < size());
    assert((*values)[var_id] != PartialAssignment::UNASSIGNED);
    return FactProxy(*task, var_id, (*values)[var_id]);
}

FactProxy PartialAssignment::operator[](VariableProxy var) const
{
    return (*this)[var.get_id()];
}

PlanningTaskProxy PartialAssignment::get_task() const
{
    return PlanningTaskProxy(*task);
}

const std::vector<int>& PartialAssignment::get_unpacked_values() const
{
    return *values;
}

const std::vector<int>& PartialAssignment::get_values() const
{
    return get_unpacked_values();
}

std::vector<FactPair> PartialAssignment::get_assigned_facts() const
{
    std::vector<FactPair> facts;
    for (unsigned int i = 0; i < size(); ++i) {
        if (assigned(i)) {
            facts.emplace_back(i, (*values)[i]);
        }
    }
    return facts;
}

PartialAssignment
PartialAssignment::get_partial_successor(OperatorProxy op) const
{
    std::vector<int> new_values = *values;
    for (FactProxy effect : op.get_effect()) {
        new_values[effect.get_variable().get_id()] = effect.get_value();
    }
    return PartialAssignment(*task, std::move(new_values));
}

bool PartialAssignment::violates_mutexes() const
{
    return contains_mutex(task, get_unpacked_values());
}

pair<bool, State> PartialAssignment::get_full_state(
    bool check_mutexes,
    utils::RandomNumberGenerator& rng) const
{
    vector<int> new_values = get_unpacked_values();
    bool success = true;
    if (check_mutexes) {
        if (contains_mutex(task, new_values)) {
            return make_pair(false, State(*task, std::move(new_values)));
        } else {
            success =
                replace_dont_cares_with_non_mutex_values(task, new_values, rng);
        }

    } else {
        for (VariableProxy var : VariablesProxy(*task)) {
            int& value = new_values[var.get_id()];
            if (value == PartialAssignment::UNASSIGNED) {
                int domain_size = var.get_domain_size();
                value = rng.random(domain_size);
            }
        }
    }
    return make_pair(success, State(*task, std::move(new_values)));
}

State::State(
    const PlanningTask& task,
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer)
    : PartialAssignment(task)
    , registry(&registry)
    , id(id)
    , buffer(buffer)
    , state_packer(&registry.get_state_packer())
    , num_variables(registry.get_num_variables())
{
    assert(id != StateID::no_state);
    assert(buffer);
    assert(num_variables == task.get_num_variables());
}

State::State(
    const PlanningTask& task,
    const StateRegistry& registry,
    StateID id,
    const PackedStateBin* buffer,
    vector<int>&& values)
    : State(task, registry, id, buffer)
{
    assert(num_variables == static_cast<int>(values.size()));
    this->values = make_shared<vector<int>>(std::move(values));
}

State::State(const PlanningTask& task, vector<int> values)
    : PartialAssignment(task, std::move(values))
    , registry(nullptr)
    , id(StateID::no_state)
    , buffer(nullptr)
    , state_packer(nullptr)
    , num_variables(this->values->size())
{
    assert(num_variables == task.get_num_variables());
}

bool operator==(const State& left, const State& right)
{
    assert(left.task == right.task);
    if (left.registry && right.registry) {
        // Both states are registered and from the same registry. Compare IDs.
        return left.id == right.id;
    }

    if (left.num_variables != right.num_variables) return false;

    // Compare values directly.
    if (!left.values) {
        if (!right.values) {
            for (int i = 0; i != left.num_variables; ++i) {
                if (left.state_packer->get(left.buffer, i) !=
                    right.state_packer->get(right.buffer, i)) {
                    return false;
                }
            }

            return true;
        } else {
            for (int i = 0; i != left.num_variables; ++i) {
                if (left.state_packer->get(left.buffer, i) !=
                    (*right.values)[i]) {
                    return false;
                }
            }

            return true;
        }
    } else if (!right.values) {
        for (int i = 0; i != left.num_variables; ++i) {
            if ((*left.values)[i] != right.state_packer->get(right.buffer, i)) {
                return false;
            }
        }

        return true;
    }

    return *left.values == *right.values;
}

void State::unpack() const
{
    if (!values) {
        int num_variables = size();
        /*
          A micro-benchmark in issue348 showed that constructing the vector
          in the required size and then assigning values was faster than the
          more obvious reserve/push_back. Although, the benchmark did not
          profile this specific code.

          We might consider a bulk-unpack method in state_packer that could be
          more efficient. (One can imagine state packer to have extra data
          structures that exploit sequentially unpacking each entry, by doing
          things bin by bin.)
        */
        values = std::make_shared<std::vector<int>>(num_variables);
        for (int var = 0; var < num_variables; ++var) {
            (*values)[var] = state_packer->get(buffer, var);
        }
    }
}

std::size_t State::size() const
{
    return num_variables;
}

FactProxy State::operator[](std::size_t var_id) const
{
    assert(var_id < size());
    if (values) {
        return FactProxy(*task, var_id, (*values)[var_id]);
    } else {
        assert(buffer);
        assert(state_packer);
        return FactProxy(*task, var_id, state_packer->get(buffer, var_id));
    }
}

FactProxy State::operator[](VariableProxy var) const
{
    return (*this)[var.get_id()];
}

const StateRegistry* State::get_registry() const
{
    return registry;
}

StateID State::get_id() const
{
    return id;
}

const PackedStateBin* State::get_buffer() const
{
    /*
      TODO: we should profile what happens if we #ifndef NDEBUG this test here
      and in other places (e.g. the next method). The 'if' itself is probably
      not costly, but the 'cerr <<' stuff might prevent inlining.
    */
    if (!buffer) {
        std::cerr << "Accessing the packed values of an unregistered state is "
                  << "treated as an error." << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    return buffer;
}

const std::vector<int>& State::get_unpacked_values() const
{
    unpack();
    return *values;
}

namespace utils {
void feed(HashState& hash_state, const PartialAssignment& assignment)
{
    /*
      Hashing a state without unpacked data will result in an error.
      We don't want to unpack states implicitly, so this rules out the option
      of unpacking the states here on demand. Mixing hashes from packed and
      unpacked states would lead to logically equal states with different
      hashes. Hashing packed (and therefore registered) states also seems like
      a performance error because it's much cheaper to hash the state IDs
      instead.
    */
    feed(hash_state, assignment.get_values());
}
void feed(HashState& hash_state, const State& state)
{
    /*
      Hashing a state without unpacked data will result in an error.
      We don't want to unpack states implicitly, so this rules out the option
      of unpacking the states here on demand. Mixing hashes from packed and
      unpacked states would lead to logically equal states with different
      hashes. Hashing packed (and therefore registered) states also seems like
      a performance error because it's much cheaper to hash the state IDs
      instead.
    */
    feed(hash_state, state.get_unpacked_values());
}
} // namespace utils
