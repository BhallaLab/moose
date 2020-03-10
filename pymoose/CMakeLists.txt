include(${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

execute_process(COMMAND 
  ${PYTHON_EXECUTABLE} -c "import numpy;print(numpy.get_include())"
  OUTPUT_VARIABLE NUMPY_INCLUDE_DIRS
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if("${NUMPY_INCLUDE_DIRS}" STREQUAL "")
  message(FATAL_ERROR "Could not find numpy: ${NUMPY_INCLUDE_DIRS}")
else()
  message(STATUS "Numpy is found at ${NUMPY_INCLUDE_DIRS}")
endif()

include_directories(${NUMPY_INCLUDE_DIRS})
add_definitions(-std=c++11)
add_definitions(-DUSE_NUMPY)
add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

set(PYTHON_SO_EXTENSION ".so")
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
# Use python executable to find include paths. 
# FIXME: cmake > 3.12 has great support for python but it is not available
# everywhere YET. When it is available on centos, we can use FindPython.
message(STATUS "Using ${PYTHON_EXECUTABLE}-config to find Python.h" )
execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --includes
         OUTPUT_VARIABLE PYTHON_INCLUDE_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if("${PYTHON_INCLUDE_FLAGS}" STREQUAL "")
  message(FATAL_ERROR "Could not determine path of Python.h.")
else()
  message(STATUS "Python.h is found at ${PYTHON_INCLUDE_FLAGS}")
endif()

execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --libs
         OUTPUT_VARIABLE PYTHON_LIBRARIES
  OUTPUT_STRIP_TRAILING_WHITESPACE)

set_target_properties(_moose PROPERTIES
    COMPILE_DEFINITIONS "PYMOOSE"
  COMPILE_FLAGS "${COMPILE_FLAGS} ${PYTHON_INCLUDE_FLAGS}"
    )

# Remove prefix lib from python module.
if(NOT(PYTHON_SO_EXTENSION STREQUAL ""))
    set_target_properties(_moose PROPERTIES SUFFIX ${PYTHON_SO_EXTENSION})
endif()
set_target_properties(_moose PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PYMOOSE_OUTPUT_DIRECTORY}
    PREFIX ""
    SUFFIX ${PYTHON_SO_EXTENSION})

# see issue #80
if(HDF5_FOUND AND WITH_NSDF)
    set_target_properties( _moose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
endif()

if(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
    message(STATUS "ADDING some linker flags ${CMAKE_EXE_LINKER_FLAGS}")
    # cmake --help-policy CMP0042
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
    ${SYSTEM_SHARED_LIBS})
endif()

# PYMOOSE DISTRIBUTION.
#find_python_module(wheel REQUIRED)
#if(NOT PY_WHEEL)
#  message(STATUS "Python module wheel is not found. Please wait while I install it..")
#  execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install wheel --user)
#endif(NOT PY_WHEEL)
  
# Create a binary distribution inside a directory. Installation is copying that
# directory to ${CMAKE_INSTALL_PREFIX}
set(_platform "CMAKE")
set(PYMOOSE_BDIST_FILE ${CMAKE_BINARY_DIR}/pymoose-${VERSION_MOOSE}.${_platform}.tar.gz)
set(PYMOOSE_INSTALL_DIR ${CMAKE_BINARY_DIR}/_pymoose_temp_install)
file(MAKE_DIRECTORY ${PYMOOSE_INSTALL_DIR})

add_custom_target(pymoose_sdist ALL
    DEPENDS ${PYMOOSE_BDIST_FILE} _moose
    COMMENT "Building pymoose sdist")

add_custom_command(OUTPUT ${PYMOOSE_BDIST_FILE}
  COMMAND ${PYTHON_EXECUTABLE} setup.py build_py  
  COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_dumb 
    --skip-build -p "${_platform}" -d ${CMAKE_BINARY_DIR} 
  COMMAND ${CMAKE_COMMAND} -E chdir ${PYMOOSE_INSTALL_DIR} tar xf ${PYMOOSE_BDIST_FILE}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "python's binary distribution is saved to ${CMAKE_BINARY_DIR}"
  VERBATIM)

# Copy python tree to BUILD directory. User can set PYTHONPATH to
# ${CMAKE_BINARY_DIR}/python.
add_custom_target(copy_python_tree ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/python ${CMAKE_BINARY_DIR}/python
    COMMENT "Copying python source tree: ${CMAKE_SOURCE_DIR}/python -> ${CMAKE_BINARY_DIR}/python"
  DEPENDS _moose VERBATIM)

install(DIRECTORY ${PYMOOSE_INSTALL_DIR}/usr/local/
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  CONFIGURATIONS Debug Release)

