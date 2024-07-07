create_test_library(
    NAME q_learning_test_utils
    HELP "Utility for the Q-Learning tests"
    SOURCES
        tests/utils/q_learning_utils
    DEPENDS
        q_learning
        torch_networks
)

create_test_library(
    NAME q_learning_public_tests
    HELP "Q-Learning public tests"
    SOURCES
        tests/public/rl_tests/q_learning_tests
    DEPENDS 
        test_tasks
        task_test_utils
        input_utils
        q_learning_test_utils
)

create_test_library(
    NAME qvf_table_public_tests
    HELP "Table-based Q value function public tests"
    SOURCES
        tests/public/rl_tests/qvf_table_tests
    DEPENDS 
        test_tasks
        task_test_utils
        input_utils
        q_learning_test_utils
)

create_test_library(
    NAME qvf_network_public_tests
    HELP "NN-based Q value function public tests"
    SOURCES
        tests/public/rl_tests/qvf_network_tests
    DEPENDS 
        test_tasks
        task_test_utils
        input_utils
        q_learning_test_utils
)

create_test_library(
    NAME on_policy_experience_sampler_public_tests
    HELP "On-Policy experience sampler public tests"
    SOURCES
        tests/public/rl_tests/on_policy_experience_sampler_tests
    DEPENDS 
        test_tasks
        task_test_utils
        input_utils
        q_learning_test_utils
)

create_test_library(
    NAME replay_buffer_experience_sampler_public_tests
    HELP "Replay-buffered experience sampler public tests"
    SOURCES
        tests/public/rl_tests/replay_buffer_experience_sampler_tests
    DEPENDS 
        test_tasks
        task_test_utils
        input_utils
        q_learning_test_utils
)

create_test_library(
    NAME q_learning_daily_tests
    HELP "Q-Learning daily tests"
    SOURCES
        tests/daily/rl_tests/q_learning_tests
    DEPENDS 
        test_tasks
        task_test_utils
        q_learning_test_utils
)

create_test_library(
    NAME qvf_table_daily_tests
    HELP "Table-based Q value function daily tests"
    SOURCES
        tests/daily/rl_tests/qvf_table_tests
    DEPENDS 
        test_tasks
        task_test_utils
        q_learning_test_utils
)

create_test_library(
    NAME qvf_network_daily_tests
    HELP "NN-based Q value function daily tests"
    SOURCES
        tests/daily/rl_tests/qvf_network_tests
    DEPENDS 
        test_tasks
        task_test_utils
        q_learning_test_utils
)

create_test_library(
    NAME on_policy_experience_sampler_daily_tests
    HELP "On-Policy experience sampler daily tests"
    SOURCES
        tests/daily/rl_tests/on_policy_experience_sampler_tests
    DEPENDS 
        test_tasks
        task_test_utils
        q_learning_test_utils
)

create_test_library(
    NAME replay_buffer_experience_sampler_daily_tests
    HELP "Replay-buffered experience sampler daily tests"
    SOURCES
        tests/daily/rl_tests/replay_buffer_experience_sampler_tests
    DEPENDS 
        test_tasks
        task_test_utils
        q_learning_test_utils
)