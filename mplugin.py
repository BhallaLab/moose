# mplugin.py ---
#
# Filename: mplugin.py
# Description:
# Author:
# Maintainer:
# Created: Tue Oct  2 17:25:41 2012 (+0530)
# Version:
# Last-Updated: Thu Jul 18 10:51:48 2013 (+0530)
#           By: subha
#     Update #: 297
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# This is to be the base class for all MOOSE GUI plugins.
#
#

# Change log:
#
#
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:

from PyQt4 import QtGui,QtCore,Qt
import moose

class MoosePluginBase(QtCore.QObject):
    """Moose GUI plugin base class.

    A GUI plugin for MOOSE should extend MoosePlugin class. It has to
    implement the methods described here.

    We keep _toolBars, _menus and _views as protected member
    variables. Derived classes should populate/update these according
    to their needs. The idea is that if a base class is populating any
    of these, those entries are generic enough for the derived classes
    to use. So a derived class should append their specific entries to
    these lists in stead of overriding the getToolBars(), getMenus()
    and getViews() functions.

    """
    modelRootChanged = QtCore.pyqtSignal(object, name='modelRootChanged')
    dataRootChanged = QtCore.pyqtSignal(object, name='dataRootChanged')
    def __init__(self, root='/', mainwindow=None):
        """Create a plugin object whose model is the tree rooted at
        `root` and whose widgets will be displayed in `mainwindow`.

        """
        QtCore.QObject.__init__(self, mainwindow)
        self._views = []
        self._menus = []
        self._toolBars = []
        self.mainWindow = mainwindow
        self.modelRoot = root
        self.dataRoot = moose.Neutral('/data').path

    def getPreviousPlugin(self):
        """Returns the plugin object that the gui is supposed to
        switch to when going to a smaller scale."""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getNextPlugin(self):
        """Returns the plugin object that the gui is supposed to
        switch to when going to a larger scale."""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getAdjacentPlugins(self):
        """Return a list of plugins that are valid transitions from this plugin"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getViews(self):
        """Return the view widgets available from this plugin."""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getCurrentView(self):
        """Return the current view of this plugin."""
        raise NotImplementedError('method must be reimplemented in subclass')

    def setCurrentView(self, view):
        """Set current view (e.g., editor/plot/run).

        Return true if successful, otherwise return False.
        """
        if view == 'editor':
            self.currentView = self.getEditorView()
        elif view == 'plot':
            self.currentView = self.getPlotView()
        elif view == 'run':
            self.currentView = self.getRunView()
        else:
            return False
        return True

    def getMenus(self):
        return self._menus

    def getToolBars(self):
        return self._toolBars

    # def close(self):
    #     for view in self._views:
    #         view.close()

    def getEditorView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getPlotView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getRunView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def setModelRoot(self, root):
        #print("Default setModelRoot called.")
        self.modelRoot = moose.element(root).path
        self.getEditorView().getCentralWidget().setModelRoot(self.modelRoot)
        self.getPlotView().getCentralWidget().setModelRoot(self.modelRoot)
        self.modelRootChanged.emit(self.modelRoot)

    def setDataRoot(self, root):
        self.dataRoot = moose.Neutral(root).path
        self.getPlotView().setDataRoot(self.dataRoot)
        self.dataRootChanged.emit(self.dataRoot)


class ViewBase(QtCore.QObject):
    """Base class for each view: Editor, Plot, Run.

    A view is a mode in a of a plugin. Each view provides

    a list of toolbars to be displayed on top.

    a list of widgets to be docked on the sides.

    a list of menus to be added to the menubar.

    a central widget to be displayed at the centre of the main window.

    """
    def __init__(self, plugin):
        QtCore.QObject.__init__(self, plugin)
        self._menus             =   []
        self._toolPanes         =   []
        self._toolBars          =   []
        self._centralWidget     =   None
        self.plugin             =   plugin

    def getToolPanes(self):
        """Return a list of widgets to be displayed as dock widgets."""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getToolBars(self):
        """Return list of toolbars for this view."""
        return self._toolBars

    def getMenus(self):
        """Return the menus for this view."""
        return self._menus

    def getPreferences(self):
        """Return a widget for setting preferences"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getCentralWidget(self):
        """Return a widget for central widget of mainwindow."""
        raise NotImplementedError('method must be reimplemented in subclass')


