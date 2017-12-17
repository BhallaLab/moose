[![Build Status - master](https://travis-ci.org/BhallaLab/moose.svg?branch=master)](https://travis-ci.org/BhallaLab/moose)   [![Documentation Status](https://readthedocs.org/projects/moose/badge/?version=latest)](https://readthedocs.org/projects/moose/?badge=latest)
[__Linux Packages__](https://software.opensuse.org//download.html?project=home%3Amoose&package=moose) 
[__Document__](http://moose.readthedocs.io/en/latest/)

This is the parent repository of [MOOSE simulator](https://moose.ncbs.res.in).
It keeps the snanshots of its components: core (written in C++) and python 
scripting interface, MOOSE's Graphical User Interface (GUI). This should be the 
source of stable MOOSE code.

# About

MOOSE is the Multiscale Object-Oriented Simulation Environment. It is designed to simulate neural systems ranging from biochemical signaling to complex models of single neurons, circuits, and large networks. A typical use case is to model neural activity-driven synaptic plasticity, in which network activity to synapses triggers biochemical reaction-diffusion events, which in turn modulate ion channels to give rise to synaptic plasticity.

MOOSE can operate at many levels of detail, from stochastic chemical computations, to multicompartment single-neuron models, to spiking neuron network models.

MOOSE is multiscale: It can do all these calculations together. For example it handles interactions seamlessly between electrical and chemical signaling. MOOSE is object-oriented. Biological concepts are mapped into classes, and a model is built by creating instances of these classes and connecting them by messages. MOOSE also has classes whose job is to take over difficult computations in a certain domain, and do them fast. There are such solver classes for stochastic and deterministic chemistry, for diffusion, and for multicompartment neuronal models. MOOSE is a simulation environment, not just a numerical engine: It provides data representations and solvers (of course!), but also a scripting interface with Python, graphical displays with Matplotlib, PyQt, and OpenGL, and support for many model formats. These include SBML, NeuroML, GENESIS kkit and cell.p formats, HDF5 and NSDF for data writing.


# VERSION

## Chamcham 3.1.2

MOOSE 3.1 (current version 3.1.2) is an evolutionary update. It has a new Python-based SBML reader
, improved handling of reaction-diffusion systems, and substantial refinements to the 
rdesigneur interface for setting up multiscale models. 
Several additional rdesigneur tutorials have been implemented. 

# LICENSE

MOOSE is released under the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

# Building and installing

See the file `INSTALL.md`.

# AUTHORS

- Upinder S. Bhalla     -   Primary Architect, Chemical kinetic solvers
- Niraj Dudani          -   Neuronal solver
- Subhasis Ray          -   PyMOOSE Design and Documentation, Python Plugin Interface, NSDF Format
- G.V.HarshaRani        -   Web page design, SBML support, Kinetikit Plugin Development
- Aditya Gilra          -   NeuroML reader development, integrate-and-fire neurons/networks, STDP
- Aviral Goel           -   Moogli/Neurokit Development
- Dilawar Singh         -   Packaging

# Examples, tutorials and Demos: 

Look in the [moose-examples repository](https://github.com/BhallaLab/moose-examples) for sample code. 

- [tutorials](https://github.com/BhallaLab/moose-examples/tree/master/tutorials): Standalone scripts meant for teaching. Students are expected
  to modify the scripts to learn the principles of the models.
- [squid](https://github.com/BhallaLab/moose-examples/tree/master/squid): The Hodkin-Huxley squid model, fully graphical interface.
- [Genesis_files](https://github.com/BhallaLab/moose-examples/tree/master/genesis): A number of kinetics models used in MOOSE demos.
- [neuroml](https://github.com/BhallaLab/moose-examples/tree/master/neuroml): A number of NeuroML models used in MOOSE demos
- [traub_2005](https://github.com/BhallaLab/moose-examples/tree/master/traub_2005): Example scripts for each of the individual cell models from
  the Traub 2005 thalamocortical model.
- [snippets](https://github.com/BhallaLab/moose-examples/tree/master/snippets): Code snippets that can be used as building blocks and to
  illustrate how to use certain kinds of objects in MOOSE. These snippets are
  all meant to run as individual files.


# Supported file formats.

MOOSE comes with a NeuroML reader. Demos/neuroml has some python scripts showing
how to load NeuroML models.

MOOSE is backward compatible with GENESIS kinetikit.  Demos/Genesis_files has
some examples. You can load a kinetikit model with the loadModel function:

    moose.loadModel(kkit_file_path, modelname )

MOOSE is backward compatible with GENESIS <model>.p files used for neuronal
model specification. The same loadModel function can be used for this but you
need to have all the channels used in the .p file preloaded in /library:

    moose.loadModel(prototype_file_path, modelname )

MOOSE can also read .swc files from NeuroMorpho.org.

# Documentation

Complete MOOSE Documentation can be found at -  https://moose.ncbs.res.in/?q=documentation
