# Try to find gnu scientific library GSL  
# (see http://www.gnu.org/software/gsl/)
# Once run this will define: 
# 
# GSL_FOUND       = system has GSL lib
#
# GSL_VERSION       = gsl version
#
# GSL_LIBRARIES   =  name of the libraries.
#
# GSL_INCLUDE_DIRS      = where to find headers 
# 
# GSL_USE_STATIC_LIBRARIES = Set it ON if you want to search for static
# libraries.
#
# Felix Woelk 07/2004
# minor corrections Jan Woetzel
#
# www.mip.informatik.uni-kiel.de
# --------------------------------
#
# Friday 18 November 2016 09:05:56 AM IST
# MODIFICATIONS: ##   dilawars@ncbs.res.in, For the MOOSE project.
#      This  version does not use gsl-config file.
#

# Set this envrionment variable to search in this path first.
SET(GSL_ROOT_DIR $ENV{GSL_ROOT_DIR})
if(GSL_ROOT_DIR)
    message( STATUS "Debug: GSL_ROOT_DIR = ${GSL_ROOT_DIR}")
endif(GSL_ROOT_DIR)

IF(WIN32)

    SET(GSL_MINGW_PREFIX "c:/msys/local" )
    SET(GSL_MSVC_PREFIX "$ENV{LIB_DIR}")
    FIND_LIBRARY(GSL_LIB gsl PATHS 
        ${GSL_MINGW_PREFIX}/lib 
        ${GSL_MSVC_PREFIX}/lib
        )
    #MSVC version of the lib is just called 'cblas'
    FIND_LIBRARY(GSLCBLAS_LIB gslcblas cblas PATHS 
        ${GSL_MINGW_PREFIX}/lib 
        ${GSL_MSVC_PREFIX}/lib
        )

    FIND_PATH(GSL_INCLUDE_DIRS gsl/gsl_blas.h 
        ${GSL_MINGW_PREFIX}/include 
        ${GSL_MSVC_PREFIX}/include
        )

    IF (GSL_LIB AND GSLCBLAS_LIB)
        SET (GSL_LIBRARIES ${GSL_LIB} ${GSLCBLAS_LIB})
    ENDIF (GSL_LIB AND GSLCBLAS_LIB)

ELSE(WIN32)
    # UNIX
    if((GSL_USE_STATIC_LIBRARIES) OR ($ENV{GSL_USE_STATIC_LIBRARIES}))
        SET(GSL_LIB_NAMES libgsl.a)
        SET(GSL_CBLAS_LIB_NAMES libgslcblas.a)
    else()
        SET(GSL_LIB_NAMES gsl)
        SET(GSL_CBLAS_LIB_NAMES gslcblas)
    endif( )

    if(GSL_ROOT_DIR)
        FIND_LIBRARY(GSL_LIB 
            NAMES ${GSL_LIB_NAMES} 
            PATHS ${GSL_ROOT_DIR}/lib NO_DEFAULT_PATH
            )

        FIND_LIBRARY(GSLCBLAS_LIB 
            NAMES ${GSL_CBLAS_LIB_NAMES}
            PATHS ${GSL_ROOT_DIR}/lib NO_DEFAULT_PATH
            )
        IF (GSL_LIB AND GSLCBLAS_LIB)
            SET (GSL_LIBRARIES ${GSL_LIB} ${GSLCBLAS_LIB})
        ENDIF (GSL_LIB AND GSLCBLAS_LIB)

        FIND_PATH(GSL_INCLUDE_DIRS NAMES gsl/gsl_blas.h
            PATHS ${GSL_ROOT_DIR}/include NO_DEFAULT_PATH
            )
    else(GSL_ROOT_DIR)
        FIND_LIBRARY(GSL_LIB NAMES ${GSL_LIB_NAMES} )
        FIND_LIBRARY(GSLCBLAS_LIB NAMES ${GSL_CBLAS_LIB_NAMES})

        IF (GSL_LIB AND GSLCBLAS_LIB)
            SET (GSL_LIBRARIES ${GSL_LIB} ${GSLCBLAS_LIB})
        ENDIF (GSL_LIB AND GSLCBLAS_LIB)

        FIND_PATH(GSL_INCLUDE_DIRS NAMES gsl/gsl_blas.h
            PATHS ${GSL_ROOT_DIR}/include /opt/include 
            )
    endif( )

ENDIF(WIN32)

# FIND version
# message(STATUS "Searching in ${GSL_INCLUDE_DIRS}") 
if(GSL_INCLUDE_DIRS)
    file(READ "${GSL_INCLUDE_DIRS}/gsl/gsl_version.h" GSL_VERSION_TEXT)
    string(REGEX REPLACE ".*define[ ]+GSL_MAJOR_VERSION[ ]*([0-9]+).*"  "\\1"
        GSL_MAJOR_VERSION "${GSL_VERSION_TEXT}")
    string(REGEX REPLACE ".*define[ ]+GSL_MINOR_VERSION[ ]*([0-9]+).*"  "\\1"
        GSL_MINOR_VERSION "${GSL_VERSION_TEXT}")
    set(GSL_VERSION "${GSL_MAJOR_VERSION}.${GSL_MINOR_VERSION}")
    message(STATUS "GSL version : ${GSL_VERSION}")
endif(GSL_INCLUDE_DIRS)

IF(GSL_LIBRARIES AND GSL_VERSION)
    IF(GSL_INCLUDE_DIRS)
        MESSAGE(STATUS "Found GSL ${GSL_LIBRARIES}")
        SET(GSL_FOUND 1)
    ENDIF(GSL_INCLUDE_DIRS)
ENDIF()

