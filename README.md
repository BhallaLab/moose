[![Build Status - master](https://travis-ci.org/BhallaLab/moose-core.svg?branch=master)](https://travis-ci.org/BhallaLab/moose-core) | [![PyPI version](https://badge.fury.io/py/pymoose.svg)](https://badge.fury.io/py/pymoose) | [Nightly Linux Build on OBS](https://software.opensuse.org//download.html?project=home%3Amoose&package=pymoose)

This is the core computational engine of [MOOSE simulator](https://github.com/BhallaLab/moose). This repository contains
C++ codebase and python interface called `pymoose`. For more details about MOOSE simulator, visit https://moose.ncbs.res.in .

# Download and Install

This repository is sufficient for using MOOSE as a python module. We provide python package via `pip`.

    $ pip install pymoose --user 

`pymoose` is part of [MOOSE simulator](https://github.com/BhallaLab/moose). We also provide linux packages of 
MOOSE simlulator. It can be downloaded from [Open Build Service](https://software.opensuse.org//download.html?project=home%3Amoose&package=moose). 
In addition to `pymoose`, it also contain a `GUI` to create and visualize chemical network, and `moogli`, a visualizer of neural activity.  

# Build 

To build `pymoose`, follow instructions given here at https://github.com/BhallaLab/moose-core/blob/master/INSTALL.md 
