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

Implemented by: Aditya Gilra, NCBS, Bangalore, October, 2014.
This variant has 2500 LIF neurons

Upi Bhalla, Nov 2014: Appended single neuron model.
This script generates the panels in Figure 6. It takes a long time,
about 65 minutes to run 30 seconds of simulation time.

'''

from __future__ import print_function

## import modules and functions to be used
import numpy as np
import matplotlib.pyplot as plt
import random
import time
import moose
from PyQt4 import Qt, QtCore, QtGui
from numpy import random as nprand
from moose.neuroml.NeuroML import NeuroML
import sys
import rdesigneur as rd
import moogli
cellname = "./cells_channels/CA1_nochans.morph.xml"
fname = "fig6bcde"

#############################################
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

PI = 3.14159265358979
useGssa = True
combineSegments = False

el = -70e-3  #V         # Resting potential
vt = -50e-3  #V         # Spiking threshold
Rm = 20e6    #Ohm       # Only taum is needed, but LIF neuron accepts 
Cm = 1e-9    #F         # Rm and Cm and constructs taum=Rm*Cm
taum = Rm*Cm #s         # Membrane time constant is 20 ms
vr = -60e-3  #V         # Reset potential
Iinject = 10e-3/Rm      # constant current injection into LIF neuron
                        # same as setting el=-70+15=-55 mV and inp=0
noiseInj = True         # inject noisy current into each cell: boolean
noiseInjSD = 5e-3/Rm #A # SD of noise added to 'current'
                        # SD*sqrt(taum) is used as noise current SD

#############################################
# Network parameters: numbers
#############################################

N = 2500          # Total number of neurons
fexc = 0.8        # Fraction of exc neurons
NE = int(fexc*N)  # Number of excitatory cells
NI = N-NE         # Number of inhibitory cells 

#############################################
# Simulation parameters
#############################################

simtime = 30     #s # Simulation time
interTetInterval = 5.0 # sec
updateDt = 0.2	  #s: time to update live display
dt = 1e-3         #s # time step

#############################################
# Network parameters: synapses (not for ExcInhNetBase)
#############################################

## With each presynaptic spike in exc / inh neuron,
## J / -g*J is added to post-synaptic Vm -- delta-fn synapse
## Since LIF neuron used below is derived from Compartment class,
## conductance-based synapses (SynChan class) can also be used.

C = 100           # Number of incoming connections on each neuron (exc or inh)
fC = fexc         # fraction fC incoming connections are exc, rest inhibitory
J = 0.2e-3 #V     # exc strength is J (in V as we add to voltage)
                  # Critical J is ~ 0.45e-3 V in paper for N = 10000, C = 1000
                  # See what happens for J = 0.2e-3 V versus J = 0.8e-3 V
g = 4.0           # -gJ is the inh strength. For exc-inh balance g >~ f(1-f)=4
syndelay = dt     # synaptic delay:
refrT = 0.0 # s   # absolute refractory time

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

eqWeight = 0.5          # initial synaptic weight
                        # gammaP/(gammaP+gammaD) = eq weight w/o noise
                        # but see eqn (22), noiseSD also appears

bistable = True        # if bistable is True, use bistable potential for weights
noisy = True           # use noisy weight updates given by noiseSD
noiseSD = 3.3501        # if noisy, use noiseSD (3.3501 from Higgins et al 2014)
#noiseSD = 0.1           # if bistable==False, use a smaller noise than in Higgins et al 2014

#############################################
# Here we set up a single neuron to fit in this network
#############################################

diffDt = 0.005
chemDt = 0.005
ePlotDt = 0.5e-3
cPlotDt = 0.005

#############################################
def buildRdesigneur():
    ##################################################################
    # Here we define which prototypes are to be loaded in to the system.
    # Each specification has the format
    # source [localName]
    # source can be any of
    # filename.extension,   # Identify type of file by extension, load it.
    # function(),           # func( name ) builds object of specified name
    # file.py:function() ,  # load Python file, run function(name) in it.
    # moose.Classname       # Make obj moose.Classname, assign to name.
    # path                  # Already loaded into library or on path.
    # After loading the prototypes, there should be an object called 'name'
    # in the library.
    ##################################################################
    cellProto = [ [cellname, 'elec'] ]
    chanProto = [
        ['./cells_channels/hd.xml'], \
        ['./cells_channels/kap.xml'], \
        ['./cells_channels/kad.xml'], \
        ['./cells_channels/kdr.xml'], \
        ['./cells_channels/na3.xml'], \
        ['./cells_channels/nax.xml'], \
        ['./cells_channels/CaConc.xml'], \
        ['./cells_channels/Ca.xml'], \
        ['./cells_channels/NMDA.xml'], \
        ['./cells_channels/Glu.xml'], \
        ['./cells_channels/GABA.xml'] \
    ]
    spineProto = [ \
        ['makeSpineProto()', 'spine' ]
    ]
    chemProto = [ \
        [ 'psd53.g', 'ltpModel'] \
    ]

    ##################################################################
    # Here we define what goes where, and any parameters. Each distribution
    # has the format
    # protoName, path, field, expr, [field, expr]...
    # where 
    #   protoName identifies the prototype to be placed on the cell
    #   path is a MOOSE wildcard path specifying where to put things
    #   field is the field to assign.
    #   expr is a math expression to define field value. This uses the
    #     muParser. Built-in variables are p, g, L, len, dia.
    #     The muParser provides most math functions, and the Heaviside 
    #     function H(x) = 1 for x > 0 is also provided.
    ##################################################################
    passiveDistrib = [ 
            [ ".", "#", "RM", "2.8", "CM", "0.01", "RA", "1.5",  \
                "Em", "-58e-3", "initVm", "-65e-3" ], \
            [ ".", "#axon#", "RA", "0.5" ] \
        ]
    chanDistrib = [ \
            ["hd", "#dend#,#apical#,#user#", "Gbar", "5e-2*(1+(p*3e4))" ], \
            ["kdr", "#", "Gbar", "100" ], \
            ["na3", "#soma#,#dend#,#apical#,#user#", "Gbar", "250" ], \
            ["nax", "#axon#", "Gbar", "1250" ], \
            ["nax", "#soma#", "Gbar", "100" ], \
            ["kap", "#axon#,#soma#", "Gbar", "300" ], \
            ["kap", "#dend#,#apical#,#user#", "Gbar", \
                "300*(H(100-p*1e6)) * (1+(p*1e4))" ], \
            ["Ca_conc", "#soma#,#dend#,#apical#,#user#", "tau", "0.0133" ], \
            ["kad", "#dend#,#apical#,#user#", "Gbar", \
                "300*H(p*1e6-100)*(1+p*1e4)" ], \
            ["Ca", "#soma#", "Gbar", "10e-3" ], \
            ["Ca", "#dend#,#apical#,#user#", "Gbar", "50e-3" ], \
            ["GABA", "#dend#,#apical#,#user#", "Gbar", "100*H(250e-6 - p)" ], \
        ]
    spineDistrib = [ \
            ["spine", '#apical#,#dend#,#user#', "spineSpacing", "6.2e-6", \
                "spineSpacingDistrib", "1e-6", \
                "angle", "0", \
                "angleDistrib", str( 2*PI ), \
                "size", "1", \
                "sizeDistrib", "0.5" ] \
        ]
    chemDistrib = [ \
            [ "ltpModel", "#apical#,#dend#,#user#", "install", "1" ] \
        ]

    '''
    '''
    ######################################################################
    # Here we define the mappings across scales. Format:
    # sourceObj sourceField destObj destField couplingExpr [wildcard][spatialExpn]
    # where the coupling expression is anything a muParser can evaluate,
    # using the input variable x. For example: 8e-5 + 300*x
    # For now, let's use existing adaptors which take an offset and scale.
    ######################################################################
    adaptorList = [
        [ 'Ca_conc', 'Ca', 'psd/Ca_input', 'concInit', 8e-5, 1 ],
        [ 'Ca_conc', 'Ca', 'dend/Ca_dend_input', 'concInit', 8e-5, 1 ],
        [ 'psd/tot_PSD_R', 'n', 'glu', 'modulation', 0.5, 0.002 ],
    ]

    ######################################################################
    # Having defined everything, now to create the rdesigneur and proceed
    # with creating the model.
    ######################################################################
    

    rdes = rd.rdesigneur(
        useGssa = useGssa, \
        combineSegments = combineSegments, \
        stealCellFromLibrary = True, \
        passiveDistrib = passiveDistrib, \
        spineDistrib = spineDistrib, \
        chanDistrib = chanDistrib, \
        chemDistrib = chemDistrib, \
        cellProto = cellProto, \
        spineProto = spineProto, \
        chanProto = chanProto, \
        chemProto = chemProto, \
        adaptorList = adaptorList
    )

    return rdes


#############################################

def makeDetailedNeuron():
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    #bcs.addAllPlots()
    
def connectDetailedNeuron():
    excProb = 0.00042
    excSeed = 1234
    inhProb = 0.00013
    inhSeed = 4567
    numExc = 0
    numNMDA = 0
    numInh = 0
    delayMax = 0.010
    delayMin = 0.002
    excWeightMax = 360
    nmdaWeightMax = 1
    inhWeightMax = 100
    # Note we use the same seed for all 3 exc connections, to make sure
    # they are all equivalent.
    seed = excSeed
    totGluWt = 0.0
    totNMDAWt = 0.0
    totGABAWt = 0.0
    for x in moose.wildcardFind( '/model/elec/#/glu/##[ISA=Synapse]' ):
        exc = moose.connect( '/network', 'spikeOut', x, 'addSpike','sparse')
        exc.setRandomConnectivity( excProb, seed )
        seed = seed + 1
        if exc.numEntries > 0:
            numExc += exc.numEntries
            assert( exc.numEntries == x.numField )
            x.vec.delay = delayMin + nprand.rand( exc.numEntries ) * ( delayMax - delayMin )
            x.vec.weight = nprand.rand( exc.numEntries ) * excWeightMax
            #x.parent.tick = 4
            x.parent.parent.tick = 4
            print('+', end=' ')
            totGluWt += sum(x.vec.weight) * x.parent.parent.Gbar

    seed = excSeed
    for x in moose.wildcardFind( '/model/elec/#/NMDA/##[ISA=Synapse]' ):
        #print " x = ", x
        exc = moose.connect( '/network', 'spikeOut', x, 'addSpike','sparse')
        exc.setRandomConnectivity( excProb, seed )
        seed = seed + 1
        if exc.numEntries > 0:
            numNMDA += exc.numEntries
            assert( exc.numEntries == x.numField )
            x.vec.delay = delayMin + nprand.rand( exc.numEntries ) * ( delayMax - delayMin )
            x.vec.weight = nprand.rand( exc.numEntries ) * nmdaWeightMax
            #x.parent.tick = 4
            x.parent.parent.tick = 4
            print('*', end=' ')
            totNMDAWt += sum(x.vec.weight) * x.parent.parent.Gbar

    seed = inhSeed
    for x in moose.wildcardFind( '/model/elec/#/GABA/##[ISA=Synapse]' ):
        #print x
        inh = moose.connect( '/network', 'spikeOut', x, 'addSpike','sparse')
        inh.setRandomConnectivity( inhProb, seed )
        seed = seed + 1
        if inh.numEntries > 0:
            numInh += inh.numEntries
            x.vec.delay = delayMin + nprand.rand( inh.numEntries ) * ( delayMax - delayMin )
            x.vec.weight = nprand.rand( inh.numEntries ) * inhWeightMax
            #x.parent.tick = 4
            x.parent.parent.tick = 4
            print('-', end=' ')
            totGABAWt += sum(x.vec.weight) * x.parent.parent.Gbar

    print('connectDetailedNeuron: numExc = ', numExc, ', numNMDA=', numNMDA, ', numInh = ', numInh)
    print('connectDetailedNeuron: totWts Glu = ', totGluWt, ', NMDA = ', totNMDAWt, ', GABA = ', totGABAWt)

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

        # Build in the LTP stimulus
        offset = Iinject * 0.5
        injBaseline = np.repeat( self.Iinject, self.T )
        start = np.ceil( simtime / (interTetInterval * dt) )
        for i in range( 3 ):
            end = start + np.ceil( 0.5 / dt )
            injBaseline[ start:end ] += offset
            start = start + np.ceil( interTetInterval / dt )

        for i in range(self.N):
            if noiseInj:
                ## Gaussian white noise SD added every dt interval should be
                ## divided by sqrt(dt), as the later numerical integration
                ## will multiply it by dt.
                ## See the Euler-Maruyama method, numerical integration in 
                ## http://www.scholarpedia.org/article/Stochastic_dynamical_systems
                self.noiseTables.vec[i].vector = injBaseline + \
                    np.random.normal( \
                        scale=self.noiseInjSD*np.sqrt(self.Rm*self.Cm/self.dt), \
                        size=self.T ) # scale = SD
                self.noiseTables.vec[i].stepSize = 0    # use current time 
                                                        # as x value for interpolation


                self.noiseTables.vec[i].stopTime = self.simtime
        
        self._init_network(**kwargs)
        if plotif:
            self._init_plots()
        

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

    def _plot(self, fig):
        """ plots the spike raster for the simulated net"""
        plt.figure(1)
        
        ax = plt.subplot(221)
        cleanAx( ax, 'B' )
        plt.ylabel( 'Neuron #', fontsize = 16 )
        for i in range(0,self.NmaxExc):
            if i==0: label = 'Exc. spike trains'
            else: label = ''
            spikes = self.spikes.vec[i].vector
            ax.plot(spikes,[i]*len(spikes),\
                'b.',marker='.', markersize = 2, label=label)
        for i in range(self.NmaxExc,self.N):
            if i==self.NmaxExc: label = 'Inh. spike trains'
            else: label = ''
            spikes = self.spikes.vec[i].vector
            ax.plot(spikes,[i]*len(spikes),\
                'r.',marker='.', markersize = 2, label=label)           

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
        self.recN = 5 # number of neurons for which to record weights and Ca
        if CaPlasticity:
            ## make tables to store weights of recN exc synapses
            ## for each post-synaptic exc neuron
            self.weights = moose.Table( '/plotWeights', self.excC*self.recN )
            for i in range(self.recN):      # range(self.N) is too large
                for j in range(self.excC):
                    moose.connect( self.weights.vec[self.excC*i+j], 'requestOut',
                        self.synsEE.vec[i*self.excC+j].synapse[0], 'getWeight')            

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
    timeseries = net.trange
    ## individual neuron firing rates
    fig3 = plt.figure()
    plt.subplot(221)
    num_to_plot = 10
    #rates = []
    for nrni in range(num_to_plot):
        rate = rate_from_spiketrain(\
            net.spikes.vec[nrni].vector,simtime,dt, 1.0 )
        plt.plot(timeseries,rate)
    plt.title("Rates of "+str(num_to_plot)+" exc nrns")
    plt.ylabel("Hz")
    plt.ylim(0,100)
    plt.subplot(222)
    for nrni in range(num_to_plot):
        rate = rate_from_spiketrain(\
            net.spikes.vec[net.NmaxExc+nrni].vector,simtime,dt, 1.0 )
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

    fig3.tight_layout()

def makeScatterPlot( m, n, v ):
    fig4 = plt.figure()

    dx = 100e-6
    dy = 100e-6
    x = np.arange( m ) * dx
    x = np.tile( x, n )
    y = np.arange( n ) * dy
    y = y.repeat( m )
    #z = np.arange( m * n )
    #ret = plt.scatter( x, y, s = 64, c = v, vmin = -0.065, vmax = -0.055  )
    cmap = plt.get_cmap('afmhot')
    ret = plt.scatter( x, y, s = 64, c = v, vmin = 0.5, vmax = 1.0, cmap = cmap )
    plt.xlim( -dx, dx * m )
    plt.ylim( -dy, dy * n )
    return fig4, ret

def buildNeuronPlots( rdes ):
    if not moose.exists( '/graphs' ):
        graphs = moose.Neutral( '/graphs' )
    vtab = moose.Table( '/graphs/vtab' )
    catab = moose.Table( '/graphs/catab' )
    moose.connect( vtab, "requestOut", rdes.soma, "getVm" )
    caSoma = moose.element( rdes.soma.path + "/Ca_conc" )
    moose.connect( catab, "requestOut", caSoma, "getCa" )
    elist = moose.wildcardFind( '/model/chem/psd/tot_PSD_R[]' )
    rtab = moose.Table2( '/graphs/rtab', len( elist ) ).vec
    for i in zip( elist, rtab ):
        moose.connect( i[1], "requestOut", i[0], "getN" )
    elist = moose.wildcardFind( '/model/chem/spine/Ca[]' )
    pcatab = moose.Table2( '/graphs/pcatab', len( elist ) ).vec
    for i in zip( elist, pcatab ):
        moose.connect( i[1], "requestOut", i[0], "getConc" )

def cleanAx( ax, label, showXlabel = False ):
    ax.spines['top'].set_visible( False )
    ax.spines['right'].set_visible( False )
    ax.tick_params( direction = 'out' )
    if not showXlabel:
        ax.set_xticklabels( [] )
    for tick in ax.xaxis.get_major_ticks():
        tick.tick2On = False
    for tick in ax.yaxis.get_major_ticks():
        tick.tick2On = False
    ax.text( -0.18, 1.0, label, fontsize = 18, weight = 'bold', transform=ax.transAxes )


def saveNeuronPlots( fig, rdes ):
    #fig = plt.figure( figsize=(12, 10), facecolor='white' )
    #fig.subplots_adjust( left = 0.18 )
    plt.figure(1)

    ax = plt.subplot(222)
    cleanAx( ax, 'C' )
    plt.ylabel( 'Vm (mV)', fontsize = 16 )
    vtab = moose.element( '/graphs/vtab' )
    t = np.arange( 0, len( vtab.vector ), 1 ) * vtab.dt
    plt.plot( t, vtab.vector * 1000, label="Vm" )
    #plt.legend()
    
    ax = plt.subplot(223)
    cleanAx( ax, 'D', showXlabel = True )
    pcatab = list( moose.vec( '/graphs/pcatab' ) )[0::50]
    t = np.arange( 0, len( pcatab[0].vector ), 1 ) * pcatab[0].dt
    for i in pcatab:
        plt.plot( t, i.vector * 1000 )
    plt.ylabel( '[Ca] (uM)', fontsize = 16 )
    plt.xlabel( 'Time (s)', fontsize = 16 )

    ax = plt.subplot(224)
    cleanAx( ax, 'E', showXlabel = True )
    rtab = list( moose.vec( '/graphs/rtab' ) )[0::50]
    t = np.arange( 0, len( rtab[0].vector ), 1 ) * rtab[0].dt
    for i in rtab:
        plt.plot( t, i.vector )
    plt.ylabel( '# of inserted GluRs', fontsize = 16 )
    plt.xlabel( 'Time (s)', fontsize = 16 )
    '''
    for i in moose.wildcardFind( '/graphs/#' ):
        i.xplot( fname + '.xplot', i.name )
    '''

def create_viewer(rdes):
    # print "Creating 3D Viewer"
    network = moogli.extensions.moose.read(path=rdes.elecid.path,
                                            vertices=10)
    # print "Read Network"
    network.set("color", moogli.colors.LIGHT_BLUE)
    network.groups["spine"].set("color", moogli.colors.ORANGE)
     # for dendrite in dendrites.values():
     #    dendrite.set_colors(moogli.core.Vec4f(173 / 255.0, 216 / 255.0, 230 / 255.0, 1.0))

    [shape.set_radius(shape.get_apex_radius() * 4.0) for shape in
     list(network.groups["spine"].groups["head"].shapes.values())]
    # print "Creating LIFS"
    soma = network.shapes[rdes.soma.path]

    center = soma.get_center()
    row_axis = moogli.geometry.X_AXIS
    row_count = 25
    row_separation = soma.get_base_radius() * 5.0
    col_axis = moogli.geometry.Z_AXIS
    col_count = 25
    col_separation = row_separation
    radii = soma.get_base_radius()
    colors = moogli.colors.GREEN
    vertices = 20
    lifs = moogli.shapes.Sphere.grid("LIF",
                                     center,
                                     row_axis,
                                     row_count,
                                     row_separation,
                                     col_axis,
                                     col_count,
                                     col_separation,
                                     radii,
                                     colors,
                                     vertices)
    # print "Created LIFS"
    # morphology.create_group("dendrites", dendrites, 0.0, 300.0, colormap)
    # print "Creating Viewer"
    viewer = moogli.Viewer("viewer") # prelude = prelude, interlude = interlude)
    # print "Created Viewer"
    viewer.attach_shapes(list(network.shapes.values()))
    viewer.attach_shapes(list(lifs.shapes.values()))
    # print "Attached Shapes"
    view = moogli.View("view")
    viewer.attach_view(view)
    # print "Attached View"
    return viewer

if __name__=='__main__':
    plt.ion()
    ## ExcInhNetBase has unconnected neurons,
    ## ExcInhNet connects them
    ## Instantiate either ExcInhNetBase or ExcInhNet below
    #net = ExcInhNetBase(N=N)
    net = ExcInhNet(N=N)
    print(net)
    moose.le( '/' )
    moose.le( '/network' )
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    buildNeuronPlots( rdes )
    connectDetailedNeuron()

    app = QtGui.QApplication(sys.argv)
    viewer = create_viewer(rdes)
    viewer.showMaximized()
    viewer.start()
    app.exec_()

    ## Important to distribute the initial Vm-s
    ## else weak coupling gives periodic synchronous firing
    plotif = True
    net.simulate(simtime,plotif=plotif,\
        v0=np.random.uniform(el-20e-3,vt,size=N))

    # moose simulation
    moose.useClock( 1, '/network', 'process' )
    moose.useClock( 2, '/plotSpikes', 'process' )
    moose.useClock( 3, '/plotVms', 'process' )
    if CaPlasticity:
        moose.useClock( 3, '/plotWeights', 'process' )
        moose.useClock( 3, '/plotCa', 'process' )
    moose.setClock( 0, dt )
    moose.setClock( 1, dt )
    moose.setClock( 2, dt )
    moose.setClock( 3, dt )
    #moose.setClock( moose.element( '/cell/hsolve' ), dt )
    moose.setClock( 9, dt )

    if plotif:
        Vm = net.network.vec.Vm
        fig = plt.figure( 1, figsize=(12, 10), facecolor='white' )
        fig.subplots_adjust( left = 0.18 )
        fig2, ret = makeScatterPlot( 50, 50, Vm )
        #cellFig = bcs.neuronPlot( '/model/elec', '/model/chem/psd/tot_PSD_R[]' )

    moose.reinit()
    t1 = time.time()
    print('starting')
    #moose.start(simtime)
    for currTime in np.arange( 0, simtime, updateDt ):
        moose.start(updateDt)
        lastt = net.network.vec.lastEventTime
        lastt = np.exp( 2 * (lastt - currTime ) )
        print(currTime, time.time() - t1)
        ret.set_array( lastt )
        fig2.canvas.draw()

    print('runtime, t = ', time.time() - t1)

    if plotif:
        net._plot( fig )

    extra_plots(net)
    #bcs.displayCellPlots( plt )
    saveNeuronPlots( fig, rdes )
    plt.show()
    plt.savefig( fname + '.svg', bbox_inches='tight')
    print( "Hit 'enter' to exit" )
    eval(input())
