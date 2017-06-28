# traub_naf.py ---
#
# Filename: traub_naf.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Mon Apr 29 21:07:30 2013 (+0530)
# Version:
# Last-Updated: Mon May  6 18:50:14 2013 (+0530)
#           By: subha
#     Update #: 436
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

"""
This is an example showing pymoose implementation of the NaF
channel in Traub et al 2005

Author: Subhasis Ray

"""

import numpy as np
from matplotlib import pyplot as plt
import os
os.environ['NUMPTHREADS'] = '1'
import moose
from moose import utils

vmin = -120e-3
vmax = 40e-3
vdivs = 640
v_array = np.linspace(vmin, vmax, vdivs+1)

def create_naf_proto():
    """
    Create an NaF channel prototype in /library. You can copy it later
    into any compartment or load a .p file with this channel using
    loadModel.

    This channel has the conductance form::

        Gk(v) = Gbar * m^3 * h (V - Ek)

    We are using all SI units

    """
    if moose.exists('/library/NaF'):
        return moose.element('/library/NaF')
    if not moose.exists('/library'):
        lib = moose.Neutral('/library')
    channel = moose.HHChannel('/library/NaF')
    shift = -3.5e-3
    # tau_m is defined piecewise:
    # tau_m = 1.0e-3 * (0.025 + 0.14 * exp(( v + shift + 30e-3) / 10)) if v + shift < -30e-3
    #       = 1.0e-3 * (0.02 + 0.145 * np.exp(( - v_array - shift - 30.0e-3) / 10.0e-3)) otherwise
    tau_m = np.where((v_array + shift) < -30e-3,
                     1.0e-3 * (0.025 + 0.14 * np.exp((v_array + shift + 30.0e-3) / 10.0e-3)), \
                     1.0e-3 * (0.02 + 0.145 * np.exp(( - v_array - shift - 30.0e-3) / 10.0e-3)))
    inf_m = 1.0 / (1.0 + np.exp(( - v_array - shift - 38e-3) / 10e-3))
    tau_h = 1.0e-3 * (0.15 + 1.15 / ( 1.0 + np.exp(( v_array + 37.0e-3) / 15.0e-3)))
    inf_h = 1.0 / (1.0 + np.exp((v_array + 62.9e-3) / 10.7e-3))
    channel.Xpower = 3 # Creates m-gate
    # In svn version of moose you can even do:
    # mgate = channel.gateX[0]
    mgate = moose.element('%s/gateX' % (channel.path))
    mgate.tableA = inf_m / tau_m
    mgate.tableB = 1 / tau_m
    channel.Ypower = 1 # Creates h-gate
    hgate = moose.element('%s/gateY' % (channel.path))
    hgate.tableA = inf_h / tau_h
    hgate.tableB = 1 / tau_h
    return channel

def create_compartment(parent_path, name):
    """This shows how to use the prototype channel on a compartment."""
    comp = moose.Compartment('%s/%s' % (parent_path, name))
    comp.Rm = 5e6
    comp.Ra = 1e9
    comp.Cm = 5e-9
    comp.initVm = -0.06
    comp.Em = -0.06
    protochan = create_naf_proto()
    chan = moose.copy(protochan, comp, 'NaF')
    chan.Gbar = 1e-6
    moose.connect(comp, 'channel', chan, 'channel')
    return comp

