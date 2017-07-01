#!/bin/bash -
#===============================================================================
#
#          FILE: travis_prepare_osx.sh
#
#         USAGE: ./travis_prepare_osx.sh
#
#   DESCRIPTION: Script to prepare OSX build on Travis CI.
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 01/02/2017 10:09:00 AM
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
set +e
rvm get head
brew update
#brew outdated cmake || brew install cmake
brew install gsl
brew outdated hdf5 || brew install homebrew/science/hdf5
brew outdated libsbml || brew install homebrew/science/libsbml
#brew outdated python || brew install python
#brew outdated numpy || brew install homebrew/python/numpy
#brew unlink numpy && brew link numpy || echo "Failed to link numpy"
# Numpy caveats
mkdir -p $HOME/Library/Python/2.7/lib/python/site-packages
echo 'import sys; sys.path.insert(1, "/usr/local/lib/python2.7/site-packages")' >> $HOME/Library/Python/2.7/lib/python/site-packages/homebrew.pth
pip install matplotlib

