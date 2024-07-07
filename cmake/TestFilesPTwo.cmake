create_test_library(
    NAME pdb_test_utils
    HELP "Utility for the PDB heuristic tests"
    SOURCES
        tests/utils/pdb_utils
    DEPENDS
        pdbs
)

create_test_library(
    NAME pdb_heuristic_public_tests
    HELP "PDB heuristic public tests"
    SOURCES
        tests/public/heuristic_tests/pdb_tests
    DEPENDS 
        pdbs
        test_tasks
        search_test_utils
        heuristic_test_utils
)

create_test_library(
    NAME cliques_heuristic_public_tests
    HELP "Cliques heuristic public tests"
    SOURCES
        tests/public/heuristic_tests/cliques_heuristic_tests
    DEPENDS 
        pdbs
        test_tasks
        search_test_utils
        heuristic_test_utils
)
