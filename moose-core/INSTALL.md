# Building MOOSE from source

To build `MOOSE` from source, you need `cmake`.

Download the latest source code of moose from github.

    $ git clone -b master https://github.com/BhallaLab/moose-core --depth 50 

## Install dependencies

- gsl-1.16 or higher.
- libhdf5-dev (optional)
- python-dev
- python-numpy

On Ubuntu-14.04 or higher, these can be installed with:

    sudo apt-get install python-dev python-numpy libhdf5-dev cmake libgsl0-dev g++

SBML support is enabled by installing [python-libsbml](http://sbml.org/Software/libSBML/docs/python-api/libsbml-installation.html). Alternatively, it can be installed by using `python-pip`

    $ sudo pip install python-libsbml

## Use `cmake` to build moose:

    $ cd /path/to/moose-core
    $ mkdir _build
    $ cd _build
    $ cmake ..
    $ make -j3  
    $ ctest -j3 --output-on-failure

This will build moose, `ctest` will run few tests to check if build process was successful.

To install MOOSE into non-standard directory, pass additional argument `-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake.

### Python3

    cmake -DPYTHON_EXECUTABLE=/opt/bin/python3 ..

### Install

    $ sudo make install

## Post installation

Now you can import moose in a Python script or interpreter with the statement:

    import moose
    moose.test()   # will take time. Not all tests will pass.
