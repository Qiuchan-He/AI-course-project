#include "downward/tasks/modified_goals_task.h"

using namespace std;

namespace extra_tasks {
ModifiedGoalsTask::ModifiedGoalsTask(
    const shared_ptr<ClassicalTask>& parent,
    vector<FactPair>&& goals)
    : DelegatingTask(parent)
    , goals(std::move(goals))
{
}

int ModifiedGoalsTask::get_num_goal_facts() const
{
    return goals.size();
}

FactPair ModifiedGoalsTask::get_goal_fact(int index) const
{
    return goals[index];
}
} // namespace extra_tasks
