#ifndef NEURALFD_SAMPLING_TECHNIQUES_SAMPLING_TECHNIQUE_H
#define NEURALFD_SAMPLING_TECHNIQUES_SAMPLING_TECHNIQUE_H

#include "downward/options/parse_tree.h"

#include "downward/tasks/root_task.h"

#include "downward/utils/hash.h"
#include "downward/utils/rng_options.h"

#include <memory>
#include <ostream>
#include <random>
#include <set>
#include <string>
#include <vector>

class ClassicalTask;
class GoalProxy;
class Heuristic;
class PartialAssignment;
class State;
class ClassicalTaskProxy;

namespace options {
class OptionParser;
class Options;
class Predefinitions;
class Registry;
} // namespace options

namespace sampling_technique {
extern std::shared_ptr<ClassicalTask> modified_task;

class SamplingTechnique {
private:
    static int next_id;

public:
    const int id;

protected:
    options::Registry* registry;
    const options::Predefinitions* predefinitions;
    const int count;
    //    const std::string dump_directory;
    const bool check_mutexes;
    const bool check_solvable;
    const bool use_alternative_mutexes;
    const std::vector<std::vector<std::string>> alternative_mutexes;
    const options::ParseTree eval_parse_tree;
    const std::unique_ptr<options::OptionParser> option_parser;
    int counter = 0;

protected:
    int remaining_upgrades;

    std::shared_ptr<utils::RandomNumberGenerator> rng;
    std::vector<std::vector<std::set<FactPair>>> alternative_task_mutexes;
    std::shared_ptr<ClassicalTask> last_task = nullptr;

    virtual std::shared_ptr<ClassicalTask> create_next(
        std::shared_ptr<ClassicalTask> seed_task,
        const ClassicalTaskProxy& task_proxy) = 0;

    bool test_mutexes(const std::shared_ptr<ClassicalTask>& task) const;
    bool test_solvable(const ClassicalTaskProxy& task_proxy) const;
    //    void dump_modifications(std::shared_ptr<ClassicalTask> task) const;
    void
    update_alternative_task_mutexes(const std::shared_ptr<ClassicalTask>& task);

    virtual void do_upgrade_parameters();

public:
    explicit SamplingTechnique(const options::Options& opts);
    SamplingTechnique(
        int count,
        bool check_mutexes,
        bool check_solvable,
        std::shared_ptr<utils::RandomNumberGenerator> rng);
    virtual ~SamplingTechnique();

    int get_count() const;
    int get_counter() const;
    bool empty() const;

    std::shared_ptr<ClassicalTask>
    next(const std::shared_ptr<ClassicalTask>& seed_task = tasks::g_root_task);
    std::shared_ptr<ClassicalTask> next(
        const std::shared_ptr<ClassicalTask>& seed_task,
        const ClassicalTaskProxy& task_proxy);

    virtual void initialize() {}
    virtual const std::string& get_name() const = 0;

    static void add_options_to_parser(options::OptionParser& parser);
    static std::vector<int> extractInitialState(const State& state);
    static std::vector<FactPair> extractGoalFacts(const GoalProxy& goals_proxy);
    //    static std::vector<FactPair> extractGoalFacts(const State &state);

    virtual bool has_upgradeable_parameters() const;
    virtual void upgrade_parameters();
    virtual void dump_upgradable_parameters(std::ostream& stream) const;

    static const std::string no_dump_directory;
};
} // namespace sampling_technique

#endif
