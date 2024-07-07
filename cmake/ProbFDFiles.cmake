create_probfd_library(
    NAME probabilistic_tasks
    HELP "Core source files for probabilistic planning tasks"
    SOURCES
        # Tasks
        probfd/probabilistic_task
        probfd/task_proxy
    DEPENDS core_sources utils core_tasks options
    CORE_LIBRARY
)
