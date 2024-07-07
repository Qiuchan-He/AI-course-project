create_test_library(
    NAME ff_test_utils
    HELP "Utility for the FF heuristic tests"
    SOURCES
        tests/utils/ff_utils
    DEPENDS
        ff_heuristic
)

create_test_library(
    NAME hmax_public_tests
    HELP "hmax public tests"
    SOURCES
        tests/public/heuristic_tests/hmax_tests
    DEPENDS 
        max_heuristic
        test_tasks
        search_test_utils
        heuristic_test_utils
)

create_test_library(
    NAME hff_public_tests
    HELP "hFF public tests"
    SOURCES
        tests/public/heuristic_tests/hff_tests
    DEPENDS 
        ff_heuristic
        test_tasks
        search_test_utils
        heuristic_test_utils
        ff_test_utils
)
