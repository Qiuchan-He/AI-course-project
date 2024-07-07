create_test_library(
    NAME sl_public_tests
    HELP "Supervised learning public tests"
    SOURCES
        tests/public/sl_tests/sl_tests
    DEPENDS 
        input_utils
)

target_link_libraries(
    sl_public_tests
    PUBLIC
    optimized torch
    debug torchd)