create_fast_downward_library(
    NAME extra_nfd_tasks
    HELP "Non-core neural FD task transformations"
    SOURCES
        neuralfd/tasks/modified_init_goals_task
        neuralfd/tasks/partial_state_wrapper_task
    DEPENDS task_properties extra_tasks
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME regression
    HELP "Tools for regression"
    SOURCES
        neuralfd/task_utils/operator_generator_factory
        neuralfd/task_utils/operator_generator_internals
        neuralfd/task_utils/predecessor_generator
        neuralfd/task_utils/predecessor_generator_factory
        neuralfd/task_utils/regression_task_proxy
        neuralfd/task_utils/sampling
    DEPENDS task_properties successor_generator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME sampling_search
    HELP "Sampling search algorithm"
    SOURCES
        neuralfd/sampling_algorithms/sample_cache
        neuralfd/sampling_algorithms/sampling_algorithm
        neuralfd/sampling_algorithms/sampling_state_algorithm
        neuralfd/sampling_algorithms/sampling_search
        neuralfd/sampling_algorithms/sampling_search_base
        neuralfd/sampling_algorithms/sampling_search_simple
        neuralfd/sampling_algorithms/sampling_v
        neuralfd/sampling_algorithms/sampling_tasks
    DEPENDS extra_nfd_tasks ordered_set sampling_techniques
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME sampling_techniques
    HELP "Sampling Techniques"
    SOURCES
        neuralfd/sampling_techniques/technique_gbackward_none
        neuralfd/sampling_techniques/technique_iforward_none
        neuralfd/sampling_techniques/technique_uniform_none
        neuralfd/sampling_techniques/technique_null
        neuralfd/sampling_techniques/technique_none_none
        neuralfd/sampling_techniques/sampling_technique
    DEPENDS sampling extra_tasks task_properties regression
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME neural_networks
    HELP "Base files for neural networks"
    SOURCES
        neuralfd/neural_networks/abstract_network
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME torch_networks
    HELP "Networks using torch"
    SOURCES
       neuralfd/neural_networks/torch_network
       neuralfd/neural_networks/torch_state_network
       neuralfd/neural_networks/torch_policy_network
    DEPENDS neural_networks
)

#if (LIBRARY_TORCH_NETWORKS_ENABLED)
    target_link_libraries(
        torch_networks
        PUBLIC
        optimized torch
        debug torchd)
#endif()

create_fast_downward_library(
    NAME policies
    HELP "Basic support for policies"
    SOURCES
        neuralfd/policy
        neuralfd/policy_cache
        neuralfd/policy_result
        neuralfd/policies/heuristic_policy
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME network_policy
    HELP "The network policy"
    SOURCES
        neuralfd/policies/network_policy
    DEPENDS neural_networks policies
)

create_fast_downward_library(
    NAME plugin_sampling_search
    HELP "Sampling search plugin"
    SOURCES
        neuralfd/sampling_algorithms/plugin_sampling
    DEPENDS sampling_search search_common
)

create_fast_downward_library(
    NAME policy_search
    HELP "Policy guided search algorithm"
    SOURCES
        neuralfd/search_algorithms/policy_walk
    DEPENDS search_common
)

create_fast_downward_library(
    NAME network_heuristic
    HELP "The network heuristic"
    SOURCES
        neuralfd/heuristics/network_heuristic
    DEPENDS neural_networks
)