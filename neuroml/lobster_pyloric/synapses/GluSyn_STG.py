
# -*- coding: utf-8 -*-
import sys
import math

from pylab import *

try:
    import moose
except ImportError:
    print("ERROR: Could not import moose. Please add the directory containing moose.py in your PYTHONPATH")
    import sys
    sys.exit(1)

from moose.utils import * # for BSplineFill

class GluSyn_STG(moose.SynChan):
    """Glutamate graded synapse"""
    def __init__(self, *args):
        moose.SynChan.__init__(self,*args)
        self.Ek = -70e-3 # V
        # For event based synapses, I had a strength of 5e-6 S
        #    to compensate for event-based,
        # but for the original graded synapses, 5e-9 S is correct.
        self.Gbar = 5e-9 # S # set weight on connecting the network
        self.tau1 = 40e-3 # s # this is Vpre dependent (see below)
        self.tau2 = 0.0 # single first order equation

        Vth = -35e-3 # V
        Delta = 5e-3 # V
        ######## Graded synapse activation
        inhsyntable = moose.Interpol(self.path+"/graded_table")
        graded = moose.Mstring(self.path+'/graded')
        graded.value = 'True'
        mgblock = moose.Mstring(self.path+'/mgblockStr')
        mgblock.value = 'False'
        # also needs a synhandler
        moosesynhandler = moose.SimpleSynHandler(self.path+'/handler')
        # connect the SimpleSynHandler to the SynChan (double exp)
        moose.connect( moosesynhandler, 'activationOut', self, 'activation' )
        
        # ds/dt = s_inf/tau - s/tau = A - Bs
        # where A=s_inf/tau is activation, B is 1/tau
        # Fill up the activation and tau tables
        # Graded synapse tau
        inhtautable = moose.Interpol(self.path+"/tau_table")
        inhtautable.xmin = -70e-3 # V
        inhtautable.xmax = 0e-3 # V
        tau = [self.tau1] # at -70 mV
        tau.extend( [self.tau1*(1. - 1./(1+math.exp((Vth-vm)/Delta))) \
            for vm in arange(-70e-3,0.00001e-3,70e-3/1000.)] )
        inhtautable.vector = array(tau)
        inhtautable.connect("lookupOut",self,"setTau1")

        # Graded synapse activation
        inhsyntable.xmin = -70e-3 # V
        inhsyntable.xmax = 0e-3 # V
        act = [0.0] # at -70 mV
        act.extend( [1/(1+math.exp((Vth-vm)/Delta)) \
            for vm in arange(-70e-3,0.00001e-3,70e-3/1000.)] )
        act = array(act) / array(tau) # element-wise division # NOTE: A = s_inf/tau
        inhsyntable.vector = array(act)
        inhsyntable.connect("lookupOut",self,"activation")
