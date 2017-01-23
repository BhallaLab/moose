
#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

'''
This LIF network with Ca plasticity is based on:
    David Higgins, Michael Graupner, Nicolas Brunel
    Memory Maintenance in Synapses with Calcium-Based
    Plasticity in the Presence of Background Activity
    PLOS Computational Biology, 2014.

Author: Aditya Gilra, NCBS, Bangalore, October, 2014.

This uses the Ca-based plasticity rule of:  
    Graupner, Michael, and Nicolas Brunel. 2012.
    Calcium-Based Plasticity Model Explains Sensitivity
     of Synaptic Changes to Spike Pattern, Rate, and Dendritic Location.
    Proceedings of the National Academy of Sciences, February, 201109359.
The Ca-based bistable synapse has been implemented as the 
    GraupnerBrunel2012CaPlasticitySynHandler MOOSE class.
'''

## import modules and functions to be used
import numpy as np
import matplotlib.pyplot as plt
import random
import time
import moose
import pickle
import os,sys

np.random.seed(100) # set seed for reproducibility of simulations
random.seed(100) # set seed for reproducibility of simulations
moose.seed(100) # set seed for reproducibility of simulations

#############################################
# All parameters as per (except N (/10), C (/10), J (*10), dt (*100)):
# David Higgins, Michael Graupner, Nicolas Brunel
#     Memory Maintenance in Synapses with Calcium-Based
#     Plasticity in the Presence of Background Activity
#     PLOS Computational Biology, 2014.
#############################################

#############################################
# Neuron model
#############################################

# equation: dv/dt = (1/taum)*(-(v-el)) + inp
# with spike when v>vt, reset to vr

el = -70e-3  #V         # Resting potential
vt = -50e-3  #V         # Spiking threshold
Rm = 20e6    #Ohm       # Only taum is needed, but LIF neuron accepts 
Cm = 1e-9    #F         # Rm and Cm and constructs taum=Rm*Cm
taum = Rm*Cm #s         # Membrane time constant is 20 ms
vr = -60e-3  #V         # Reset potential
Iinject = 11.5e-3/Rm    # constant current injection into LIF neuron
                        # same as setting el=-70+15=-55 mV and inp=0
noiseInj = True         # inject noisy current into each cell: boolean
noiseInjSD = 5e-3/Rm #A # SD of noise added to 'current'
                        # SD*sqrt(taum) is used as noise current SD

#############################################
# Network parameters: numbers
#############################################

red_fact = 10           # reduction factor for N,C,J
N = 10000/red_fact      # Total number of neurons # 10000 in paper
fexc = 0.8              # Fraction of exc neurons
NE = int(fexc*N)        # Number of excitatory cells
NI = N-NE               # Number of inhibitory cells 

#############################################
# Simulation parameters
#############################################

simtime = 30.0          #s # Simulation time
dt = 1e-3               #s # time step # 1e-5 in paper

#############################################
# Network parameters: synapses (not for ExcInhNetBase)
#############################################

## With each presynaptic spike in exc / inh neuron,
## J / -g*J is added to post-synaptic Vm -- delta-fn synapse
## Since LIF neuron used below is derived from Compartment class,
## conductance-based synapses (SynChan class) can also be used.

C = 500/red_fact  # Number of incoming connections on each neuron (exc or inh)
                  # 5% conn prob between any two neurons
                  # Since we reduced N from 10000 to 1000, C = 50 instead of 500
                  #  but we need to increase J by 10 to maintain total input per neuron
fC = fexc         # fraction fC incoming connections are exc, rest inhibitory
J = 0.1e-3 #V     # exc strength is J (in V as we add to voltage) # 0.1 in paper
                  # Critical J is ~ 0.45e-3 V in paper for N = 10000, C = 1000
                  # See what happens for J = 0.2e-3 V versus J = 0.8e-3 V
J *= red_fact     # Multiply J by red_fact to compensate C/red_fact.
g = 4.0           # -gJ is the inh strength. For exc-inh balance g >~ f(1-f)=4
syndelay = dt     # synaptic delay is same as time step
refrT = 0.0 # s   # absolute refractory time

