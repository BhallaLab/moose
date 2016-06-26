# mgui.py ---
#
# Filename: mgui.py
# Description:
# Author:
# Maintainer:
# Created: Mon Nov 12 09:38:09 2012 (+0530)
# Version:
# Last-Updated: Fri Oct 30 11:54:33 2015 (+0530)
#           By: Harsha
#     Update #: 1338
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# The gui driver
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
import imp
import inspect
import code
import traceback
import sys
sys.path.append('../python')
#sys.path.append('utils')
import os
from collections import defaultdict
import posixpath # We use this to create MOOSE paths
from PyQt4 import QtGui, QtCore, Qt
import config
import mplugin
import moose
import mexception
from moose import utils
from mload import loadFile
from loaderdialog import LoaderDialog
from shell import get_shell_class
from objectedit import ObjectEditDockWidget
from newmodeldialog import DialogWidget
import re
from biomodelsclient import BioModelsClientWidget
from PyQt4 import Qt, QtCore, QtGui
from PyQt4.QtGui import *
from MdiArea import MdiArea
import os
from setsolver import *
from defines import *
from collections import OrderedDict

__author__ = 'Subhasis Ray , HarshaRani, Aviral Goel, NCBS'

# This maps model subtypes to corresponding plugin names. Should be
# moved to a separate property file perhaps
subtype_plugin_map = {  'genesis/kkit': 'kkit'
                     ,  'cspace/': 'kkit'
                     ,  'xml/sbml': 'kkit'
                     ,  'xml/neuroml': 'NeuroKit'
                     }

#APPLICATION_ICON_PATH = os.path.join( os.path.dirname(os.path.realpath(__file__))
#                                    , "icons/moose_icon.png"
#                                    )


def busyCursor():
    app = QtGui.qApp
    app.setOverrideCursor(QtGui.QCursor(Qt.Qt.BusyCursor)) #shows a hourglass - or a busy/working arrow

def freeCursor():
    app = QtGui.qApp
    app.restoreOverrideCursor()



