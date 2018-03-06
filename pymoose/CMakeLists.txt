add_definitions(-DPYMOOSE)
include_directories(../basecode ../msg)

set(PYMOOSE_SRCS
    moosemodule.cpp
    vec.cpp
    mfield.cpp
    pymooseinit.cpp
    melement.cpp
    test_moosemodule.cpp
    PyRun.cpp
    )

# Build _moose.so in source directory and them copy everything to
# current binary directory using cmake.
add_library( _moose MODULE ${PYMOOSE_SRCS} )
set(PYMOOSE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python/moose")

find_package(PythonInterp REQUIRED)
execute_process( COMMAND
    ${PYTHON_EXECUTABLE} -c
        "import importlib.machinery
print(importlib.machinery.EXTENSION_SUFFIXES[-1])"
    OUTPUT_VARIABLE PYTHON_SO_EXTENSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
if(NOT PYTHON_SO_EXTENSION)
    set(PYTHON_SO_EXTENSION ".so")
endif()
message( STATUS "Python so extension ${PYTHON_SO_EXTENSION}" )

find_package(NumPy REQUIRED)
include_directories(${NUMPY_INCLUDE_DIRS})
add_definitions(-DUSE_NUMPY)

add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --includes
    OUTPUT_VARIABLE PYTHON_INCLUDE_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --libs
    OUTPUT_VARIABLE PYTHON_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
message( STATUS "Python include flags: ${PYTHON_INCLUDE_FLAGS}" )
set_target_properties(_moose PROPERTIES
    COMPILE_DEFINITIONS "PYMOOSE"
    COMPILE_FLAGS "${PYTHON_INCLUDE_FLAGS}"
    LIBRARY_OUTPUT_DIRECTORY ${PYMOOSE_OUTPUT_DIRECTORY}
    PREFIX ""
    SUFFIX ${PYTHON_SO_EXTENSION}
    )

if(NOT(PYTHON_SO_EXTENSION STREQUAL ""))
    set_target_properties(_moose PROPERTIES
    SUFFIX ${PYTHON_SO_EXTENSION})
endif()

# see issue #80
if(HDF5_LIBRARY_DIRS)
    set_target_properties( _moose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
endif()


if(MACOSX)
    set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
    message(STATUS "ADDING some linker flags ${CMAKE_EXE_LINKER_FLAGS}")
endif(MACOSX)

if(MACOSX)
    target_link_libraries( _moose
        "-Wl,-all_load"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )
    target_link_libraries(_moose
        ${SYSTEM_SHARED_LIBS}
        )
else(MACOSX)
    target_link_libraries(_moose
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        # ${PYTHON_LIBRARIES}
        ${SYSTEM_SHARED_LIBS}
        )
endif(MACOSX)
add_custom_command(TARGET _moose POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan
        "MOOSE python extention is successfully built. Now "
        " Run 'sudo make install' to install it. "
        " "
        "NOTE: Run 'pip uninstall moose' to uninstall moose."
    VERBATIM
    )

