# settings.py --- 
# 
# Filename: settings.py
# Description: 
# Author: 
# Maintainer: 
# Created: Wed May 23 12:04:34 2012 (+0530)
# Version: 
# Last-Updated: Thu May 24 15:18:06 2012 (+0530)
#           By: subha
#     Update #: 167
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
# 
# 
# 

# Code:

class Settings(object):    
    pass
    
class SimulationSettings(Settings):
    """Class for storing simulation settings.

    initialTime: initial value of simulation time (default 0)

    endTime: end of simulation time.

    simulationDt: integration time step for the main simulation.

    outputDt: time interval between recording output points.

    method: what algorithm to use.
    
    """
    def __init__(self):
        self.initialTime = 0.0
        self.endTime = 1.0
        self.simulationDt = 25e-6
        self.outputDt = 0.25e-3
        self.method = 'ee'
        self.moose_seed = 0
    

class ModelSettings(Settings):    
    """Class for storing model settings."""
    def __init__(self):
        self.container = '/'
        self.libpath = '/library'
        self.protodir = 'proto'
        self.populationSize = {}
        self.numpy_seed = None
        

class DataSettings(Settings):
    """Settings for specifying data to be recorded.
    
    
    targets: path to objects to record from, fraction to record from).
    
    fractions: list of single number for fraction of targets to record from when each target is a wild card

    fields: (field-name, dtype) dict for recording
                dtype is `Event` or `AnalogSignal` as per neo library.

    """
    def __init__(self):
        self.random = True
        self.fractions = []
        self.targets = []
        self.fields = {}
        

class ChangeSettings(Settings):
    def __init__(self):
        self.changes = []


# 
# settings.py ends here
