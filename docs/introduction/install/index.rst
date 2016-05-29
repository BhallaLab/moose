Installation
============

Use pre-built packages
----------------------

Linux
^^^^^

We recommend that you use our repositories hosted at `Open Build Service
<http://build.opensuse.org>`_.  We have packages of both ``MOOSE`` and
``moogli`` for following distributions

- Debian 7.0, 8.0
- Ubuntu 12.04, 14.04, 15.04, 15.10, 16.04 
- CentOS 6, 7 
- Fedora 20, 21, 22, 23
- OpenSUSE 13.1, 13.2, Factory ARM, Leap 42.1, Tumbleweed
- SLE 12, 12 SP1
- RHEL 7
- Scientific Linux 7
  
Visit `this page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
pick you distribution and follow instructions.

.. note:: 
    Package ``moogli`` will also be available in addition to moose on almost all
    of these distributions (except CentOS-6). Use the same command to install
    moogli as well e.g. ``sudo apt-get install moogli`` or ``sudo yum
    install moogli``.

.. raw:: html
    <iframe
    src="http://software.opensuse.org/download/package.iframe?project=<projectname>&package=<packagename>"></iframe>

.. todo:: Packages for gentoo, Arch Linux


Mac OSX
^^^^^^

Download the ``dmg`` file from `here <https://github.com/BhallaLab/moose/releases/download/ghevar_3.0.2/Moose_3.0.2_OSX.dmg>`_.

Alternatively, you can use ``homebrew`` to install ``moose``
::
    $ brew install moose 

.. note::
    Check the status of pull request here https://github.com/Homebrew/homebrew-science/pull/2958

Windows/Cygwin
^^^^^^^^^^^^^^

Building MOOSE 
--------------

In case your distribution is not listed on `our repository page
<https://software.opensuse.org/download.html?project=home:moose&package=moose>`_
, or if you want to build the lastest development code, read on.

First, you need to get the source code. You can use ``git`` (clone the 
repository) or download snapshot of github repo by clicking on `this link
<https://github.com/BhallaLab/moose/archive/master.zip>`_.::
    $ git clone https://github.com/BhallaLab/moose

Or,
::
    $ wget https://github.com/BhallaLab/moose/archive/master.zip
    $ unzip master.zip

If you don't want lasest snapshot of ``MOOSE``, you can download other released
versions from here `https://github.com/BhallaLab/moose/releases`.

Install dependencies
~~~~~~~~~~~~~~~~~~~
Next, you need to install required dependencies.

- cmake (version 2.8 or higher)
- gsl-1.16 or higher `download <ftp://ftp.gnu.org/gnu/gsl/>`_.
- libhdf5 development package.
- libsbml (5.9.0, optional).  `download <https://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/>`_
- python development package
- numpy 

.. note:: 
    `libsml` needs to be compiled with ``zlib`` and ``lxml`` support without
    which, you might face link time errors. Following instructions are known to 
    work::
        $ wget http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/libSBML-5.9.0-core-src.tar.gz
        $ tar -xzvf libSBML-5.9.0-core-src.tar.gz 
        $ cd libsbml-5.9.0 
        $ ./configure --with-zlib --with-libxml 
        $ make 
        $ sudo make install 

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

.. code-block:: python
    import moose


Graphical User Interface (GUI)
------------------------------

You can get the source of ``moose-gui`` from `here
<https://github.com/BhallaLab/moose-gui>`_. You can download it either by
clicking on `this link <https://github.com/BhallaLab/moose-gui/archive/master.zip>`_ 
or by using ``git`` ::
    $ git clone https://github.com/BhallaLab/moose-gui

To be able to launch ``moose-gui``, you need to install some more packages

- matplotlib 
- python-qt4

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
    If you have installed ``moose`` using the package, then GUI is launched by
    running following commnad::
        $ moosegui

Building moogli 
---------------

``moogli`` is subproject of ``MOOSE`` for visualizing models. More details can
be found `here <http://moose.ncbs.res.in/moogli>`_.

Prefer installing ``moogli`` packages from the repository as indicated on the
top of this page. Link to ``moogli`` repository is given below

`https://software.opensuse.org/download.html?project=home%3Amoose&package=moogli`

.. note::
    If you have already added the repository, ``moogli`` is already available in
    your package-manager.


To get the latest source code of ``moogli``, click on `this link <https://github.com/BhallaLab/moogli/archive/master.zip>`_.

Building ``moogli`` could be tricky. It depends on ``OpenSceneGraph`` (version
3.2.0 or higher) which may not be easily available for your operating system.
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