def setup_electronics(model_container, data_container, compartment):
    """Setup voltage and current clamp circuit using DiffAmp and PID and
    RC filter"""
    command = moose.PulseGen('%s/command' % (model_container.path))
    command.delay[0] = 20e-3
    command.width[0] = 50e-3
    command.level[0] = 100e-9
    command.delay[1] = 1e9
    lowpass = moose.RC('%s/lowpass' % (model_container.path))
    lowpass.R = 1.0
    lowpass.C = 5e-4
    vclamp = moose.DiffAmp('%s/vclamp' % (model_container.path))
    vclamp.saturation = 1e10
    iclamp = moose.DiffAmp('%s/iclamp' % (model_container.path))
    iclamp.gain = 0.0
    iclamp.saturation = 1e10
    pid = moose.PIDController('%s/pid' % (model_container.path))
    pid.gain = compartment.Cm / 100e-6 # Cm/dt is a good number for gain
    pid.tauI = 100e-6 # same as dt
    pid.tauD = 0.0
    pid.saturation = 1e10
    # Current clamp circuit: connect command output to iclamp amplifier
    # and the output of the amplifier to compartment.
    moose.connect(command, 'output', iclamp, 'plusIn')
    moose.connect(iclamp, 'output', compartment, 'injectMsg')
    # Setup voltage clamp circuit:
    # 1. Connect command output (which is now command) to lowpass
    # filter.
    # 2. Connect lowpass output to vclamp amplifier.
    # 3. Connect amplifier output to PID's command input.
    # 4. Connect Vm of compartment to PID's sensed input.
    # 5. Connect PID output to compartment's injectMsg.
    moose.connect(command, 'output', lowpass, 'injectIn')
    moose.connect(lowpass, 'output', vclamp, 'plusIn')
    moose.connect(vclamp, 'output', pid, 'commandIn')
    moose.connect(compartment, 'VmOut', pid, 'sensedIn')
    moose.connect(pid, 'output', compartment, 'injectMsg')
    command_table = moose.Table('%s/command' % (data_container.path))
    moose.connect(command_table, 'requestOut', command, 'getOutputValue')
    inject_table = moose.Table('%s/inject' % (data_container.path))
    moose.connect(inject_table, 'requestOut', compartment, 'getIm')
    return {'command_tab': command_table,
            'inject_tab': inject_table,
            'iclamp': iclamp,
            'vclamp': vclamp,
            'pid': pid,
            'command': command}

def do_vclamp(vclamp, iclamp, pid):
    """Turn on voltage clamp and turn off current clamp"""
    vclamp.gain = 1.0
    iclamp.gain = 0.0
    pid.gain = 5e-9/100e-6  # compartment.Cm / simulation.dt

def do_iclamp(vclamp, iclamp, pid):
    """Turn on current clamp and turn off voltage clamp"""
    vclamp.gain = 0.0
    pid.gain = 0.0
    iclamp.gain = 1.0

def setup_model():
    """Setup the model and the electronic circuit. Also creates the data container."""
    model = moose.Neutral('model')
    data =moose.Neutral('/data')
    comp = create_compartment(model.path, 'soma')
    ret = setup_electronics(model, data, comp)
    vmtab = moose.Table('%s/Vm' % (data.path))
    moose.connect(vmtab, 'requestOut', comp, 'getVm')
    gktab = moose.Table('%s/Gk' % (data.path))
    moose.connect(gktab, 'requestOut', moose.element(comp.path + '/NaF'), 'getGk')
    ret.update({'model': model, 'data': data, 'vm_tab': vmtab, 'gk_tab': gktab})
    return ret

inited = False
def run_sim(model, data, simtime=100e-3, simdt=1e-6, plotdt=1e-4, solver='ee'):
    """Reset and run the simulation.

        model: model container element \n
        data: data container element \n
        simtime: simulation run time \n
        simdt: simulation timestep \n
        plotdt: plotting time step \n
        solver: neuronal solver to use \n

    """
    global inited
    if not inited:
        utils.resetSim([model.path, data.path], simdt, plotdt, simmethod=solver)
        inited = True
    else:
        moose.reinit()
    moose.start(simtime)

