--------------

**MOOSE GUI: Graphical interface for MOOSE**
============================================

Upinder Bhalla, Harsha Rani, Aviral Goel

Aug 28 2013.

--------------

Contents
--------

-  `Introduction <#introduction>`_
-  `Interface <#interface>`_

   -  `Menu Bar <#menu-bar>`_

      -  `File <#menu-file>`_

         -  `New <#file-new>`_
         -  `Load Model <#file-load-model>`_
         -  `Connect BioModels <#file-connect-biomodels>`_
         -  `Quit <#file-quit>`_

      -  `View <#menu-view>`_

         -  `Editor View <#editor-view>`_
         -  `Run View <#run-view>`_
         -  `Dock Widgets <#dock-widgets>`_
         -  `SubWindows <#subwindows>`_

      -  `Help <#menu-help>`_

         -  `About MOOSE <#about-moose>`_
         -  `Built-in Documentation <#built-in-documentation>`_
         -  `Report a bug <#report-a-bug>`_

   -  `Editor View <#editor-view>`_

      -  `Model Editor <#model-editor>`_
      -  `Property Editor <#property-editor>`_

   -  `Run View <#run-view>`_

      -  `Simulation Controls <#simulation-controls>`_
      -  `Plot Widget <#plot-widget>`_

         -  `Toolbar <#plot-widget-toolbar>`_
         -  `Context Menu <#plot-widget-context-menu>`_

Introduction
------------

The Moose GUI currently allow you work on
`chemical <Kkit12Documentation.html>`_ models using a interface. This
document describes the salient features of the GUI

Interface
---------

The interface layout consists of a `menu bar <#menu-bar>`_ and two
views, `editor view <#editor-view>`_ and `run view <#run-view>`_.

Menu Bar
~~~~~~~~

.. figure:: ../../images/MooseGuiMenuImage.png
   :align: center
   :alt: 

The menu bar appears at the top of the main window. In Ubuntu 12.04, the
menu bar appears only when the mouse is in the top menu strip of the
screen. It consists of the following options -

File
^^^^

The File menu option provides the following sub options -

-  `New <#file-new>`_ - Create a new chemical signalling model.
-  `Load Model <#file-load-model>`_ - Load a chemical signalling or
   compartmental neuronal model from a file.
-  `Paper\_2015\_Demos Model <#paper-2015-demos-model>`_ - Loads and
   Runs chemical signalling or compartmental neuronal model from a file.
-  `Recently Loaded Models <#recently-loaded-models>`_ - List of models
   loaded in MOOSE. (Atleast one model should be loaded)
-  `Connect BioModels <#file-connect-biomodels>`_ - Load chemical
   signaling models from the BioModels database.
-  `Save <#file-quit>`_ - Saves chemical model to Genesis/SBML format.
-  `Quit <#file-quit>`_ - Quit the interface.

View
^^^^

View menu option provides the following sub options -

-  `Editor View <#editor-view>`_ - Switch to the editor view for editing
   models.
-  `Run View <#run-view>`_ - Switch to run view for running models.
-  `Dock Widgets <#dock-widgets>`_ - Following dock widgets are provided
   -

   -  `Python <#dock-widget-python>`_ - Brings up a full fledged python
      interpreter integrated with MOOSE GUI. You can interact with
      loaded models and load new models through the PyMoose API. The
      entire power of python language is accessible, as well as
      MOOSE-specific functions and classes.
   -  `Edit <#dock-widget-edit>`_ - A property editor for viewing and
      editing the fields of a selected object such as a pool, enzyme,
      function or compartment. Editable field values can be changed by
      clicking on them and overwriting the new values. Please be sure to
      press enter once the editing is complete, in order to save your
      changes.

-  `SubWindows <#subwindows>`_ - This allows you to tile or tabify the
   run and editor views.

Help
^^^^

-  `About Moose <#about-moose>`_ - Version and general information about
   MOOSE.
-  `Built-in documentation <#butilt-in-documentation>`_ - Documentation
   of MOOSE GUI.
-  `Report a bug <#report-a-bug>`_ - Directs to the github bug tracker
   for reporting bugs.

Editor View
~~~~~~~~~~~

The editor view provides two windows -

