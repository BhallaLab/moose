# Pre-built packages

Use our repositories hosted at [Open Build Service](http://build.opensuse.org).
We have packages for Debian, Ubuntu, CentOS, Fedora, OpenSUSE/SUSE, RHEL,
Scientific Linux.  Visit the following page and follow the instructions there. 

https://software.opensuse.org/download.html?project=home:moose&package=moose


# Building from source

First, Download the latest source code of moose from github.

    $ git clone https://github.com/BhallaLab/moose
    $ cd moose

## Install dependencies

For moose-core:

- gsl-1.16 or higher.
- libhdf5 
- libsbml (optional)

    Make sure that `libsml` is installed with `zlib` and `lxml` support.
    If you are using buildtools, then use the following to install libsbml.

        - wget http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/libSBML-5.9.0-core-src.tar.gz
        - tar -xzvf libSBML-5.9.0-core-src.tar.gz 
        - cd libsbml-5.9.0 
        - ./configure --prefix=/usr --with-zlib --with-bzip2 --with-libxml 
        - make 
        - ctest --output-on-failure # optional
        - sudo make install 

For python module of MOOSE, following additional packages are required:

- Development package of python e.g. libpython-dev 
- python-numpy 

For python-gui, we need some more addtional packages
    
- matplotlib
- setuptools  (cmake uses it to install moose python extension and moogli)
- suds 
- Python bindings for Qt4 or higher
- Python OpenGL
- Python bindings for Qt's OpenGL module

On Ubuntu-120.4 or higher, these can be installed with:
    
    sudo apt-get install python-matplotlib python-qt4 python-qt4-gl 

## Use `cmake` to build moose:

    $ mkdir _build
    $ cd _build 
    $ cmake -DWITH_DOC=OFF ..
    $ make 
    $ ctest --output-on-failure

This will build moose and its python extentions, `ctest` will run few tests to
check if build process was successful.

To install MOOSE into non-standard directory, pass additional argument
`-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake.

After that installation is pretty easy.

## Install

    $ sudo make install

# Building and installing moogli 

MOOGLI is subproject of moogli for visualizing models. Details can be found
[here](http://moose.ncbs.res.in/moogli).

MOOGLI dependencies are huge! It uses `OpenSceneGraph` which has its own
dependencies. In nutshell, depending on your distribution, you would need
following packages to be installed.

- Development package of libopenscenegraph 
- [libQGLViewer-2.3.15-py](https://gforge.inria.fr/frs/?group_id=773). Install
instructions [here](http://www.libqglviewer.com//installUnix.html#linux)

- [PyQGLViewer0.10](https://gforge.inria.fr/frs/?group_id=773) (first install
libQGLViewer-2.3.15-py) and untar contents.

    $ cd / PyQGLViewer0.10
    $ python setup.py build # to compile
    $ python setup.py install # to install on your system
    $ python setup.py bdist # to create a binary distribution

On Ubuntu, following packages should suffice:

    $ sudo apt-get install python-qt4-dev python-qt4-gl libopenscenegraph-dev python-sip-dev
    libqt4-dev 


## Travis

We use `Travis-CI` to build MOOSE after every commit. You can see `.travis.yml`
file in our repository. It has all instructions to build MOOSE on `Ubuntu-12.04
64bit` server.
