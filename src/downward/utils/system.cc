#include "downward/utils/system.h"

#include "downward/utils/logging.h"
#include "downward/utils/timer.h"

#include <cstdlib>

using namespace std;

namespace utils {
const char* get_exit_code_message_reentrant(ExitCode exitcode)
{
    switch (exitcode) {
    case ExitCode::SUCCESS: return "Solution found.";
    case ExitCode::SEARCH_CRITICAL_ERROR: return "Unexplained error occurred.";
    case ExitCode::SEARCH_INPUT_ERROR: return "Usage error occurred.";
    case ExitCode::SEARCH_UNSUPPORTED:
        return "Tried to use unsupported feature.";
    case ExitCode::SEARCH_UNSOLVABLE: return "Task is provably unsolvable.";
    case ExitCode::SEARCH_UNSOLVED_INCOMPLETE:
        return "Search stopped without finding a solution.";
    case ExitCode::SEARCH_OUT_OF_MEMORY:
        return "Memory limit has been reached.";
    case ExitCode::SEARCH_OUT_OF_TIME: return "Time limit has been reached.";
    default: return nullptr;
    }
}

bool is_exit_code_error_reentrant(ExitCode exitcode)
{
    switch (exitcode) {
    case ExitCode::SUCCESS:
    case ExitCode::SEARCH_UNSOLVABLE:
    case ExitCode::SEARCH_UNSOLVED_INCOMPLETE:
    case ExitCode::SEARCH_OUT_OF_MEMORY:
    case ExitCode::SEARCH_OUT_OF_TIME: return false;
    case ExitCode::SEARCH_CRITICAL_ERROR:
    case ExitCode::SEARCH_INPUT_ERROR:
    case ExitCode::SEARCH_UNSUPPORTED:
    default: return true;
    }
}

void throw_not_implemented(std::source_location location)
{
    std::cout << "Unimplemented function in " << location.file_name()
              << ", line " << location.line() << ", column "
              << location.column() << ".\n"
              << "Unimplemented function was: " << location.function_name()
              << ";" << std::endl;

    throw std::runtime_error("Not implemented.");
}

void exit_with(ExitCode exitcode)
{
    report_exit_code_reentrant(exitcode);
    utils::g_log << "Search time: " << utils::g_search_timer << endl;
    utils::g_log << "Total time: " << utils::g_timer << endl;
    exit(static_cast<int>(exitcode));
}

// Out-of-memory variant which only uses stack memory.
void oom_exit_with(ExitCode exitcode)
{
    report_exit_code_reentrant(exitcode);
    exit(static_cast<int>(exitcode));
}

void exit_after_receiving_signal(ExitCode exitcode)
{
    /*
      In signal handlers, we have to use the "safe function" _Exit() rather
      than the unsafe function exit().
    */
    report_exit_code_reentrant(exitcode);
    _Exit(static_cast<int>(exitcode));
}

} // namespace utils

#if OPERATING_SYSTEM == WINDOWS
#include "system_windows.cc"
#elif OPERATING_SYSTEM == LINUX || OPERATING_SYSTEM == OSX
#include "system_unix.cc"
#else
#error "Unsupported OS"
#endif