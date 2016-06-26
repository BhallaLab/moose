# Pre-built packages

Use our repositories hosted at [Open Build Service](http://build.opensuse.org).
We have packages for Debian, Ubuntu, CentOS, Fedora, OpenSUSE/SUSE, RHEL,
Scientific Linux.  Visit the following page and follow the instructions there. 

https://software.opensuse.org/download.html?project=home:moose&package=moose

# Building MOOSE from source

If you really want to build `MOOSE` from source, you can either use `cmake` (recommended) 
or GNU `make` based flow.

Download the latest source code of moose from github or sourceforge.

    $ git clone -b master https://github.com/BhallaLab/moose-core

## Install dependencies

For moose-core:

- gsl-1.16 or higher.
- libhdf5-dev (optional) 
- libsbml-dev (5.9.0, optional)
- python-dev 
- python-numpy 

__Note on libsbml__
Make sure that `libsml` is installed with `zlib` and `lxml` support. Following instructions 
are known to work.

    - wget http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/libSBML-5.9.0-core-src.tar.gz
    - tar -xzvf libSBML-5.9.0-core-src.tar.gz 
    - cd libsbml-5.9.0 
    - ./configure --prefix=/usr --with-zlib --with-bzip2 --with-libxml 
    - make 
    - sudo make install 

On Ubuntu-12.04 or higher, these can be installed with:
    
    sudo apt-get install python-dev python-numpy libhdf5-dev cmake libgsl0-dev g++ 

__NOTE__ : On Ubuntu 12.04, gsl version is 1.15. You should skip `libgsl0-dev` install gsl-1.16 or higher manually.

## Use `cmake` to build moose:
    
    $ cd /path/to/moose-core 
    $ mkdir _build
    $ cd _build 
    $ cmake ..
    $ make  
    $ ctest --output-on-failure

This will build moose and its python extentions, `ctest` will run few tests to
check if build process was successful.

To install MOOSE into non-standard directory, pass additional argument `-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake.

### Python3 

You just need to one command in previous set of instructions to following 

    cmake -DPYTHON_EXECUTABLE=/opt/bin/python3 ..

### Install

    $ sudo make install

##  Using gnu-make

__This may or may not work (not maintained by packager)__

You may need to inform make of C++ include directories and library directories
if your installed packages are at non-standard location. For example, if your
have libsbml installed in `/opt/libsbml` and the header files are located in
`/opt/libsbml/include` and lib files are located in `/opt/libsbml/lib`, you can
set the environment variables `CXXFLAGS` and `LDFLAGS` to include these before
calling make:

    export CXXFLAGS= -I/opt/libsbml/include
    export LDFLAGS= -L/opt/libsbml/lib

  
## Release build:

    cd moose
    make BUILD=release

## Debug build:
    
    cd moose
    make BUILD=debug

## Python 3K

By default, MOOSE is built for Python 2. In case you want to build MOOSE for
Python 3K, you need to pass the additional flag:

    PYTHON=3

like:
     
    make BUILD=release PYTHON=3

## Installation:

For system-wide installation you can run:

    sudo make install

## Post installation 

Now you can import moose in a Python script or interpreter with the statement:

    import moose
     
If you have installed the GUI dependencies below for running the graphical user
interface, then you can run the GUI by double-clicking on the desktop icon or
via the main-menu.  The squid axon tutorial/demo is also accessible via these
routes.

## Local-installation

If you do not have permission to install it in system directories, you can let
it be where it was built or copy the `python` subdirectory of MOOSE source tree
to a location of your choice and add the path to your PYTHONPATH environment
variable. Suppose you have a ~/lib directory where you keep all your locally
built libraries, do:

    cp -r {moose-source-directory}/python ~/lib/
 
and add this to your .bashrc file (if you use bash shell):

    export PYTHONPATH="$HOME/lib/python":"$PYTHONPATH"

For other shells, look up your shell's manual to find out how to set environment
variable in it.
