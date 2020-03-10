# Building MOOSE 

To build `MOOSE` from source, you need `cmake` and `python-setuptools`. We
recommend to use Python 3.5 or higher. Python 2.7 is also supported. 

Before running the following command to build and install, make sure that
followings are installed.

- gsl-1.16 or higher.
- python-numpy

On Ubuntu-16.04 or higher, these dependencies can be installed with:

```
sudo apt-get install python-pip python-numpy cmake libgsl-dev g++
```

Now use `pip` to download and install the `pymoose`.

```
$ pip install git+https://github.com/BhallaLab/moose-core --user
```

## Using cmake (For developers)

`pip`  builds `pymoose` with default options, it runs `cmake` behind the scene.
If you are developing moose, build it with different options, or needs to test
and profile it, `cmake` based flow is recommended.

Install the required dependencies and download the latest source code of moose
from github.

    $ git clone https://github.com/BhallaLab/moose-core --depth 50 
    $ cd moose-core
    $ mkdir _build
    $ cd _build
    $ cmake ..
    $ make -j3  
    $ ctest -j3 --output-on-failure

This will build moose, `ctest` will run few tests to check if build process was
successful.

To install MOOSE into non-standard directory, pass additional argument
`-DCMAKE_INSTALL_PREFIX=path/to/install/dir` to during configuration. E.g.,

   $ mkdir _build && cd _build    # inside moose-core directory.
   $ cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
   $ make && make install

Will build and install pymoose to `~/.local`.

To use a non-default python installation, set
`PYTHON_EXECUTATBLE=/path/to/python` e.g.,

    $ cmake -DPYTHON_EXECUTABLE=/opt/bin/python3 ..

## Post installation

Now you can import moose in a Python script or interpreter with the statement:

    >>> import moose
    >>> moose.test()   # will take time. Not all tests will pass.

# Notes

SBML support is enabled by installing
[python-libsbml](http://sbml.org/Software/libSBML/docs/python-api/libsbml-installation.html).
Alternatively, it can be installed by using `python-pip`

    $ sudo pip install python-libsbml
