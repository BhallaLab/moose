# Copyright (C) 2012 - 2013 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

# Locate libsbml
# This module defines:
# LIBSBML_INCLUDE_DIRS, where to find the headers
#
# LIBSBML_LIBRARIES, LIBSBML_LIBRARIES_DEBUG
# LIBSBML_FOUND
#
# $LIBSBML_DIR is an environment variable that would
# correspond to the ./configure --prefix=$LIBSBML_DIR
#
# Created by Robert Osfield.
# Modified by Ralph Gauges
# Modified by Dilawar Singh

message (STATUS "Environment variable pointing to SBML?: $ENV{LIBSBML_DIR}")

find_path(LIBSBML_INCLUDE_DIRS sbml/SBase.h
    PATHS $ENV{LIBSBML_DIR}/include
          $ENV{LIBSBML_DIR}
          ~/Library/Frameworks
          /Library/Frameworks
          /sw/include        # Fink
          /usr
          /usr/include
          /usr/local/include
          /opt/local/include # MacPorts
          /opt/csw/include   # Blastwave
          /opt/include
          /usr/freeware/include
    NO_DEFAULT_PATH)

if (NOT LIBSBML_INCLUDE_DIRS)
    find_path(LIBSBML_INCLUDE_DIRS sbml/SBase.h)
endif (NOT LIBSBML_INCLUDE_DIRS)

find_library(LIBSBML_LIBRARIES
    NAMES libsbml
          sbml
	  sbml-static 
          libsbml-static 
          libsbml-static
          libsbml
    PATHS $ENV{LIBSBML_DIR}/lib
          $ENV{LIBSBML_DIR}
          /usr/lib
          /usr/local/lib
	  /usr/lib64
	  /usr/local/lib64
          ~/Library/Frameworks
          /Library/Frameworks
          /sw/lib        # Fink
          /sw/lib64        # Fink
          /opt/local/lib # MacPorts
	  /opt/local/lib64
          /opt/csw/lib   # Blastwave
          /opt/csw/lib64   # Blastwave
          /opt/lib
          /opt/lib64
          /usr/freeware/lib64
      )


if (NOT LIBSBML_LIBRARIES)
    find_library(LIBSBML_LIBRARIES
        NAMES sbml-static 
              sbml
        )
endif (NOT LIBSBML_LIBRARIES)

set(LIBSBML_FOUND "NO")
if(LIBSBML_LIBRARIES)
    if   (LIBSBML_INCLUDE_DIRS)
        SET(LIBSBML_FOUND "YES")
    endif(LIBSBML_INCLUDE_DIRS)
endif(LIBSBML_LIBRARIES)

# handle the QUIETLY and REQUIRED arguments and set LIBSBML_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBSBML DEFAULT_MSG LIBSBML_LIBRARIES LIBSBML_INCLUDE_DIRS)

mark_as_advanced(LIBSBML_INCLUDE_DIRS LIBSBML_LIBRARIES)
