# Project to test the scripts.
cmake_minimum_required(VERSION 2.8)
project(moose-examples LANGUAGES)  

# Pass PYTHON_EXECUTABLE from command line to use a  different one.
find_package( PythonInterp REQUIRED )
message( STATUS "Found ${PYTHON_EXECUTABLE}" )

# Now find all python files.
add_custom_target( lint )
file( GLOB_RECURSE PY_SCRIPTS *.py )
set( MATPLOTLIB_INITRC ${CMAKE_CURRENT_SOURCE_DIR}/_travis/matplotlibrc )

set( TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/_cmake_test_data" )
file( MAKE_DIRECTORY ${TEST_DATA_DIR} )

# Function to check if given script is suitable for execution.
set( SCRIPTS_TO_EXECUTE "" )
macro( suitable_for_execution script)
    file(READ ${script} _text)
    string( REGEX MATCH "input(\\s*)|raw_input(\\s*)" FORBIDDEN_TEXT ${_text} )
    string( REGEX MATCH "__main__" MUST_HAVE_TEXT ${_text} )
    if( FORBIDDEN_TEXT OR (NOT MUST_HAVE_TEXT) )
        message( STATUS "|| File is not suitable for running" )
    elseif( ${_script} MATCHES ".+traub_2005/.+" )
        message( STATUS "|| Script is blacklisted" )
    else()
        list(APPEND SCRIPTS_TO_EXECUTE "${script}" )
    endif( )
endmacro( )

foreach( _script ${PY_SCRIPTS} )
    get_filename_component( _script_filename ${_script} NAME )
    get_filename_component( _script_dirname ${_script} DIRECTORY )
    file(MD5 ${_script} _script_MD5_sum )
    message( STATUS "Added ${_script_filename} for checking" )

    set(STATUS_FILE
        "${TEST_DATA_DIR}/_status_${_script_filename}_${_script_MD5_sum}" 
        )

    set(TARGET_NAME "check_${_script_filename}_${_script_MD5_sum}" )

    add_custom_target( ${TARGET_NAME} DEPENDS ${STATUS_FILE})

    add_custom_command( OUTPUT ${STATUS_FILE}
        COMMAND timeout 30 ${PYTHON_EXECUTABLE} -m py_compile "${_script}"
        MAIN_DEPENDENCY ${_script}
        COMMENT "Compiling with ${PYTHON_EXECUTABLE}"
        WORKING_DIRECTORY ${_script_dirname} 
        VERBATIM 
        )
    add_dependencies( lint ${TARGET_NAME} )

    # Check if given script for suitable for running.
    suitable_for_execution( ${_script} )
endforeach( )

add_custom_target( execute ALL )
message( STATUS "Following script will run" )
foreach( _script ${SCRIPTS_TO_EXECUTE} )
    message( STATUS "++ Script ${_script} will execute" )
    get_filename_component( _script_filename ${_script} NAME )
    get_filename_component( _script_dirname ${_script} DIRECTORY )
    file(MD5 ${_script} _script_MD5_sum )

    set(STATUS_FILE
        "${TEST_DATA_DIR}/_run_${_script_filename}_${_script_MD5_sum}" 
        )

    set(TARGET_NAME "run_${_script_filename}_${_script_MD5_sum}" )
    add_custom_target( ${TARGET_NAME} DEPENDS ${STATUS_FILE})

    file(COPY ${MATPLOTLIB_INITRC} DESTINATION ${_script_dirname} )
    add_custom_command( 
        OUTPUT ${STATUS_FILE}
        COMMAND ${PYTHON_EXECUTABLE} "${_script}"
        MAIN_DEPENDENCY ${_script}
        WORKING_DIRECTORY ${_script_dirname} 
        COMMENT "Running with ${PYTHON_EXECUTABLE}"
        VERBATIM 
        )
    add_dependencies( execute ${TARGET_NAME} )
endforeach( )


add_dependencies( execute lint )
