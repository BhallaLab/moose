# test_utils.py --- 
# 
# Filename: test_utils.py
# Description: 
# Author: 
# Maintainer: 
# Created: Sat May 26 10:41:37 2012 (+0530)
# Version: 
# Last-Updated: Sat Aug  6 15:45:51 2016 (-0400)
#           By: subha
#     Update #: 414
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


import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')
import uuid
import numpy as np
from matplotlib import pyplot as plt
import unittest

import moose
from moose import utils as mutils
import config
import channelbase

INITCLOCK = 0
ELECCLOCK = 1
CHANCLOCK = 2
POOLCLOCK = 3
LOOKUPCLOCK = 6
STIMCLOCK = 7
PLOTCLOCK = 8

SIMDT = 5e-6
PLOTDT = 0.25e-3

lib = moose.Neutral(config.modelSettings.libpath)

def setup_clocks(simdt, plotdt):
    print( 'Setting up clocks: simdt', simdt, 'plotdt', plotdt)
    moose.setClock(INITCLOCK, simdt)
    moose.setClock(ELECCLOCK, simdt)
    moose.setClock(CHANCLOCK, simdt)
    moose.setClock(POOLCLOCK, simdt)
    moose.setClock(LOOKUPCLOCK, simdt)
    moose.setClock(STIMCLOCK, simdt)
    moose.setClock(PLOTCLOCK, plotdt)
    moose.le('/clock')
    

def assign_clocks(model_container, data_container, solver='euler'):
    """Assign clockticks to elements.
    
    Parameters
    ----------
    model_container: element

    All model components are under this element. The model elements
    are assigned clocks as required to maintain the right update
    sequence. 

    INITCLOCK = 0 calls `init` method in Compartments

    ELECCLOCK = 1 calls `process` method of Compartments

    CHANCLOCK = 2 calls `process` method for HHChannels

    POOLCLOCK = 3 is not used in electrical simulation

    LOOKUPCLOCK = 6 is not used in these simulations.

    STIMCLOCK = 7 calls `process` method in stimulus objects like
    PulseGen.

    data_container: element     
    All data recording tables are under this element. They are
    assigned PLOTCLOCK, the clock whose update interval is plotdt.

    PLOTCLOCK = 8 calls `process` method of Table elements under
    data_container

    """
    moose.useClock(STIMCLOCK,
                   model_container.path+'/##[TYPE=PulseGen]',
                   'process')
    moose.useClock(PLOTCLOCK,
                   data_container.path+'/##[TYPE=Table]',
                   'process')
    if solver == 'hsolve':
        for neuron in moose.wildcardFind('%s/##[TYPE=Neuron]'):
            solver = moose.HSolve(neuron.path+'/solve')
            solver.dt = moose.element('/clock/tick[0]').dt
            solver.target = neuron.path
        moose.useClock(INITCLOCK,
                       model_container.path+'/##[TYPE=HSolve]',
                       'process')
    else:
        moose.useClock(INITCLOCK, 
                       model_container.path+'/##[TYPE=Compartment]', 
                       'init')
        moose.useClock(ELECCLOCK,
                       model_container.path+'/##[TYPE=Compartment]', 
                       'process')
        moose.useClock(CHANCLOCK, 
                       model_container.path+'/##[TYPE=HHChannel]',
                       'process')
        moose.useClock(POOLCLOCK,
                       model_container.path+'/##[TYPE=CaConc]',
                       'process')
    
def step_run(simtime, steptime, verbose=True):
    """Run the simulation in steps of `steptime` for `simtime`."""
    clock = moose.Clock('/clock')
    if verbose:
        print( 'Starting simulation for', simtime)
    while clock.currentTime < simtime - steptime:
        moose.start(steptime)
        if verbose:
            print( 'Simulated till', clock.currentTime, 's')
    remaining = simtime - clock.currentTime
    if remaining > 0:
        if verbose:
            print( 'Running the remaining', remaining, 's')
        moose.start(remaining)
    if verbose:
        print( 'Finished simulation')
    

