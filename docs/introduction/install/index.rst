Installation
============

Use pre-built packages
----------------------

We recommend that you use our repositories hosted at [Open Build
Service](http://build.opensuse.org).  We have `MOOSE` and `moogli` packages for Debian, Ubuntu,
CentOS, Fedora, OpenSUSE/SUSE, RHEL, Scientific Linux.  Visit the following page
and follow the instructions. 

https://software.opensuse.org/download.html?project=home:moose&package=moose

After adding the repositories to you package manager (as instructed on the site) 
You can install both `moose` and `moogli` using your package manager:

On Debian/Ubuntu ::
    
    $ sudo apt-get install moose moogli 

On CentOS/RHEL/Fedora/Scientific Linux::
    
    $ sudo yum install moose moogli 

On openSUSE ::

    $ sudo zypper install moose moolgi


In case, your distribution is not listed on the repository page or
you want to build the lastest development code, following section lists out the
steps to build MOOSE from its source code.

Building from source
-------------------

First, Download the latest source code of moose from github using `git`::

    $ git clone https://github.com/BhallaLab/moose

Or, alternatively, you can download the ``zip`` file by clicking on the following
link, https://github.com/BhallaLab/moose/archive/master.zip. Unzip the file to
get the source code.

Install dependencies
~~~~~~~~~~~~~~~~~~~
Next, you have to install all required dependencies.

- cmake (version 2.8 or higher)
- gsl-1.16 or higher `Source code <ftp://ftp.gnu.org/gnu/gsl/>`_.
- libhdf5 development package. Get it from
- libsbml (5.9.0, optional). You can download it from
  `here <https://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/>`_

    Make sure that `libsml` is installed with `zlib` and `lxml` support.
    If you are using buildtools, then use the following to install libsbml.

        - wget http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/libSBML-5.9.0-core-src.tar.gz
        - tar -xzvf libSBML-5.9.0-core-src.tar.gz 
        - cd libsbml-5.9.0 
        - ./configure --prefix=/usr --with-zlib --with-libxml 
        - make 
        - sudo make install 

- python development package
- python numpy 

On Ubuntu these can be installed by following command:
    
    $ sudo apt-get install libhdf5-dev cmake libgsl0-dev 

On CentOS/Fedora/RHEL
    $ sudo yum install hdf5-devel cmake libgsl-dev

On OpenSUSE 
    $ sudo zypper install hdf5-devel cmake libgsl-dev

For MOOSE Graphical User Interface (GUI), there are additional dependencies: 
    
- matplotlib 
- Python-qt4

On Ubuntu/Debian, these can be installed with:

    $ sudo apt-get install python-matplotlib python-qt4

On CentOS/Fedora/RHEL,

    $ sudo yum install python-matplotlib python-qt4 

Now use `cmake` to build moose
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    $ cd /to/moose/source/code
    $ mkdir _build
    $ cd _build 
    $ cmake  ..
    $ make 
    $ ctest --output-on-failure

This will build pyMOOSE (MOOSE's python extention), `ctest` will run few tests to
check if build process was successful.

    To install MOOSE into non-standard directory, pass additional argument
    `-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake e.g.

        $ cmake -DCMAKE_INSTALL_PREFIC=$HOME/.local .. 

    To use, different version of python,

        $ cmake -DPYTHON_EXECUTABLE=/opt/python3/bin/python3 ..

After that installation is pretty easy.

    $ sudo make install

Building and installing moogli 
-----------------------------

Prefer the packages from the repository.

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
