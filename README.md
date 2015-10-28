[![Build Status - master](https://travis-ci.org/BhallaLab/moose.svg?branch=master)](https://travis-ci.org/BhallaLab/moose)

This is the parent repository of [MOOSE simulator](https://moose.ncbs.res.in). It contains core (written in C++) of MOOSE, it's python scripting interface, and its graphical user interface (GUI) as [git-submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). 

You can find detailed description of each MOOSE component in respective `README.md` in  their repositories below:

- [MOOSE with python support](https://github.com/BhallaLab/moose-core)
- [GUI](https://github.com/BhallaLab/moose-gui)
- [Examples and Demos](https://github.com/BhallaLab/moose-examples)

# MOOSE Packages

The easiest way of installing MOOSE on your system is to use pre-compiled packages.

## Debian/Ubuntu

MOOSE has [a PPA](https://launchpad.net/~bhallalab/+archive/ubuntu/moose). 

You can use it on Ubuntu 12.04, 14.04, 14.10, and 15.04. 

### Terminal
    
    sudo -E add-apt-repository ppa:bhallalab/moose
    sudo apt-get update
    sudo apt-get install moose-python moose-gui

For more information on how to use PPA with package manager, see [What are PPAs and how to use them](http://askubuntu.com/questions/4983/what-are-ppas-and-how-do-i-use-them)


## RPM/Fedora and others

There are packages for other distributions as well, especially Feodra, CentOS and OpenSuse. We are using [Open Build Service](https://build.opensuse.org/package/show/home:moose/moose) to build these packages. Download and installation instructions can be [found here](http://software.opensuse.org/download.html?project=home%3Amoose&package=moose).


# Build and install using cmake

[See the instructions here](https://github.com/BhallaLab/moose-core/blob/master/INSTALL.cmake.md). Once all the listed dependencies are installed, you can use the following script to build and install MOOSE on your system.

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

# Reporting issues

Please report all build and packaging related issues here. For development
issue, bugs and feature request, please use the respective repositories

- [MOOSE C++ and python bindings](https://github.com/BhallaLab/moose-core)
- [PyQt GUI of MOOSE](https://github.com/BhallaLab/moose-gui)
- [Example snippets and tutorial
  scripts](https://github.com/BhallaLab/moose-examples)
