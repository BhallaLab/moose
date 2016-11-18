# Try to find gnu scientific library GSL  
# (see http://www.gnu.org/software/gsl/)
# Once run this will define: 
# 
# GSL_FOUND       = system has GSL lib
#
# GSL_LIBRARIES   = full path to the libraries
#    on Unix/Linux with additional linker flags from "gsl-config --libs"
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
message( STATUS "GSL_ROOT_DIR value $ENV{GSL_ROOT_DIR}")

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

    IF(GSL_USE_STATIC_LIBRARIES)
        SET(GSL_LIB_NAMES libgsl.a)
        SET(GSL_CBLAS_LIB_NAMES libgslclabs.a)
    ELSE(GSL_USE_STATIC_LIBRARIES)
        SET(GSL_LIB_NAMES gsl)
        SET(GSL_CBLAS_LIB_NAMES gslcblas)
    ENDIF( )

    FIND_LIBRARY(GSL_LIB 
        NAMES ${GSL_LIB_NAMES} 
        PATHS 
            $ENV{GSL_ROOT_DIR}/lib $ENV{GSL_ROOT_DIR}/lib64
            /opt/lib /opt/lib64
        )

    FIND_LIBRARY(GSLCBLAS_LIB 
        NAMES ${GSL_CBLAS_LIB_NAMES}
        PATHS 
            $ENV{GSL_ROOT_DIR}/lib $ENV{GSL_ROOT_DIR}/lib64
            /opt/lib /opt/lib64
        )

    FIND_PATH(GSL_INCLUDE_DIRS NAMES gsl/gsl_blas.h gsl/gsl.h
        PATHS 
            $ENV{GSL_ROOT_DIR}/include $ENV{GSL_ROOT_DIR}/include
            /opt/include /opt/include
        )

    IF (GSL_LIB AND GSLCBLAS_LIB)
        SET (GSL_LIBRARIES ${GSL_LIB} ${GSLCBLAS_LIB})
    ENDIF (GSL_LIB AND GSLCBLAS_LIB)

ENDIF(WIN32)


IF(GSL_LIBRARIES)
    IF(GSL_INCLUDE_DIRS OR GSL_CXX_FLAGS)
        SET(GSL_FOUND 1)
        MESSAGE(STATUS "Found GSL ${GSL_LIBRARIES}")
    ENDIF(GSL_INCLUDE_DIRS OR GSL_CXX_FLAGS)
ENDIF(GSL_LIBRARIES)

