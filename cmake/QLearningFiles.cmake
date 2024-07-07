create_probfd_library(
    NAME q_learning
    HELP "Plugin containing the code for Q learning"
    SOURCES
        q_learning/q_learning

        q_learning/fp_compare

        q_learning/state_encoder

        q_learning/epsilon_greedy_policy
        q_learning/random_policy

        q_learning/experience_sample

        q_learning/on_policy_experience_sampler
        q_learning/replay_buffer_experience_sampler

        q_learning/qvf_approximator_table
        q_learning/qvf_approximator_network

        q_learning/maxprob_simulator

        q_learning/trainable_network
    DEPENDS probabilistic_tasks successor_generator
)

create_probfd_library(
    NAME network_implementations
    HELP "Network implementations"
    SOURCES
        q_learning/torch_networks
    DEPENDS q_learning
)

target_link_libraries(
    q_learning
    PRIVATE
    debug torchd
    optimized torch)

#if (LIBRARY_TORCH_NETWORKS_ENABLED)
    target_link_libraries(
        network_implementations
        PRIVATE
        debug torchd
        optimized torch)
#endif()