#!/usr/bin/env python
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
This script generates Panel A from Figure 6. It is a dummy for showing
the layout, and loads in about 20 seconds.
'''

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
sys.path.append( "/home/bhalla/moose/trunk/Demos/util" )
import rdesigneur as rd
import moogli
cellname = "./cells_channels/CA1_nochans.morph.xml"
#cellname = "./ca1_minimal.p"
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
        cellProto = cellProto, \
        spineProto = spineProto, \
        chanProto = chanProto, \
    )

    return rdes


#############################################

def makeDetailedNeuron():
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )
    #bcs.addAllPlots()
    

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

    def _init_plots(self):
        ## make a few tables to store a few Vm-s
        numVms = 10
        self.plots = moose.Table( '/plotVms', numVms )
        ## draw numVms out of N neurons
        nrnIdxs = random.sample(range(self.N),numVms)
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

        moose.useClock( 0, '/network/synsIE', 'process' )
        moose.useClock( 0, '/network/synsI', 'process' )

#############################################
# Make plots
#############################################

def interlude( view ):
    view.yaw( 0.005 )

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
     network.groups["spine"].groups["head"].shapes.values()]
    # print "Creating LIFS"
    soma = network.shapes[rdes.soma.path]

    center = soma.get_center()
    row_axis = moogli.geometry.X_AXIS
    row_count = 50
    row_separation = soma.get_base_radius() * 5.0
    col_axis = moogli.geometry.Z_AXIS
    col_count = 50
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
    viewer.attach_shapes(network.shapes.values())
    viewer.attach_shapes(lifs.shapes.values())
    # print "Attached Shapes"
    view = moogli.View("view", interlude=interlude)
    viewer.attach_view(view)
    # print "Attached View"
    viewer.showMaximized()
    viewer.start()
    view.zoom( 0.4 )
    view.pitch( PI/2.5 )
    return viewer

if __name__=='__main__':
    ## ExcInhNetBase has unconnected neurons,
    ## ExcInhNet connects them
    ## Instantiate either ExcInhNetBase or ExcInhNet below
    #net = ExcInhNetBase(N=N)
    net = ExcInhNet(N=N)
    print net
    moose.le( '/' )
    moose.le( '/network' )
    rdes = buildRdesigneur()
    rdes.buildModel( '/model' )

    app = QtGui.QApplication(sys.argv)
    viewer = create_viewer(rdes)
    app.exec_()
