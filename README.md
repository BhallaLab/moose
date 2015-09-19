[![Build Status - master](https://travis-ci.org/BhallaLab/moose.svg?branch=master)](https://travis-ci.org/BhallaLab/moose)

This is meta repository of MOOSE simulator. It contains C++ core of MOOSE,
python scripting interface, its GUI repositories as submodule. 

For more details about these sub-projects, read respective README:

- [MOOSE and PyMOOSE](https://github.com/BhallaLab/moose-core)
- [Gui](https://github.com/BhallaLab/moose-gui)
- [Examples and Demos](https://github.com/BhallaLab/moose-examples)

# Installation using cmake

First, run the `bootstrap` script. It will fetch the submodules and/or update
them. You should have `cmake` installed on your system to build and install
moose from this repository.

    cmake .
    make 
    sudo make install

## Fine tuning build process

The cmake can be passed command line arguments to fine-tune the build process.
For example, we don't build documentation by default. If you want to build
documentation, use `-DWITH_DOC=ON` option as following:

    $ cmake -DWITH_DOC=ON ..
    $ make

Building documentation requires `python-sphinx` and `doxygen`.  __TODO: Document cmake options__.

# Uninstalling

To uninstall moose, run the following command in the same directory where you ran
previous `make install` command.

    sudo make uninstall

# Packages

The easiest way of installing MOOSE on your system is to use packages. 

## Ubuntu

There is a [PPA](https://launchpad.net/~bhallalab/+archive/ubuntu/moose).

You can use it on Ubuntu 12.04, 14.04, 14.10, and 15.04.
    
    sudo -E add-apt-repository ppa:bhallalab/moose
    sudo apt-get update
    sudo apt-get install moose-python moose-gui


## Others

We are building packages for other distribution as well. We use [Open Build Service](https://build.opensuse.org/package/show/home:moose/moose). Packages and installation instructions can be [found here](http://software.opensuse.org/download.html?project=home%3Amoose&package=moose).

# Reporting issues

Please report all build and packaging related issues here. For development
issue, bugs and feature request, please use the respective repositories

- [MOOSE C++ and python bindings](https://github.com/BhallaLab/moose-core)
- [PyQt GUI of MOOSE](https://github.com/BhallaLab/moose-gui)
- [Example snippets and tutorial
  scripts](https://github.com/BhallaLab/moose-examples)
