#!/usr/bin/env bash
set -e
REPODIR=$(pwd)
TEMPDIR=$HOME/moose-example-test
mkdir -p $TEMPDIR
# Run in the current directory. Unfortunately travis is not pushing from other
# directory.
( 
    cd $TEMPDIR
    cmake -DPYTHON_EXECUTABLE=/usr/bin/python $REPODIR && make -j4 

    echo "We did not run following scripts"
    if [[ -f SUCCEEDED ]]; then
        echo "Following scripts ran successfully"
        cat SUCCEEDED
    fi

    if [ -f FAILED ]; then
        echo "Following scripts failed"
        cat FAILED 
    fi

    if [ -f TIMEOUT ]; then
        echo "Following scripts timed-out"
        cat TIMEOUT
    fi
)