def make_testcomp(containerpath):
    comp = moose.Compartment('%s/testcomp' % (containerpath))
    comp.Em = -65e-3
    comp.initVm = -65e-3
    comp.Cm = 1e-12
    comp.Rm = 1e9
    comp.Ra = 1e5
    return comp

def make_pulsegen(containerpath):
    pulsegen = moose.PulseGen('%s/testpulse' % (containerpath))
    pulsegen.firstLevel = 1e-12
    pulsegen.firstDelay = 50e-3
    pulsegen.firstWidth = 100e-3
    pulsegen.secondLevel = -1e-12
    pulsegen.secondDelay = 150e-3
    pulsegen.secondWidth = 100e-3
    pulsegen.count = 3
    pulsegen.delay[2] = 1e9
    return pulsegen


def setup_single_compartment(model_container, data_container, channel_proto, Gbar):
    """Setup a single compartment with a channel

    Parameters
    ----------
    model_container: element
     The model compartment is created under this element

    data_container: element
     The tables to record data are created under this

    channel_proto: element
     Channel prototype in library

    Gbar: float
     Maximum conductance density of the channel

    """
    comp = make_testcomp(model_container.path)
    channel = moose.copy(channel_proto, comp, channel_proto.name)[0]
    moose.connect(channel, 'channel', comp, 'channel')
    channel.Gbar = Gbar
    pulsegen = make_pulsegen(model_container.path)
    moose.connect(pulsegen, 'output', comp, 'injectMsg')
    vm_table = moose.Table('%s/Vm' % (data_container.path))
    moose.connect(vm_table, 'requestOut', comp, 'getVm')
    gk_table = moose.Table('%s/Gk' % (data_container.path))
    moose.connect(gk_table, 'requestOut', channel, 'getGk')
    ik_table = moose.Table('%s/Ik' % (data_container.path))
    moose.connect(ik_table, 'requestOut', channel, 'getIk')
    return {'compartment': comp,
            'stimulus': pulsegen,
            'channel': channel,
            'Vm': vm_table,
            'Gk': gk_table,
            'Ik': ik_table}

def insert_hhchannel(compartment, channelclass, gbar):
    channel = moose.copy(channelclass.prototype, compartment)
    channel[0].Gbar = gbar
    moose.connect(channel, 'channel', compartment, 'channel')
    return channel[0]
    
