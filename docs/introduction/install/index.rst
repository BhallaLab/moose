Installation
============

Use pre-built packages
----------------------

We recommend that you use our repositories hosted at `Open Build Service
<http://build.opensuse.org>`_.  We have packages of both ``MOOSE`` and
``moogli`` for Debian, Ubuntu, CentOS, Fedora, OpenSUSE/SUSE, RHEL, Scientific
Linux.  Visit `this page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
and follow instructions.

.. raw:: html
    <iframe
    src="http://software.opensuse.org/download/package.iframe?project=<projectname>&package=<packagename>"></iframe>

After adding the repositories to your package manager (read the instructed on the site) 
You can install both `moose` and `moogli` using your package manager.

On ``Debian/Ubuntu``::
    $ sudo apt-get install moose moogli 

On ``CentOS/RHEL/Fedora/Scientific Linux``::
    $ sudo yum install moose moogli 

On ``openSUSE``::
    $ sudo zypper install moose moolgi

.. todo:: gentoo, Arch Linux

In case your distribution is not listed on `our repository page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
, or if you want to build the lastest development code, following section lists
out the steps to build MOOSE from its source code.

Building MOOSE 
--------------

First, you need to get the source code. You can use ``git`` (to clone the whole
repository) or download snapshot of github repo by clicking on `this link
<https://github.com/BhallaLab/moose/archive/master.zip>`_.::
    $ git clone https://github.com/BhallaLab/moose

Or,::
    $ wget https://github.com/BhallaLab/moose/archive/master.zip

If you don't want lasest snapshot of ``MOOSE``, you can download other released
versions from here `https://github.com/BhallaLab/moose/releases`.

Install dependencies
~~~~~~~~~~~~~~~~~~~
Next, you need to install required dependencies.

- cmake (version 2.8 or higher)
- gsl-1.16 or higher `Source code <ftp://ftp.gnu.org/gnu/gsl/>`_.
- libhdf5 development package.
- libsbml (5.9.0, optional). You can download it from
  `here <https://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/>`_

    Make sure that `libsml` is installed with `zlib` and `lxml` support.
    If you are using buildtools, then use the following to install libsbml::
        $ wget http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/libSBML-5.9.0-core-src.tar.gz
        $ tar -xzvf libSBML-5.9.0-core-src.tar.gz 
        $ cd libsbml-5.9.0 
        $ ./configure --prefix=/usr --with-zlib --with-libxml 
        $ make 
        $ sudo make install 

- python development package
- numpy 

On Ubuntu these can be installed by following command::
    $ sudo apt-get install libhdf5-dev cmake libgsl0-dev libpython-dev python-numpy 

On CentOS/Fedora/RHEL::
    $ sudo yum install hdf5-devel cmake libgsl-dev python-devel python-numpy

On OpenSUSE::
    $ sudo zypper install hdf5-devel cmake libgsl-dev python-devel python-numpy 

For MOOSE Graphical User Interface (``moose-gui``), some additional dependencies
are required
    
- matplotlib 
- Python-qt4

On Ubuntu/Debian, these can be installed with::
    $ sudo apt-get install python-matplotlib python-qt4

On CentOS/Fedora/RHEL::
    $ sudo yum install python-matplotlib python-qt4 

Now use `cmake` to build moose
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. codeblock:: bash
    $ cd /to/moose/source/code
    $ mkdir _build
    $ cd _build 
    $ cmake  ..
    $ make 
    $ ctest --output-on-failure  # optional

This will build pyMOOSE (MOOSE's python extention), `ctest` will run few tests to
check if build process was successful.

    To install MOOSE into non-standard directory, pass additional argument
    `-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake e.g.

        $ cmake -DCMAKE_INSTALL_PREFIC=$HOME/.local .. 

    To use, different version of python,

        $ cmake -DPYTHON_EXECUTABLE=/opt/python3/bin/python3 ..

After that installation is pretty easy.

    $ sudo make install

If everything went fine, you should be able to import moose in python shell.

.. codeblock:: python
    $ python 
    >>> import moose


Building moogli 
---------------

Prefer the packages from the repository.

``moogli`` is subproject of ``MOOSE`` for visualizing models. More details can
be found `here<http://moose.ncbs.res.in/moogli>`_.

Installing ``moogli`` could be tricky. It depends on ``OpenSceneGraph`` (version
3.0.x) which not be easily available for your system. Depending on
your distribution, you would need following packages to be installed.

- Development package of libopenscenegraph 
- `libQGLViewer-2.3.15-py<https://gforge.inria.fr/frs/?group_id=773>`_. Install
instructions `here<http://www.libqglviewer.com//installUnix.html#linux>`_.

- `PyQGLViewer0.10<https://gforge.inria.fr/frs/?group_id=773>`_ (first install
libQGLViewer-2.3.15-py) and untar contents::
    $ cd /path/to/PyQGLViewer0.10
    $ python setup.py build # to compile
    $ python setup.py install # to install on your system
    $ python setup.py bdist # to create a binary distribution

On Ubuntu, following packages should suffice::
    $ sudo apt-get install python-qt4-dev python-qt4-gl libopenscenegraph-dev python-sip-dev
    libqt4-dev 

