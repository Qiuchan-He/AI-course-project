#include "downward/command_line.h"
#include "downward/option_parser.h"
#include "downward/search_algorithm.h"

#include "downward/options/registries.h"
#include "downward/task_utils/task_properties.h"
#include "downward/tasks/root_task.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include <iostream>

using namespace std;
using utils::ExitCode;

int main(int argc, const char** argv)
{
    utils::register_event_handlers();

    if (argc < 2) {
        utils::g_log << usage(argv[0]) << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    bool unit_cost = false;
    if (static_cast<string>(argv[1]) != "--help") {
        utils::g_log << "reading input..." << endl;
        tasks::read_root_task(cin);
        utils::g_log << "done reading input!" << endl;
        ClassicalTaskProxy task_proxy(*tasks::g_root_task);
        unit_cost = task_properties::is_unit_cost(task_proxy);
    }

    shared_ptr<SearchAlgorithm> algorithm;

    // The command line is parsed twice: once in dry-run mode, to
    // check for simple input errors, and then in normal mode.
    try {
        options::Registry registry(*options::RawRegistry::instance());
        parse_cmd_line(argc, argv, registry, true, unit_cost);
        algorithm = parse_cmd_line(argc, argv, registry, false, unit_cost);
    } catch (const ArgError& error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const OptionParserError& error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const ParseError& error) {
        error.print();
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    utils::Timer search_timer;
    algorithm->search();
    search_timer.stop();
    utils::g_timer.stop();

    algorithm->save_plan_if_necessary();
    algorithm->print_statistics();
    utils::g_log << "Search time: " << search_timer << endl;
    utils::g_log << "Total time: " << utils::g_timer << endl;

    ExitCode exitcode = algorithm->found_solution()
                            ? ExitCode::SUCCESS
                            : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}
