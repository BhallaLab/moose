# deadlock_bug.py --- 
# 
# Filename: deadlock_bug.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Mon Jul 16 22:34:26 2012 (+0530)
# Version: 
# Last-Updated: Sat Aug  6 15:32:49 2016 (-0400)
#           By: subha
#     Update #: 22
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This isolates a single celltype for testing.  In some runs, the
# control never returns from ReadCell and I suspected it to be a
# thread deadlock. Hence this file to isolate the issue.
# 

# Change log:
# 
# 
# 

# Code:
from __future__ import print_function

from collections import defaultdict
import moose
import config
from config import logger
import nachans
import kchans
import archan
import cachans
import capool
from channelinit import init_chanlib
        
def read_keyvals(filename):
    """Read the mapping between key value pairs from file.
    
    The file filename should have two columns:

    key value

    """
    ret = defaultdict(set)
    try:
        with(open(filename, 'r')) as level_file:
            for line in level_file:
                tokens = line.split()
                if not tokens:
                    continue
                if len(tokens) != 2:
                    print(filename, ' - Tokens: ', tokens, len(tokens))
                    return None
                ret[tokens[1]].add(tokens[0])
    except IOError:
        config.logger.info('No such file %s' % (filename))
    return ret

def adjust_chanlib(cdict):
    """Update the revarsal potentials for channels. Set the initial X
    value for AR channel. Set the tau for Ca pool."""
    channel_dict = init_chanlib()
    for ch in channel_dict.values():
        if isinstance(ch, kchans.KChannel):
            ch.Ek = cdict['EK']
        elif isinstance(ch, nachans.NaChannel):
            ch.Ek = cdict['ENa']
        elif isinstance(ch, cachans.CaChannel):
            ch.Ek = cdict['ECa']
        elif isinstance(ch, archan.AR):
            ch.Ek = cdict['EAR']
            if 'X_AR' in cdict:
                ch.X = cdict['X_AR']        
        elif isinstance(ch, capool.CaPool):
            ch.tau = cdict['TauCa']            

def read_prototype(celltype, cdict):
    """Read the cell prototype file for the specified class. The
    channel properties are updated using values in cdict."""
    filename = '%s/%s.p' % (config.modelSettings.protodir, celltype)
    logger.debug('Reading prototype file %s' % (filename))
    adjust_chanlib(cdict)
    cellpath = '%s/%s' % (config.modelSettings.libpath, celltype)
    if moose.exists(cellpath):
        return moose.element(cellpath)
    for handler in logger.handlers:
        handler.flush()
    proto = moose.loadModel(filename, cellpath)
    # If prototype files do not have absolute compartment positions,
    # set the compartment postions to origin. This will avoid
    # incorrect assignemnt of position when the x/y/z values in
    # prototype file is just for setting the compartment length.
    if not config.modelSettings.morph_has_postion:
        for comp in moose.wildcardFind('%s/#[ISA=Compartment]' % (proto.path)):
            comp.x = 0.0
            comp.y = 0.0
            comp.z = 0.0
    leveldict = read_keyvals('%s/%s.levels' % (config.modelSettings.protodir, celltype))
    depths = read_keyvals('%s/%s.depths' % (config.modelSettings.protodir, celltype))
    depthdict = {}
    for level, depthset in depths.items():
        if len(depthset) != 1:
            raise Exception('Depth set must have only one entry.')
        depthdict[level] = depthset.pop()
    assign_depths(proto, depthdict, leveldict)
    return proto

def assign_depths(cell, depthdict, leveldict):
    """Assign depths to the compartments in the cell. The original
    model assigns sets of compartments to particular levels and a
    depth is specified for each level. This should not be required if
    we have the z value in prototype file.

    cell : (prototype) cell instance

    depth : dict mapping level no. to physical depth

    level : dict mapping level no. to compartment nos. belonging to
    that level.
    """
    if not depthdict:
        return
    for level, depth in depthdict.items():
        z = float(depth)
        complist = leveldict[level]
        for comp_number in complist:
            comp = moose.element('%s/comp_%s' % (cell.path, comp_number))
            comp.z = z

            
