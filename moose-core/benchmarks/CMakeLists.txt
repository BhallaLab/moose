include_directories(../msg)
include_directories(../basecode)
add_library(benchmarks 
    benchmarks.cpp
    kineticMarks.cpp
    )

if(ENABLE_BENCHMARKS)
    # This target run benchmarks. Not a part of moose distribution
    add_executable( 
        randnum_benchmark 
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_random_numbers.cpp
        )
    target_link_libraries(randnum_benchmark randnum  )

    if(GSL_FOUND)
        target_link_libraries(randnum_benchmark ${GSL_LIBRARIES})
    endif(GSL_FOUND)

    # Add a targe to run the benchmarks.
    #MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_target( run_randnum_benchmark ALL)
    add_custom_command( TARGET run_randnum_benchmark POST_BUILD
        COMMAND ${CMAKE_BINARY_DIR}/benchmarks/randnum_benchmark
        WORKING_DIR ${CMAKE_BINARY_DIR}
        )
endif(ENABLE_BENCHMARKS)
