create_test_library(
    NAME student_tests
    HELP "Local student tests"
    SOURCES
        tests/student/example_tests
    DEPENDS 
        blind_search_heuristic
        goal_count_heuristic
        max_heuristic
        ff_heuristic
        pdbs
        q_learning
        torch_networks
        student_files
        test_tasks
        task_test_utils
        input_utils
        heuristic_test_utils
        search_test_utils
    SOFT_DEPENDS
        ff_test_utils
        pdb_test_utils
        q_learning_test_utils
)