class CellMeta(type):
    def __new__(cls, name, bases, cdict):
        if name != 'CellBase':
            proto = read_prototype(name, cdict)
            if 'soma_tauCa' in cdict:
                moose.element(proto.path + '/comp_1/CaPool').tau = cdict['soma_tauCa']
            cdict['prototype'] = proto
        return type.__new__(cls, name, bases, cdict)

    
class CellBase(moose.Neutral):
    __metaclass__ = CellMeta
    def __init__(self, path):
        if not moose.exists(path):
            path_tokens = path.rpartition('/')
            moose.copy(self.prototype, path_tokens[0], path_tokens[-1])
        moose.Neutral.__init__(self, path)
        
    def comp(self, number):
        return moose.element('%s/comp_%d' % (self.path, number))

    @property
    def soma(self):
        return self.comp(1)

    @property
    def presynaptic(self):
        """Presynaptic compartment. Each subclass should define
        _presynaptic as the index of this compartment."""
        return self.comp[self.__class__._presynaptic]

class TCR(CellBase):
    _presynaptic = 135
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -81e-3
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')
import time
import unittest
import uuid

import moose

def setupClocks(dt):
    print('Setting up clocks')
    for ii in range(10):
        moose.setClock(ii, dt)

def setupCurrentStepModel(testId, celltype, pulsearray, dt):
    """Setup a single cell simulation.

    simid - integer identifying the model

    celltype - str cell type
    
    pulsearray - an nx3 array with row[i] = (delay[i], width[i], level[i]) of current injection.
    """
    modelContainer = moose.Neutral('/test%d' % (testId))
    dataContainer = moose.Neutral('/data%d' % (testId))
    cell = TCR('%s/TCR' % (modelContainer.path)) # moose.copy(cells.TCR.prototype, modelContainer.path)#
    pulsegen = moose.PulseGen('%s/pulse' % (modelContainer.path))
    pulsegen.count = len(pulsearray)
    for ii in range(len(pulsearray)):
        pulsegen.delay[ii] = pulsearray[ii][0]
        pulsegen.width[ii] = pulsearray[ii][1]
        pulsegen.level[ii] = pulsearray[ii][2]
    moose.connect(pulsegen, 'output', cell.soma, 'injectMsg')
    somaVm = moose.Table('%s/vm' % (dataContainer.path))
    moose.connect(somaVm, 'requestOut', cell.soma, 'getVm')
    pulseTable = moose.Table('%s/pulse' % (dataContainer.path))
    moose.connect(pulseTable, 'requestOut', pulsegen, 'getOutputValue')
    setupClocks(dt)
    moose.useClock(0, '%s/##[ISA=Compartment]' % (cell.path), 'init')
    moose.useClock(1, '%s/##[ISA=Compartment]' % (cell.path), 'process')
    moose.useClock(7, pulsegen.path, 'process')
    moose.useClock(8, '%s/##' % (dataContainer.path), 'process')
    return {'cell': cell,
            'stimulus': pulsegen,
            'vmTable': somaVm,
            'stimTable': pulseTable
            }
    
def runsim(simtime, steplength=0.01):
    moose.reinit()
    clock = moose.element('/clock')
    while clock.currentTime < simtime:
        moose.start(steplength)
        print('Current simulation time:', clock.currentTime)
        time.sleep(0.05)

pulsearray = [[.05, 100e-3, 0.9e-9],
              [1e9, 10e-3, 0.3e-9], # This will block stimulation after the first one
              [0.05, 10e-3, 0.1e-9],
              [0.05, 10e-3, -0.1e-9],
              [0.05, 10e-3, -0.3e-9]]

simdt = 0.25e-4
simtime = 0.1

class TestTCR(unittest.TestCase):
    def setUp(self):
        self.testId = uuid.uuid4().int
        params = setupCurrentStepModel(self.testId, 'TCR', pulsearray, simdt)
        print('Starting simulation')
        runsim(simtime)
        
    def testDefault(self):
        pass

if __name__ == '__main__':
    unittest.main()
        


# 
# deadlock_bug.py ends here
