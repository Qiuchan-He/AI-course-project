create_test_library(
    NAME blind_heuristic_public_tests
    HELP "Blind heuristic public tests"
    SOURCES
        tests/public/heuristic_tests/blind_heuristic_tests
    DEPENDS 
        blind_search_heuristic
        test_tasks
        heuristic_test_utils
)

create_test_library(
    NAME goal_counting_heuristic_public_tests
    HELP "Goal-counting heuristic public tests"
    SOURCES
        tests/public/heuristic_tests/goal_count_tests
    DEPENDS 
        goal_count_heuristic
        test_tasks
        search_test_utils
        heuristic_test_utils
)
