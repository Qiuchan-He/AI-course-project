# See http://www.fast-downward.org/ForDevelopers/AddingSourceFiles
# for general information on adding source files and CMake plugins.
#
# All plugins are enabled by default and users can disable them by specifying
#    -DPLUGIN_FOO_ENABLED=FALSE
# The default behavior can be changed so all non-essential plugins are
# disabled by default by specifying
#    -DDISABLE_PLUGINS_BY_DEFAULT=TRUE
# In that case, individual plugins can be enabled with
#    -DPLUGIN_FOO_ENABLED=TRUE
#
# Defining a new plugin:
#    create_fast_downward_library(
#        NAME <NAME>
#        [ DISPLAY_NAME <DISPLAY_NAME> ]
#        [ HELP <HELP> ]
#        SOURCES
#            <FILE_1> [ <FILE_2> ... ]
#        [ DEPENDS <PLUGIN_NAME_1> [ <PLUGIN_NAME_2> ... ] ]
#        [ DEPENDENCY_ONLY ]
#        [ CORE_LIBRARY ]
#    )
#
# <DISPLAY_NAME> defaults to lower case <NAME> and is used to group files
#   in IDEs and for messages.
# <HELP> defaults to <DISPLAY_NAME> and is used to describe the cmake option.
# SOURCES lists the source files that are part of the plugin. Entries are
#   listed without extension. For an entry <file>, both <file>.h and <file>.cc
#   are added if the files exist.
# DEPENDS lists plugins that will be automatically enabled if this plugin is
#   enabled. If the dependency was not enabled before, this will be logged.
# DEPENDENCY_ONLY disables the plugin unless it is needed as a dependency and
#   hides the option to enable the plugin in cmake GUIs like ccmake.
# CORE_LIBRARY always enables the plugin (even if DISABLE_PLUGINS_BY_DEFAULT
#   is used) and hides the option to disable it in CMake GUIs like ccmake.

option(
    DISABLE_PLUGINS_BY_DEFAULT
    "If set to YES only plugins that are specifically enabled will be compiled"
    NO)
# This option should not show up in CMake GUIs like ccmake where all
# plugins are enabled or disabled manually.
mark_as_advanced(DISABLE_PLUGINS_BY_DEFAULT)

create_fast_downward_library(
    NAME core_sources
    HELP "Core source files"
    SOURCES
        downward/command_line
        downward/evaluation_context
        downward/evaluation_result
        downward/evaluator
        downward/evaluator_cache
        downward/heuristic
        downward/open_list
        downward/open_list_factory
        downward/operator_cost
        downward/operator_id
        downward/option_parser
        downward/option_parser_util
        downward/per_state_array
        downward/per_state_information
        downward/per_task_information
        downward/plan_manager
        downward/planning_task
        downward/plugin
        downward/search_algorithm
        downward/search_node_info
        downward/search_progress
        downward/search_space
        downward/search_statistics
        downward/state
        downward/state_id
        downward/state_registry
        downward/task_id
        downward/task_proxy
    DEPENDS int_hash_set int_packer ordered_set segmented_vector subscriber successor_generator task_properties policies
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME options
    HELP "Option parsing and plugin definition"
    SOURCES
        downward/options/any
        downward/options/bounds
        downward/options/doc_printer
        downward/options/doc_utils
        downward/options/errors
        downward/options/option_parser
        downward/options/options
        downward/options/parse_tree
        downward/options/predefinitions
        downward/options/plugin
        downward/options/raw_registry
        downward/options/registries
        downward/options/type_namer
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME utils
    HELP "System utilities"
    SOURCES
        downward/utils/collections
        downward/utils/countdown_timer
        downward/utils/distribution
        downward/utils/exceptions
        downward/utils/hash
        downward/utils/logging
        downward/utils/markup
        downward/utils/math
        downward/utils/rng
        downward/utils/rng_options
        downward/utils/strings
        downward/utils/system
        downward/utils/timer
    CORE_LIBRARY
)

# On Linux, find the rt library for clock_gettime().
if(UNIX AND NOT APPLE)
    target_link_libraries(utils INTERFACE rt)
endif()

# On Windows, find the psapi library for determining peak memory.
if(WIN32)
    cmake_policy(SET CMP0074 NEW)
    target_link_libraries(utils INTERFACE psapi)
endif()

create_fast_downward_library(
    NAME tiebreaking_open_list
    HELP "Tiebreaking open list"
    SOURCES
        downward/open_lists/tiebreaking_open_list
)

