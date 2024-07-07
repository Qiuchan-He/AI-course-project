#include "neuralfd/sampling_techniques/technique_uniform_none.h"

#include "neuralfd/tasks/modified_init_goals_task.h"

#include "downward/plugin.h"
#include "downward/state.h"

using namespace std;

namespace sampling_technique {
const std::string TechniqueUniformNone::name = "uniform_none";

const string& TechniqueUniformNone::get_name() const
{
    return name;
}

TechniqueUniformNone::TechniqueUniformNone(const options::Options& opts)
    : SamplingTechnique(opts)
{
}

std::shared_ptr<ClassicalTask> TechniqueUniformNone::create_next(
    shared_ptr<ClassicalTask> seed_task,
    const ClassicalTaskProxy&)
{
    ClassicalTaskProxy seed_task_proxy(*seed_task);
    while (true) {
        vector<int> unassigned(seed_task->get_num_variables(), -1);
        auto state = PartialAssignment(*seed_task, std::move(unassigned))
                         .get_full_state(check_mutexes, *rng);
        if (state.first) {
            vector<int> values = state.second.get_values();
            return make_shared<extra_tasks::ModifiedInitGoalsTask>(
                seed_task,
                std::move(values),
                extractGoalFacts(seed_task_proxy.get_goal()));
        }
    }
}

/* PARSING TECHNIQUE_UNIFORM_NONE*/
static shared_ptr<SamplingTechnique>
_parse_technique_uniform_none(options::OptionParser& parser)
{
    SamplingTechnique::add_options_to_parser(parser);

    options::Options opts = parser.parse();

    shared_ptr<TechniqueUniformNone> technique;
    if (!parser.dry_run()) {
        technique = make_shared<TechniqueUniformNone>(opts);
    }
    return technique;
}

static Plugin<SamplingTechnique> _plugin_technique_uniform_none(
    TechniqueUniformNone::name,
    _parse_technique_uniform_none);

} // namespace sampling_technique
