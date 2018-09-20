"""
This script demonstrates how to use moogli to carry out a simulation and
simultaneously update the visualizer. Also another viewer shows the compartment
rm values. The visualizers remain active while the simulation is running.
"""

try:
    import moogli
except ImportError as e:
    print( "[INFO ] Could not import moogli. Quitting..." )
    quit()

import moose
from moose import neuroml
import sys
import os
import random
import numpy as np
import math
from PyQt4.QtGui import QApplication

# The QApplication class manages the GUI application's
# control flow and main settings
app = QApplication(sys.argv)

# Load model from the neuroml file into moose
filename = os.path.join( os.path.split(os.path.realpath(__file__))[0]
                       , "../neuroml/PurkinjeCellPassivePulseInput/PurkinjePassive.net.xml"
                       )
popdict, projdict = moose.neuroml.loadNeuroML_L123(filename)

# setting up hsolve object for each neuron
for popinfo in list(popdict.values()):
    for cell in list(popinfo[1].values()):
        solver = moose.HSolve(cell.path + "/hsolve")
        solver.target = cell.path

# reinit moose to bring to a reliable initial state.
moose.reinit()

SIMULATION_DELTA = 0.001
SIMULATION_TIME  = 0.03

ALL_COMPARTMENTS = [x.path for x in moose.wildcardFind("/cells[0]/##[ISA=CompartmentBase]")]

BASE_RM_VALUE = min( [moose.element(x).Rm for x in ALL_COMPARTMENTS]
                   )
PEAK_RM_VALUE = max( [moose.element(x).Rm for x in ALL_COMPARTMENTS]
                   )
BASE_RM_COLOR = [0.0, 1.0, 0.0, 0.1]
PEAK_RM_COLOR = [1.0, 0.0, 0.0, 1.0]

BASE_VM_VALUE = -0.065
PEAK_VM_VALUE = -0.060
BASE_VM_COLOR = [1.0, 0.0, 0.0, 0.1]
PEAK_VM_COLOR = [0.0, 0.0, 1.0, 1.0]


def create_vm_visualizer():
    # Moogli requires a morphology object. Create a morphology object
    # by reading the geometry details from all objects of type CompartmentBase
    # inside /cells[0]
    vm_morphology = moogli.read_morphology_from_moose(name = "", path = "/cells[0]")


    # Create a named group of compartments called 'group-all'
    # which will contain all the compartments of the model.
    # Each group has a strict upper and lower limit for the
    # variable which is being visualized.
    # Both limits map to colors provided to the api.
    # The value of the variable is linearly mapped to a color value
    # lying between the upper and lower color values.

    vm_morphology.create_group( "group-all"    # group name
                              , ALL_COMPARTMENTS  # sequence of compartments belonging to this group
                              , BASE_VM_VALUE   # base value of variable
                              , PEAK_VM_VALUE   # peak value of variable
                              , BASE_VM_COLOR # color corresponding to base value
                              , PEAK_VM_COLOR # color corresponding to peak value
                           )


    # set initial color of all compartments in accordance with their vm
    vm_morphology.set_color( "group-all"
                           , [moose.element(x).Vm for x in ALL_COMPARTMENTS]
                           )

    # instantiate the visualizer with the morphology object created earlier
    vm_viewer = moogli.DynamicMorphologyViewerWidget(vm_morphology)

    # Callback function will be called by the visualizer at regular intervals.
    # The callback can modify both the morphology and viewer object's properties
    # since they are passed as arguments.
    def callback(morphology, viewer):
        # run simulation for 1 ms
        moose.start(SIMULATION_DELTA)

        # change color of all the compartments according to their vm values.
        # a value higher than peak value will be clamped to peak value
        # a value lower than base value will be clamped to base value.
        morphology.set_color( "group-all"
                            , [moose.element(x).Vm for x in ALL_COMPARTMENTS]
                            )
        # if the callback returns true, it will be called again.
        # if it returns false it will not be called again.
        # the condition below ensures that simulation runs for 1 sec
        if moose.element("/clock").currentTime < SIMULATION_TIME : return True
        else                                                     : return False


    # set the callback function to be called after every idletime milliseconds
    vm_viewer.set_callback(callback, idletime = 0)

    # make sure that entire model is visible
    vm_viewer.pitch(math.pi / 2)
    vm_viewer.zoom(0.25)
    vm_viewer.setWindowTitle("Vm Visualization")
    return vm_viewer

def create_rm_visualizer():
    # Moogli requires a morphology object. Create a morphology object
    # by reading the geometry details from all objects of type CompartmentBase
    # inside /cells[0]
    morphology = moogli.read_morphology_from_moose(name = "", path = "/cells[0]")

    # Create a named group of compartments called 'group-all'
    # which will contain all the compartments of the model.
    # Each group has a strict upper and lower limit for the
    # variable which is being visualized.
    # Both limits map to colors provided to the api.
    # The value of the variable is linearly mapped to a color value
    # lying between the upper and lower color values.
    morphology.create_group( "group-all"    # group name
                           , ALL_COMPARTMENTS  # sequence of compartments belonging to this group
                           , BASE_RM_VALUE # base value of variable
                           , PEAK_RM_VALUE # peak value of variable
                           , BASE_RM_COLOR # color corresponding to base value
                           , PEAK_RM_COLOR # color corresponding to peak value
                           )

    # set initial color of all compartments in accordance with their rm
    morphology.set_color( "group-all"
                        , [moose.element(x).Rm for x in ALL_COMPARTMENTS]
                        )

    # instantiate the visualizer with the morphology object created earlier
    rm_viewer = moogli.MorphologyViewerWidget(morphology)

    # make sure that entire model is visible
    rm_viewer.pitch(math.pi / 2)
    rm_viewer.zoom(0.25)
    rm_viewer.setWindowTitle("Rm")
    return rm_viewer

vm_visualizer = create_vm_visualizer()
rm_visualizer = create_rm_visualizer()
vm_visualizer.show()
rm_visualizer.show()



# http://stackoverflow.com/questions/15861839/error-upon-app-shutdown-qglcontextmakecurrent-cannot-make-invalid-context-cu
def delete_gl_widget():
    global vm_visualizer
    global rm_visualizer
    vm_visualizer.setParent(None)
    del vm_visualizer
    rm_visualizer.setParent(None)
    del rm_visualizer

QApplication.instance().aboutToQuit.connect( delete_gl_widget )

# Enter the main event loop and wait until exit() is called.
# It is necessary to call this function to start event handling.
# The main event loop receives events from the window system and
# dispatches these to the application widgets.
app.exec_()
