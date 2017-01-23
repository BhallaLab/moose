
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
'''

## import modules and functions to be used
import numpy as np
import matplotlib.pyplot as plt
import random
import time
import moose

np.random.seed(100) # set seed for reproducibility of simulations
random.seed(100) # set seed for reproducibility of simulations
moose.seed(100) # set seed for reproducibility of simulations

#############################################
# All parameters as per:
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
N = 10000/red_fact      # Total number of neurons
fexc = 0.8              # Fraction of exc neurons
NE = int(fexc*N)        # Number of excitatory cells
NI = N-NE               # Number of inhibitory cells 

#############################################
# Simulation parameters
#############################################

simtime = 1200.0        #s # Simulation time
dt = 1e-3               #s # time step
plotDt = 1.0            #s # Time step for storing output.

#############################################
# Network parameters: synapses (not for ExcInhNetBase)
#############################################

## With each presynaptic spike in exc / inh neuron,
## J / -g*J is added to post-synaptic Vm -- delta-fn synapse
## Since LIF neuron used below is derived from Compartment class,
## conductance-based synapses (SynChan class) can also be used.

C = 500/red_fact        # Number of incoming connections on each neuron (exc or inh)
                        # 5% conn prob between any two neurons
                        # Since we reduced N from 10000 to 1000, C = 50 instead of 500
                        #  but we need to increase J by 10 to maintain total input per neuron
fC = fexc               # fraction fC incoming connections are exc, rest inhibitory
J = 0.2e-3 #V           # exc strength is J (in V as we add to voltage)
                        # Critical J is ~ 0.45e-3 V in paper for N = 10000, C = 1000
                        # See what happens for J = 0.2e-3 V versus J = 0.8e-3 V
J *= red_fact           # Multiply J by red_fact to compensate C/red_fact.
g = 4.0                 # -gJ is the inh strength. For exc-inh balance g >~ f(1-f)=4
syndelay = dt           # synaptic delay:
refrT = 0.0 # s         # absolute refractory time

#############################################
# Ca Plasticity parameters: synapses (not for ExcInhNetBase)
#############################################

CaPlasticity = True    # set it True or False to turn on/off plasticity
tauCa = 22.6936e-3      # s # Ca decay time scale
tauSyn = 346.3615       # s # synaptic plasticity time scale
## in vitro values in Higgins et al 2014, faster plasticity
CaPre = 0.56175         # mM
CaPost = 1.2964         # mM
## in vivo values in Higgins et al 2014, slower plasticity
#CaPre = 0.33705         # mM
#CaPost = 0.74378        # mM
delayD = 4.6098e-3      # s # CaPre is added to Ca after this delay
                        # proxy for rise-time of NMDA
thetaD = 1.0            # mM # depression threshold for Ca
thetaP = 1.3            # mM # potentiation threshold for Ca
gammaD = 331.909        # factor for depression term
gammaP = 725.085        # factor for potentiation term

eqWeight = 0.16          # initial synaptic weight
                        # gammaP/(gammaP+gammaD) = eq weight w/o noise
                        # but see eqn (22), noiseSD also appears

bistable = True        # if bistable is True, use bistable potential for weights
noisy = True           # use noisy weight updates given by noiseSD
noiseSD = 3.3501        # if noisy, use noiseSD (3.3501 from Higgins et al 2014)
#noiseSD = 0.1           # if bistable==False, use a smaller noise than in Higgins et al 2014

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
                        size=int(self.T)
                        ) # scale = SD
                self.noiseTables.vec[i].stepSize = 0    # use current time 
                                                        # as x value for interpolation
                self.noiseTables.vec[i].stopTime = self.simtime
        
        self._init_network(**kwargs)
        if plotif:
            self._init_plots()
        
        # moose simulation
        #moose.useClock( 1, '/network', 'process' )
        #moose.setClock( 0, dt )
        #moose.setClock( 1, dt )
        #moose.setClock( 2, dt )
        #moose.setClock( 3, dt )
        #moose.setClock( 9, dt )
        ## Do need to set the dt for MOOSE clocks
        for i in range(10):
            moose.setClock( i, dt )
        moose.setClock( 18, plotDt )
        t1 = time.time()
        print('reinit MOOSE -- takes a while ~20s.')
        moose.reinit()
        print(('reinit time t = ', time.time() - t1))
        t1 = time.time()
        print('starting')
        simadvance = self.simtime / 50.0
        for i in range( 50 ):
            moose.start( simadvance )
            print(('at t = ', i * simadvance, 'realtime = ', time.time() - t1))
        #moose.start(self.simtime)
        print(('runtime for ', self.simtime, 'sec, is t = ', time.time() - t1))

        if plotif:
            self._plot()

    def _init_plots(self):
        ## make a few tables to store a few Vm-s
        numVms = 10
        self.plots = moose.Table2( '/plotVms', numVms )
        ## draw numVms out of N neurons
        nrnIdxs = random.sample(list(range(self.N)),numVms)
        for i in range( numVms ):
            moose.connect( self.network.vec[nrnIdxs[i]], 'VmOut', \
                self.plots.vec[i], 'input')

        ## make self.N tables to store spikes of all neurons
        self.spikes = moose.Table2( '/plotSpikes', self.N )
        moose.connect( self.network, 'spikeOut', \
            self.spikes, 'input', 'OneToOne' )

        ## make 2 tables to store spikes of all exc and all inh neurons
        self.spikesExc = moose.Table2( '/plotSpikesAllExc' )
        for i in range(self.NmaxExc):
            moose.connect( self.network.vec[i], 'spikeOut', \
                self.spikesExc, 'input' )
        self.spikesInh = moose.Table2( '/plotSpikesAllInh' )
        for i in range(self.NmaxExc,self.N):
            moose.connect( self.network.vec[i], 'spikeOut', \
                self.spikesInh, 'input' )

    def _plot(self):
        """ plots the spike raster for the simulated net"""

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
        self.recN = 50 # number of neurons for which to record weights and Ca
        if CaPlasticity:
            ## make tables to store weights of recN exc synapses
            ## for each post-synaptic exc neuron
            self.weights = moose.Table2( '/plotWeights', self.excC*self.recN )
            for i in range(self.recN):      # range(self.N) is too large
                for j in range(self.excC):
                    moose.connect( self.weights.vec[self.excC*i+j], 'requestOut',
                        self.synsEE.vec[i*self.excC+j].synapse[0], 'getWeight')            
            self.CaTables = moose.Table2( '/plotCa', self.recN )
            for i in range(self.recN):      # range(self.N) is too large
                moose.connect( self.CaTables.vec[i], 'requestOut',
                        self.synsEE.vec[i*self.excC+j], 'getCa')            

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
        moose.useClock( 0, '/network/synsEE', 'process' )

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
        for i in range(0,self.NmaxExc):
            self.synsIE.vec[i].numSynapses = self.incC-self.excC

            ## Connections from some Exc neurons to each Exc neuron
            ## draw excC number of neuron indices out of NmaxExc neurons
            preIdxs = random.sample(list(range(self.NmaxExc)),self.excC)
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
                                self.J*2.0    # 0.2 mV, weight*weightScale is activation
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
                    if np.random.uniform()<0.05:
                        synij.weight = 1.0
                else:
                    synij.weight = self.J   # no weightScale here, activation = weight

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
            preIdxs = random.sample(list(range(self.NmaxExc,self.N)),self.incC-self.excC)
            ## connect these presynaptically to i-th post-synaptic neuron
            for synnum,preIdx in enumerate(preIdxs):
                synij = self.synsI.vec[i].synapse[ self.excC + synnum ]
                connectInhId = moose.connect( self.network.vec[preIdx], \
                    'spikeOut', synij, 'addSpike')
                synij.delay = syndelay
                synij.weight = -self.scaleI*self.J  # activation = weight

        moose.useClock( 0, '/network/synsIE', 'process' )
        moose.useClock( 0, '/network/synsI', 'process' )

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

    timeseries = net.trange
    ## individual neuron firing rates

    ## population firing rates

    ## Ca plasticity: weight vs time plots
    if CaPlasticity:
        ## Ca versus time in post-synaptic neurons
        for i in range(net.recN):      # range(net.N) is too large
            net.CaTables.vec[i].xplot( 'ca.xplot', 'Ca_' + str(i) )

        for i in range(net.recN):      # range(net.N) is too large
            for j in range(net.excC):
                k = net.excC*i+j
                net.weights.vec[k].xplot( 'wt.xplot', 'w_' + str(k) )

        ## all EE weights are used for a histogram
        weights = [ net.synsEE.vec[i*net.excC+j].synapse[0].weight \
                    for i in range(net.NmaxExc) for j in range(net.excC) ]
        histo, edges = np.histogram( weights, bins=100 )
        print()
        print(histo)
        print() 
        print(edges)
        print() 

        plt.figure()
        plt.hist(weights, bins=100)
        plt.title("Histogram of efficacies")
        plt.xlabel("Efficacy (arb)")
        plt.ylabel("# per bin")

if __name__=='__main__':
    ## ExcInhNetBase has unconnected neurons,
    ## ExcInhNet connects them
    ## Instantiate either ExcInhNetBase or ExcInhNet below
    #net = ExcInhNetBase(N=N)
    net = ExcInhNet(N=N)
    print(net)
    ## Important to distribute the initial Vm-s
    ## else weak coupling gives periodic synchronous firing
    net.simulate(simtime,plotif=True, v0=np.random.uniform(el-20e-3,vt,size=N))
    plt.figure()
    extra_plots(net)
    plt.show()
    
