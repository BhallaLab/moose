# electronics.py --- 
# 
# Filename: electronics.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Feb 22 00:53:38 2012 (+0530)
# Version: 
# Last-Updated: Tue Jul 10 10:28:40 2012 (+0530)
#           By: subha
#     Update #: 221
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 2012-02-22 23:22:30 (+0530) Subha - the circuitry put in a class.
# 

# Code:

import numpy
import moose

class ClampCircuit(moose.Neutral):
    """Container for a Voltage-Clamp/Current clamp circuit."""
    defaults = {
        'level1': 25.0,
        'width1': 50.0,
        'delay1': 2.0,
        'delay2': 1e6,
        'trigMode': 0,
        'delay3': 1e9
        }
    def __init__(self, path, compartment):
        moose.Neutral.__init__(self, path)        
        self.pulsegen = moose.PulseGen(path+"/pulse") # holding voltage/current generator
        self.pulsegen.count = 2
        self.pulsegen.firstLevel = 25.0
        self.pulsegen.firstWidth = 50.0
        self.pulsegen.firstDelay = 2.0
        self.pulsegen.secondDelay = 0.0
        self.pulsegen.trigMode = 2
        self.gate = moose.PulseGen(path+"/gate") # holding voltage/current generator
        self.gate.level[0] = 1.0
        self.gate.delay[0] = 0.0
        self.gate.width[0] = 1e9
        moose.connect(self.gate, 'output', self.pulsegen, 'input')
        self.lowpass = moose.RC(path+"/lowpass") # lowpass filter
        self.lowpass.R = 1.0
        self.lowpass.C = 0.03
        self.vclamp = moose.DiffAmp(path+"/vclamp")
        self.vclamp.gain = 0.0
        self.vclamp.saturation = 1e10
        self.iclamp = moose.DiffAmp(path+"/iclamp")
        self.iclamp.gain = 0.0
        self.iclamp.saturation = 1e10
        self.pid = moose.PIDController(path+"/pid")
        self.pid.gain = 0.5
        self.pid.tauI = 0.02
        self.pid.tauD = 0.005
        self.pid.saturation = 1e10
        # Connect current clamp circuitry
        moose.connect(self.pulsegen, "output", self.iclamp, "plusIn")
        moose.connect(self.iclamp, "output", compartment, "injectMsg")
        # Connect voltage clamp circuitry
        moose.connect(self.pulsegen, "output", self.lowpass, "injectIn")
        moose.connect(self.lowpass, "output", self.vclamp, "plusIn")
        moose.connect(self.vclamp, "output", self.pid, "commandIn")
        moose.connect(compartment, "VmOut", self.pid, "sensedIn")
        moose.connect(self.pid, "output", compartment, "injectMsg")
        current_table = moose.Table("/data/Im")
        moose.connect(current_table, "requestOut", compartment, "getIm")

    def configure_pulses(self, baseLevel=0.0, firstLevel=0.1, firstDelay=5.0, firstWidth=40.0, secondLevel=0.0, secondDelay=1e6, secondWidth=0.0, singlePulse=True):
        """Set up the pulse generator."""        
        self.pulsegen.baseLevel = baseLevel
        self.pulsegen.firstLevel = firstLevel
        self.pulsegen.firstWidth = firstWidth
        self.pulsegen.firstDelay = firstDelay
        self.pulsegen.secondLevel = secondLevel
        self.pulsegen.secondDelay = secondDelay
        self.pulsegen.secondWidth = secondWidth
        if singlePulse:
            self.pulsegen.trigMode = 1  
        else:
            self.pulsegen.trigMode = 0
        
    def do_voltage_clamp(self):
        """Switch to voltage clamp circuitry. After this the simdt may
        need to be changed for correct performance."""
        self.vclamp.gain = 1.0
        self.iclamp.gain = 0.0
        self.pid.gain = 0.5
        self.pid.tauD = 0.005
        self.pid.tauI = 0.02
            
    def do_current_clamp(self):
        """Switch to current clamp circuitry. After this the simdt may
        need to be changed for correct performance."""
        self.iclamp.gain = 1.0
        self.vclamp.gain = 0.0
        self.pid.gain = 0.0

        
# 
# electronics.py ends here
