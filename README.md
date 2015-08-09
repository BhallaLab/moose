Build Status - master (This is stable branch): [![Build Status - master](https://travis-ci.org/BhallaLab/moose.svg?branch=master)](https://travis-ci.org/BhallaLab/moose)

This is the meta repository of MOOSE simulator. It contains C++ core of MOOSE,
python scripting interface and its GUI. 

For more details about these projects, read respective README files.

# Installation using cmake

First, run the `bootstrap` script. It will fetch the submodules and/or update
them. You should have `cmake` installed on your system to build and install
moose from this repository.

    cmake .
    make 
    sudo make install

To uninstall moose, do the following in the same directory where you ran
previous `make install` command.

    sudo make uninstall
