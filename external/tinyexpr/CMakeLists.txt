cmake_minimum_required(VERSION 2.8.4)

project(tinyexpr)

option(TE_POW_FROM_RIGHT "Evaluate exponents from right to left." OFF)
option(TE_NAT_LOG "Define the log function as natural logarithm." OFF)
option(ENABLE_TESTS "Enabled tests" ON)
option(ENABLE_EXAMPALES "Enabled examples" ON)
option(ENABLE_BENCHMAKRS "Enabled benchamarks" ON)

find_library(MATH_LIB m)

set(SOURCE_FILES tinyexpr.c)
add_library(tinyexpr STATIC ${SOURCE_FILES})

if (TE_POW_FROM_RIGHT)
    target_compile_definitions(tinyexpr PRIVATE TE_POW_FROM_RIGHT)
endif()
if (TE_NAT_LOG)
    target_compile_definitions(tinyexpr PRIVATE TE_NAT_LOG)
endif()
target_link_libraries(tinyexpr ${MATH_LIB})

install(TARGETS tinyexpr ARCHIVE DESTINATION lib)
install(FILES tinyexpr.h DESTINATION include COMPONENT Devel)

# Now tests.

if (ENABLE_TESTS)
    add_executable(tinyexpr_test test.c )
    target_link_libraries(tinyexpr_test tinyexpr)

    add_executable(tinyexpr_test_pr test.c)
    target_compile_definitions(tinyexpr_test_pr PRIVATE TE_POW_FROM_RIGHT TE_NAT_LOG)
    target_link_libraries(tinyexpr_test_pr tinyexpr ${MATH_LIB})
endif()

if (ENABLE_BENCHMAKRS)
    add_executable(tinyexpr_benchmark benchmark.c)
    target_link_libraries(tinyexpr_benchmark tinyexpr ${MATH_LIB})
endif()

if (ENABLE_EXAMPALES)
    add_custom_target(examples)
    file(GLOB _example_src "example*.c")
    foreach( _ex_src ${_example_src})
        message(STATUS "Adding ${_ex_src} to examples" )
        get_filename_component(TGT_NAME ${_ex_src} NAME_WE)
        set(TGT_EXAMPLE_NAME tinyexpr_${TGT_NAME})
        add_executable(${TGT_EXAMPLE_NAME} ${_ex_src} )
        target_link_libraries(${TGT_EXAMPLE_NAME} tinyexpr)
        add_dependencies(examples ${TGT_EXAMPLE_NAME})
    endforeach()
endif()

enable_testing()
add_test( NAME test1 
    COMMAND $<TARGET_FILE:tinyexpr_test>
    WORKING_DIRECTORY $CMAKE_CURRENT_BINARY_DIR}
    )
add_test( NAME test2
    COMMAND $<TARGET_FILE:tinyexpr_test_pr>
    WORKING_DIRECTORY $CMAKE_CURRENT_BINARY_DIR}
    )
