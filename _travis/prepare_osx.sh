#!/bin/bash -
#===============================================================================
#
#          FILE: prepare_osx.sh
#
#         USAGE: ./prepare_osx.sh
#
#   DESCRIPTION: 
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: Wednesday 28 June 2017 10:23:28  IST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
brew install gsl
brew install cmake 
brew install homebrew/science/hdf5
brew install homebrew/science/matplotlib
