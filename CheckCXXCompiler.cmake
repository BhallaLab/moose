if(COMPILER_IS_TESTED)
    return()
endif()

########################### COMPILER MACROS #####################################
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG( "-std=c++11" COMPILER_SUPPORTS_CXX11 )
CHECK_CXX_COMPILER_FLAG( "-std=c++0x" COMPILER_SUPPORTS_CXX0X )
CHECK_CXX_COMPILER_FLAG( "-Wno-strict-aliasing" COMPILER_WARNS_STRICT_ALIASING )

# Turn warning to error: Not all of the options may be supported on all
# versions of compilers. be careful here.
add_definitions(-Wall
    #-Wno-return-type-c-linkage
    -Wno-unused-variable
    -Wno-unused-function
    #-Wno-unused-private-field
    )
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions( -Wno-unused-local-typedefs )
elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    add_definitions( -Wno-unused-local-typedef )
endif()


add_definitions(-fPIC)
if(COMPILER_WARNS_STRICT_ALIASING)
    add_definitions( -Wno-strict-aliasing )
endif(COMPILER_WARNS_STRICT_ALIASING)

# Disable some harmless warnings.
CHECK_CXX_COMPILER_FLAG( "-Wno-unused-but-set-variable"
    COMPILER_SUPPORT_UNUSED_BUT_SET_VARIABLE_NO_WARN
    )
if(COMPILER_SUPPORT_UNUSED_BUT_SET_VARIABLE_NO_WARN)
    add_definitions( "-Wno-unused-but-set-variable" )
endif(COMPILER_SUPPORT_UNUSED_BUT_SET_VARIABLE_NO_WARN)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_definitions( -DENABLE_CPP11 )
    if(APPLE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
        # message(STATUS "NOTE: Making clang to inline more aggresively" )
        add_definitions( -mllvm -inline-threshold=1000 )
    endif(APPLE)
else(COMPILER_SUPPORTS_CXX11)
    add_definitions( -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS )
    message(FATAL_ERROR "\
        The compiler ${CMAKE_CXX_COMPILER} is too old. \
        Please use a compiler which has c++11 support.
        ")
endif(COMPILER_SUPPORTS_CXX11)

set(COMPILER_IS_TESTED ON)
