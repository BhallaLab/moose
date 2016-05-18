-----

# **MOOSE GUI: Graphical interface for MOOSE**

Upinder Bhalla, Harsha Rani, Aviral Goel

Aug 28 2013.

-----

## Contents
- [Introduction](#introduction)
- [Interface](#interface)
    * [Menu Bar](#menu-bar)
        * [File](#menu-file)
            * [New](#file-new)
            * [Load Model](#file-load-model)
            * [Connect BioModels](#file-connect-biomodels)
            * [Quit](#file-quit)
        * [View](#menu-view)
            * [Editor View](#editor-view)
            * [Run View](#run-view)
            * [Dock Widgets](#dock-widgets)
            * [SubWindows](#subwindows)
        * [Help](#menu-help)
            * [About MOOSE](#about-moose)
            * [Built-in Documentation](#built-in-documentation)
            * [Report a bug](#report-a-bug)
    * [Editor View](#editor-view)
        * [Model Editor](#model-editor)
        * [Property Editor](#property-editor)
    * [Run View](#run-view)
        * [Simulation Controls](#simulation-controls)
        * [Plot Widget](#plot-widget)
            * [Toolbar](#plot-widget-toolbar)
            * [Context Menu](#plot-widget-context-menu)

## Introduction

The Moose GUI currently allow you work on [chemical](Kkit12Documentation.html) models using a interface. This document describes the salient features of the GUI

## Interface

 The interface layout consists of a [menu bar](#menu-bar) and two views, [editor view](#editor-view) and [run view](#run-view).

### Menu Bar

![](../../images/MooseGuiMenuImage.png)

The menu bar appears at the top of the main window. In Ubuntu 12.04, the menu bar appears only when the mouse is in the top menu strip of the screen. It consists of the following options -

#### File

The File menu option provides the following sub options -

- [New](#file-new) - Create a new chemical signalling model.
- [Load Model](#file-load-model) - Load a chemical signalling or compartmental neuronal model from a file.
- [Paper_2015_Demos Model](#paper-2015-demos-model) - Loads and Runs chemical signalling or compartmental neuronal model from a file.
- [Recently Loaded Models](#recently-loaded-models) - List of models loaded in MOOSE. (Atleast one model should be loaded)
- [Connect BioModels](#file-connect-biomodels) - Load chemical signaling models from the BioModels database.
- [Save](#file-quit) - Saves chemical model to Genesis/SBML format.
- [Quit](#file-quit) - Quit the interface.

#### View

View menu option provides the following sub options -

- [Editor View](#editor-view) - Switch to the editor view for editing models.
- [Run View](#run-view) - Switch to run view for running models.
- [Dock Widgets](#dock-widgets) - Following dock widgets are provided -
    - [Python](#dock-widget-python) - Brings up a full fledged python interpreter integrated with MOOSE GUI. You can interact with loaded models and load new models through the PyMoose API. The entire power of python language is accessible, as well as MOOSE-specific functions and classes.
    - [Edit](#dock-widget-edit) - A property editor for viewing and editing the fields of a selected object such as a pool, enzyme, function or compartment. Editable field values can be changed by clicking on them and overwriting the new values. Please be sure to press enter once the editing is complete, in order to save your changes.
- [SubWindows](#subwindows) - This allows you to tile or tabify the run and editor views.

#### Help

- [About Moose](#about-moose) - Version and general information about MOOSE.
- [Built-in documentation](#butilt-in-documentation) - Documentation of  MOOSE GUI.
- [Report a bug](#report-a-bug) - Directs to the github bug tracker for reporting bugs.

### Editor View

The editor view provides two windows -

- [Model Editor](#model-editor) - The model editor is a workspace to edit and create models. Using click-and-drag from the icons in the menu bar, you can create model entities such as chemical pools, reactions, and so on. A click on any object brings its property editor on screen (see below). In objects that can be interconnected, a click also brings up a special arrow icon that is used to connect objects together with messages. You can move objects around within the edit window using click-and-drag. Finally, you can delete objects by selecting one or more, and then choosing the delete option from the pop-up menu.
The links below is the screenshots point to the details for the chemical signalling model editor.

![Chemical Signalling Model Editor](../../images/ChemicalSignallingEditor.png)

- [Property Editor](#property-editor) - The property editor provides a way of viewing and editing the properties of objects selected in the model editor.

![Property Editor](../../images/PropertyEditor.png)


### Run View

The Run view, as the name suggests, puts the GUI into a mode where the model can be simulated. As a first step in this, you can click-and-drag an object to the graph window in order to create a time-series plot for that object. For example, in a chemical reaction, you could drag a pool into the graph window and subsequent simulations will display a graph of the concentration of the pool as a function of time.
Within the Run View window, the time-evolution of the simulation is
displayed as an animation. For chemical kinetic models, the size of the icons for reactant pools scale to indicate concentration.
Above the Run View window, there is a special tool bar with a set of simulation controls to run the simulation.

#### Simulation Controls

![Simulation Control](../../images/SimulationControl.png)

This panel allows you to control the various aspects of the simulation.

- [Run Time](#run-time) - Determines duration for which simulation is to run. A simulation which has already run, runs further for the specified additional period.
- [Reset](#reset) - Restores simulation to its initial state; re-initializes all variables to t = 0.
- [Stop](#stop) - This button halts an ongoing simulation.
- [Current time](#current-time) - This reports the current simulation time.
- [Preferences](#preferences) - Allows you to set simulation and visualization related preferences.


#### Plot Widget


##### Toolbar

On top of plot window there is a little row of icons:

![](../../images/PlotWindowIcons.png)

These are the plot controls. If you hover the mouse over them for a few seconds, a tooltip pops up. The icons represent the following functions:

* ![](../../images/Addgraph.png) - Add a new plot window

* ![](../../images/delgraph.png) - Deletes current plot window

* ![](../../images/grid.png) - Toggle X-Y axis grid

* ![](../../images/MatPlotLibHomeIcon.png) - Returns the plot display to its default position

* ![](../../images/MatPlotLibDoUndo.png) - Undoes or re-does manipulations you                       have done to the display.

* ![](../../images/MatPlotLibPan.png) - The plots will pan around with the mouse when you hold the left button down. The plots will zoom with the mouse when you hold the right button down.

* ![](../../images/MatPlotLibZoom.png) - With the **`left mouse button`**, this will zoom in to the specified rectangle so that the plots become bigger. With the **`right mouse button`**, the entire plot display will be shrunk to fit into the specified rectangle.

* ![](../../images/MatPlotLibConfigureSubplots.png) - You don't want to mess with these .

* ![](../../images/MatPlotLibSave.png) - Save the plot.

##### Context Menu

The context menu is enabled by right clicking on the plot window. It has the following options -

* **Export to CSV** - Exports the plotted data to CSV format
* **Toggle Legend** - Toggles the plot legend
* **Remove** - Provides a list of plotted entities. The selected entity will not be plotted.
