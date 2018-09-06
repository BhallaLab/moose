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

# NOTE: On travis, don't enable -j`nproc` option. It may not compile properly.

(

    # Make sure not to pick up python from /opt.
    PATH=/usr/bin:/usr/local/bin:$PATH
    mkdir -p _GSL_BUILD && cd _GSL_BUILD \
        && cmake -DDEBUG=ON \
        -DPYTHON_EXECUTABLE=`which python` ..
    make && ctest --output-on-failure

    cd .. # Now with boost.
    mkdir -p _BOOST_BUILD && cd _BOOST_BUILD \
        && cmake -DWITH_BOOST_ODE=ON -DDEBUG=ON \
        -DPYTHON_EXECUTABLE=`which python` ..

    make && ctest --output-on-failure
    cd ..
    set +e

)
set +e

