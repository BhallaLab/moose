#!/bin/bash
set -x
set -e
mkdir -p _build
(
    cd _build
    cmake -DCMAKE_C_COMPILER=gcc-mp-4.8 \
    -DCMAKE_CXX_COMPILER=g++-mp-4.8 .. \
    -DCMAKE_CXX_LINK_EXECUTABLE=/opt/local/bin/ld
    make VERBOSE=1 -j4
)
