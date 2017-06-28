
#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

'''
This LIF network is based on:
Ostojic, S. (2014).
Two types of asynchronous activity in networks of
excitatory and inhibitory spiking neurons.
Nat Neurosci 17, 594-600.
 
Key parameter to change is synaptic coupling J (mV).
Critical J is ~ 0.45e-3 V in paper for C/N = 0.1
See what happens for J = 0.2e-3 V versus J = 0.8e-3 V

Author: Aditya Gilra, NCBS, Bangalore, October, 2014.
'''

## import modules and functions to be used
import numpy as np
import matplotlib.pyplot as plt
import time
import moose

import random

np.random.seed(100) # set seed for reproducibility of simulations
random.seed(100) # set seed for reproducibility of simulations

#############################################
# Neuron model
#############################################

# equation: dv/dt = (1/taum)*(-(v-el)) + inp
# with spike when v>vt, reset to vr

el = -65e-3  #V        # Resting potential
vt = -45e-3  #V        # Spiking threshold
Rm = 20e6    #Ohm      # Only taum is needed, but LIF neuron accepts 
Cm = 1e-9    #F        # Rm and Cm and constructs taum=Rm*Cm
taum = Rm*Cm #s        # Membrane time constant is 20 ms in Ostojic 2014.
vr = -55e-3  #V        # Reset potential
inp = 20.1e-3/taum #V/s  # inp = Iinject/Cm to each neuron
                       # same as setting el=-41 mV and inp=0
Iinject = inp*Cm       # LIF neuron has injection current as param

#############################################
# Network parameters: numbers
#############################################

N = 1000          # Total number of neurons
fexc = 0.8        # Fraction of exc neurons
NE = int(fexc*N)  # Number of excitatory cells
NI = N-NE         # Number of inhibitory cells 

#############################################
# Simulation parameters
#############################################

simtime = 10.0    #s # Simulation time
dt = 1e-5         #s # time step

#############################################
# Network parameters: synapses (not for ExcInhNetBase)
#############################################

## With each presynaptic spike in exc / inh neuron,
## J / -g*J is added to post-synaptic Vm -- delta-fn synapse
## Since LIF neuron used below is derived from Compartment class,
## conductance-based synapses (SynChan class) can also be used.

C = 100           # Number of incoming connections on each neuron (exc or inh)
fC = fexc         # fraction fC incoming connections are exc, rest inhibitory
J = 0.8e-3 #V     # exc strength is J (in V as we add to voltage)
                  # Critical J is ~ 0.45e-3 V in paper for N = 10000, C = 1000
                  # See what happens for J = 0.2e-3 V versus J = 0.8e-3 V
g = 5.0           # -gJ is the inh strength. For exc-inh balance g >~ f(1-f)=4
syndelay = 0.5e-3 + dt # s     # synaptic delay:
                               # 0 ms gives similar result contrary to Ostojic?!
refrT = 0.5e-3    # s     # absolute refractory time -- 0 ms gives similar result

#############################################
# Exc-Inh network base class without connections
#############################################

