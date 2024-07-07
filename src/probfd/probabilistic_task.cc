#include "probfd/probabilistic_task.h"

#include "downward/plugin.h"

#include <iostream>

using namespace std;

static PluginTypePlugin<probfd::ProbabilisticTask> _type_plugin(
    "ProbabilisticTask",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
