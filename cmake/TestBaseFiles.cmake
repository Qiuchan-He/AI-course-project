create_test_library(
    NAME test_tasks
    HELP "Test planning tasks"
    SOURCES
        tests/tasks/simple_task
        tests/tasks/blocksworld
        tests/tasks/gripper
        tests/tasks/nomystery
        tests/tasks/visitall
        tests/tasks/sokoban
    DEPENDS options core_sources utils core_tasks
    CORE_LIBRARY
)

create_test_library(
    NAME task_test_utils
    HELP "Utility for test tasks"
    SOURCES
        tests/utils/task_utils
)

create_test_library(
    NAME input_utils
    HELP "Utility for input processing"
    SOURCES
        tests/utils/input_utils
    DEPENDS
        network_implementations
)

create_test_library(
    NAME heuristic_test_utils
    HELP "Utility for heuristic tests"
    SOURCES
        tests/utils/heuristic_utils
)

create_test_library(
    NAME search_test_utils
    HELP "Utility for search tests"
    SOURCES
        tests/utils/search_utils
    DEPENDS
        search_common
        eager_search
)
