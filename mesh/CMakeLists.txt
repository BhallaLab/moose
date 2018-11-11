cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
add_library(mesh
    ChemCompt.cpp
    MeshCompt.cpp
    MeshEntry.cpp
    CubeMesh.cpp
    CylBase.cpp
    CylMesh.cpp
    NeuroNode.cpp
    NeuroMesh.cpp
    SpineEntry.cpp
    SpineMesh.cpp
    PsdMesh.cpp
    EndoMesh.cpp
    testMesh.cpp
    )
