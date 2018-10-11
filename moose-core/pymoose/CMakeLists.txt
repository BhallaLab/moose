cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

find_package(PythonInterp REQUIRED)

# Find Numpy
find_package(NumPy REQUIRED)
include_directories(${NUMPY_INCLUDE_DIRS})
add_definitions( -std=c++11 )
add_definitions(-DUSE_NUMPY)
add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

# set module extensiton. default is .so. Also check ../python/setup.cmake.py
execute_process( COMMAND
    ${PYTHON_EXECUTABLE} -c
"import importlib.machinery
print(importlib.machinery.EXTENSION_SUFFIXES[-1])"
    OUTPUT_VARIABLE PYTHON_SO_EXTENSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
if(NOT PYTHON_SO_EXTENSION)
    message(STATUS "Python so ext could not be determined. Using default .so")
    set(PYTHON_SO_EXTENSION ".so")
endif()
message(STATUS "Python so extension ${PYTHON_SO_EXTENSION}" )

# TARGET
set(PYMOOSE_SRCS
    moosemodule.cpp
    vec.cpp
    mfield.cpp
    pymooseinit.cpp
    melement.cpp
    PyRun.cpp
    test_moosemodule.cpp
    )

# Build python module in source directory and them copy everything to
# current binary directory using cmake.
add_library( _moose MODULE ${PYMOOSE_SRCS} )
set(PYMOOSE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../python/moose")
message(STATUS "Python module will be saved to ${PYMOOSE_OUTPUT_DIRECTORY}" )

# make sure the Python.h is found.
find_package( PythonLibs)
if(PYTHON_INCLUDE_DIRS)
    include_directories( ${PYTHON_INCLUDE_DIRS} )
else()
     # Use python executable to find include paths. For building wheel, we are
     # stuck with cmake-2.8 with which PythonLibs module might fail.
     message( STATUS "Using ${PYTHON_EXECUTABLE}-config to find Python.h" )
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
        )

endif()

# Remove prefix lib from python module.
if(NOT(PYTHON_SO_EXTENSION STREQUAL ""))
    set_target_properties(_moose PROPERTIES SUFFIX ${PYTHON_SO_EXTENSION})
endif()
set_target_properties(_moose PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PYMOOSE_OUTPUT_DIRECTORY}
    PREFIX ""
    SUFFIX ${PYTHON_SO_EXTENSION}
    )

# see issue #80
if(HDF5_FOUND AND WITH_NSDF)
    set_target_properties( _moose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
endif()

if(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
    message(STATUS "ADDING some linker flags ${CMAKE_EXE_LINKER_FLAGS}")
endif(APPLE)

# cmake --help-policy CMP0042
if(APPLE)
    set_target_properties( _moose PROPERTIES MACOSX_RPATH OFF)
endif(APPLE)

if(APPLE)
    target_link_libraries( _moose
        "-Wl,-all_load"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )
    target_link_libraries(_moose
        ${SYSTEM_SHARED_LIBS}
        )
else(APPLE)
    target_link_libraries(_moose
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${SYSTEM_SHARED_LIBS}
        )
endif(APPLE)
add_custom_command(TARGET _moose POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan
        "MOOSE python extention is successfully built. Now "
        " Run 'sudo make install' to install it. "
        " "
        "NOTE: Run 'pip uninstall moose' to uninstall moose."
    VERBATIM
    )

