#include "downward/tasks/cost_adapted_task.h"

#include "downward/task_utils/task_properties.h"

#include "downward/operator_cost.h"
#include "downward/option_parser.h"
#include "downward/plugin.h"

#include "downward/tasks/root_task.h"
#include "downward/utils/system.h"

#include <iostream>
#include <memory>

using namespace std;
using utils::ExitCode;

namespace tasks {
CostAdaptedTask::CostAdaptedTask(
    const shared_ptr<ClassicalTask>& parent,
    OperatorCost cost_type)
    : DelegatingTask(parent)
    , cost_type(cost_type)
    , parent_is_unit_cost(
          task_properties::is_unit_cost(ClassicalTaskProxy(*parent)))
{
}

int CostAdaptedTask::get_operator_cost(int index) const
{
    OperatorProxy op(*parent, index);
    return get_adjusted_action_cost(op, cost_type, parent_is_unit_cost);
}

static shared_ptr<ClassicalTask> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Cost-adapted task",
        "A cost-adapting transformation of the root task.");
    add_cost_type_option_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    } else {
        OperatorCost cost_type = opts.get<OperatorCost>("cost_type");
        return make_shared<CostAdaptedTask>(g_root_task, cost_type);
    }
}

static Plugin<ClassicalTask> _plugin("adapt_costs", _parse);
} // namespace tasks
