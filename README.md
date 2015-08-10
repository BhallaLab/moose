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

To uninstall moose, run the following command in the same directory where you ran
previous `make install` command.

    sudo make uninstall


# Installing using packages

## Ubuntu User

There is a [PPA here](https://launchpad.net/~bhallalab/+archive/ubuntu/moose).

You can use it on Ubuntu 12.04, 14.04, 14.10, and 15.04.
    
    sudo -E add-apt-repository ppa:bhallalab/moose
    sudo apt-get update
    sudo apt-get install moose-python moose-gui


## Others

We are currently in process of building packages for other distribution. We are
using [Open Build Service](https://build.opensuse.org/package/show/home:moose/moose).
