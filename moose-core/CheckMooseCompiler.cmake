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
add_definitions(-fPIC)
if(COMPILER_WARNS_STRICT_ALIASING)
    add_definitions( -Wno-strict-aliasing )
endif(COMPILER_WARNS_STRICT_ALIASING)


if(COMPILER_SUPPORTS_CXX11)
    message(STATUS "Your compiler supports c++11 features. Enabling it")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_definitions( -DENABLE_CPP11 )
elseif(COMPILER_SUPPORTS_CXX0X)
    message(STATUS "Your compiler supports c++0x features. Enabling it")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    add_definitions( -DENABLE_CXX11 )
    add_definitions( -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS )
else()
    add_definitions( -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS )
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()


