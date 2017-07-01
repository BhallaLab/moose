include_directories(../msg ../basecode)
IF(USE_MPI)
    find_package(MPI REQUIRED)
    include_directories(MPI_INCLUDE_PATH)
    ADD_DEFINITIONS(-DUSE_MPI)
ENDIF(USE_MPI)

add_library(moose_mpi
    PostMaster.cpp
    testMpi.cpp
    )

