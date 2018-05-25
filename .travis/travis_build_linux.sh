#!/usr/bin/env bash

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

set -e -x

PYTHON2="/usr/bin/python2"
PYTHON3="/usr/bin/python3"
MAKEFLAGS="-j`nproc`"


if [ ! -n "$MAKE" ]; then
    MAKE="make -j`nproc`"
else
    MAKE="$MAKE -j`nproc`"
fi

unset PYTHONPATH

# Bug: `which python` returns /opt/bin/python* etc on travis. For which numpy
# many not be available. Therefore, it is neccessary to use fixed path for
# python executable.

$PYTHON2 -m compileall -q .
if type $PYTHON3 > /dev/null; then $PYTHON3 -m compileall -q . ; fi

echo "Currently in `pwd`"
(
    mkdir -p _GSL_BUILD && cd _GSL_BUILD 
    cmake -DDEBUG=ON -DPYTHON_EXECUTABLE="$PYTHON2" ..
    $MAKE && ctest --output-on-failure
    sudo make install 
    $PYTHON2 -c 'import moose;print(moose.__file__);print(moose.version())'
)

(
    # Now with boost.
    mkdir -p _BOOST_BUILD && cd _BOOST_BUILD && \
        cmake -DWITH_BOOST=ON -DDEBUG=ON -DQUIET_MODE=ON -DPYTHON_EXECUTABLE="$PYTHON2" ..
    $MAKE && ctest --output-on-failure
)

# This is only applicable on linux build.
echo "Python3: Removed python2-networkx and install python3"
if type $PYTHON3 > /dev/null; then
    sudo apt-get remove -qq python-networkx
    sudo apt-get install -qq python3-networkx
    (
        mkdir -p _GSL_BUILD2 && cd _GSL_BUILD2 && \
            cmake -DDEBUG=ON -DPYTHON_EXECUTABLE="$PYTHON3" ..
        $MAKE && ctest --output-on-failure
    )
    (
        mkdir -p _BOOST_BUILD2 && cd _BOOST_BUILD2 && \
            cmake -DWITH_BOOST=ON -DDEBUG=ON -DPYTHON_EXECUTABLE="$PYTHON3" ..
        $MAKE && ctest --output-on-failure
    )
    echo "All done"
else
    echo "Python3 is not found. Build disabled"
fi