#############################################
# Ca Plasticity parameters: synapses (not for ExcInhNetBase)
#############################################

CaPlasticity = True     # set it True or False to turn on/off plasticity
tauCa = 22.6936e-3      # s # Ca decay time scale
tauSyn = 346.3615       # s # synaptic plasticity time scale
## in vitro values in Higgins et al 2014, faster plasticity
CaPre = 0.56175         # mM
CaPost = 1.2964         # mM
## in vivo values in Higgins et al 2014, slower plasticity
#CaPre = 0.33705         # mM
#CaPost = 0.74378        # mM
### accelerated values compared to Higgins et al 2014, faster plasticity
#CaPre = 1.2             # mM
#CaPost = 2.0            # mM
delayD = 4.6098e-3      # s # CaPre is added to Ca after this delay
                        # proxy for rise-time of NMDA
thetaD = 1.0            # mM # depression threshold for Ca
thetaP = 1.3            # mM # potentiation threshold for Ca
gammaD = 331.909        # factor for depression term
gammaP = 725.085        # factor for potentiation term

#eqWeight = 0.5          # initial synaptic weight
                        # gammaP/(gammaP+gammaD) = eq weight w/o noise
                        # but see eqn (22), noiseSD also appears
eqWeight = 0.15         # from Fig 5 of Higgins et al, simulated, in vitro params

bistable = False       # if bistable is True, use bistable potential for weights
noisy = True           # use noisy weight updates given by noiseSD
noiseSD = 3.3501        # if noisy, use noiseSD (3.3501 from Higgins et al 2014)
#noiseSD = 0.1           # if bistable==False, use a smaller noise than in Higgins et al 2014

#############################################
# Exc-Inh network base class without connections
#############################################

class ExcInhNetBase:
    """Simulates and plots LIF neurons (exc and inh separate).
    """
    
    def __init__(self,N=N,fexc=fexc,el=el,vt=vt,Rm=Rm,Cm=Cm,vr=vr,\
        refrT=refrT,Iinject=Iinject):
        """ Constructor of the class """
        
        self.N = N                   # Total number of neurons
        self.fexc = fexc             # Fraction of exc neurons
        self.NmaxExc = int(fexc*N)   # max idx of exc neurons, rest inh

        self.el = el                 # Resting potential
        self.vt = vt                 # Spiking threshold
        self.taum = taum             # Membrane time constant
        self.vr = vr                 # Reset potential
        self.refrT = refrT           # Absolute refractory period
        self.Rm = Rm                 # Membrane resistance
        self.Cm = Cm                 # Membrane capacitance
        self.Iinject = Iinject       # constant input current
        self.noiseInjSD = noiseInjSD # SD of injected noise
        
        self.simif = False           # whether the simulation is complete
        
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
        if not noiseInj:
            self.network.vec.inject = self.Iinject
        else:
            ## inject a constant + noisy current
            ## values are set in self.simulate()
            self.noiseTables = moose.StimulusTable('noiseTables',self.N)
            moose.connect( self.noiseTables, 'output', \
                self.network, 'setInject', 'OneToOne')

    def _init_network(self,v0=el):
        """Initialises the network variables before simulation"""        
        self.network.vec.initVm = v0
        
    def simulate(self,simtime=simtime,dt=dt,plotif=False,**kwargs):
        
        self.dt = dt
        self.simtime = simtime
        self.T = np.ceil(simtime/dt)
        self.trange = np.arange(0,self.simtime,dt)   

        print("Noise injections being set ...")
        for i in range(self.N):
            if noiseInj:
                ## Gaussian white noise SD added every dt interval should be
                ## divided by sqrt(dt), as the later numerical integration
                ## will multiply it by dt.
                ## See the Euler-Maruyama method, numerical integration in 
                ## http://www.scholarpedia.org/article/Stochastic_dynamical_systems
                self.noiseTables.vec[i].vector = self.Iinject + \
                    np.random.normal( \
                        scale=self.noiseInjSD*np.sqrt(self.Rm*self.Cm/self.dt), \
                        size=self.T ) # scale = SD
                self.noiseTables.vec[i].stepSize = 0    # use current time 
                                                        # as x value for interpolation
                self.noiseTables.vec[i].stopTime = self.simtime
        
        print("init membrane potentials being set ... ")
        self._init_network(**kwargs)
        print("initializing plots ... ")
        if plotif:
            self._init_plots()
        
        ## MOOSE simulation
        
        ## MOOSE assigns clocks by default, no need to set manually
        #print "setting clocks ... "
        #moose.useClock( 1, '/network', 'process' )
        #moose.useClock( 2, '/plotSpikes', 'process' )
        #moose.useClock( 3, '/plotVms', 'process' )
        #if CaPlasticity:
        #    moose.useClock( 3, '/plotWeights', 'process' )
        #    moose.useClock( 3, '/plotCa', 'process' )
        ## Do need to set the dt for MOOSE clocks
        for i in range(10):
            moose.setClock( i, dt )

        t1 = time.time()
        print('reinit MOOSE -- takes a while ~20s.')
        moose.reinit()
        print(('reinit time t = ', time.time() - t1))
        t1 = time.time()
        print('starting run ...')
        moose.start(self.simtime)
        print(('runtime, t = ', time.time() - t1))

        if plotif:
            self._plot()

    def _init_plots(self):
        ## make a few tables to store a few Vm-s
        numVms = 10
        self.plots = moose.Table( '/plotVms', numVms )
        ## draw numVms out of N neurons
        nrnIdxs = random.sample(list(range(self.N)),numVms)
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
                'b.',marker='.',label=label)
        for i in range(self.NmaxExc,self.N):
            if i==self.NmaxExc: label = 'Inh. spike trains'
            else: label = ''
            spikes = self.spikes.vec[i].vector
            plt.plot(spikes,[i]*len(spikes),\
                'r.',marker='.',label=label)           
        plt.xlabel('Time (s)')
        plt.ylabel('Neuron number [#]')
        plt.xlim([0,self.simtime])
        plt.title("%s" % self, fontsize=14,fontweight='bold')
        plt.legend(loc='upper left')