-  `Model Editor <#model-editor>`_ - The model editor is a workspace to
   edit and create models. Using click-and-drag from the icons in the
   menu bar, you can create model entities such as chemical pools,
   reactions, and so on. A click on any object brings its property
   editor on screen (see below). In objects that can be interconnected,
   a click also brings up a special arrow icon that is used to connect
   objects together with messages. You can move objects around within
   the edit window using click-and-drag. Finally, you can delete objects
   by selecting one or more, and then choosing the delete option from
   the pop-up menu. The links below is the screenshots point to the
   details for the chemical signalling model editor.

.. figure:: ../../images/ChemicalSignallingEditor.png
   :align: center
   :alt: Chemical Signalling Model Editor

   Chemical Signalling Model Editor

-  `Property Editor <#property-editor>`_ - The property editor provides
   a way of viewing and editing the properties of objects selected in
   the model editor.

.. figure:: ../../images/PropertyEditor.png
   :align: center
   :alt: Property Editor

   Property Editor
Run View
~~~~~~~~

The Run view, as the name suggests, puts the GUI into a mode where the
model can be simulated. As a first step in this, you can click-and-drag
an object to the graph window in order to create a time-series plot for
that object. For example, in a chemical reaction, you could drag a pool
into the graph window and subsequent simulations will display a graph of
the concentration of the pool as a function of time. Within the Run View
window, the time-evolution of the simulation is displayed as an
animation. For chemical kinetic models, the size of the icons for
reactant pools scale to indicate concentration. Above the Run View
window, there is a special tool bar with a set of simulation controls to
run the simulation.

Simulation Controls
^^^^^^^^^^^^^^^^^^^

.. figure:: ../../images/SimulationControl.png
   :align: center
   :alt: Simulation Control

   Simulation Control
This panel allows you to control the various aspects of the simulation.

-  `Run Time <#run-time>`_ - Determines duration for which simulation is
   to run. A simulation which has already run, runs further for the
   specified additional period.
-  `Reset <#reset>`_ - Restores simulation to its initial state;
   re-initializes all variables to t = 0.
-  `Stop <#stop>`_ - This button halts an ongoing simulation.
-  `Current time <#current-time>`_ - This reports the current simulation
   time.
-  `Preferences <#preferences>`_ - Allows you to set simulation and
   visualization related preferences.

Plot Widget
^^^^^^^^^^^

Toolbar
'''''''

On top of plot window there is a little row of icons:

.. figure:: ../../images/PlotWindowIcons.png
   :align: center
   :alt: 

These are the plot controls. If you hover the mouse over them for a few
seconds, a tooltip pops up. The icons represent the following functions:

-  |image0| - Add a new plot window

-  |image1| - Deletes current plot window

-  |image2| - Toggle X-Y axis grid

-  |image3| - Returns the plot display to its default position

-  |image4| - Undoes or re-does manipulations you have done to the
   display.

-  |image5| - The plots will pan around with the mouse when you hold the
   left button down. The plots will zoom with the mouse when you hold
   the right button down.

-  |image6| - With the **``left mouse button``**, this will zoom in to
   the specified rectangle so that the plots become bigger. With the
   **``right mouse button``**, the entire plot display will be shrunk to
   fit into the specified rectangle.

-  |image7| - You don't want to mess with these .

-  |image8| - Save the plot.

Context Menu
''''''''''''

The context menu is enabled by right clicking on the plot window. It has
the following options -

-  **Export to CSV** - Exports the plotted data to CSV format
-  **Toggle Legend** - Toggles the plot legend
-  **Remove** - Provides a list of plotted entities. The selected entity
   will not be plotted.

.. |image0| image:: ../../images/Addgraph.png
.. |image1| image:: ../../images/delgraph.png
.. |image2| image:: ../../images/grid.png
.. |image3| image:: ../../images/MatPlotLibHomeIcon.png
.. |image4| image:: ../../images/MatPlotLibDoUndo.png
.. |image5| image:: ../../images/MatPlotLibPan.png
.. |image6| image:: ../../images/MatPlotLibZoom.png
.. |image7| image:: ../../images/MatPlotLibConfigureSubplots.png
.. |image8| image:: ../../images/MatPlotLibSave.png
