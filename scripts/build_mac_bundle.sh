#!/bin/bash
echo "Create a bundle on MAC"
mkdir -p _build
(
    cd _build
    cmake -DCMAKE_INSTALL_PREFIX=_install ..
    make -j4
    cpack -G Bundle 
)