class MWindow(QtGui.QMainWindow):
    """The main window for MOOSE GUI.

    This is the driver class that uses the mplugin API. mplugin based
    classes will provide the toolbar, plugin specific menu items and a
    set of panes to be displayed on the docks.

    1. Setting a plugin

       When a plugin is set as the current plugin, the view and the
       menus are updated.

    1.a) Updating menus:

    the plugin can provide its own list of menus by implementing the
    function getMenus().

    the view of the plugin can also provide its own list of
    menus by implementing the function getMenus().

    the currentView provides a set of toolbars that are added to the
    main window.

    1.b) Updating views

    central widget is set to the currentView (a ViewBase instance) of
    the plugin.

    the currentView provides a set of panes that are inserted in the
    right dock area one by one.

    """
    def __init__(self, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.setWindowTitle('MOOSE')
        self.pluginNames = None
        self.plugin = None
        self.fileMenu = None
        self.editMenu = None
        self.helpMenu = None
        self.helpActions = None
        self.viewActions = None
        self.editActions = None
        self.connectMenu = None

        self.toolBars       = []
        self._loadedPlugins = {}
        self._plugins       = {}
        self._loadedModels  = []
        self.setUnifiedTitleAndToolBarOnMac(True)
        self.setDockOptions(self.AnimatedDocks and self.AllowNestedDocks and self.AllowTabbedDocks)
        self.mdiArea = MdiArea()

        self.quitAction = QtGui.QAction('&Quit', self)
        self.connect(self.quitAction, QtCore.SIGNAL('triggered()'), self.quit)
        self.quitAction.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+Q", None, QtGui.QApplication.UnicodeUTF8))
        self.getMyDockWidgets()
        self.setCentralWidget(self.mdiArea)
        #self.setWindowIcon(QIcon(APPLICATION_ICON_PATH))
        
        # pixmap = QPixmap("icons/moose_icon.png")

        # pixmap = pixmap.scaled(self.mdiArea.size())
        # self.mdiArea.setStyleSheet("QMdiArea { background-image: url(icons/moose_icon_large.png); }")
        # palette = QPalette()
        # palette.setBrush(QPalette.Background, QBrush(pixmap))
        # self.setPalette(palette)


        # self.mdiArea.setStyleSheet("border-image: url(icons/moose_icon_large.png)")
        # self.mdiArea.setBackground(QBrush(pixmap))

        self.mdiArea.setViewMode(QtGui.QMdiArea.TabbedView)
        self.mdiArea.subWindowActivated.connect(self.switchSubwindowSlot)
        self.setPlugin('default', '/')
        self.plugin.getEditorView().getCentralWidget().parent().close()
        self.popup = None
        self.createPopup()

    def createPopup(self):
        self.popup = dialog = QDialog(self)
        #dialog.setWindowFlags(Qt.Qt.Dialog | Qt.Qt.FramelessWindowHint)
        dialog.setWindowFlags(Qt.Qt.Dialog | Qt.Qt.CustomizeWindowHint)
        #dialog.setStyleSheet("border:1px solid rgb(0, 0, 0); ")
        qapp = QApplication.desktop().screenGeometry();
        dialog.setGeometry((qapp.bottomLeft().x()+100),(qapp.bottomLeft().y()-250),100,100)
        #dialog.move(qapp.bottomLeft().x()+10,qapp.bottomLeft().y()-10)
        layout = QGridLayout()
        self.setStyleSheet("QPushButton{border-radius: 5px; border-color: rgb(0,0,0); border-width: 2px; border-style: outset; padding-top: 2px; padding-bottom: 5px; padding-left: 5px; padding-right: 5px}")
        #self.setStyleSheet("QToolTip{border-radius: 5px; border-width: 2px; border-style: outset; padding-top: 2px; padding-bottom: 5px; padding-left: 5px; padding-right: 5px; color: black}")
        createKineticModelButton = QPushButton("Create Kinetic Model")
        loadKineticModelButton   = QPushButton("Load Model")
        loadNeuronalModelButton  = QPushButton("Load Neuronal Model")
        layout.setContentsMargins(QtCore.QMargins(20,20,20,20))

        self.menuitems = OrderedDict([("Fig2C" ,            "../moose-examples/paper-2015/Fig2_elecModels/Fig2C.py"),
                                      ("Fig2D (35s)",       "../moose-examples/paper-2015/Fig2_elecModels/Fig2D.py"),
                                      ("Fig2E" ,            "../moose-examples/paper-2015/Fig2_elecModels/Fig2E.py"),
                                      ("Fig3B_Gssa",        "../moose-examples/paper-2015/Fig3_chemModels/Fig3ABC.g"),
                                      ("Fig3C_Gsl",         "../moose-examples/paper-2015/Fig3_chemModels/Fig3ABC.g"),
                                      ("Fig3D",             "../moose-examples/paper-2015/Fig3_chemModels/Fig3D.py"),
                                      ("Fig4B",             "../moose-examples/paper-2015/Fig4_ReacDiff/Fig4B.py"  ),
                                      ("Fig4K",             "../moose-examples/paper-2015/Fig4_ReacDiff/rxdSpineSize.py"),
                                      ("Fig5A (20s)",       "../moose-examples/paper-2015/Fig5_CellMultiscale/Fig5A.py"),
                                      ("Fig5BCD (240s)" ,   "../moose-examples/paper-2015/Fig5_CellMultiscale/Fig5BCD.py"),
                                      ("Fig6A (60s)",       "../moose-examples/paper-2015/Fig6_NetMultiscale/Fig6A.py" ),
                                      ("Reduced6 (200s)",   "../moose-examples/paper-2015/Fig6_NetMultiscale/ReducedModel.py"),
                                      ("Squid" ,            "../moose-examples/squid/squid_demo.py")
                                     ])
        layout.setContentsMargins(QtCore.QMargins(20,20,20,20))
        layout1 = QHBoxLayout()
        layout1.addWidget(createKineticModelButton)
        layout1.addWidget(loadKineticModelButton)
        layout2 = QHBoxLayout()
        layout3 = QHBoxLayout()
        layout4 = QHBoxLayout()
        layout5 = QHBoxLayout()
        layout6 = QHBoxLayout()
        layout7 = QHBoxLayout()
        listofButtons = {}
        for i in range(0,len(self.menuitems)):
            k = self.menuitems.popitem(0)
            t = k[0]
            button = QPushButton(k[0])
            if k[0] == "Fig2E":
                button.setToolTip("<span style=\"color:black;\">Illustrates loading a model from an SWC file, inserting  channels, and running it</span>")
            elif k[0] == "Fig2D (35s)":
                button.setToolTip("<span style=\"color:black;\">Illustrates loading a model from an SWC file, inserting  spines, and running it</span>")
            elif k[0] == "Fig2C":
                button.setToolTip("<span style=\"color:black;\">Illustrates building a panel of multiscale models to test neuronal plasticity in different contexts</span>")    
            elif k[0] == "Fig3B_Gssa":
                button.setToolTip("<span style=\"color:black;\">Loades Repressilator model into Gui with Gssa solver and runs the model</span>")
            elif k[0] == "Fig3C_Gsl":
                button.setToolTip("<span style=\"color:black;\">Loades Repressilator model into Gui with Gsl solver and runs the model</span>")
            elif k[0] == "Fig3D":
                button.setToolTip("<span style=\"color:black;\">This example implements a reaction-diffusion like system which is bistable and propagates losslessly</span>")
            elif k[0] == "Fig4B":
                button.setToolTip("<span style=\"color:black;\">This program builds a multiscale model with a few spines inserted into a simplified cellular morphology. Each spine has a signaling model in it too. The program doesn't run the model, it just displays it in 3D</span>")
            elif k[0] == "Fig4K":
                button.setToolTip("<span style=\"color:black;\">Builds a cell with spines and a propagating reaction wave</span>")
            elif k[0] == "Fig5A (20s)":
                button.setToolTip("<span style=\"color:black;\">Illustrates building a panel of multiscale models to test neuronal plasticity in different contexts</span>")
            elif k[0] == "Fig5BCD (240s)":
                button.setToolTip("<span style=\"color:black;\">Illustrates building a panel of multiscale models to test neuronal plasticity in different contexts</span>")
            elif k[0] == "Fig6A (60s)":
                button.setToolTip("<span style=\"color:black;\">This LIF network with Ca plasticity is based on: Memory Maintenance in Synapses with Calcium-Based Plasticity in the Presence of Background Activity PLOS Computational Biology, 2014</span>")
            elif k[0] == "Reduced6 (200s)":
                button.setToolTip("<span style=\"color:black;\">This is the Reduced version of LIF network with Ca plasticity model based on: Memory Maintenance in Synapses with Calcium-Based Plasticity in the Presence of Background Activity PLOS Computational Biology, 2014</span>")
            elif k[0] == "Squid":
                button.setToolTip("<span style=\"color:black;\">squid Demo</span>")
            if k[0] in ["Fig2E","Fig2D (35s)","Fig2C"]:
                layout2.addWidget(button)
            elif k[0] in ["Fig3B_Gssa","Fig3C_Gsl","Fig3D"]:
                layout3.addWidget(button)
            elif k[0] in ["Fig4B","Fig4K"]:
                layout4.addWidget(button)
            elif k[0] in ["Fig5A (20s)","Fig5BCD (240s)"]:
                layout5.addWidget(button)
            elif k[0] in ["Fig6A (60s)","Reduced6 (200s)"]:
                layout6.addWidget(button)
            elif k[0] in ["Squid"]:
                layout7.addWidget(button)

            if k[0] == "Fig3C_Gsl":
                button.clicked.connect(lambda x, script = k[1]: self.run_genesis_script(script,"gsl"))
            elif k[0] == "Fig3B_Gssa":
                button.clicked.connect(lambda x, script = k[1]: self.run_genesis_script(script,"gssa"))
            else:
                button.clicked.connect(lambda x, script = k[1]: self.run_python_script(script))        

        layout.addLayout(layout1,0,0)
        layout.addLayout(layout2,1,0)
        layout.addLayout(layout3,2,0)
        layout.addLayout(layout4,3,0)
        layout.addLayout(layout5,4,0)
        layout.addLayout(layout6,5,0)
        layout.addLayout(layout7,6,0)
        dialog.setStyleSheet("border:1px solid rgb(0, 0, 0); ")
        dialog.setLayout(layout)

        createKineticModelButton.clicked.connect(self.newModelDialogSlot)
        loadKineticModelButton.clicked.connect(self.loadModelDialogSlot)
        loadNeuronalModelButton.clicked.connect(self.loadModelDialogSlot)
        
        dialog.show()
        freeCursor()
        return dialog

    def run_genesis_script(self,filepath,solver):
        self.popup.hide()
        abspath = os.path.abspath(filepath)
        directory, modulename = os.path.split(abspath)
        modelName = os.path.splitext(modulename)[0]
        ret = loadFile(str(abspath),'%s' %(modelName),solver,merge=False)
        self.setPlugin("kkit", ret["model"].path)
        self.setCurrentView("run")        
        widget = self.plugin.view.getSchedulingDockWidget().widget()
        widget.runSimulation()

    def run_python_script(self, filepath):
        busyCursor()
        import subprocess, shlex
        t = os.path.abspath(filepath)
        directory, filename = os.path.split(t)
        p = subprocess.Popen(["python", filename], cwd=directory)
        p.wait()
        freeCursor()

    def quit(self):
        QtGui.qApp.closeAllWindows()

    def handleException(self, t, v, s):
        """This handler will show warning messages for error exceptions. Show
        info at status bar for non-error exceptions. It will replace
        sys.excepthook and has the same signature (except being bound
        to this object).

        t : exception type

        v : exception value

        s: traceback object.

        """
        traceback.print_exception(t, v, s)
        title = ''.join(traceback.format_exception_only(t, v))
        trace = ''.join(traceback.format_exception(t, v, s))
        if isinstance(v, mexception.MooseInfo):
            self.statusBar().showMessage(title, 5000)
        elif isinstance(v, mexception.MooseWarning):
            QtGui.QMessageBox.warning(self, title, '\n'.join((title, trace)))
        else:
            QtGui.QMessageBox.critical(self, title, '\n'.join((title, trace)))

    def getPluginNames(self):
        """Return pluginNames attribute or create it by retrieving
        available plugin names from plugin/list.txt file.

        """
        if self.pluginNames is None:
            with open(os.path.join(config.MOOSE_GUI_DIR,
                                   'plugins',
                                   'list.txt')) as lfile:
                self.pluginNames = [line.strip() for line in lfile]
                self.pluginNames = [name for name in self.pluginNames if name]

        return self.pluginNames

    def loadPluginModule(self, name, re=False):
        """Load a plugin module by name.

        First check if the plugin is already loaded. If so return the
        existing one. Otherwise, search load the plugin as a python
        module from {MOOSE_GUI_DIRECTORY}/plugins directory.

        If re is True, the plugin is reloaded.
        """
        if (not re) and name in sys.modules:
            return sys.modules[name]
        fp, pathname, description = imp.find_module(name, [config.MOOSE_PLUGIN_DIR])
        try:
            module = imp.load_module(name, fp, pathname, description)
        finally:
            if fp:
                fp.close()
        return module

    def getMyDockWidgets(self):
        """Return a list of dockwidgets that belong to the top
        level. This is needed to keep them separate from those
        provided by the plugins.

        Currently we only have shell for this."""
        if not hasattr(self, 'dockWidgets') or self.dockWidgets is None:
            self.dockWidgets = {}
            dockWidget = QtGui.QDockWidget('Python')
            dockWidget.setWidget(self.getShellWidget())
            self.dockWidgets[dockWidget] = True
            self.addDockWidget(Qt.Qt.BottomDockWidgetArea, dockWidget)
            dockWidget.setVisible(False)
            dockWidget = ObjectEditDockWidget('/')
            self.dockWidgets[dockWidget] = True
            self.objectEditDockWidget = dockWidget
            self.addDockWidget(Qt.Qt.RightDockWidgetArea, dockWidget)
            dockWidget.setVisible(False)
        return self.dockWidgets.keys()

    def getShellWidget(self):
        """Create an instance of shell widget. This can be either a
        QSciQScintialla widget or a PyCute widget (extends QTextArea)
        if the first is not available"""
        if not hasattr(self, 'shellWidget') or self.shellWidget is None:
            self.shellWidget = get_shell_class()( code.InteractiveInterpreter()
                                                , message='MOOSE version %s' % (moose._moose.__version__)
                                                )
            self.shellWidget.interpreter.runsource('from moose import *')
            self.shellWidget.setVisible(False)
        return self.shellWidget

    def loadPluginClass(self, name, re=False):
        """Load the plugin class from a plugin module.

        A plugin module should have only one subclass of
        MoosePluginBase. Otherwise the first such class found will be
        loaded.
        """
        try:
            return self._loadedPlugins[name]
        except KeyError:
            pluginModule = self.loadPluginModule(name, re=re)
            for classname, classobj in inspect.getmembers(pluginModule, inspect.isclass):
                if issubclass(classobj, mplugin.MoosePluginBase):
                    self._loadedPlugins[name] = classobj
                    # classobj.getEditorView().getCentralWidget().editObject.connect(self.objectEditSlot)
                    return self._loadedPlugins[name]
        raise Exception('No plugin with name: %s' % (name))

    def setPlugin(self, name, root='/'):
        """Set the current plugin to use.

        This -

        1. sets the `plugin` attribute.

        2. updates menus by clearing and reinstating menus including
        anything provided by the plugin.

        3. sets the current view  to the plugins editor view.

        """
        busyCursor()
        for model in self._loadedModels:
            if model[0] != root:
                self.disableModel(model[0])
        for i in range(0, len(self._loadedModels)):
            if self._loadedModels[i][0]== root:
                c = moose.Clock('/clock')
                compts = moose.wildcardFind(root+'/##[ISA=ChemCompt]')
                for simdt in CHEMICAL_SIMULATION_DT_CLOCKS:
                    c.tickDt[simdt] = self._loadedModels[i][3]
                for plotdt in CHEMICAL_PLOT_UPDATE_INTERVAL_CLOCKS:
                    c.tickDt[plotdt] = self._loadedModels[i][4]
        
                if compts:
                    #setCompartmentSolver(self._loadedModels[i][0],"gsl")
                    addSolver(self._loadedModels[i][0],"gsl")
                else:
                    c.tickDt[7] = self._loadedModels[i][3]
                    c.tickDt[8] = self._loadedModels[i][4]
                    neurons = moose.wildcardFind(root + "/model/cells/##[ISA=Neuron]")
                    for neuron in neurons:
                        solver = moose.element(neuron.path + "/hsolve")
                        solver.tick = 7
                    for x in moose.wildcardFind( root+'/data/graph#/#' ):
                        x.tick = 8
                break
        
        self.plugin = self.loadPluginClass(str(name))(str(root), self)
        moose.reinit()

        self.updateMenus()
        for action in self.pluginsMenu.actions():
            if str(action.text()) == str(name):
                action.setChecked(True)
            elif action.isChecked():
                action.setChecked(False)
        for subwin in self.mdiArea.subWindowList():
            subwin.close()

        if name != "default" :
            self.setCurrentView('editor')
            self.setCurrentView('run')

        if name == 'kkit':
            self.objectEditDockWidget.objectNameChanged.connect(self.plugin.getEditorView().getCentralWidget().updateItemSlot)
            self.objectEditDockWidget.colorChanged.connect(self.plugin.getEditorView().getCentralWidget().updateColorSlot)

        self.setCurrentView('editor')
        freeCursor()
        return self.plugin

    def updateExistingMenu(self, menu):
        """Check if a menu with same title
        already exists. If so, update the same and return
        True. Otherwise return False.
        """
        if not isinstance(menu, QtGui.QMenu):
            return False
        for action in self.menuBar().actions():
            if menu.title() == action.text():
                # if not action.menu().isEmpty():
                #     action.menu().addSeparator()
                action.menu().addActions(menu.actions())
                return True
        return False

    def updateMenus(self):
        """Clear the menubar and reinstate the basic menus.  Go
        through the menus provided by current plugin and add those to
        menubar.

        If a menu provided by a plugin has same name as one of the
        core menus, the menu items provided by the plugin are appended
        to the existing menu after a separator.

        """
        self.menuBar().clear()
        self.getPluginsMenu()
        menus = [self.getFileMenu(),
                 self.getEditMenu(),
                 self.getViewMenu(),
                 #self.getRunMenu(),
                 #self.getConnectMenu(),
                 self.getHelpMenu()]
        for menu in menus:
            self.menuBar().addMenu(menu)

        for menu in self.plugin.getMenus():
            if not self.updateExistingMenu(menu):
                if not self.menuBar().isEmpty():
                    action.menu().addSeparator()
                self.menuBar().addMenu(menu)
        menus[0].addSeparator()
        menus[0].addAction(self.quitAction)

    def updateToolbars(self):
        for toolbar in self.toolBars:
            self.removeToolBar(toolbar)
        self.toolBars = []
        self.toolBars.extend(self.getMyToolBars())
        self.toolBars.extend(self.plugin.getToolBars())
        self.toolBars.extend(self.plugin.getCurrentView().getToolBars())
        if len(self.toolBars):
        	for toolbar in self.toolBars:
        		self.addToolBar(toolbar)
        		toolbar.setVisible(True)

    def switchSubwindowSlot(self, window):
        """Change view based on what subwindow `window` is activated."""
        if not window:
            return
        view = str(window.windowTitle()).partition(':')[0]
        self.setCurrentView(view)

    def setCurrentView(self, view):
        """Set current view to a particular one: options are 'editor',
        'plot', 'run'. A plugin can provide more views if necessary.
        """
        self.plugin.setCurrentView(view)
        if view =='run':
            #Harsha: This will clear out object editor's objectpath and make it invisible
            self.objectEditSlot('/',False)

        targetView = None
        newSubWindow = True
        widget = self.plugin.getCurrentView().getCentralWidget()
        current = self.mdiArea.activeSubWindow()
        subwin = None
        for subwin in self.mdiArea.subWindowList():
            if subwin.widget() == widget:
                newSubWindow = False
                break
        if newSubWindow:
            subwin = self.mdiArea.addSubWindow(widget)
            title = widget.modelRoot+'/model'
            #subwin.setWindowTitle('%s: %s' % (view, widget.modelRoot))
            subwin.setWindowTitle('%s: %s' % (view, title))
            subwin.setSizePolicy(QtGui.QSizePolicy.Minimum |
                                 QtGui.QSizePolicy.Expanding,
                                 QtGui.QSizePolicy.Minimum |
                                 QtGui.QSizePolicy.Expanding)
            subwin.resize(600, 400)
        # Make dockwidgets from other views invisible and make those
        # from current view visible or add them if not already part of
        # main window.
        dockWidgets = set([dockWidget for dockWidget in self.findChildren(QtGui.QDockWidget)])
        for dockWidget in dockWidgets:
            if dockWidget not in self.dockWidgets:
                dockWidget.setVisible(False)
        for dockWidget in self.plugin.getCurrentView().getToolPanes():
            if dockWidget not in dockWidgets:
                if view == "run":
                    if dockWidget.windowTitle() == "Scheduling":
                        self.addDockWidget(Qt.Qt.TopDockWidgetArea, dockWidget)
                else:
                    self.addDockWidget(Qt.Qt.RightDockWidgetArea, dockWidget)
            dockWidget.setVisible(True)
        subwin.setVisible(True)
        self.mdiArea.setActiveSubWindow(subwin)
        self.updateMenus()
        for menu in self.plugin.getCurrentView().getMenus():
            if not self.updateExistingMenu(menu):
                self.menuBar().addMenu(menu)
        self.updateToolbars()
        return subwin

    def getMyToolBars(self):
        self._toolBars = []
        '''
        #Harsha: removing the toolbars (plot,run,edit) from the Gui
        if not hasattr(self, 'viewToolBar'):
            self.viewToolBar = QtGui.QToolBar('View')
            #Harsha:removing plotView from the ToolBar
            for t in self.getViewActions():
                if t.text() != "&Plot view":
                    self.viewToolBar.addAction(t)
            #self.viewToolBar.addActions(self.getViewActions())
        #return [self.viewToolBar]
        '''
        return self._toolBars

    def getFileMenu(self):
        if self.fileMenu is None:
            self.fileMenu = QtGui.QMenu('&File')
        else:
            self.fileMenu.clear()

        if not hasattr(self, 'newModelAction'):
            self.newModelAction = QtGui.QAction('New', self)
            self.newModelAction.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+N", None, QtGui.QApplication.UnicodeUTF8))
            self.connect(self.newModelAction, QtCore.SIGNAL('triggered()'), self.newModelDialogSlot)
        self.fileMenu.addAction(self.newModelAction)
        if not hasattr(self, 'loadModelAction'):
            self.loadModelAction = QtGui.QAction('L&oad model', self)
            self.loadModelAction.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+O", None, QtGui.QApplication.UnicodeUTF8))
            self.connect(self.loadModelAction, QtCore.SIGNAL('triggered()'), self.loadModelDialogSlot)
        self.fileMenu.addAction(self.loadModelAction)

        if not hasattr(self, 'Paper_2015'):
            self.menuitems = OrderedDict([
                                        ("Fig2C (6s)" ,     "../moose-examples/paper-2015/Fig2_elecModels/Fig2C.py"),
                                        ("Fig2D (35s)",     "../moose-examples/paper-2015/Fig2_elecModels/Fig2D.py"),
                                        ("Fig2E (5s)" ,     "../moose-examples/paper-2015/Fig2_elecModels/Fig2E.py"),
                                        ("Fig3B_Gssa (2s)", "../moose-examples/paper-2015/Fig3_chemModels/Fig3ABC.g"),
                                        ("Fig3C_Gsl (2s)",  "../moose-examples/paper-2015/Fig3_chemModels/Fig3ABC.g"),
                                        ("Fig3D (1s)",      "../moose-examples/paper-2015/Fig3_chemModels/Fig3D.py"),
                                        ("Fig4B (10s)",     "../moose-examples/paper-2015/Fig4_ReacDiff/Fig4B.py"  ),
                                        ("Fig4K",           "../moose-examples/paper-2015/Fig4_ReacDiff/rxdSpineSize.py"),
                                        ("Fig5A (20s)",     "../moose-examples/paper-2015/Fig5_CellMultiscale/Fig5A.py"),
                                        ("Fig5BCD (240s)" , "../moose-examples/paper-2015/Fig5_CellMultiscale/Fig5BCD.py"),
                                        ("Fig6A (60s)",     "../moose-examples/paper-2015/Fig6_NetMultiscale/Fig6A.py" ),
                                        ("Reduced6 (200s)", "../moose-examples/paper-2015/Fig6_NetMultiscale/ReducedModel.py"),
                                        ("Squid" ,          "../moose-examples/squid/squid_demo.py")
                                     ])
            self.subMenu = QtGui.QMenu('Paper_2015_Demos')
            for i in range(0,len(self.menuitems)):
                k = self.menuitems.popitem(0)
                t = "self."+k[0]+"Action"
                t = QtGui.QAction(k[0],self)
                self.subMenu.addAction(t)
                if k[0] == "Fig3C_Gsl (2s)":
                    t.connect(t,QtCore.SIGNAL('triggered()'),lambda script = k[1]: self.run_genesis_script(script,"gsl"))
                elif k[0] == "Fig3B_Gssa (2s)":
                    t.connect(t,QtCore.SIGNAL('triggered()'),lambda script = k[1]: self.run_genesis_script(script,"gssa"))
                else:
                    t.connect(t,QtCore.SIGNAL('triggered()'),lambda : self.run_python_script(k[1]))
                self.subMenu.addAction(t)    
            self.fileMenu.addMenu(self.subMenu)

        if not hasattr(self,'loadedModels'):
            self.loadedModelAction = QtGui.QAction('Recently Loaded Models',self)
            self.loadedModelAction.setCheckable(False)
            #self.fileMenu.addAction(QtGui.QAction(self.loadedModelAction,checkable=True))
            if bool(self._loadedModels):
                self.fileMenu.addSeparator()
                self.fileMenu.addAction(self.loadedModelAction)
                self.loadedModelAction.setEnabled(False)
                for (model, modeltype, action,simdt,plotdt) in reversed(self._loadedModels):
                    self.fileMenu.addAction(action)
                self.fileMenu.addSeparator()

        if not hasattr(self,'connectBioModelAction'):
            self.connectBioModelAction = QtGui.QAction('&Connect BioModels', self)
            self.connectBioModelAction.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+B", None, QtGui.QApplication.UnicodeUTF8))
            self.connect(self.connectBioModelAction, QtCore.SIGNAL('triggered()'), self.connectBioModel)
        self.fileMenu.addAction(self.connectBioModelAction)
        return self.fileMenu

    def getEditMenu(self):
        if self.editMenu is None:
            self.editMenu = QtGui.QMenu('&Edit')
        else:
            self.editMenu.clear()
        #self.editMenu.addActions(self.getEditActions())
        return self.editMenu

    def getPluginsMenu(self):
        """Populate plugins menu if it does not exist already."""
        if (not hasattr(self, 'pluginsMenu')) or (self.pluginsMenu is None):
            self.pluginsMenu = QtGui.QMenu('&Plugins')
            mapper = QtCore.QSignalMapper(self)
            pluginsGroup = QtGui.QActionGroup(self)
            pluginsGroup.setExclusive(True)
            for pluginName in self.getPluginNames():
                action = QtGui.QAction(pluginName, self)
                action.setObjectName(pluginName)
                action.setCheckable(True)
                mapper.setMapping(action, QtCore.QString(pluginName))
                self.connect(action, QtCore.SIGNAL('triggered()'), mapper, QtCore.SLOT('map()'))
                self.pluginsMenu.addAction(action)
                pluginsGroup.addAction(action)
            self.connect(mapper, QtCore.SIGNAL('mapped(const QString &)'), self.setPlugin)
            #self.pluginsMenu.addMenu(self.defaultPluginMenu)
            #self.pluginsMenu.addMenu(self.kkitPluginMenu)
            #self.pluginsMenu.addMenu(self.neurokitPluginMenu)
            #openRootAction = self.defaultPluginMenu.addAction("/")
            #openRootAction.triggered.connect(lambda : self.setPlugin("default", "/") )
            # if (not hasattr(self, 'loadedModelAction')) or (self.loadedModelAction is None)  :
            #     self.loadedModelAction = QtGui.QAction("kkit",self)
            #     self.loadedModelAction.addMenu('test')
            # self.pluginsMenu.addAction(self.loadedModelAction)
            # self.pluginsMenu.addMenu(self.insertkkitMenu)
            # self.insertMapperkkit = QtCore.QSignalMapper(self)
            #insertMapperkkit,actions = self.getInsertkkitActions(self.loadedModels)
            # ignored_bases = ['ZPool', 'Msg', 'Panel', 'SolverBase', 'none']
            # ignored_classes = ['ZPool','ZReac','ZMMenz','ZEnz','CplxEnzBase']
            # classlist = [ch[0].name for ch in moose.element('/classes').children
            #              if (ch[0].baseClass not in ignored_bases)
            #              and (ch[0].name not in (ignored_bases + ignored_classes))
            #              and not ch[0].name.startswith('Zombie')
            #              and not ch[0].name.endswith('Base')
            #          ]
            # insertMapper, actions = self.getInsertActions(classlist)
            # for action in actions:
            #     self.insertMenu.addAction(action)
            # self.connect(insertMapper, QtCore.SIGNAL('mapped(const QString&)'), self.tree.insertElementSlot)
            # self.editAction = QtGui.QAction('Edit', self.treeMenu)
            # self.editAction.triggered.connect(self.editCurrentObjectSlot)
            # self.tree.elementInserted.connect(self.elementInsertedSlot)
            # self.treeMenu.addAction(self.editAction)
        return self.pluginsMenu

    def getHelpMenu(self):
        if self.helpMenu is None:
            self.helpMenu = QtGui.QMenu('&Help')
        else:
            self.helpMenu.clear()
        self.helpMenu.addActions(self.getHelpActions())
        return self.helpMenu
    '''
    def getConnectMenu(self):
        if self.connectMenu is None:
            self.connectMenu = QtGui.QMenu('&Connect')
        else:
            self.connectMenu.clear()
        self.connectMenu.addActions(self.getConnectActions())
        return self.connectMenu
    '''
    def getViewMenu(self):
        if (not hasattr(self, 'viewMenu')) or (self.viewMenu is None):
            self.viewMenu = QtGui.QMenu('&View')
        else:
            self.viewMenu.clear()
        self.viewMenu.addActions(self.getViewActions())
        self.docksMenu = self.viewMenu.addMenu('&Dock widgets')
        self.docksMenu.addActions(self.getDockWidgetsToggleActions())
        self.subWindowMenu = self.viewMenu.addMenu('&Subwindows')
        self.subWindowMenu.addActions(self.getSubWindowActions())
        return self.viewMenu

    # def getSubWindowVisibilityActions(self):
    #     if not hasattr(self, 'subwindowToToggles'):
    #         self.subWindowToToggle = QtGui.QSignalMapper()
    #         self.toggleToSubWindow = QtGui.QSignalMapper()
    #     for subwindow in self.mdiArea.subWindowList():
    #         if self.subWindowToToggle.mapping(subwindow) is None:
    #             action = QtGui.QAction('Show: %s' % (subwindow.windowTitle), self)
    #             self.toggleToSubWindow.setMapping(action, subwindow)
    #             self.connect(action, QtCore.SIGNAL('triggered()'),
    #                          self.toggleToSubWindow,
    #                          QtCore.SLOT('mapped(QWidget*)'))
    #             self.subWindowToToggle.setMapping(subwindow, action)
    #             self.connect(subwindow, QtCore.SIGNAL('closed()')

    #     self.subWindowVisibilityMenu = QtGui.Q
    #     for subwin in self.mdiArea.subWindowList():

    # Removed from the menu
    # def getRunMenu(self):
    #     if (not hasattr(self, 'runMenu')) or (self.runMenu is None):
    #         self.runMenu = QtGui.QMenu('&Run')
    #     else:
    #         self.runMenu.clear()
    #     self.runMenu.addActions(self.getRunActions())
    #     return self.runMenu

    def getEditActions(self):

        # self.editActions = []
        # if (not hasattr(self, 'editActions')) or (self.editActions is None):
        #     self.setModelRootAction = QtGui.QAction('&Set model root', self)
        #     self.setModelRootAction.triggered.connect(self.showSetModelRootDialog)
        #     self.setDataRootAction = QtGui.QAction('Set &data root', self)
        #     self.setDataRootAction.triggered.connect(self.showSetDataRootDialog)
        #     self.editActions = [self.setModelRootAction, self.setDataRootAction]
        # return self.editActions
        return None

    def showSetModelRootDialog(self):
        root, ok = QtGui.QInputDialog.getText(self, 'Model Root', 'Enter the model root path:', text=moose.element(self.plugin.modelRoot).path)
        if not ok:
            return
        root = str(root) #convert from QString to python str
        self.plugin.setModelRoot(root)
        for subwin in self.mdiArea.subWindowList():
            if hasattr(subwin.widget(), 'modelRoot'):
                subwin.setWindowTitle(root)

    def showSetDataRootDialog(self):
        root, ok = QtGui.QInputDialog.getText(self, 'Data Root', 'Enter the data root path:', text=moose.element(self.plugin.dataRoot).path)
        if not ok:
            return
        root = str(root) #convert from QString to python str
        self.plugin.setDataRoot(root)
        for subwin in self.mdiArea.subWindowList():
            if hasattr(subwin.widget(), 'dataRoot'):
                subwin.setWindowTitle(root)

    def getViewActions(self):
        if (not hasattr(self, 'viewActions')) or (self.viewActions is None):
            self.editorViewAction = QtGui.QAction('&Editor view', self)
            self.editorViewAction.triggered.connect(self.openEditorView)
            #self.plotViewAction = QtGui.QAction('&Plot view', self)
            #self.plotViewAction.triggered.connect(self.openPlotView)
            self.runViewAction = QtGui.QAction('&Run view', self)
            self.runViewAction.triggered.connect(self.openRunView)
            #self.viewActions = [self.editorViewAction, self.plotViewAction, self.runViewAction]
            self.viewActions = [self.editorViewAction, self.runViewAction]
        return self.viewActions

    def setTabbedView(self):
        self.mdiArea.setViewMode(QtGui.QMdiArea.TabbedView)

    def setSubWindowView(self):
        self.mdiArea.setViewMode(QtGui.QMdiArea.SubWindowView)

    def getSubWindowActions(self):
        if not hasattr(self, 'subWindowActions') or self.subWindowActions is None:
            self.tabbedViewAction = QtGui.QAction('&Tabbed view', self)
            self.tabbedViewAction.triggered.connect(self.setTabbedView)
            self.subWindowViewAction = QtGui.QAction('&SubWindow view', self)
            self.subWindowViewAction.triggered.connect(self.setSubWindowView)
            self.tileSubWindowsAction = QtGui.QAction('Ti&le subwindows', self)
            self.tileSubWindowsAction.triggered.connect(self.mdiArea.tileSubWindows)
            self.cascadeSubWindowsAction = QtGui.QAction('&Cascade subwindows', self)
            self.cascadeSubWindowsAction.triggered.connect(self.mdiArea.cascadeSubWindows)
            self.subWindowActions = [self.tabbedViewAction,
                                     self.subWindowViewAction,
                                     self.tileSubWindowsAction,
                                     self.cascadeSubWindowsAction]
        return self.subWindowActions

    def getDockWidgetsToggleActions(self):
        """Get a list of actions for toggling visibility of dock
        widgets

        """
        return [widget.toggleViewAction() for widget in self.findChildren(QtGui.QDockWidget)]
    # Removed form the menu item
    # def getRunActions(self):
    #     if (not hasattr(self, 'runActions')) or \
    #             (self.runActions is None):
    #         self.startAction = QtGui.QAction('Start', self)
    #         self.startAction.triggered.connect(self.resetAndStartSimulation)
    #         self.pauseAction = QtGui.QAction('Pause', self)
    #         self.pauseAction.triggered.connect(self.pauseSimulation)
    #         self.continueAction = QtGui.QAction('Continue', self)
    #         self.continueAction.triggered.connect(self.continueSimulation)
    #         self.runActions = [self.startAction, self.pauseAction, self.continueAction]
    #     return self.runActions

    def getHelpActions(self):
        if (not hasattr(self, 'helpActions')) or (self.helpActions is None):
            self.actionAbout = QtGui.QAction('About MOOSE', self)
            self.connect(self.actionAbout, QtCore.SIGNAL('triggered()'), self.showAboutMoose)
            self.actionBuiltInDocumentation = QtGui.QAction('Built-in documentation', self)
            self.connect(self.actionBuiltInDocumentation, QtCore.SIGNAL('triggered()'), self.showBuiltInDocumentation)
            self.actionGuiBug = QtGui.QAction('Report gui bug', self)
            self.connect(self.actionGuiBug, QtCore.SIGNAL('triggered()'), self.reportGuiBug)
            self.actionCoreBug = QtGui.QAction('Report core bug', self)
            self.connect(self.actionCoreBug, QtCore.SIGNAL('triggered()'), self.reportCoreBug)
            self.helpActions = [self.actionAbout, self.actionBuiltInDocumentation, self.actionCoreBug,self.actionGuiBug]
        return self.helpActions
    # Removed from the main menu item replace with File menu
    # def getConnectActions(self):
    #     if(not hasattr(self,'connectActions')) or(self.connectActions is None):
    #         self.actionBioModel = QtGui.QAction('BioModels',self)
    #         self.connect(self.actionBioModel, QtCore.SIGNAL('triggered()'), self.connectBioModel)
    #         self.connectActions = [self.actionBioModel]
    #     return self.connectActions

    def connectBioModel(self):
        connecttoBioModel = BioModelsClientWidget()
        if connecttoBioModel.exec_():
            pass
        filepath = connecttoBioModel.filePath
        if filepath:
            head, fileName = os.path.split(filepath)
            modelName = os.path.splitext(fileName)[0]
            pwe = moose.getCwe()
            ret = loadFile(str(filepath), '/model/%s' % (modelName), merge=False)
            self.objectEditSlot('/',False)
            pluginLookup = '%s/%s' % (ret['modeltype'], ret['subtype'])
            try:
                pluginName = subtype_plugin_map['%s/%s' % (ret['modeltype'], ret['subtype'])]
            except KeyError:
                pluginName = 'default'
            self._loadedModels.append([ret['model'].path,pluginName])
            if len(self._loadedModels)>5:
                self._loadedModels.pop(0)

            if not moose.exists(ret['model'].path+'/info'):
                    moose.Annotator(ret['model'].path+'/info')

            modelAnno = moose.Annotator(ret['model'].path+'/info')
            if ret['subtype']:
                modelAnno.modeltype = ret['subtype']
            else:
                modelAnno.modeltype = ret['modeltype']
            modelAnno.dirpath = str(dialog.directory().absolutePath())
            self.loadedModelsAction(ret['model'].path,pluginName)
            self.setPlugin(pluginName, ret['model'].path)


    def showAboutMoose(self):
        with open(config.MOOSE_ABOUT_FILE, 'r') as aboutfile:
            QtGui.QMessageBox.about(self, 'About MOOSE', ''.join(aboutfile.readlines()))

    def showDocumentation(self, source):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(config.MOOSE_DOC_URL))
        '''
        if not hasattr(self, 'documentationViewer'):
            self.documentationViewer = QtGui.QTextBrowser()
            self.documentationViewer.setOpenLinks(True)
            self.documentationViewer.setOpenExternalLinks(True)
            #print " path ",config.settings[config.KEY_DOCS_DIR], os.path.join(config.settings[config.KEY_DOCS_DIR], 'html'), os.path.join(config.settings[config.KEY_DOCS_DIR], 'images')
            self.documentationViewer.setSearchPaths([config.settings[config.KEY_DOCS_DIR],
                                                     os.path.join(config.settings[config.KEY_DOCS_DIR], 'html'),
                                                     os.path.join(config.settings[config.KEY_DOCS_DIR], 'images')])
            self.documentationViewer.setMinimumSize(800, 480)
        self.documentationViewer.setSource(QtCore.QUrl(source))
        result = self.documentationViewer.loadResource(QtGui.QTextDocument.HtmlResource, self.documentationViewer.source())
        if not result.isValid():
            QtGui.QMessageBox.warning(self, 'Could not access documentation', 'The link %s could not be accessed' % (source))
            return
        self.documentationViewer.setWindowTitle(source)
        self.documentationViewer.reload()
        self.documentationViewer.setVisible(True)
        '''
    def reportGuiBug(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(config.MOOSE_GUI_BUG_URL))
    def reportCoreBug(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(config.MOOSE_CORE_BUG_URL))
    '''        
    def reportBug(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(config.MOOSE_REPORT_BUG_URL))
    '''
    def showBuiltInDocumentation(self):
        self.showDocumentation('moose_builtins.html')

    # openEditorView, openPlotView and openRunView are identical
    # except the view they ask from the plugin. Consider using a
    # mapper.
    def openEditorView(self):
        """Switch to the editor view of current plugin. If there is
        already a subwindow for this, make that the active
        one. Otherwise create a new one.

        """
        self.setCurrentView('editor')

    def openPlotView(self):
        self.setCurrentView('plot')

    def openRunView(self):
        self.setCurrentView('run')

    def resetAndStartSimulation(self):
        """TODO this should provide a clean scheduling through all kinds
        of simulation or default scheduling should be implemented in MOOSE
        itself. We need to define a policy for handling scheduling. It can
        be pushed to the plugin-developers who should have knowledge of
        the scheduling criteria for their domain."""
        settings = config.MooseSetting()
        try:
            simdt_kinetics = float(settings[config.KEY_KINETICS_SIMDT])
        except ValueError:
            simdt_kinetics = 0.1
        try:
            simdt_electrical = float(settings[config.KEY_ELECTRICAL_SIMDT])
        except ValueError:
            simdt_electrical = 0.25e-4
        try:
            plotdt_kinetics = float(settings[config.KEY_KINETICS_PLOTDT])
        except ValueError:
            plotdt_kinetics = 0.1
        try:
            plotdt_electrical = float(settings[config.KEY_ELECTRICAL_PLOTDT])
        except ValueError:
            plotdt_electrical = 0.25e-3
        try:
            simtime = float(settings[config.KEY_SIMTIME])
        except ValueError:
            simtime = 1.0
        moose.reinit()
        view = self.plugin.getRunView()
        moose.start(simtime)

        if view.getCentralWidget().plotAll:
            view.getCentralWidget().plotAllData()
        self.setCurrentView('run')

    def pauseSimulation(self):
        moose.stop()
    '''
    def continueSimulation(self):
        """TODO implement this somewhere else"""
        try:
            simtime = float(config.MooseSetting()[config.KEY_SIMTIME])
        except ValueError:
            simtime = 1.0
        moose.start(simtime)
    '''
    #Harsha: added visible=True so that loadModelDialogSlot and NewModelDialogSlot call this function
    #        to clear out object path
    def objectEditSlot(self, mobj, visible=True):
        """Slot for switching the current object in object editor."""
        self.objectEditDockWidget.setObject(mobj)
        self.objectEditDockWidget.setVisible(visible)

    def loadedModelsAction(self,modelPath,pluginName):
        #Harsha: added under file Menu, Recently Loaded Models
        #All the previously loaded chemical models, solver's and table's ticks are made -1
        for model in self._loadedModels:
            self.disableModel(model[0])

        action = QAction(modelPath[1:],self)
        action.triggered.connect(lambda : self.setPlugin(pluginName, modelPath))
        compt = moose.wildcardFind(modelPath + '/##[ISA=ChemCompt]')
        c = moose.Clock('/clock')
        self.simulationdt = c.tickDt[7]
        self.plotdt = c.tickDt[8]
        if compt:
            self.simulationdt = c.tickDt[11]
            self.plotdt = c.tickDt[16]
        #index = [(ind, self._loadedModels[ind].index(modelPath)) for ind in xrange(len(self.loadedModels)) if item in self._loadedModels[ind]]
        # for i,j in enumerate(self._loadedModels):
        #     if j[0] == modelPath:
        #         #del(self._loadedModels[i])
        #         pass
        #         break

        self._loadedModels.append([modelPath,pluginName,action,self.simulationdt,self.plotdt])
        if len(self._loadedModels)>5:
            self._loadedModels.pop(0)

    def disableModel(self, modelPath):
        compt = moose.wildcardFind(modelPath + '/##[ISA=ChemCompt]')
        if compt:
            if moose.exists(compt[0].path+'/ksolve'):
                ksolve = moose.Ksolve( compt[0].path+'/ksolve' )
                ksolve.tick = -1
            if moose.exists(compt[0].path+'/gsolve'):
                gsolve = moose.Gsolve( compt[0].path+'/gsolve' )
                gsolve.tick = -1
            if moose.exists(compt[0].path+'/stoich'):
                stoich = moose.Stoich( compt[0].path+'/stoich' )
                stoich.tick = -1
        
        else :
            neurons = moose.wildcardFind(modelPath + "/model/cells/##[ISA=Neuron]")
            for neuron in neurons:
                solver = moose.element(neuron.path + "/hsolve")
                solver.tick = -1
        for table in moose.wildcardFind( modelPath+'/data/graph#/#' ):
            table.tick = -1

    def loadModelDialogSlot(self):
        """Start a file dialog to choose a model file.

        Once the dialog succeeds, we should hand-over the duty of
        actual model loading to something else. Then refresh the
        views. Things to check from the user:

        1) The file type

        2) Target element

        3) Whether we should update the current window or start a new
        window.

        4) Plugin to use for displaying this model (can be automated
        by looking into the model file for a regular expression)

        """
        self.popup.close()
        activeWindow = None # This to be used later to refresh the current widget with newly loaded model
        dialog = LoaderDialog(self,
                              self.tr('Load model from file'))

        if dialog.exec_():
            valid = False
            ret = []
            ret,pluginName = self.checkPlugin(dialog)
            if pluginName == 'kkit':
                compt = moose.wildcardFind(ret['model'].path+'/##[ISA=ChemCompt]')
                if not len(compt):
                    reply = QtGui.QMessageBox.question(self, "Model is empty","Model has no compartment, atleast one compartment should exist to display the widget\n Do you want another file",
                                               QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
                    if reply == QtGui.QMessageBox.Yes:
                        dialog = LoaderDialog(self,self.tr('Load model from file'))
                        if dialog.exec_():
                            ret,pluginName = self.checkPlugin(dialog)
                            ret,valid = self.dialog_check(ret)
                    else:
                        QtGui.QApplication.restoreOverrideCursor()        
                        return
                else:
                    valid = True
            if valid == True:
                modelAnno = moose.Annotator(ret['model'].path+'/info')
                if ret['subtype']:
                    modelAnno.modeltype = ret['subtype']
                else:
                    modelAnno.modeltype = ret['modeltype']
                modelAnno.dirpath = str(dialog.directory().absolutePath())
                self.loadedModelsAction(ret['model'].path,pluginName)
                self.setPlugin(pluginName, ret['model'].path)
                if pluginName == 'kkit':
                    QtCore.QCoreApplication.sendEvent(self.plugin.getEditorView().getCentralWidget().view, QtGui.QKeyEvent(QtCore.QEvent.KeyPress, Qt.Qt.Key_A, Qt.Qt.NoModifier))

    def checkPlugin(self,dialog):
        fileNames = dialog.selectedFiles()
        for fileName in fileNames:
            modelName = dialog.getTargetPath()
            if '/' in modelName:
                raise mexception.ElementNameError('Model name cannot contain `/`')
            ret = loadFile(str(fileName),'%s' %(modelName),merge=False)
            #ret = loadFile(str(fileName), '/model/%s' % (modelName), merge=False)
            #This will clear out object editor's objectpath and make it invisible
            self.objectEditSlot('/',False)
            #if subtype is None, in case of cspace then pluginLookup = /cspace/None
            #     which will not call kkit plugin so cleaning to /cspace
            pluginLookup = '%s/%s' % (ret['modeltype'], ret['subtype'])
            try:
                pluginName = subtype_plugin_map['%s/%s' % (ret['modeltype'], ret['subtype'])]
            except KeyError:
                pluginName = 'default'
            print 'Loaded model', ret['model'].path
            return ret,pluginName

    def dialog_check(self,ret):
        pluginLookup = '%s/%s' % (ret['modeltype'], ret['subtype'])
        try:
            pluginName = subtype_plugin_map['%s/%s' % (ret['modeltype'], ret['subtype'])]
        except KeyError:
            pluginName = 'default'
        if pluginName == 'kkit':
            compt = moose.wildcardFind(ret['model'].path+'/##[ISA=ChemCompt]')
            if not len(compt):
                reply = QtGui.QMessageBox.question(self, "Model is empty","Model has no compartment, atleast one compartment should exist to display the widget\n Do you want another file",
                                           QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
                if reply == QtGui.QMessageBox.Yes:
                    dialog = LoaderDialog(self,self.tr('Load model from file'))
                    if dialog.exec_():
                        ret,pluginName = self.checkPlugin(dialog)
                else:
                    QtGui.QApplication.restoreOverrideCursor()        
                    return
            else:
                return ret,True
    def newModelDialogSlot(self):
        #Harsha: Create a new dialog widget for model building
        self.popup.close()
        newModelDialog = DialogWidget()
        if newModelDialog.exec_():
            modelPath = str(newModelDialog.modelPathEdit.text()).strip()
            if len(modelPath) == 0:
                raise mexception.ElementNameError('Model path cannot be empty')
            if re.search('[ /]',modelPath) is not None:
                raise mexception.ElementNameError('Model path should not containe / or whitespace')
            #plugin = str(newModelDialog.submenu.currentText())
            plugin = str(newModelDialog.getcurrentRadioButton())
            #Harsha: All model will be forced to load/build under /model,
            #2014 sep 10: All the model will be forced to load/build model under /modelName/model
            '''
            modelContainer = moose.Neutral('/model')
            modelRoot = moose.Neutral('%s/%s' % (modelContainer.path, modelPath))
            '''
            if moose.exists(modelPath+'/model'):
                moose.delete(modelPath)

            modelContainer = moose.Neutral('%s' %(modelPath))
            modelRoot = moose.Neutral('%s/%s' %(modelContainer.path,"model"))
            if not moose.exists(modelRoot.path+'/info'):
                moose.Annotator(modelRoot.path+'/info')
            
            modelAnno = moose.element(modelRoot.path+'/info')
            modelAnno.modeltype = "new_kkit"
            modelAnno.dirpath = " "
            self.loadedModelsAction(modelRoot.path,plugin)
            self.setPlugin(plugin, modelRoot.path)
            #Harsha: This will clear out object editor's objectpath and make it invisible
            self.objectEditSlot('/', False)

def main():
    # create the GUI application
    app = QtGui.QApplication(sys.argv)
    QtGui.qApp = app
    #icon = QtGui.QIcon(os.path.join(config.KEY_ICON_DIR,'moose_icon.png'))
    #app.setWindowIcon(icon)
    # instantiate the main window
    #moose.loadModel('../Demos/Genesis_files/Kholodenko.g','/kho')
    mWindow =  MWindow()
    mWindow.setWindowState(QtCore.Qt.WindowMaximized)
    sys.excepthook = mWindow.handleException
    # show it
    mWindow.show()
    # start the Qt main loop execution, exiting from this script
    #http://code.google.com/p/subplot/source/browse/branches/mzViewer/PyMZViewer/mpl_custom_widget.py
    #http://eli.thegreenplace.net/files/prog_code/qt_mpl_bars.py.txt
    #http://lionel.textmalaysia.com/a-simple-tutorial-on-gui-programming-using-qt-designer-with-pyqt4.html
    #http://www.mail-archive.com/matplotlib-users@lists.sourceforge.net/msg13241.html
    # with the same return code of Qt application
    config.settings[config.KEY_FIRSTTIME] = 'False' # string not boolean
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

#
# mgui.py ends here
