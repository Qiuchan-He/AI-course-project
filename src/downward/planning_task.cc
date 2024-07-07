#include "downward/planning_task.h"

#include "downward/per_task_information.h"
#include "downward/plugin.h"

#include <iostream>
#include <sstream>

using namespace std;

const FactPair FactPair::no_fact = FactPair(-1, -1);

ostream& operator<<(ostream& os, const FactPair& fact_pair)
{
    os << fact_pair.var << "=" << fact_pair.value;
    return os;
}

static PluginTypePlugin<ClassicalTask> _type_plugin(
    "ClassicalTask",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
