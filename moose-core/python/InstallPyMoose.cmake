# CMake script to install python.
if(CPACK_TEMPORARY_INSTALL_DIRECTORY)
    MESSAGE("++ Using CPACK to build")
    set(CMAKE_INSTALL_PREFIX ${CPACK_TEMPORARY_INSTALL_DIRECTORY})
else()
    message("CPACK_TEMPORARY_INSTALL_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY}")
endif()
execute_process(COMMAND 
    python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(prefix=\"${CMAKE_INSTALL_PREFIX}\")" 
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE
    )
MESSAGE("++ Installing python scripts: ${CMAKE_INSTALL_PREFIX}")
MESSAGE("+++ Updating PYTHONPATH: ${PYTHON_SITE_PACKAGES}")
MESSAGE("+++ Creating directory ${PYTHON_SITE_PACKAGES}")
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PYTHON_SITE_PACKAGES})
set(ENV{PYTHONPATH} ${PYTHON_SITE_PACKAGES})
set(PYTHON_DIR ${CMAKE_CURRENT_LIST_DIR})
execute_process(COMMAND 
    python setup.py build_py build install
        --skip-build
        --prefix=${CMAKE_INSTALL_PREFIX}
        -O2 --force 
    WORKING_DIRECTORY ${PYTHON_DIR}
    )
FILE(REMOVE_RECURSE ${PYTHON_DIR}/build)
