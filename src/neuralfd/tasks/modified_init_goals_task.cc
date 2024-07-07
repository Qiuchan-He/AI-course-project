#include "neuralfd/tasks/modified_init_goals_task.h"

using namespace std;

namespace extra_tasks {
ModifiedInitGoalsTask::ModifiedInitGoalsTask(
    const shared_ptr<ClassicalTask>& parent,
    const vector<int>&& initial_state,
    const vector<FactPair>&& goals)
    : DelegatingTask(parent)
    , initial_state(std::move(initial_state))
    , goals(std::move(goals))
{
}

int ModifiedInitGoalsTask::get_num_goal_facts() const
{
    return goals.size();
}

FactPair ModifiedInitGoalsTask::get_goal_fact(int index) const
{
    return goals[index];
}

vector<int> ModifiedInitGoalsTask::get_initial_state_values() const
{
    return initial_state;
}
} // namespace extra_tasks
