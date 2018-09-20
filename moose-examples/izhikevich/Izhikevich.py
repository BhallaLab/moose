# Izhikevich.py --- 
# 
# Filename: Izhikevich.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri May 28 14:42:33 2010 (+0530)
# Version: 
# Last-Updated: Tue Sep 11 14:27:18 2012 (+0530)
#           By: subha
#     Update #: 1212
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# threhold variablity to be checked.
# Bistability not working.
# DAP working with increased parameter value 'a'
# inhibition induced spiking kind of working but not matching with the paper figure
# inhibition induced bursting kind of working but not matching with the paper figure
# Accommodation cannot work with the current implementation: because the equation for u is not what is mentioned in the paper
# it is: u = u + tau*a*(b*(V+65)); [It is nowhere in the paper and you face it only if you look at the matlab code for figure 1].
# It is not possible to tune a, b, c, d in any way to produce this from: u = u + tau*a*(b*V - u) 
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

import time
from numpy import *
import os
import sys
import moose

class IzhikevichDemo:
    """Class to setup and simulate the various kind of neuronal behaviour using Izhikevich model.
    
    Fields:
    """    
    # Paramteres for different kinds of behaviour described by Izhikevich
    # (1. IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 14, NO. 6, NOVEMBER 2003
    # and 2. IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 15, NO. 5, SEPTEMBER
    # 2004)
    # Modified and enhanced using: http://www.izhikevich.org/publications/figure1.m
    # The entries in the tuple are as follows:
    # fig. no. in paper (2), parameter a, parameter b, parameter c (reset value of v in mV), parameter d (after-spike reset value of u), injection current I (uA), initial value of Vm, duration of simulation (ms)
    # 
    # They are all in whatever unit they were in the paper. Just before use we convert them to SI.
    parameters = {
        "tonic_spiking":    ['A', 0.02  ,    0.2  ,   -65.0,     6.0  ,      14.0,      -70.0,  100.0], # Fig. 1.A
        "phasic_spiking":   ['B', 0.02  ,    0.25 ,   -65.0,     6.0  ,      0.5,       -64.0,  200.0], # Fig. 1.B
        "tonic_bursting":   ['C', 0.02  ,    0.2  ,   -50.0,     2.0  ,      15.0,      -70.0,  220.0], # Fig. 1.C
        "phasic_bursting":  ['D', 0.02  ,    0.25 ,   -55.0,     0.05 ,      0.6,       -64.0,  200.0], # Fig. 1.D
        "mixed_mode":       ['E', 0.02  ,    0.2  ,   -55.0,     4.0   ,     10.0,      -70.0,  160.0], # Fig. 1.E
        "spike_freq_adapt": ['F', 0.01  ,    0.2  ,   -65.0,     8.0   ,     30.0,      -70.0,  85.0 ],  # Fig. 1.F # spike frequency adaptation
        "Class_1":          ['G', 0.02  ,    -0.1 ,   -55.0,     6.0   ,     0,         -60.0,  300.0], # Fig. 1.G # Spikining Frequency increases with input strength
        "Class_2":          ['H', 0.2   ,    0.26 ,   -65.0,     0.0   ,     0,         -64.0,  300.0], # Fig. 1.H # Produces high frequency spikes  
        "spike_latency":    ['I', 0.02  ,    0.2  ,   -65.0,     6.0   ,     7.0,       -70.0,  100.0], # Fig. 1.I
        "subthresh_osc":    ['J', 0.05  ,    0.26 ,   -60.0,     0.0   ,     0,         -62.0,  200.0], # Fig. 1.J # subthreshold oscillations
        "resonator":        ['K', 0.1   ,    0.26 ,   -60.0,     -1.0  ,     0,         -62.0,  400.0], # Fig. 1.K 
        "integrator":       ['L', 0.02  ,    -0.1 ,   -55.0,     6.0   ,     0,         -60.0,  100.0], # Fig. 1.L 
        "rebound_spike":    ['M', 0.03  ,    0.25 ,   -60.0,     4.0   ,     -15,       -64.0,  200.0], # Fig. 1.M 
        "rebound_burst":    ['N', 0.03  ,    0.25 ,   -52.0,     0.0   ,     -15,       -64.0,  200.0], # Fig. 1.N 
        "thresh_var":       ['O', 0.03  ,    0.25 ,   -60.0,     4.0   ,     0,         -64.0,  100.0], # Fig. 1.O # threshold variability
        "bistable":         ['P', 0.1   ,    0.26  ,  -60.0,     0.0   ,     1.24,      -61.0,  300.0], # Fig. 1.P 
        "DAP":              ['Q', 1.15   ,    0.2  ,   -60.0,     -21.0 ,     20,        -70.0,  50.0], # Fig. 1.Q # Depolarizing after-potential - a had to be increased in order to reproduce the figure
        "accommodation":    ['R', 0.02  ,    1.0  ,   -55.0,     4.0   ,     0,         -65.0,  400.0], # Fig. 1.R 
        "iispike":          ['S', -0.02 ,    -1.0 ,   -60.0,     8.0   ,     75.0,      -63.8,  350.0], # Fig. 1.S # inhibition-induced spiking
        "iiburst":          ['T', -0.026,    -1.0 ,   -45.0,     0.0   ,     75.0,      -63.8,  350.0]  # Fig. 1.T # inhibition-induced bursting
    }

    documentation = {
        "tonic_spiking":        """
Neuron is normally silent but spikes when stimulated with a current injection.""",

        "phasic_spiking":       """
Neuron fires a single spike only at the start of a current pulse.""",

        "tonic_bursting":       """
Neuron is normally silent but produces bursts of spikes when
stimulated with current injection.""",

        "phasic_bursting":      """
Neuron is normally silent but produces a burst of spikes at the
beginning of an input current pulse.""",

        "mixed_mode":           """
Neuron fires a burst at the beginning of input current pulse, but then
switches to tonic spiking.""",

        "spike_freq_adapt":     """
Neuron fires spikes when a current injection is applied, but at a
gradually reducing rate.""",

        "Class_1":              """
Neuron fires low frequency spikes with weak input current injection.""",

        "Class_2":              """
Neuron fires high frequency (40-200 Hz) spikes when stimulated with
current injection.""",

        "spike_latency":        """
The spike starts after a delay from the onset of current
injection. The delay is dependent on strength of input.""",

        "subthresh_osc":        """
Even at subthreshold inputs a neuron exhibits oscillatory membrane potential.""",

        "resonator":            """
Neuron fires spike only when an input pulsetrain of a frequency
 similar to that of the neuron's subthreshold oscillatory frequency is
 applied.""",

        "integrator":           """
The chances of the neuron firing increases with increase in the frequency 
of input pulse train.""",

        "rebound_spike":        """
When the neuron is released from an inhibitory input, it fires a spike.""",

        "rebound_burst":        """
When the neuron is released from an inhibitory input, it fires a burst
 of action potentials.""",

        "thresh_var":           """
Depending on the previous input, the firing threshold of a neuron may
change.  In this example, the first input pulse does not produce
spike, but when the same input is applied after an inhibitory input,
it fires.""",

        "bistable":             """
These neurons switch between two stable modes (resting and tonic spiking). 
The switch happens via an excitatory or inhibitory input.""",

        "DAP":                  """
After firing a spike, the membrane potential shows a prolonged depolarized 
after-potential.""",

        "accommodation":        """
These neurons do not respond to slowly rising input, but a sharp increase 
in input may cause firing.""",

        "iispike":              """
These neurons fire in response to inhibitory input.""",

        "iiburst":              """
These neurons show bursting in response to inhibitory input."""
        }
    
    def __init__(self):
        """Initialize the object."""
        self.model_container = moose.Neutral('/model')
        self.data_container = moose.Neutral('/data')
        self.neurons = {}
        self.Vm_tables = {}
        self.u_tables = {}
        self.inject_tables = {}
        self.inputs = {}
        self.simtime = 100e-3
        self.dt = 0.25e-3
        self.steps = int(self.simtime/self.dt)
        moose.setClock(0, self.dt)
        moose.setClock(1, self.dt)
        moose.setClock(2, self.dt)
        self.scheduled = {} # this is to bypass multiple clock issue
        self.neuron = None

    def setup(self, key):
        neuron = self._get_neuron(key)
        pulsegen = self._make_pulse_input(key)
        if pulsegen is None:
            print((key, 'Not implemented.'))
            
    def simulate(self, key):
        self.setup(key)
        return self.run(key)

    def run(self, key):
        try:
            Vm = self.Vm_tables[key]
            u = self.u_tables[key]
        except KeyError as e:
            Vm = moose.Table(self.data_container.path + '/' + key + '_Vm')
            nrn = self.neurons[key]
            moose.connect(Vm, 'requestOut', nrn, 'getVm')
            utable = moose.Table(self.data_container.path + '/' + key + '_u')
            utable.connect('requestOut', self.neurons[key], 'getU')
            self.Vm_tables[key] = Vm
            self.u_tables[key] = utable
        try:
            Im = self.inject_tables[key]
        except KeyError as e:
            Im = moose.Table(self.data_container.path + '/' + key + '_inject') # May be different for non-pulsegen sources.
            Im.connect('requestOut', self._get_neuron(key), 'getIm')
            self.inject_tables[key] = Im
        self.simtime = IzhikevichDemo.parameters[key][7] * 1e-3
        for obj in moose.wildcardFind('%s/##' % (self.model_container.path)):
            if obj not in self.scheduled:
                moose.useClock(0, obj.path, 'process')
                self.scheduled[obj] = True
        for obj in moose.wildcardFind('%s/##' % (self.data_container.path)):
            if obj not in self.scheduled:
                moose.useClock(2, obj.path, 'process')
                self.scheduled[obj] = True
        moose.reinit()
        moose.start(self.simtime)
        while moose.isRunning():
            time.sleep(100)
        t = linspace(0, IzhikevichDemo.parameters[key][7], len(Vm.vector))
        # DEBUG
        nrn = self._get_neuron(key)
        print(('a = %g, b = %g, c = %g, d = %g, initVm = %g, initU = %g' % (nrn.a,nrn.b, nrn.c, nrn.d, nrn.initVm, nrn.initU)))
        #! DEBUG
        return (t, Vm, Im)


    def _get_neuron(self, key):
        try:
            params = IzhikevichDemo.parameters[key]
        except KeyError as e:
            print((' %s : Invalid neuron type. The valid types are:' % (key)))
            for key in IzhikevichDemo.parameters:
                print(key)
            raise e
        try:
            neuron = self.neurons[key]
            return neuron
        except KeyError as e:
            neuron = moose.IzhikevichNrn(self.model_container.path + '/' + key)

        if key == 'integrator' or key == 'Class_1': # Integrator has different constants
            neuron.beta = 4.1e3
            neuron.gamma = 108.0
        if key == 'accommodation':
            neuron.accommodating = True
            neuron.u0 = -0.065
        self.neuron = neuron
        neuron.a = params[1] * 1e3 # ms^-1 -> s^-1
        neuron.b = params[2] * 1e3 # ms^-1 -> s^-1
        neuron.c = params[3] * 1e-3 # mV -> V
        neuron.d = params[4]  # d is in mV/ms = V/s
        neuron.initVm = params[6] * 1e-3 # mV -> V
        neuron.Vmax = 0.03 # mV -> V
        if key != 'accommodation':
            neuron.initU = neuron.initVm * neuron.b
        else:
            neuron.initU = -16.0 # u is in mV/ms = V/s
            moose.showfield(neuron)
        self.neurons[key] = neuron
        return neuron

    def _make_pulse_input(self, key):
        """This is for creating a pulse generator for use as a current
        source for all cases except Class_1, Class_2, resonator,
        integrator, thresh_var and accommodation."""
        try:
            return self.inputs[key]
        except KeyError:
            pass # continue to the reset of the function
        baseLevel = 0.0
        firstWidth = 1e6
        firstDelay = 0.0
        firstLevel = IzhikevichDemo.parameters[key][5] * 1e-6
        secondDelay = 1e6
        secondWidth = 0.0
        secondLevel = 0.0
        if key == 'tonic_spiking':
            firstDelay = 10e-3
        elif key == 'phasic_spiking':
            firstDelay = 20e-3
        elif key == 'tonic_bursting':
            firstDelay = 22e-3
        elif key == 'phasic_bursting':
            firstDelay = 20e-3
        elif key == 'mixed_mode':
            firstDelay = 16e-3
        elif key == 'spike_freq_adapt':
            firstDelay = 8.5e-3
        elif key == 'spike_latency':
            firstDelay = 10e-3
            firstWidth = 3e-3
        elif key == 'subthresh_osc':
            firstDelay = 20e-3
            firstWidth = 5e-3
            firstLevel = 2e-9
        elif key == 'rebound_spike':
            firstDelay = 20e-3
            firstWidth = 5e-3
        elif key == 'rebound_burst':
            firstDelay = 20e-3
            firstWidth = 5e-3
        elif key == 'bistable':
            input_table = self._make_bistable_input()
            self.inputs[key] = input_table
            return input_table
        elif key == 'DAP':
            firstDelay = 9e-3
            firstWidth = 2e-3
        elif (key == 'iispike') or (key == 'iiburst'):
            baseLevel = 80e-9
            firstDelay = 50e-3
            firstWidth = 200e-3
            fisrtLevel = 75e-9
        elif key == 'Class_1':
            input_table = self._make_Class_1_input()
            self.inputs[key] = input_table
            return input_table
        elif key == 'Class_2':
            input_table = self._make_Class_2_input()
            self.inputs[key] = input_table
            return input_table
        elif key == 'resonator':
            input_table = self._make_resonator_input()
            self.inputs[key] = input_table
            return input_table
        elif key == 'integrator':
            input_table = self._make_integrator_input()
            self.inputs[key] = input_table
            return input_table
        elif key == 'accommodation':
            input_table = self._make_accommodation_input()
            self.inputs[key] = input_table
            return input_table            
        elif key == 'thresh_var':
            input_table = self._make_thresh_var_input()
            self.inputs[key] = input_table
            return input_table                        
        else:
            raise RuntimeError( key + ': Stimulus is not based on pulse generator.')

        pulsegen = self._make_pulsegen(key, 
                                      firstLevel,
                                      firstDelay,
                                      firstWidth,
                                      secondLevel,
                                      secondDelay,
                                      secondWidth, baseLevel)
        self.inputs[key] = pulsegen
        return pulsegen
                                               

    def _make_pulsegen(self, key, firstLevel, firstDelay, firstWidth=1e6, secondLevel=0, secondDelay=1e6, secondWidth=0, baseLevel=0):
        pulsegen = moose.PulseGen(self.model_container.path + '/' + key + '_input')
        pulsegen.firstLevel = firstLevel
        pulsegen.firstDelay = firstDelay
        pulsegen.firstWidth = firstWidth
        pulsegen.secondLevel = secondLevel
        pulsegen.secondDelay = secondDelay
        pulsegen.secondWidth = secondWidth
        pulsegen.baseLevel = baseLevel
        nrn = self._get_neuron(key)
        moose.connect(pulsegen, 'output', nrn, 'injectMsg')
        # self.stimulus_table = moose.Table(self.data_container.path + '/stimulus')
        # self.stimulus_table.connect('requestOut', pulsegen, 'getOutputValue')
        return pulsegen    
        
    def _make_Class_1_input(self):
        input_table = moose.StimulusTable(self.model_container.path + '/' + 'Class_1_input')
        input_table.stepSize = self.dt
        input_table.startTime = 30e-3 # The ramp starts at 30 ms
        input_table.stopTime = IzhikevichDemo.parameters['Class_1'][7] * 1e-3
        # matlab code: if (t>T1) I=(0.075*(t-T1)); else I=0;
        input_vec = np.arange(0, int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize)), 1.0) * 0.075 * self.dt * 1e3 * 1e-9
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron('Class_1'), 'injectMsg')
        self.stimulus_table = moose.Table(self.data_container.path + '/stimulus')
        moose.connect(input_table, 'output', self.stimulus_table, 'input')        
        return input_table

    def _make_Class_2_input(self):
        key = 'Class_2'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime = 30e-3 # The ramp starts at 30 ms
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        # The matlab code is: if (t>T1) I=-0.5+(0.015*(t-T1)); else I=-0.5
        # convert dt from s to ms, and convert total current from nA to A.
        input_vec = np.arange(0, int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize)), 1.0) * 0.015 * self.dt * 1e3 * 1e-9 - 0.05*1e-9
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron(key), 'injectMsg')
        return input_table

    def _make_bistable_input(self):
        key = 'bistable'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime =  0
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        t1 = IzhikevichDemo.parameters[key][7] * 1e-3/8
        t2 = 216e-3
        t = np.arange(0, 
                      int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize))) * self.dt
        input_vec = np.where(np.logical_or(np.logical_and(t > t1, t < t1+5e-3),
                                           np.logical_and(t > t2, t < t2+5e-3)),
                             1.24e-9,
                             0.24e-9)
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron(key), 'injectMsg')
        return input_table

    def _make_resonator_input(self):
        key = 'resonator'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime =  0
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        t1 = IzhikevichDemo.parameters[key][7] * 1e-3/10
        t2 = t1 + 20e-3
        t3 = 0.7 * IzhikevichDemo.parameters[key][7] * 1e-3
        t4 = t3 + 40e-3
        t = np.arange(0, int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize)), 1) * self.dt
        input_vec = np.zeros(t.shape)
        idx = np.nonzero(((t > t1) & (t < t1 + 4e-3)) |
                  ((t > t2) & (t < t2 + 4e-3)) |
                  ((t > t3) & (t < t3 + 4e-3)) |
                  ((t > t4) & (t < t4 + 4e-3)))[0]
        input_vec[idx] = 0.65e-9
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron(key), 'injectMsg')
        return input_table
        
    def _make_integrator_input(self):
        key = 'integrator'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime =  0
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        t1 = IzhikevichDemo.parameters[key][7] * 1e-3/11
        t2 = t1 + 5e-3
        t3 = 0.7 * IzhikevichDemo.parameters[key][7] * 1e-3
        t4 = t3 + 10e-3
        t = np.arange(0, int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize))) * self.dt
        input_vec = np.where(((t > t1) & (t < t1 + 2e-3)) |  
                             ((t > t2) & (t < t2 + 2e-3)) |
                             ((t > t3) & (t < t3 + 2e-3)) |
                             ((t > t4) & (t < t4 + 2e-3)),
                             9e-9, 
                             0.0)
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron(key), 'injectMsg')
        return input_table
        
    def _make_accommodation_input(self):
        key = 'accommodation'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime =  0
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        input_vec = np.zeros(int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize)))
        t = 0.0
        for ii in range(len(input_vec)):
            if t < 200e-3:
                input_vec[ii] = t * 1e-6/25
            elif t < 300e-3:
                input_vec[ii] = 0.0
            elif t < 312.5e-3:
                input_vec[ii] = 4e-6 * (t-300e-3)/12.5 
            else:
                input_vec[ii] = 0.0
            t = t + self.dt
        input_table.vector = input_vec
        input_table.connect('output', self._get_neuron(key), 'injectMsg')
        return input_table
        
    def _make_thresh_var_input(self):
        key = 'thresh_var'
        input_table = moose.StimulusTable(self.model_container.path + '/' + key + '_input')
        input_table.stepSize = self.dt
        input_table.startTime =  0
        input_table.stopTime = IzhikevichDemo.parameters[key][7] * 1e-3
        t = np.arange(0, int(ceil((input_table.stopTime - input_table.startTime) / input_table.stepSize)), 1) * self.dt
        input_vec = np.zeros(t.shape)
        input_vec[((t > 10e-3) & (t < 15e-3)) | ((t > 80e-3) & (t < 85e-3))] = 1e-9
        input_vec[(t > 70e-3) & (t < 75e-3)] = -6e-9
        input_table.vector = input_vec
        nrn = self._get_neuron(key)
        input_table.connect('output', nrn, 'injectMsg')
        return input_table

    def getEquation(self, key):
        params = IzhikevichDemo.parameters[key]
        if key != 'accommodation':
            equationText = "<i>v' = 0.04v^2 + 5v + 140 - u + I</i><br><i>u' = a(bv - u)</i><p>If <i>v >= 30 mV, v = c</i> and <i>u = u + d</i><br>where <i>a = %g</i>, <i>b = %g</i>, <i>c = %g</i> and <i>d = %g</i>."  % (params[1], params[2], params[3], params[4])
        else:
            equationText = "<i>v' = 0.04v^2 + 5v + 140 - u + I</i><br><i>u' = ab(v + 65)</i><p>If <i>v >= 30 mV, v = c</i> and <i>u = u + d</i><br>where <i>a = %g</i>, <i>b = %g</i>, <i>c = %g</i> and <i>d = %g</i>."  % (params[1], params[2], params[3], params[4])
        return equationText
        
import sys
try:
    from pylab import *
    if __name__ == '__main__':
        key = 'thresh_var'
        if len(sys.argv) > 1:
            key = sys.argv[1]
        demo = IzhikevichDemo()
        (t, Vm, Im) = demo.simulate(key)
        title(IzhikevichDemo.parameters[key][0] + '. ' + key)
        subplot(3,1,1)
        plot(t, Vm.vector)
        subplot(3,1,2)
        plot(t, Im.vector)
        subplot(3,1,3)
        show()
        print('Finished simulation.')
except ImportError:
    print('Matplotlib not installed.')

# 
# Izhikevich.py ends here