def run_clamp(model_dict, clamp, levels, holding=0.0, simtime=100e-3):
    """Run either voltage or current clamp for default timing settings
    with multiple levels of command input.

    model_dict: dictionary containing the model components - \n
        `vlcamp` - the voltage clamp amplifier \n
        `iclamp` - the current clamp amplifier \n
        `model` - the model container \n
        `data` - the data container \n
        `inject_tab` - table recording membrane \n
        `command_tab` - table recording command input for voltage or current clamp \n
        `vm_tab` - table recording membrane potential \n

    clamp: string specifying clamp mode, either `voltage` or `current` \n

    levels: sequence of values for command input levels to be simulated \n

    holding: holding current or voltage \n

    Returns:
    a dict containing the following lists of time series:

        `command` - list of  command input time series \n
        `inject` - list of of membrane current (includes injected current) time series \n
        `vm` - list of membrane voltage time series \n
        `t` - list of time points for all of the above

    """
    if clamp == 'voltage':
        do_vclamp(model_dict['vclamp'], model_dict['iclamp'], model_dict['pid'])
    elif clamp == 'current':
        do_iclamp(model_dict['vclamp'], model_dict['iclamp'], model_dict['pid'])
    else:
        raise Exception('Only allowed clamp options are `voltage` and `current`')
    cvec = []
    ivec = []
    vvec = []
    gvec = []
    tvec = []
    for level in levels:
        model_dict['command'].level[0] = level
        model_dict['command'].baseLevel = holding
        print(('Running %s with holding=%g, level=%g' % (clamp, holding, model_dict['command'].level[0])))
        run_sim(model_dict['model'], model_dict['data'], simtime)
        ivec.append(np.asarray(model_dict['inject_tab'].vector))
        cvec.append(np.asarray(model_dict['command_tab'].vector))
        vvec.append(np.asarray(model_dict['vm_tab'].vector))
        gvec.append(np.asarray(model_dict['gk_tab'].vector))
        tvec.append(np.linspace(0, simtime, len(vvec[-1])))
    return {'command': cvec,
            'inject': ivec,
            'vm': vvec,
            'gk': gvec,
            't': tvec}



if __name__ == '__main__':
    mdict = setup_model()
    current_levels = (-0.3e-8, 0.1e-8, 0.3e-8, 0.5e-8)
    iclamp_data = run_clamp(mdict, 'current', current_levels)
    voltage_levels = (-30e-3, -10e-3, 10e-3, 30e-3)
    vclamp_data = run_clamp(mdict, 'voltage', voltage_levels, holding=-60e-3)
    colors = ('r', 'g', 'b', 'k')
    # Plot current clamp data
    ifigure = plt.figure(1)
    ifigure.suptitle('Current clamp')
    vax = ifigure.add_subplot(2,2,1)
    vax.set_title('Vm')
    iax = ifigure.add_subplot(2,2,2)
    iax.set_title('Injected current')
    cax = ifigure.add_subplot(2,2,3)
    cax.set_title('Command')
    ivax = ifigure.add_subplot(2,2,4)
    for ii in range(len(current_levels)):
        t = iclamp_data['t'][ii]
        vax.plot(t, iclamp_data['vm'][ii], color=colors[ii % len(colors)])
        iax.plot(t, iclamp_data['inject'][ii], color=colors[ii % len(colors)])
        cax.plot(t, iclamp_data['command'][ii], color=colors[ii % len(colors)])
        ivax.plot(t, iclamp_data['gk'][ii], color=colors[ii% len(colors)])
    # Plot voltage clamp data
    vfigure = plt.figure(2)
    vfigure.suptitle('Voltage clamp')
    vax = vfigure.add_subplot(2,2,1)
    vax.set_title('Vm')
    iax = vfigure.add_subplot(2,2,2)
    iax.set_title('Injected current')
    cax = vfigure.add_subplot(2,2,3)
    cax.set_title('Command')
    ivax = vfigure.add_subplot(2,2,4)
    # iv = []
    for ii in range(len(voltage_levels)):
        t = vclamp_data['t'][ii]
        vax.plot(t, vclamp_data['vm'][ii], color=colors[ii % len(colors)])
        iax.plot(t, vclamp_data['inject'][ii], color=colors[ii % len(colors)])
        cax.plot(t, vclamp_data['command'][ii], color=colors[ii% len(colors)])
        ivax.plot(t, vclamp_data['gk'][ii], color=colors[ii% len(colors)])
        # Select the i and v at midpoint
    #     mid = max(vclamp_data['vm'][ii]) + 0.5 * min(vclamp_data['vm'][ii])
    #     sindices = np.argsort(vclamp_data['vm'][ii])
    #     sorted_vm = np.sort(vclamp_data['vm'][ii])
    #     mididx = np.searchsorted(sorted_vm, mid)
    #     mididx = sindices[mididx]
    #     iv.append((vclamp_data['inject'][ii][mididx], vclamp_data['vm'][ii][mididx]))
    # iv = np.array(iv)
    # ivax.plot(iv[:,0], iv[:,1], 'ko-')

    plt.show()

#
# traub_naf.py ends here
