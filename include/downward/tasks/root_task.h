#ifndef DOWNWARD_TASKS_ROOT_TASK_H
#define DOWNWARD_TASKS_ROOT_TASK_H

#include "downward/planning_task.h"

#include <memory>

namespace tasks {
extern std::shared_ptr<ClassicalTask> g_root_task;
extern std::unique_ptr<ClassicalTask> read_task_from_sas(std::istream& in);
extern void read_root_task(std::istream& in);
} // namespace tasks
#endif
