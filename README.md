[![Build Status - master](https://travis-ci.org/BhallaLab/moose.svg?branch=master)](https://travis-ci.org/BhallaLab/moose)

# About

MOOSE is the Multiscale Object-Oriented Simulation Environment. It is designed
to simulate neural systems ranging from subcellular components and biochemical
reactions to complex models of single neurons, circuits, and large networks.
MOOSE can operate at many levels of detail, from stochastic chemical
computations, to multicompartment single-neuron models, to spiking neuron
network models.

MOOSE is multiscale: It can do all these calculations together. For example it
handles interactions seamlessly between electrical and chemical signaling. MOOSE
is object-oriented. Biological concepts are mapped into classes, and a model is
built by creating instances of these classes and connecting them by messages.
MOOSE also has classes whose job is to take over difficult computations in a
certain domain, and do them fast. There are such solver classes for stochastic
and deterministic chemistry, for diffusion, and for multicompartment neuronal
models. MOOSE is a simulation environment, not just a numerical engine: It
provides data representations and solvers (of course!), but also a scripting
interface with Python, graphical displays with Matplotlib, PyQt, and OpenGL, and
support for many model formats. These include SBML, NeuroML, GENESIS kkit and
cell.p formats, HDF5 and NSDF for data writing.

This is the parent repository of [MOOSE simulator](https://moose.ncbs.res.in).
It keeps the snanshots of components of MOOSE: core (written in C++) of MOOSE,
python scripting interface, and its graphical user interface (GUI). This is your
source stable MOOSE code.

# VERSION

This is MOOSE 3.0.2pre "Ghevar"

# ABOUT VERSION 3.0.2, Ghevar

The Ghevar release is the third of series 3 of MOOSE releases.

Ghevar is a Rajasthani sweet with a stiff porous body soaked in sugar syrup.

MOOSE 3.0.2pre is an evolutionary increment over 3.0.1::

- There has been substantial development on the multiscale modeling front, with
the implementation of the rdesigneur class and affiliated features. 
- MOOSE can now read NeuroMorpho .swc files natively.

# LICENSE

MOOSE is released under the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

# HOMEPAGE 

http://moose.ncbs.res.in/

# SOURCE REPOSITORY

Old [SourceForge repository](https://sourceforge.net/projects/moose/) is no
longer maintained. Current source repository is hosted on
[github](https://github.com/BhallaLab/moose-core) with almost all revision
history.

# Building and installing

See the file `INSTALL.md`.

# MOOSE repositories

You can find detailed description of each MOOSE component in respective
`README.md` in  their repositories below:

- [MOOSE with python support](https://github.com/BhallaLab/moose-core)
- [GUI](https://github.com/BhallaLab/moose-gui)
- [Examples and Demos](https://github.com/BhallaLab/moose-examples)
- [MOOGLI](https://github.com/BhallaLab/moogli) 