create_fast_downward_library(
    NAME int_hash_set
    HELP "Hash set storing non-negative integers"
    SOURCES
        downward/algorithms/int_hash_set
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME int_packer
    HELP "Greedy bin packing algorithm to pack integer variables with small domains tightly into memory"
    SOURCES
        downward/algorithms/int_packer
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME max_cliques
    HELP "Implementation of the Max Cliques algorithm by Tomita et al."
    SOURCES
        downward/algorithms/max_cliques
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME ordered_set
    HELP "Set of elements ordered by insertion time"
    SOURCES
        downward/algorithms/ordered_set
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME segmented_vector
    HELP "Memory-friendly and vector-like data structure"
    SOURCES
        downward/algorithms/segmented_vector
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME subscriber
    HELP "Allows object to subscribe to the destructor of other objects"
    SOURCES
        downward/algorithms/subscriber
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME evaluators_plugin_group
    HELP "Plugin group for basic evaluators"
    SOURCES
        downward/evaluators/plugin_group
)

create_fast_downward_library(
    NAME const_evaluator
    HELP "The constant evaluator"
    SOURCES
        downward/evaluators/const_evaluator
    DEPENDS evaluators_plugin_group
)

create_fast_downward_library(
    NAME g_evaluator
    HELP "The g-evaluator"
    SOURCES
        downward/evaluators/g_evaluator
    DEPENDS evaluators_plugin_group
)

create_fast_downward_library(
    NAME combining_evaluator
    HELP "The combining evaluator"
    SOURCES
        downward/evaluators/combining_evaluator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME max_evaluator
    HELP "The max evaluator"
    SOURCES
        downward/evaluators/max_evaluator
    DEPENDS combining_evaluator evaluators_plugin_group
)

create_fast_downward_library(
    NAME weighted_evaluator
    HELP "The weighted evaluator"
    SOURCES
        downward/evaluators/weighted_evaluator
    DEPENDS evaluators_plugin_group
)

create_fast_downward_library(
    NAME sum_evaluator
    HELP "The sum evaluator"
    SOURCES
        downward/evaluators/sum_evaluator
    DEPENDS combining_evaluator evaluators_plugin_group
)

create_fast_downward_library(
    NAME limited_pruning
    HELP "Method for limiting another pruning method"
    SOURCES
        downward/pruning/limited_pruning
)

create_fast_downward_library(
    NAME search_common
    HELP "Basic classes used for all search algorithms"
    SOURCES
        downward/search_algorithms/search_common
    DEPENDS g_evaluator sum_evaluator tiebreaking_open_list
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME eager_search
    HELP "Eager search algorithm"
    SOURCES
        downward/search_algorithms/eager_search
    DEPENDS ordered_set successor_generator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME plugin_astar
    HELP "A* search"
    SOURCES
        downward/search_algorithms/plugin_astar
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME plugin_eager
    HELP "Eager (i.e., normal) best-first search"
    SOURCES
        downward/search_algorithms/plugin_eager
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME relaxation_heuristic
    HELP "The base class for relaxation heuristics"
    SOURCES
        downward/heuristics/relaxation_heuristic
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME blind_search_heuristic
    HELP "The 'blind search' heuristic"
    SOURCES
        downward/heuristics/blind_search_heuristic
    DEPENDS task_properties
)

create_fast_downward_library(
    NAME ff_heuristic
    HELP "The FF heuristic (an implementation of the RPG heuristic)"
    SOURCES
        downward/heuristics/ff_heuristic
    DEPENDS max_heuristic task_properties
)

create_fast_downward_library(
    NAME goal_count_heuristic
    HELP "The goal-counting heuristic"
    SOURCES
        downward/heuristics/goal_count_heuristic
)

create_fast_downward_library(
    NAME max_heuristic
    HELP "The Max heuristic"
    SOURCES
        downward/heuristics/max_heuristic
    DEPENDS relaxation_heuristic
)

create_fast_downward_library(
    NAME core_tasks
    HELP "Core task transformations"
    SOURCES
        downward/tasks/cost_adapted_task
        downward/tasks/delegating_task
        downward/tasks/root_task
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME extra_tasks
    HELP "Non-core task transformations"
    SOURCES
        downward/tasks/modified_goals_task
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME sampling
    HELP "Sampling"
    SOURCES
        downward/task_utils/sampling
    DEPENDS successor_generator task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME successor_generator
    HELP "Successor generator"
    SOURCES
        downward/task_utils/successor_generator
        downward/task_utils/successor_generator_factory
        downward/task_utils/successor_generator_internals
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME task_properties
    HELP "Task properties"
    SOURCES
        downward/task_utils/task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME pdbs
    HELP "Plugin containing the code for PDBs"
    SOURCES
        downward/heuristics/cliques_heuristic
        downward/heuristics/pdb_heuristic
        downward/pdbs/plugin_group
        downward/pdbs/syntactic_projection
        downward/pdbs/types
    DEPENDS max_cliques sampling successor_generator task_properties
)
