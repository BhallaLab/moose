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

## Fine tuning build process

You can pass options to cmake from command line to fine-tune the build process.
For example, by default, we don't build documentation. If you want to build
documentation,

    $ cmake -DWITH_DOC=ON ..
    $ make

Doing so, cmake will also build documentation (and later install it when `make
install` is used). Building documentation requires `python-sphinx` and
`doxygen`.  __TODO: Document cmake options__.

# Uninstalling

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
using [Open Build Service](https://build.opensuse.org/package/show/home:moose/moose). Packages can be [found here](http://software.opensuse.org/download.html?project=home%3Amoose&package=moose)

# Reporting issues

Please report all build and packaging related issues here. For development
issue, bugs and feature request, please use the respective repositories

- [MOOSE C++ and python bindings](https://github.com/BhallaLab/moose-core)
- [PyQt GUI of MOOSE](https://github.com/BhallaLab/moose-gui)
- [Example snippets and tutorial
  scripts](https://github.com/BhallaLab/moose-examples)