#############################################
# Exc-Inh network class with Ca plasticity based connections
# (inherits from ExcInhNetBase)
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
        if CaPlasticity:
            self.recNCa = 5 # number of synapses for which to record Ca
                            #  as range(self.N) is too large
            self.recNwt = 20 # number of synapses for which to record weights

            ## make tables to store weights of recN exc synapses
            self.weightsEq = moose.Table( '/plotWeightsEq', self.recNwt )
            wtidx = 0
            for i in range(self.N):
                for j in range(self.excC):
                    if self.excC*i+j not in self.potSyns:
                        moose.connect( self.weightsEq.vec[wtidx], 'requestOut',
                            self.synsEE.vec[i*self.excC+j].synapse[0], 'getWeight')
                        wtidx += 1
                        if wtidx >= self.recNwt: break
                ## break only breaks out of one loop, hence repeated here!
                if wtidx >= self.recNwt: break

            self.weightsUp = moose.Table( '/plotWeightsUp', self.recNwt )
            for i in range(self.recNwt):      # range(self.N) is too large
                moose.connect( self.weightsUp.vec[i], 'requestOut',
                    self.synsEE.vec[self.potSyns[i]].synapse[0], 'getWeight')
                    
            self.CaTables = moose.Table( '/plotCa', self.recNCa )
            for i in range(self.recNCa):      # range(self.N) is too large
                moose.connect( self.CaTables.vec[i], 'requestOut',
                        self.synsEE.vec[i*self.excC], 'getCa')            

    def _setup_network(self):
        ## Set up the neurons without connections
        ExcInhNetBase._setup_network(self)  

        ## Now, add in the connections...        
        ## Each pre-synaptic spike cause Vm of post-neuron to rise by
        ##  synaptic weight in one time step i.e. delta-fn synapse.
        ## Since LIF neuron is derived from Compartment class,
        ## conductance-based synapses (SynChan class) can also be used.

        ## E to E synapses can be plastic
        ## Two ways to do this:
        ## 1) Each LIF neuron has one incoming postsynaptic SynHandler,
        ##  which collects the activation from all presynaptic neurons,
        ##  but then a common Ca pool is used.
        ## 2) Each LIF neuron has multiple postsyanptic SynHandlers,
        ##  one for each pre-synaptic neuron, i.e. one per synapse,
        ##  then each synapse has a different Ca pool.
        ## Here we go with option 2) as per Higgins et al 2014 (Brunel private email)
        ## separate SynHandler per EE synapse, thus NmaxExc*excC
        if CaPlasticity:
            self.synsEE = moose.GraupnerBrunel2012CaPlasticitySynHandler( \
                '/network/synsEE', self.NmaxExc*self.excC )
        else:
            self.synsEE = moose.SimpleSynHandler( \
                '/network/synsEE', self.NmaxExc*self.excC )
        #moose.useClock( 0, '/network/synsEE', 'process' )

        ## I to E synapses are not plastic
        self.synsIE = moose.SimpleSynHandler( '/network/synsIE', self.NmaxExc )
        ## all synapses to I neurons are not plastic
        self.synsI = moose.SimpleSynHandler( '/network/synsI', self.N-self.NmaxExc )
        ## connect all SynHandlers to their respective neurons
        for i in range(self.NmaxExc):
            moose.connect( self.synsIE.vec[i], 'activationOut', \
                self.network.vec[i], 'activation' )
        for i in range(self.NmaxExc,self.N):
            moose.connect( self.synsI.vec[i-self.NmaxExc], 'activationOut', \
                self.network.vec[i], 'activation' )

        ## Connections from some Exc/Inh neurons to each Exc neuron
        self.potSyns = []           # list of potentiated synapses
        for i in range(0,self.NmaxExc):
            self.synsIE.vec[i].numSynapses = self.incC-self.excC

            ## Connections from some Exc neurons to each Exc neuron
            ## draw excC number of neuron indices out of NmaxExc neurons
            prelist = list(range(self.NmaxExc))
            prelist.remove(i)       # disallow autapse
            preIdxs = random.sample(prelist,self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synidx = i*self.excC+synnum
                synHand = self.synsEE.vec[synidx]

                ## connect each synhandler to the post-synaptic neuron
                moose.connect( synHand, 'activationOut', \
                    self.network.vec[i], 'activation' )
                ## important to set numSynapses = 1 for each synHandler,
                ## doesn't create synapses if you set the full array of SynHandlers
                synHand.numSynapses = 1
                    
                synij = synHand.synapse[0]
                connectExcId = moose.connect( self.network.vec[preIdx], \
                                'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                if CaPlasticity:
                    ## set parameters for the Ca Plasticity SynHandler
                    ## have to be set for each SynHandler
                    ## doesn't set for full array at a time
                    synHand.CaInit = 0.0
                    synHand.tauCa = tauCa
                    synHand.tauSyn = tauSyn
                    synHand.CaPre = CaPre
                    synHand.CaPost = CaPost
                    synHand.delayD = delayD
                    synHand.thetaD = thetaD
                    synHand.thetaP = thetaP
                    synHand.gammaD = gammaD
                    synHand.gammaP = gammaP
                    synHand.weightMax = 1.0 # bounds on the weight
                    synHand.weightMin = 0.0
                    synHand.weightScale = \
                                self.J*2.0  # 0.2 mV, weight*weightScale is activation
                                            # typically weight <~ 0.5, so activation <~ J
                    synHand.noisy = noisy
                    synHand.noiseSD = noiseSD
                    synHand.bistable = bistable

                    moose.connect( self.network.vec[i], \
                        'spikeOut', synHand, 'addPostSpike')
                    synij.weight = eqWeight # activation = weight*weightScale
                                            # weightScale = 2*J
                                            # weight <~ 0.5
                    ## Randomly set 5% of them to be 1.0
                    ##  for Fig 5 of paper
                    if np.random.uniform()<0.05:
                        synij.weight = 1.0
                        self.potSyns.append(synidx)
                else:
                    synij.weight = self.J   # no weightScale if not plastic, activation = weight

            ## Connections from some Inh neurons to each Exc neuron
            ## draw inhC=incC-excC number of neuron indices out of inhibitory neurons
            preIdxs = random.sample(list(range(self.NmaxExc,self.N)),self.incC-self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.synsIE.vec[i].synapse[synnum]
                connectInhId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = -self.scaleI*self.J # activation = weight

        ## Connections from some Exc/Inh neurons to each Inh neuron
        for i in range(self.N-self.NmaxExc):
            ## each neuron has incC number of synapses
            self.synsI.vec[i].numSynapses = self.incC

            ## draw excC number of neuron indices out of NmaxExc neurons
            preIdxs = random.sample(list(range(self.NmaxExc)),self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.synsI.vec[i].synapse[synnum]
                connectExcId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = self.J   # activation = weight

            ## draw inhC=incC-excC number of neuron indices out of inhibitory neurons
            prelist = list(range(self.NmaxExc,self.N))
            prelist.remove(i+self.NmaxExc)      # disallow autapse
            preIdxs = random.sample(prelist,self.incC-self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.synsI.vec[i].synapse[ self.excC + synnum ]
                connectInhId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = -self.scaleI*self.J  # activation = weight

        #moose.useClock( 0, '/network/synsIE', 'process' )
        #moose.useClock( 0, '/network/synsI', 'process' )

#############################################
# Analysis functions
#############################################

def rate_from_spiketrain(spiketimes,fulltime,dt,tau=200e-3):
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
# Make plots, save data
#############################################

def save_data(net):
    f = open("fig5_data.pickle", "wb")
    timeseries = net.trange
    pickle.dump((timeseries,simtime,dt),f)
    pickle.dump((net.N,net.NmaxExc),f)
    for nrni in range(net.N):
        pickle.dump(net.spikes.vec[nrni].vector,f)
    pickle.dump(net.spikesExc.vector,f)
    pickle.dump(net.spikesInh.vector,f)
    if CaPlasticity:
        pickle.dump(net.recNCa,f)
        for i in range(net.recNCa):
            pickle.dump(net.CaTables.vec[i].vector[:len(timeseries)],f)
        pickle.dump(net.recNwt,f)
        for i,wtarray in enumerate(net.weightsEq.vec):
            pickle.dump(wtarray.vector[:len(timeseries)],f)
        for i,wtarray in enumerate(net.weightsUp.vec):
            pickle.dump(wtarray.vector[:len(timeseries)],f)
        ## all EE weights are used for a histogram
        weights = [ net.synsEE.vec[i*net.excC+j].synapse[0].weight \
                    for i in range(net.NmaxExc) for j in range(net.excC) ]    
        pickle.dump(weights,f)
    f.close()

####### figure defaults
label_fontsize = 8 # pt
plot_linewidth = 0.5 # pt
linewidth = 1.0#0.5
axes_linewidth = 0.5
marker_size = 3.0 # markersize=<...>
cap_size = 2.0 # for errorbar caps, capsize=<...>
columnwidth = 85/25.4 # inches
twocolumnwidth = 174/25.4 # inches
linfig_height = columnwidth*2.0/3.0
fig_dpi = 300

def set_tick_widths(ax,tick_width):
    for tick in ax.xaxis.get_major_ticks():
        tick.tick1line.set_markeredgewidth(tick_width)
        tick.tick2line.set_markeredgewidth(tick_width)
    for tick in ax.xaxis.get_minor_ticks():
        tick.tick1line.set_markeredgewidth(tick_width)
        tick.tick2line.set_markeredgewidth(tick_width)
    for tick in ax.yaxis.get_major_ticks():
        tick.tick1line.set_markeredgewidth(tick_width)
        tick.tick2line.set_markeredgewidth(tick_width)
    for tick in ax.yaxis.get_minor_ticks():
        tick.tick1line.set_markeredgewidth(tick_width)
        tick.tick2line.set_markeredgewidth(tick_width)

def axes_labels(ax,xtext,ytext,adjustpos=False,\
                    fontsize=label_fontsize,xpad=None,ypad=None):
    ax.set_xlabel(xtext,fontsize=fontsize,labelpad=xpad)
    # increase xticks text sizes
    for label in ax.get_xticklabels():
        label.set_fontsize(fontsize)
    ax.set_ylabel(ytext,fontsize=fontsize,labelpad=ypad)
    # increase yticks text sizes
    for label in ax.get_yticklabels():
        label.set_fontsize(fontsize)
    if adjustpos:
        ## [left,bottom,width,height]
        ax.set_position([0.135,0.125,0.84,0.75])
    set_tick_widths(ax,axes_linewidth)

def biglegend(legendlocation='upper right',ax=None,\
                    fontsize=label_fontsize, **kwargs):
    if ax is not None:
        leg=ax.legend(loc=legendlocation, **kwargs)
    else:
        leg=plt.legend(loc=legendlocation, **kwargs)
    # increase legend text sizes
    for t in leg.get_texts():
        t.set_fontsize(fontsize)

def load_plot_Fig5():
    if os.path.isfile("fig5_data.pickle"):
        f = open("fig5_data.pickle", "rb")
    else:
        print("You need to simulate first before loading data file.")
        print("re-run with sim as a command line argument.")
        sys.exit()
    fig = plt.figure(facecolor="w",\
            figsize=(columnwidth,linfig_height),dpi=fig_dpi)
    timeseries,simtime,dt = pickle.load(f)

    ## population firing rates
    N,NmaxExc = pickle.load(f)
    ax = plt.subplot(211)
    for nrni in range(N):
        strain = pickle.load(f)
        #plt.plot(strain,[nrni]*len(strain),'.')
    strainExc = pickle.load(f)
    rate = rate_from_spiketrain(np.array(strainExc),simtime,dt)\
            /float(NmaxExc) # per neuron
    plt.plot(timeseries/60,rate,label="exc",linewidth=plot_linewidth)
    strainInh = pickle.load(f)
    rate = rate_from_spiketrain(np.array(strainInh),simtime,dt)\
            /float(N-NmaxExc) # per neuron
    plt.plot(timeseries/60,rate,label="inh",linewidth=plot_linewidth)
    #biglegend()
    plt.ylim(0,2)
    plt.xticks([])
    axes_labels(ax,"","mean rate (Hz)")
        
    if CaPlasticity:
        NCa = pickle.load(f)
        #plt.subplot(312)
        caconcs = []
        for i in range(NCa):
            caconcs.append(pickle.load(f))
        #plt.plot(timeseries/60,np.mean(caconcs,axis=0))

        ax = plt.subplot(212)
        Nwt = pickle.load(f)
        wtarrayseq = []
        for i in range(Nwt):
            wtarray = pickle.load(f)
            wtarrayseq.append(wtarray)
            plt.plot(timeseries/60,wtarray,color='#ffaaaa',\
                                linewidth=plot_linewidth)

        wtarraysup = []
        for i in range(Nwt):
            wtarray = pickle.load(f)
            wtarraysup.append(wtarray)
            plt.plot(timeseries/60,wtarray,color='#aaaaff',\
                                linewidth=plot_linewidth)
        plt.plot(timeseries/60,np.mean(wtarrayseq,axis=0),color='r',\
                                linewidth=plot_linewidth)
        plt.plot(timeseries/60,np.mean(wtarraysup,axis=0),color='b',\
                                linewidth=plot_linewidth)
        #plt.title("Evolution of efficacies",fontsize=label_fontsize)
        axes_labels(ax,"Time (min)","Efficacy")

        #plt.subplot(133)
        ### all EE weights are used for a histogram
        weights = pickle.load(f)  
        #plt.hist(weights, bins=100)
        #plt.title("Histogram of efficacies")
        #plt.xlabel("Efficacy (arb)")
        #plt.ylabel("# per bin")

    fig.tight_layout()
    # plt.show( )
    # f.close()
    # fig.savefig("HGB2014_Fig5ab_MOOSE.tif",dpi=fig_dpi)

def extra_plots(net):
    ## extra plots apart from the spike rasters
    ## individual neuron Vm-s
    plt.figure()
    plt.plot(net.trange,net.plots.vec[0].vector[0:len(net.trange)])
    plt.plot(net.trange,net.plots.vec[1].vector[0:len(net.trange)])
    plt.plot(net.trange,net.plots.vec[2].vector[0:len(net.trange)])
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
        plt.plot(timeseries,rate)
    plt.title("Rates of "+str(num_to_plot)+" exc nrns")
    plt.ylabel("Hz")
    plt.ylim(0,100)
    plt.subplot(222)
    for nrni in range(num_to_plot):
        rate = rate_from_spiketrain(\
            net.spikes.vec[net.NmaxExc+nrni].vector,simtime,dt)
        plt.plot(timeseries,rate)
    plt.title("Rates of "+str(num_to_plot)+" inh nrns")
    plt.ylim(0,100)

    ## population firing rates
    plt.subplot(223)
    rate = rate_from_spiketrain(net.spikesExc.vector,simtime,dt)\
        /float(net.NmaxExc) # per neuron
    plt.plot(timeseries,rate)
    plt.ylim(0,100)
    plt.title("Exc population rate")
    plt.ylabel("Hz")
    plt.xlabel("Time (s)")
    plt.subplot(224)
    rate = rate_from_spiketrain(net.spikesInh.vector,simtime,dt)\
        /float(net.N-net.NmaxExc) # per neuron    
    plt.plot(timeseries,rate)
    plt.ylim(0,100)
    plt.title("Inh population rate")
    plt.xlabel("Time (s)")

    fig.tight_layout()

    ## Ca plasticity: weight vs time plots
    if CaPlasticity:
        ## Ca versus time in post-synaptic neurons
        plt.figure()
        for i in range(net.recNCa):      # range(net.N) is too large
            plt.plot(timeseries,\
                net.CaTables.vec[i].vector[:len(timeseries)])
        plt.title("Evolution of Ca in some neurons")
        plt.xlabel("Time (s)")
        plt.ylabel("Ca (mM)")

        plt.figure()
        wtarrays = np.zeros((len(timeseries),net.recNwt))
        for i,wtarray in enumerate(net.weightsEq.vec):
            wtarrays[:,i] = wtarray.vector[:len(timeseries)]
            plt.plot(timeseries,wtarrays[:,i],color='r',alpha=0.2)
        plt.plot(timeseries,np.mean(wtarrays,axis=1),color='r')
        for i,wtarray in enumerate(net.weightsUp.vec):
            wtarrays[:,i] = wtarray.vector[:len(timeseries)]
            plt.plot(timeseries,wtarrays[:,i],color='b',alpha=0.2)
        plt.plot(timeseries,np.mean(wtarrays,axis=1),color='b')
        plt.title("Evolution of some efficacies")
        plt.xlabel("Time (s)")
        plt.ylabel("Efficacy")

        ## all EE weights are used for a histogram
        weights = [ net.synsEE.vec[i*net.excC+j].synapse[0].weight \
                    for i in range(net.NmaxExc) for j in range(net.excC) ]
        plt.figure()
        plt.hist(weights, bins=100)
        plt.title("Histogram of efficacies")
        plt.xlabel("Efficacy (arb)")
        plt.ylabel("# per bin")

if __name__=='__main__':
    if 'sim' in sys.argv:
        ## ExcInhNetBase has unconnected neurons,
        ## ExcInhNet connects them
        ## Instantiate either ExcInhNetBase or ExcInhNet below
        #net = ExcInhNetBase(N=N)
        net = ExcInhNet(N=N)
        print(net)
        ## Important to distribute the initial Vm-s
        ## else weak coupling gives periodic synchronous firing
        print("Preparing to simulate ... ")
        net.simulate(simtime,plotif=True,\
            v0=np.random.uniform(el-10e-3,vt+1e-3,size=N))

        save_data(net)
        #extra_plots(net)
    else:
        print("just plotting old results for Fig 5.")
        print("To simulate and save, give sim as commandline argument.")
        load_plot_Fig5()
        plt.show()
