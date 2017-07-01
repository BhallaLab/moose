#!/bin/bash
mkdir -p _build
(
    cd _build
    rm -f moose*.dmg
    rm -rf moose-3.0.1-Linux*
    cmake -DCMAKE_INSTALL_PREFIX=/Moose.app ..
    make -j4
    #cpack -G Bundle
    cpack -G DragNDrop -V
    7z x moose-3.0.1-Linux-.dmg
)
