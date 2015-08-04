#!/bin/bash
set -e
set -x
(
cd ..
mkdir -p _build
cd _build
cmake ..
make | tee $0.log
cpack -G DEB
sudo dpkg -i moose*.deb
moosegui
)