def compare_data_arrays(left, right, relative='maxw', plot=False, x_range=None):
    """Compare two data arrays and return some measure of the
    error. 

    The arrays must have the same number of dimensions (1 or 2) and
    represent the same range of x values. In case they are 1
    dimensional, we take x values as relative position of that data
    point in the total x-range.

    We interpolate the y values for the x-values of the series with
    lower resolution using the heigher resolution series as the
    interpolation table.

    The error is calculated as the maximum difference between the
    interpolated values and the actual values in the lower resolution
    array divided by the difference between the maximum and minimum y
    values of both the series.

    If plot is True, left, right and their difference at common points
    are plotted.

    relative: `rms` - return root mean square of the error values
    `taxicab` - mean of the absolute error values
    `maxw` - max(abs(error))/(max(y) - min(y))    
    `meany` - rms(error)/mean(y)

    x_range : (minx, maxx) range of X values to consider for comparison

    """
    if len(left.shape) != len(right.shape):
        print( left.shape, right.shape)
        raise ValueError('Arrays to be compared must have same dimensions.')
    # y is the intrepolation result for x array using xp and fp when xp and x do not match.
    # xp and fp are interpolation table's independent and dependent variables
    # yp is a view of the original y values
    x = None
    y = None
    xp = None
    fp = None
    yp = None
    # arbitrarily keep series with more datapoint as left
    if left.shape[0] < right.shape[0]:
        tmp = left
        left = right
        right = tmp
    if len(right.shape) == 1:
        x = np.arange(right.shape[0]) * 1.0 / right.shape[0]
        yp = right
        xp = np.arange(left.shape[0]) * 1.0 / left.shape[0]
        fp = left
    elif len(right.shape) == 2:
        x = right[:,0]
        yp = right[:,1]
        xp = left[:,0]
        fp = left[:,1]
    else:
        raise ValueError('Cannot handle more than 2 dimensional arrays.')
    if left.shape[0] != right.shape[0]:
        print( 'Array sizes not matching: (%d <> %d) - interpolating' % (left.shape[0], right.shape[0]))
        y = np.interp(x, xp, fp)
    else: # assume we have the same X values when sizes are the same
        y = np.array(fp)
    if x_range:
        indices = np.nonzero((x > x_range[0]) & (x <= x_range[1]))[0]
        y = np.array(y[indices])
        yp = np.array(yp[indices])
        x = np.array(x[indices])
        # We update xp and fp to have the same plotting x-range
        indices = np.nonzero((xp > x_range[0]) & (xp <= x_range[1]))[0]
        xp = xp[indices]
        fp = fp[indices]
    err = y - yp
    print( min(err), max(err), min(y), max(y), min(yp), max(yp))
    # I measure a conservative relative error as maximum of all the
    # errors between pairs of points with
    all_y = np.r_[y, yp]
    if plot:        
        plt.plot(x, yp, 'b-.', label='right')
        plt.plot(xp, fp, 'g--', label='left')
        plt.plot(x, err, 'r:', label='error')
        plt.legend()
        plt.show()
    if relative == 'rms':
        return np.sqrt(np.mean(err**2))
    elif relative == 'taxicab':
        return np.mean(np.abs(err))
    elif relative == 'maxw':
        return max(np.abs(err))/(max(all_y) - min(all_y))
    elif relative == 'meany':
        return np.sqrt(np.mean(err**2)) / np.mean(all_y)
    else:
        return err
    
import csv

def compare_cell_dump(left, right, rtol=1e-3, atol=1e-8, row_header=True, col_header=True):
    """This is a utility function to compare various compartment
    parameters for a single cell model dumped in csv format using
    NEURON and MOOSE."""
    print( 'Comparing:', left, 'with', right)
    ret = True
    left_file = open(left, 'rb')
    right_file = open(right, 'rb')
    left_reader = csv.DictReader(left_file, delimiter=',')
    right_reader = csv.DictReader(right_file, delimiter=',')
    lheader = list(left_reader.fieldnames)
    lheader.remove('comp')
    lheader = sorted(lheader)
    rheader = list(right_reader.fieldnames)
    rheader.remove('comp')
    rheader = sorted(rheader)
    if len(lheader) != len(rheader):
        print( 'Column number mismatch: left %d <-> right %d' % (len(lheader), len(rheader)))
        return False
    for ii in range(len(lheader)):
        if lheader[ii] != rheader[ii]:
            print( ii, '-th column name mismatch:', lheader[ii], '<->', rheader[ii])
            return False    
    index = 2
    left_end = False
    right_end = False
    while True:
        try:
            left_row = next(left_reader)
        except StopIteration:
            left_end = True
        try:
            right_row = next(right_reader)
        except StopIteration:
            right_end = True
        if left_end and not right_end:
            print( left, 'run out of line after', index, 'rows')
            return False
        if right_end and not left_end:
            print( right, 'run out of line after', index, 'rows')
            return False
        if left_end and right_end:
            return ret
        if len(left_row) != len(right_row):
            print( 'No. of columns differ: left - ', len(left_row), 'right -', len(right_row))
            ret = False
            break        
        for key in lheader:
            try:
                left = float(left_row[key])
                right = float(right_row[key])
                if not np.allclose(float(left), float(right), rtol=rtol, atol=atol):
                    print(('Mismatch in row:%s, column:%s. Values: %g <> %g' % (index, key, left, right)))
                    ret = False
            except ValueError as e:
                print(e)
                print(('Row:', index, 'Key:', key, left_row[key], right_row[key]))
        index = index + 1
    return ret




    
# 
# test_utils.py ends here