class ExcInhNetBase:
    """Simulates and plots LIF neurons (exc and inh separate).
    Author: Aditya Gilra, NCBS, Bangalore, India, October 2014
    """
    
    def __init__(self,N=N,fexc=fexc,el=el,vt=vt,Rm=Rm,Cm=Cm,vr=vr,\
        refrT=refrT,Iinject=Iinject):
        """ Constructor of the class """
        
        self.N = N                 # Total number of neurons
        self.fexc = fexc           # Fraction of exc neurons
        self.NmaxExc = int(fexc*N) # max idx of exc neurons, rest inh

        self.el = el        # Resting potential
        self.vt = vt        # Spiking threshold
        self.taum = taum    # Membrane time constant
        self.vr = vr        # Reset potential
        self.refrT = refrT  # Absolute refractory period
        self.Rm = Rm        # Membrane resistance
        self.Cm = Cm        # Membrane capacitance
        self.Iinject = Iinject # constant input current
        
        self.simif = False  # whether the simulation is complete
        
        self._setup_network()

    def __str__(self):
         return "LIF network of %d neurons "\
             "having %d exc." % (self.N,self.NmaxExc)
    
    def _setup_network(self):
        """Sets up the network (_init_network is enough)"""
        self.network = moose.LIF( 'network', self.N );
        moose.le( '/network' )
        self.network.vec.Em = self.el
        self.network.vec.thresh = self.vt
        self.network.vec.refractoryPeriod = self.refrT
        self.network.vec.Rm = self.Rm
        self.network.vec.vReset = self.vr
        self.network.vec.Cm = self.Cm
        self.network.vec.inject = self.Iinject

    def _init_network(self,v0=el):
        """Initialises the network variables before simulation"""        
        self.network.vec.initVm = v0
        
    def simulate(self,simtime=simtime,dt=dt,plotif=False,**kwargs):
        
        self.dt = dt
        self.simtime = simtime
        self.T = np.ceil(simtime/dt)
        self.trange = np.arange(0,self.simtime+dt,dt)   
        
        self._init_network(**kwargs)
        if plotif:
            self._init_plots()
        
        # moose simulation
        # moose auto-schedules
        #moose.useClock( 0, '/network/syns', 'process' )
        #moose.useClock( 1, '/network', 'process' )
        #moose.useClock( 2, '/plotSpikes', 'process' )
        #moose.useClock( 3, '/plotVms', 'process' )
        #moose.useClock( 3, '/plotWeights', 'process' )
        for i in range(10):
            moose.setClock( i, dt )

        t1 = time.time()
        print('reinit MOOSE')
        moose.reinit()
        print(('reinit time t = ', time.time() - t1))
        t1 = time.time()
        print('starting')
        moose.start(self.simtime)
        print(('runtime, t = ', time.time() - t1))

        if plotif:
            self._plot()

    def _init_plots(self):
        ## make a few tables to store a few Vm-s
        numVms = 10
        self.plots = moose.Table( '/plotVms', numVms )
        ## draw numVms out of N neurons
        # not using random.sample() here since Brian version isn't
        #nrnIdxs = random.sample(range(self.N),numVms)
        nrnIdxs = list(range(self.N))
        for i in range( numVms ):
            moose.connect( self.network.vec[nrnIdxs[i]], 'VmOut', \
                self.plots.vec[i], 'input')

        ## make self.N tables to store spikes of all neurons
        self.spikes = moose.Table( '/plotSpikes', self.N )
        moose.connect( self.network, 'spikeOut', \
            self.spikes, 'input', 'OneToOne' )

        ## make 2 tables to store spikes of all exc and all inh neurons
        self.spikesExc = moose.Table( '/plotSpikesAllExc' )
        for i in range(self.NmaxExc):
            moose.connect( self.network.vec[i], 'spikeOut', \
                self.spikesExc, 'input' )
        self.spikesInh = moose.Table( '/plotSpikesAllInh' )
        for i in range(self.NmaxExc,self.N):
            moose.connect( self.network.vec[i], 'spikeOut', \
                self.spikesInh, 'input' )

    def _plot(self):
        """ plots the spike raster for the simulated net"""
        
        plt.figure()
        for i in range(0,self.NmaxExc):
            if i==0: label = 'Exc. spike trains'
            else: label = ''
            spikes = self.spikes.vec[i].vector
            plt.plot(spikes,[i]*len(spikes),\
                'b.',marker=',',label=label)
        for i in range(self.NmaxExc,self.N):
            if i==self.NmaxExc: label = 'Inh. spike trains'
            else: label = ''
            spikes = self.spikes.vec[i].vector
            plt.plot(spikes,[i]*len(spikes),\
                'r.',marker=',',label=label)           
        plt.xlabel('Time [ms]')
        plt.ylabel('Neuron number [#]')
        plt.xlim([0,self.simtime])
        plt.title("%s" % self, fontsize=14,fontweight='bold')
        plt.legend(loc='upper left')

#############################################
# Exc-Inh network class with connections (inherits from ExcInhNetBase)
#############################################