class EditorBase(ViewBase):
    """Base class for editor view.

    This is the default view of a plugin. It should essentially
    display a loaded model in an appropriate visual form.

    It is ultimately intended to allow editing of the model, but that
    is not a strict requirement.

    """
    def __init__(self, plugin):
        ViewBase.__init__(self, plugin)

    def getToolPanes(self):
        return self._toolPanes

    def getLibraryPane(self):
        """TODO: display everything under library as a tree"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getSelectionPane(self):
        """TODO: provide a widget to apply selection rules"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsPane(self):
        """TODO: provide a widget to apply operations on selected
        elements."""
        raise NotImplementedError('method must be reimplemented in subclass')


class PlotBase(ViewBase):
    """Base class for plot configuration view.

    In each plugin, this should provide utility to setup the plotting
    of object fields. This is supposed to be used by intermediate
    users.
    """
    def __init__(self, *args):
        ViewBase.__init__(self, *args)

    def getSelectionPane(self):
        """TODO: provide a widget to apply selection rules"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsPane(self):
        """TODO: provide a widget to apply operations on selected
        elements."""
        raise NotImplementedError('method must be reimplemented in subclass')


class RunBase(ViewBase):
    """Base class for runtime view.

    When the simulation runs, this view displays the runtime
    visualization and controls for the simulation.
    """
    def __init__(self, plugin, *args):
        super(RunBase, self).__init__(plugin)


class EditorWidgetBase(QtGui.QWidget):
    """Base class for central widget displayed in editor view.

    The widget should display the model components in the tree rooted
    at `modelRoot` in appropriate visual representation.

    `updateModelView` function should do the actual creation and laying
    out of the visual objects.

    Signals: editObject - emitted
    with currently selected element's path as argument. Should be
    connected to whatever slot is responsible for firing the object
    editor in top level.


    """
    editObject = QtCore.pyqtSignal('PyQt_PyObject')
    def __init__(self, *args):
        QtGui.QWidget.__init__(self, *args)
        self.modelRoot      = '/'
        self._menus         = []
        self._toolBars      = []
        self._insertActions = []
        self._insertMapper  = None

    def getInsertActions(self, classlist):
        """Create actions to be used in menus/toolbars for inserting class
        instances. This function needs to be called only once. This
        also creates the signal mapping from the insert actions.

        Returns: (mapper, actions)

        mapper is a QSignalMapper and actions is a list of QAction
        objects. The triggering of any action in `actions` list causes
        the `mapper` to emit a mapped(action-name) signal. This can be
        connected to a slot in the editor's slot for inserting
        elements. For MooseTreeWidget, this is the insertElement slot
        (see default.py and mtree.py).

        """
        if len(self._insertActions) == 0:
            self._insertMapper = QtCore.QSignalMapper(self)
            for classname in classlist:
                action = QtGui.QAction(classname, self)
                self._insertMapper.setMapping(action, QtCore.QString(classname))
                self.connect(action,
                             QtCore.SIGNAL('triggered()'),
                             self._insertMapper,
                             QtCore.SLOT('map()'))
                doc = moose.element('/classes/%s' % (classname)).docs
                doc = doc.split('Description:')[-1].split('Name:')[0].strip()
                action.setToolTip(doc)
                self._insertActions.append(action)
        return self._insertMapper, self._insertActions

    def setModelRoot(self, path):
        """Set the root of the model tree to be displayed.

        This calls `updateModelView` which should update the scene to
        represent current model tree.

        This function can be a slot for connecting actions that should
        cause a change in modelRoot.

        """
        self.modelRoot = path
        self.updateModelView()

    def updateModelView(self):
        """Update view by going through the model.

        When model root is changed, this function is called. It should
        update the scene to represent the current model tree rooted at
        modelRoot.

        """
        raise NotImplementedError('must be implemented in derived class.')

    def getMenus(self):
        return self._menus

    def getToolBars(self):
        return self._toolBars

    def elementInsertedSlot(self, mobj):
        self.editObject.emit(mobj.path)

    def objectEditSlot(self, mobj):
        """Emits an `editObject(str)` signal with moose element path of currently selected tree item as
        argument"""
        self.editObject.emit(moose.element(mobj).path)

    def sizeHint(self):
        return self.size()

    def getCurrentMobj(self):
        raise NotImplementedError('should be reimplemented in subclass')



#
# mplugin.py ends here
