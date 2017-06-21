****************
Chemical Aspects
****************

.. _ckbk-chem-load-run:

Load - Run - Save models
========================

Load a Kinetic Model
--------------------

.. automodule:: loadKineticModel
   :members:

Load an SBML Model
-------------------

.. automodule:: loadSbmlmodel
   :members:

Load a CSpace Model
-------------------

.. automodule:: loadCspaceModel
   :members:

Save a model into SBML format
-----------------------------

.. automodule:: convert_Genesis2Sbml
   :members:

Save a model
------------

.. automodule:: savemodel
   :members:

.. _ckbk-chem-snippets:

Simple Examples
===============================

Set-up a kinetic solver and model
---------------------------------

with Scripting
^^^^^^^^^^^^^^

.. automodule:: scriptGssaSolver
   :members:

With something else
^^^^^^^^^^^^^^^^^^^

.. automodule:: changeFuncExpression
   :members:

Building a chemical Model from Parts
------------------------------------

.. automodule:: scriptKineticSolver
   :members:

Cross-Compartment Reaction Systems
----------------------------------

.. automodule:: crossComptSimpleReac
   :members:

.. automodule:: crossComptOscillator
   :members:

.. automodule:: crossComptNeuroMesh
   :members:

.. automodule:: crossComptSimpleReacGSSA
   :members:

Tweaking Parameters
-------------------

.. automodule:: tweakingParameters
   :members:

Models' Demonstration
---------------------

Oscillation Model
^^^^^^^^^^^^^^^^^

.. automodule:: slowFbOsc
   :members:

Bistability Models
^^^^^^^^^^^^^^^^^^

MAPK feedback loop model
~~~~~~~~~~~~~~~~~~~~~~~~

.. automodule:: mapkFB
   :members:

Simple minimal bistable model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. automodule:: scaleVolumes
   :members:

Strongly bistable Model
~~~~~~~~~~~~~~~~~~~~~~~

.. automodule:: strongBis
   :members:

Model of bidirectional synaptic plasticity
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[showing bistable chemical switch]

.. automodule:: bidirectionalPlasticity
   :members:

Reaction Diffusion Models
-------------------------
[Reaction-diffusion + transport in a tapering cylinder]

.. automodule:: cylinderDiffusion
   :members:

.. automodule:: cylinderMotor
   :members:

.. automodule:: gssaCylinderDiffusion
   :members:

Neuronal Diffusion Reaction
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: rxdFuncDiffusion
   :members:

.. automodule:: rxdReacDiffusion
   :members:

.. automodule:: rxdFuncDiffusionStoch
   :members:

A Turing Model
--------------

.. automodule:: TuringOneDim
   :members:

A Spatial Bistable Model
------------------------

.. automodule::
   :members:

Reaction Diffusion in Neurons
-----------------------------

.. automodule:: reacDiffConcGradient
   :members:

.. automodule:: reacDiffBranchingNeuron
   :members:

.. automodule:: reacDiffSpinyNeuron
   :members:

.. automodule:: diffSpinyNeuron
   :members:

Manipulating Chemical Models
----------------------------

Running with different numerical methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: switchKineticSolvers
   :members:

Changing volumes
^^^^^^^^^^^^^^^^

.. automodule:: scaleVolumes
   :members:

Feeding tabulated input to a model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: analogStimTable
   :members:

Finding steady states
^^^^^^^^^^^^^^^^^^^^^

.. automodule:: findChemSteadyState
   :members:

Making a dose-response curve
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: chemDoseResponse
   :members:

Transport in branching dendritic tree
-------------------------------------

.. automodule:: transportBranchingNeuron
   :members:

.. _ckbk-chem-rde:

RDesigneur
==========

.. _ckbk-chem-tut:

Tutorials
=========

Deterministic Simulation
------------------------

Stochastic Simulation
---------------------

Finding Steady State (CSpace)
-----------------------------

.. automodule:: cspaceSteadyState
   :members:

Building Simple Reaction Model
------------------------------

Define a kinetic model using the scripting
---------------------

.. automodule:: scriptKineticModel
   :members:

.. _ckbk-chem-adv:

Advanced Examples
=================