class ExcInhNet(ExcInhNetBase):
    """ Recurrent network simulation """
    
    def __init__(self,J=J,incC=C,fC=fC,scaleI=g,syndelay=syndelay,**kwargs):
        """Overloads base (parent) class"""
        self.J = J              # exc connection weight
        self.incC = incC        # number of incoming connections per neuron
        self.fC = fC            # fraction of exc incoming connections
        self.excC = int(fC*incC)# number of exc incoming connections
        self.scaleI = scaleI    # inh weight is scaleI*J
        self.syndelay = syndelay# synaptic delay
                 
        # call the parent class constructor
        ExcInhNetBase.__init__(self,**kwargs) 
    
    def __str__(self):
         return "LIF network of %d neurons "\
             "of which %d are exc." % (self.N,self.NmaxExc) 
 
    def _init_network(self,**args):
        ExcInhNetBase._init_network(self,**args)
        
    def _init_plots(self):
        ExcInhNetBase._init_plots(self)

    def _setup_network(self):
        ## Set up the neurons without connections
        ExcInhNetBase._setup_network(self)  

        ## Now, add in the connections...
        ## Each LIF neuron has one incoming synapse SimpleSynHandler,
        ##  which collects the activation from all presynaptic neurons
        ## Each pre-synaptic spike cause Vm of post-neuron to rise by
        ##  synaptic weight in one time step i.e. delta-fn synapse.
        ## Since LIF neuron is derived from Compartment class,
        ## conductance-based synapses (SynChan class) can also be used.
        self.syns = moose.SimpleSynHandler( '/network/syns', self.N );
        moose.connect( self.syns, 'activationOut', self.network, \
            'activation', 'OneToOne' )

        random.seed(100) # set seed for reproducibility of simulations
        ## Connections from some Exc/Inh neurons to each neuron
        for i in range(0,self.N):
            ## each neuron has incC number of synapses
            self.syns.vec[i].numSynapses = self.incC

            ## draw excC number of neuron indices out of NmaxExc neurons
            preIdxs = random.sample(list(range(self.NmaxExc)),self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.syns.vec[i].synapse[synnum]
                connectExcId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = self.J

            ## draw inhC=incC-excC number of neuron indices out of inhibitory neurons
            preIdxs = random.sample(list(range(self.NmaxExc,self.N)),self.incC-self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.syns.vec[i].synapse[self.excC+synnum]
                connectInhId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = -self.J*self.scaleI

#############################################
# Analysis functions
#############################################

def rate_from_spiketrain(spiketimes,fulltime,dt,tau=50e-3):
    """
    Returns a rate series of spiketimes convolved with a Gaussian kernel;
    all times must be in SI units.
    """
    sigma = tau/2.
    ## normalized Gaussian kernel, integral with dt is normed to 1
    ## to count as 1 spike smeared over a finite interval
    norm_factor = 1./(np.sqrt(2.*np.pi)*sigma)
    gauss_kernel = np.array([norm_factor*np.exp(-x**2/(2.*sigma**2))\
        for x in np.arange(-5.*sigma,5.*sigma+dt,dt)])
    kernel_len = len(gauss_kernel)
    ## need to accommodate half kernel_len on either side of fulltime
    rate_full = np.zeros(int(fulltime/dt)+kernel_len)
    for spiketime in spiketimes:
        idx = int(spiketime/dt)
        rate_full[idx:idx+kernel_len] += gauss_kernel
    ## only the middle fulltime part of the rate series
    ## This is already in Hz,
    ## since should have multiplied by dt for above convolution
    ## and divided by dt to get a rate, so effectively not doing either.
    return rate_full[kernel_len/2:kernel_len/2+int(fulltime/dt)]

#############################################
# Make plots
#############################################

def extra_plots(net):
    ## extra plots apart from the spike rasters
    ## individual neuron Vm-s
    plt.figure()
    tlen = len(net.plots.vec[0].vector)
    plt.plot(net.trange[:tlen],net.plots.vec[0].vector)
    plt.plot(net.trange[:tlen],net.plots.vec[1].vector)
    plt.plot(net.trange[:tlen],net.plots.vec[2].vector)
    plt.xlabel('time (s)')
    plt.ylabel('Vm (V)')
    plt.title("Vm-s of 3 LIF neurons (spike = reset).")

    timeseries = net.trange
    ## individual neuron firing rates
    fig = plt.figure()
    plt.subplot(221)
    num_to_plot = 10
    #rates = []
    for nrni in range(num_to_plot):
        rate = rate_from_spiketrain(\
            net.spikes.vec[nrni].vector,simtime,dt)
        plt.plot(timeseries[:len(rate)],rate)
    plt.title("Rates of "+str(num_to_plot)+" exc nrns")
    plt.ylabel("Hz")
    #plt.ylim(0,100)
    plt.subplot(222)
    for nrni in range(num_to_plot):
        rate = rate_from_spiketrain(\
            net.spikes.vec[net.NmaxExc+nrni].vector,simtime,dt)
        plt.plot(timeseries[:len(rate)],rate)
    plt.title("Rates of "+str(num_to_plot)+" inh nrns")
    #plt.ylim(0,100)

    ## population firing rates
    plt.subplot(223)
    allspikes = []
    for nrni in range(net.NmaxExc):
        allspikes.extend(net.spikes.vec[nrni].vector)
    #rate = rate_from_spiketrain(net.spikesExc.vector,simtime,dt)\
    #    /float(net.NmaxExc) # per neuron
    rate = rate_from_spiketrain(allspikes,simtime,dt)\
        /float(net.NmaxExc) # per neuron
    plt.plot(timeseries[:len(rate)],rate)
    #plt.ylim(0,100)
    plt.title("Exc population rate")
    plt.ylabel("Hz")
    plt.xlabel("Time (s)")
    plt.subplot(224)
    rate = rate_from_spiketrain(net.spikesInh.vector,simtime,dt)\
        /float(net.N-net.NmaxExc) # per neuron    
    plt.plot(timeseries[:len(rate)],rate)
    #plt.ylim(0,100)
    plt.title("Inh population rate")
    plt.xlabel("Time (s)")

    fig.tight_layout()
            
if __name__=='__main__':
    ## ExcInhNetBase has unconnected neurons,
    ## ExcInhNet connects them
    ## Instantiate either ExcInhNetBase or ExcInhNet below
    #net = ExcInhNetBase(N=N)
    net = ExcInhNet(N=N)
    print(net)
    ## Important to distribute the initial Vm-s
    ## else weak coupling gives periodic synchronous firing
    ## not distributing Vm-s randomly to ensure match with Brian data
    #net.simulate(simtime,plotif=True,\
    #    v0=np.random.uniform(el-20e-3,vt,size=N))
    net.simulate(simtime,plotif=True,\
        v0=np.linspace(el-20e-3,vt,N))

    extra_plots(net)
    plt.show()
    
