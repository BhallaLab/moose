file(GLOB random_SRC "*.cpp")

include_directories(../basecode )
if(WITH_GSL)
    include_directories(${GSL_INCLUDE_DIRS})
endif(WITH_GSL)

add_library(randnum ${random_SRC})
