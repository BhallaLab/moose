cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake )

# NSDF5 support. Disabled by default.
if(WITH_NSDF)
    find_package(HDF5 COMPONENTS CXX HL)
    if(NOT HDF5_FOUND)
        message(
            "==================================================================\n"
            " HDF5 not found. Disabling NSDF support.\n\n"
            " If you need NSDF support, please install hdf5-dev or hdf5-devel\n"
            " package or equivalent.\n\n"
            "     $ sudo apt-get install libhdf5-dev \n"
            "     $ sudo yum install libhdf5-devel \n"
            "     $ brew install hdf5 \n\n"
            " Otherwise, continue with 'make' and 'make install' \n"
            " If you install hdf5 to non-standard path, export environment \n"
            " variable HDF5_ROOT to the location. Rerun cmake \n"
            "================================================================ \n"
            )
    endif(NOT HDF5_FOUND)

    if(HDF5_FOUND)
        include_directories(${HDF5_CXX_INCLUDE_DIRS} )
        add_definitions(-DUSE_HDF5 -DENABLE_NSDF )
        add_definitions(${HDF5_CXX_DEFINITIONS})
        if(HDF5_USE_STATIC_LIBRARIES)
            message(STATUS "Finding static HDF5 libraries in $ENV{HDF5_ROOT}")
            find_library(HDF5_CXX_LIBRARIES NAMES libhdf5.a
                PATHS $ENV{HDF5_ROOT}/lib $ENV{HDF5_ROOT}/lib64
                )
            find_library(HDF5_HL_LIBRARIES NAMES libhdf5_hl.a
                PATHS $ENV{HDF5_ROOT}/lib $ENV{HDF5_ROOT}/lib64
                )
            set(HDF5_LIBRARIES ${HDF5_CXX_LIBRARIES} ${HDF5_HL_LIBRARIES})
        endif()

        # Make sure, HDF5_HL_LIBRARIES are set. The COMPONENTS in find_package may
        # or may not work. See BhallaLab/moose-core#163.
        if(NOT HDF5_HL_LIBRARIES)
            set(HDF5_HL_LIBRARIES ${HDF5_HL_LIBRARIES})
        endif(NOT HDF5_HL_LIBRARIES)
        list(APPEND HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})

    else(HDF5_FOUND)
        message(STATUS "HDF5 is not found. Disabling NSDF support." )
        add_definitions(-UUSE_HDF5 -UENABLE_NSDF)
    endif( HDF5_FOUND )
endif(WITH_NSDF)

set(SRCS
    Arith.cpp
    Group.cpp
    Mstring.cpp
    Function.cpp
    Variable.cpp
    InputVariable.cpp
    TableBase.cpp
    Table.cpp
    Interpol.cpp
    StimulusTable.cpp
    TimeTable.cpp
    StreamerBase.cpp
    Streamer.cpp
    Stats.cpp
    Interpol2D.cpp
    SpikeStats.cpp
    MooseParser.cpp
    SocketStreamer.cpp
    testBuiltins.cpp
    )

if(WITH_NSDF AND HDF5_FOUND)
    list(APPEND SRCS
    HDF5WriterBase.cpp
    NSDFWriter.cpp
    HDF5DataWriter.cpp
    SpikeStats.cpp
    testBuiltins.cpp
    testNSDF.cpp
    )
endif()

add_library(moose_builtins ${SRCS} )
if(WITH_NSDF AND HDF5_FOUND)
    target_link_libraries(moose_builtins ${HDF5_CXX_LIBRARIES} ${HDF5_HL_LIBRARIES})
endif()
