include_directories( ${CMAKE_SOURCE_DIR}/msg )
include_directories( ${CMAKE_SOURCE_DIR}/basecode )
include_directories( ${CMAKE_SOURCE_DIR}/external/muparser/include )
include_directories( ${CMAKE_SOURCE_DIR}/scheduling )

add_library(moose_builtins
    Arith.cpp
    Group.cpp
    Mstring.cpp
    Func.cpp
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
    HDF5WriterBase.cpp
    NSDFWriter.cpp
    HDF5DataWriter.cpp
    SpikeStats.cpp
    testBuiltins.cpp
    testNSDF.cpp
    )
