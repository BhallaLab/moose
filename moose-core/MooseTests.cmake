ENABLE_TESTING()

FIND_PACKAGE(PythonInterp REQUIRED)

# If CTEST_OUTPUT_ON_FAILURE environment variable is set, the output is printed
# onto the console if a test fails.
SET(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

ADD_TEST(NAME moose.bin-raw-run
    COMMAND moose.bin -u -q
    )

## PyMOOSE tests.

SET(PYMOOSE_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/python)

# This test does not work with python-2.6 because of unittest changed API.
#ADD_TEST(NAME pymoose-test-pymoose
#    COMMAND ${PYTHON_EXECUTABLE} test_pymoose.py
#    WORKING_DIRECTORY ${PYMOOSE_TEST_DIRECTORY}
#    )

IF(WITH_MPI)
    SET(TEST_COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4
        ${MPIEXEC_PREFLAGS} ${PYTHON_EXECUTABLE} ${MPIEXEC_POSTFLAGS}
        )
else(WITH_MPI)
    SET(TEST_COMMAND ${PYTHON_EXECUTABLE})
endif(WITH_MPI)

ADD_TEST(NAME pymoose-test-synchan
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_synchan.py
    )
set_tests_properties(pymoose-test-synchan 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

ADD_TEST(NAME pymoose-test-function
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_function.py
    )
set_tests_properties(pymoose-test-function
     PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
     )

ADD_TEST(NAME pymoose-test-vec
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_vec.py
    )
set_tests_properties(pymoose-test-vec PROPERTIES ENVIRONMENT 
    "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

ADD_TEST(NAME pymoose-pyrun
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_pyrun.py
    )
set_tests_properties(pymoose-pyrun 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# Do not run this test after packaging.
ADD_TEST(NAME pymoose-neuroml-reader-test 
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_neuroml.py
    )
set_tests_properties(pymoose-neuroml-reader-test 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

ADD_TEST(NAME pymoose-nsdf-sanity-test
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_nsdf.py
    )
set_tests_properties(pymoose-nsdf-sanity-test 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# Test Ksolve
ADD_TEST(NAME pymoose-ksolve-test
    COMMAND ${TEST_COMMAND} ${PROJECT_SOURCE_DIR}/tests/python/test_ksolve.py
    )
set_tests_properties(pymoose-ksolve-test
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

## Test basic SBML support. Only for python2.
if( PYTHON_VERSION_STRING VERSION_LESS "3.0.0" )
    ADD_TEST(NAME pymoose-test-basic-sbml-support
        COMMAND ${TEST_COMMAND}
        ${PROJECT_SOURCE_DIR}/tests/python/test_sbml.py
        )
    set_tests_properties(pymoose-test-basic-sbml-support 
        PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
        )
endif( )

ADD_TEST(NAME pymoose-test-rng
    COMMAND ${PROJECT_SOURCE_DIR}/tests/python/test_random_gen.sh
    ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/python
    )
set_tests_properties(pymoose-test-rng PROPERTIES 
        ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python PYTHON=${PYTHON_EXECUTABLE}"
    )

# Test Streamer class
ADD_TEST( NAME pymoose-test-streamer 
    COMMAND ${TEST_COMMAND} 
    ${PROJECT_SOURCE_DIR}/tests/python/test_streamer.py 
    )
set_tests_properties(pymoose-test-streamer
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# Test streaming supports in tables.
ADD_TEST( NAME pymoose-test-streaming_in_tables 
    COMMAND ${TEST_COMMAND} 
    ${PROJECT_SOURCE_DIR}/tests/python/test_table_streaming_support.py
    )
set_tests_properties(pymoose-test-streaming_in_tables
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# Test kkit support.
ADD_TEST( NAME pymoose-test-kkit 
    COMMAND ${TEST_COMMAND} 
    ${PROJECT_SOURCE_DIR}/tests/python/test_kkit.py
    )
set_tests_properties(pymoose-test-kkit
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# Test Calcium hsolve support.
ADD_TEST( NAME pymoose-test-calcium-hsolve 
    COMMAND ${TEST_COMMAND} 
    ${PROJECT_SOURCE_DIR}/tests/python/test_hsolve_externalCalcium.py
    )
set_tests_properties(pymoose-test-calcium-hsolve
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
    )

# # NOTE: These tests are not enabled yet. They take lot of time. Not all scripts
# # are fixed yet.
# # Test moose-examples with very short timeout. 
# ADD_TEST( NAME pymoose-test-moose-examples
#     COMMAND ${TEST_COMMAND} -c "import moose; moose.test( timeout = 10 );"
#     )
# set_tests_properties(pymoose-test-moose-examples
#    PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/python"
#    )
