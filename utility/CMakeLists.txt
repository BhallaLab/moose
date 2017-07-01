IF(WITH_BOOST)
include(CheckIncludeFiles)
check_include_files( ${Boost_INCLUDE_DIRS}/boost/random/random_device.hpp
    BOOST_RANDOM_DEVICE_EXISTS
    )
endif(WITH_BOOST)

include_directories(../msg)
include_directories(../basecode)
add_library(utility
    strutil.cpp
    types.cpp
    setupenv.cpp
    numutil.cpp
    Annotator.cpp
    Vec.cpp
    cnpy.cpp
    #matrix_util.cpp
    )
