#!/bin/bash -
#===============================================================================
#
#          FILE: travis_build_linux.sh
#
#         USAGE: ./travis_build_linux.sh
#
#   DESCRIPTION:  Build  on linux environment.
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 01/02/2017 10:11:46 AM
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
set -e

BUILDDIR=_build_travis

NPROC=$(nproc)
(
    # Make sure not to pick up python from /opt.
    PATH=/usr/local/bin:/usr/bin:$PATH

    PYTHON3=$(which python3)

    # Get pylint
    $PYTHON3 -m pip install pylint --user
    $PYTHON3 -m pip install python-libsbml --user
    $PYTHON3 -m pip install pyneuroml --user

    mkdir -p $BUILDDIR && cd $BUILDDIR \
        && cmake -DPYTHON_EXECUTABLE=$PYTHON3 \
        ..
    make pylint -j$NPROC
    make -j$NPROC && MOOSE_NUM_THREAD=$NPROC ctest --output-on-failure -j$NPROC

    cd .. # Now with boost.
    mkdir -p $BUILDDIR && cd $BUILDDIR \
        && cmake -DWITH_BOOST_ODE=ON \
        -DPYTHON_EXECUTABLE=`which python3` ..

    make -j$NPROC && MOOSE_NUM_THREAD=$NPROC ctest -j$NPROC --output-on-failure 
)
set +e
