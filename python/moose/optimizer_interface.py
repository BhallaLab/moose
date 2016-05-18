# optimizer_interface.py --- 
# 
# Filename: optimizer_interface.py
# Description: Provides an interface between Optimizer and MOOSE
# Author: Viktor Toth
# Maintainer: 
# Copyright (C) 2014 Viktor Toth, all rights reserved.
# Created: 7 Aug 14:45:30 2014 (+0530)
# Version: 1.0
# Last-Updated: 
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Code:

import os
from . import moose

class OptimizerInterface:
    """
    Establish connection between MOOSE and Optimzer, parameter fitting tool.
    
    Usage: create an OptimizerInterface object at the beginning of the
    script running the MOOSE simulation. Call getParams() to retrieve
    the parameters advised by Optimizer, then run the simulation using
    these parameters. When the simulation has already run, call addTrace()
    passing every trace as a moose.Table or list of floats. When all the
    traces are added, call writeTraces() so when your script finished
    Optimizer is able to read these traces from traceFile.
    
    On the second 'layer' of the Optimizer GUI select external
    (as type of simulator) and type into the command text box:
    'python /path_to_your_model_script/script.py 3' if the number of
    parameters to fit is 3.
    """
    paramFile = 'params.param' # contains parameters, separated by newline
    traceFile = 'trace.dat'    # contains traces, TAB separated in columns
    params = []
    traces = []
    def __init__(self, paramFile = paramFile, traceFile = traceFile):
        """
        Opens or create paramFile and traceFile. Reads the parameters
        from paramFile if able to.
        """
        self.paramFile = paramFile
        self.traceFile = traceFile
        
        # parameter file
        if os.path.isfile(self.paramFile):
            with open(self.paramFile) as f:
                self.params = [float(line) for line in f]
        else:
            open(self.paramFile, 'a').close() # create file
        
        # trace file
        if not os.path.isfile(self.traceFile):
            open(self.traceFile, 'a').close() # create file
    
    def addTrace(self, trace):
        """
        A trace can be a moose.Table object or a list of float numbers.
        """
        if isinstance(trace, moose.Table):
            self.traces.append(trace.vec)
        else:
            self.traces.append(trace)
    
    def writeTraces(self):
        """
        Writes the content of traces to traceFile. Every column is a
        trace separated by TABs.
        """
        # TODO implement interpolation if trace is not the same length - not really needed
        # all traces should have the same length (same sampling freq)
        assert len(self.traces) > 0 and len(self.traces[0]) > 0, 'No traces or empty trace found!'
        for i in range(1, len(self.traces)):
            assert len(self.traces[i - 1]) == len(self.traces[i]), 'All traces should have the same length! Use identical sampling frequency!'
        
        with open(self.traceFile, 'w') as f:
            for i in range(len(self.traces[0])):
                row = [str(trace[i]) for trace in self.traces]
                f.write('\t'.join(row) + '\n')
    
    def getParams(self):
        """
        Returns the list of parameters read from paramFile.
        """
        return self.params
