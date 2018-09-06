Use pre-built packages
-----------------------

pip
^^^^

If you only need `python` interface, use `pip`. This is the easiest way to install Python interface.
This solution has been tested for Linux and MacOSX.
::
     pip install pymoose 

We also build moose package with additional components such as gui and `moogli`. 

Linux
^^^^^^

We recommend that you use our repositories hosted at `Open Build Service
<https://build.opensuse.org/package/show/home:moose/moose>`_.  Packages for most 
linux distributions are available. Visit `this page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
to pick your distribution and follow instructions.

.. note::
    ``moogli`` (tool to visualize network activity) is not available for CentOS-6.

.. todo:: Packages for gentoo

Mac OSX
^^^^^^^^

MacOSX support for moose-gui is not complete yet. However, the python-scripting interface can be installed on OSX using ``homebrew``
::
    $ brew tap BhallaLab/moose
    $ brew install moose

Or alternatively, via pip 
::
    $ pip install pymoose --user 
    

Docker Images
^^^^^^^^^^^^^^

Docker images of stable version are available from public repository.
::
      $ docker pull bhallalab/moose
      $ docker run -it --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY bhallalab/moose
      
This will launch `xterm`; run `moosegui` in terminal to lauch the GUI. 


Building MOOSE
--------------

In case your distribution is not listed on `our repository page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
, or if you want to build the latest development code, read on.

First, you need to get the source code. You can use ``git`` (clone the
repository) or download snapshot of github repo by clicking on `this link
<https://github.com/BhallaLab/moose/archive/master.zip>`__.
::

    $ git clone https://github.com/BhallaLab/moose

(This will create folder called "moose")
Or,
::

    $ wget https://github.com/BhallaLab/moose/archive/master.zip
    $ unzip master.zip

If you don't want latest snapshot of ``MOOSE``, you can download other released
versions from `here <https://github.com/BhallaLab/moose/releases>`__.

Install dependencies
^^^^^^^^^^^^^^^^^^^^

Next, you need to install required dependencies. Depending on your OS, names of
following packages may vary.

Core MOOSE
""""""""""
- Required:
    - cmake (version 2.8 or higher)
    - g++ (>= 4.6.x) For building the C++ MOOSE core.
    - gsl-1.16 or higher.

- Optional
    - HDF5 (>=1.8.x) For reading and writing data into HDF5 based formats

Python interface for core MOOSE API (pymoose)
"""""""""""""""""""""""""""""""""""""""""""""
- Required
    - Python2 ( >= 2.7.x) For building the MOOSE Python bindings
    - Python-dev ( >= 2.7.x) Python development headers and libraries, e.g. `python-dev` or `python-devel`
    - NumPy ( >= 1.6.x) For array interface, e.g. `python-numpy` or `numpy`

- Optional
    - NetworkX (1.x) For automatical layout
    - pygraphviz For automatic layout for chemical models
    - Matplotlib (>=1.1.x) For plotting simulation results
    - python-libsbml For reading and writing chemical models from and into SBML format

Most of the dependencies can be installed using package manager.

On ``Debian/Ubuntu``
::

    $ sudo apt-get install libhdf5-dev cmake libgsl0-dev libpython-dev python-numpy

.. note::

    Ubuntu 12.04 does not have required version of ``gsl`` (required 1.16 or
    higher, available 1.15). On Ubuntu 16.04, package name is ``libgsl-dev``.

On ``CentOS/Fedora/RHEL/Scientific Linux``
::

    $ sudo yum install hdf5-devel cmake libgsl-dev python-devel python-numpy

On ``OpenSUSE``
::

  $ sudo zypper install hdf5-devel cmake libgsl-dev python-devel python-numpy

build moose
^^^^^^^^^^^

.. code-block:: bash

   $ cd /to/moose_directory/moose-core/ 
   $ mkdir _build
   $ cd _build
   $ cmake  ..
   $ make
   $ ctest --output-on-failure  # optional
   $ sudo make install 

This will build pyMOOSE (MOOSE's python extention), `ctest` will run few tests to
check if build process was successful.

.. note::

  To install MOOSE into non-standard directory, pass additional argument
  `-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to cmake
  ::

    $ cmake -DCMAKE_INSTALL_PREFIC=$HOME/.local ..

  To use different version of python
  ::

    $ cmake -DPYTHON_EXECUTABLE=/opt/python3/bin/python3 ..

After that installation is pretty easy
::

  $ sudo make install

If everything went fine, you should be able to import moose in python shell.

.. code-block::  python

   >>> import moose

Graphical User Interface (GUI)
------------------------------

If you have installed the pre-built package, then you already have the GUI.
You can launch it by runnung `moosegui` command in terminal.

You can get the source of ``moose-gui`` from `here
<https://github.com/BhallaLab/moose-gui>`__. You can download it either by
clicking on `this link <https://github.com/BhallaLab/moose-gui/archive/master.zip>`__
or by using ``git`` ::

    $ git clone https://github.com/BhallaLab/moose-gui


Alternatively the moose-gui folder exists within the moose folder downloaded and built earlier in the installation process. It can be found under ``location_of_moose_folder/moose/moose-gui/``.

Below are packages which you may want to install to use MOOSE Graphical User Interface.

- Required:
    - PyQt4 (4.8.x) For Python GUI
    - Matplotlib ( >= 1.1.x) For plotting simulation results
    - NetworkX (1.x) For automatical layout
    - suds/suds-jurko (0.4) For accessing models hosted on biomodels database.
- Optional:
    - python-libsbml For reading and writing signalling models from and into SBML format

On ``Ubuntu/Debian``, these can be installed with
::

    $ sudo apt-get install python-matplotlib python-qt4

On ``CentOS/Fedora/RHEL``
::

    $ sudo yum install python-matplotlib python-qt4

Now you can fire up the GUI
::

    $ cd /to/moose-gui
    $ python mgui.py

.. note::

    If you have installed ``moose`` package, then GUI is launched by
    running following commnad::

    $ moosegui

Building moogli
---------------

``moogli`` is subproject of ``MOOSE`` for visualizing models. More details can
be found `here <http://moose.ncbs.res.in/moogli>`__.

`Moogli` is part of `moose` package. Building moogli can be tricky because of
multiple depednecies it has.

- Required
    - OSG (3.2.x) For 3D rendering and simulation of neuronal models
    - Qt4 (4.8.x) For C++ GUI of Moogli

To get the latest source code of ``moogli``, click on `this link <https://github.com/BhallaLab/moogli/archive/master.zip>`__.

Moogli depends on ``OpenSceneGraph`` (version 3.2.0 or higher) which may not
be easily available for your operating system.
For this reason, we distribute required ``OpenSceneGraph`` with ``moogli``
source code.

Depending on distribution of your operating system, you would need following
packages to be installed.

On ``Ubuntu/Debian``
::

    $ sudo apt-get install python-qt4-dev python-qt4-gl python-sip-dev libqt4-dev

On ``Fedora/CentOS/RHEL``
::

    $ sudo yum install sip-devel PyQt4-devel qt4-devel libjpeg-devel PyQt4

On ``openSUSE``
::

    $ sudo zypper install python-sip python-qt4-devel libqt4-devel python-qt4

After this, building and installing ``moogli`` should be as simple as
::

    $ cd /path/to/moogli
    $ mkdir _build
    $ cd _build
    $ cmake ..
    $ make
    $ sudo make install

If you run into troubles, please report it on our `github repository
<https://github.com/BhallaLab/moose/issues>`_.
