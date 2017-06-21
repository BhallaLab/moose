#!/usr/bin/env python

"""tree_cable.py: A depth 10 binary tree like cable with following properties.

Depth		Number		Length (microns)	Diameter (microns)
==========================================================================
0		1		32.0			16.0
1		2		25.4			10.08
2		4		20.16			6.35
3		8		16.0			4.0
4		16		12.7			2.52
5		32		10.08			1.587
6		64		8.0			1.0
7		128		6.35			0.63
8		256		5.04			0.397
9		512		4.0			0.25

The membrane properties are :
RA = 1.0 ohms meter		= 100 ohms cm
RM = 4.0 ohms meter^2		= 40000 ohms cm^2
CM = 0.01 Farads/meter^2	= 1.0 uf/cm^2
EM = -0.065 Volts		= -65 mV

Last modified: Sat Jan 18, 2014  05:01PM

"""
from __future__ import print_function
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import moose
import moose.utils as utils
import compartment as comp
import pylab
import numpy as np
import moose.backend.graphviz as graphviz

def nextValuePowerOf2Law( d1, power=2.0/3.0  ):
    ''' Given a value, compute the next value using 2^power law '''
    return pow(pow(d1, 1.0/power)/2.0, power)

def testPowerLaw():
    """Test power law """
    l = [ 32.0 ]
    d = [ 16.0 ]
    print(nextValuePowerOf2Law( d[-1] ))
    print(nextValuePowerOf2Law( l[-1], 1.0/3.0 ))

class BinaryCable( ):

    def __init__(self, depth):
        ''' init '''
        self.depth = depth
        self.size = pow(2, self.depth) - 1
        self.compLenAtLevelZero = 32e-6
        self.compDiamAtLevelZero = 16.0e-6
        self.compLengthList = [ self.compLenAtLevelZero ]
        self.compDiamList = [ self.compDiamAtLevelZero ]
        self.cablePath = '/cable'
        moose.Neutral(self.cablePath)
        self.tablePath = '/data'
        moose.Neutral(self.tablePath)
        self.stimTables = []
    
    def buildParameterLists(self):
        ''' Build list of parameters in moose '''
        while len(self.compDiamList) < self.depth:
            self.compDiamList.append(
                    nextValuePowerOf2Law(self.compDiamList[-1] )
                    )
        while len(self.compLengthList) < self.depth:
            self.compLengthList.append(
                    nextValuePowerOf2Law(self.compLengthList[-1], 1.0/3.0)
                    )

    def buildCable(self, args):
        ''' Build binary cable '''
        self.args = args
        self.buildParameterLists()
        # Cable is a list of lists.
        self.cable = list()
        for n, (l, d) in enumerate(zip(self.compLengthList, self.compDiamList)):
            utils.dump("STEP"
                    , "Binary tree level {}: length {}, diameter {}".format(
                        n, l, d
                        )
                    )
            noOfCompartments = pow(2, n)
            compartmentList = []
            for i in range(noOfCompartments):
                compPath = '{}/comp_{}_{}'.format(self.cablePath, n, i)
                m = comp.MooseCompartment( compPath, l, d, args )
                compartmentList.append( m.mc_ )
            self.cable.append( compartmentList )
        self.connectCable()

    def connectCable(self):
        ''' Connect the binary tree cable '''
        for i, parentList in enumerate(self.cable[:-1]):
            childrenList = self.cable[i+1]
            for ii, p in enumerate(parentList):
                leftChild = childrenList[ 2*ii + 0 ]
                rightChild = childrenList[ 2*ii + 1 ]
                moose.connect( p, 'raxial', leftChild, 'axial')
                moose.connect( p, 'raxial', rightChild, 'axial' )

    def setupDUT( self ):
        ''' Setup cable for recording '''

        # Create a pulse input
        moose.Neutral( self.tablePath )
        stim = moose.PulseGen( '{}/input'.format( self.tablePath) )
        stim.level[0] = self.args['inj']
        stim.width[0] = self.args['run_time']

        # Inject the current from stim to first compartment.
        moose.connect( stim, 'output', self.cable[0][0], 'injectMsg' )
        
        # Fill the data from stim into table.
        inputTable = moose.Table( '{}/inputTable'.format( self.tablePath ) )
        self.stimTables.append( inputTable )
        moose.connect( inputTable, 'requestOut', stim, 'getOutputValue' )

    def recordAt( self, depth, index ):
        ''' Parameter index is python list-like index. Index -1 is the last
        elements in the list 
        '''
        utils.dump( "RECORD"
                , "Adding probe at index {} and depth {}".format(index, depth)
                )
        c = self.cable[depth][index]
        t = moose.Table( '{}/output_at_{}'.format( self.tablePath, index ))
        moose.connect( t, 'requestOut', c, 'getVm' )
        return t

    def setupSolver(self, path = '/hsolve'):
        """Setting up HSolver """
        hsolve = moose.HSolve( path )
        hsolve.dt = self.simDt
        moose.setClock(1, self.simDt)
        moose.useClock(1, hsolve.path, 'process')
        hsolve.target = self.cablePath

    def simulate(self, simTime, simDt, plotDt=None):
        '''Simulate the cable 
        '''
        self.simDt = simDt
        self.setupDUT( )
 
        # Setup clocks 
        moose.setClock( 0, self.simDt )

        # Use clocks
        moose.useClock( 0, '/##', 'process' )
        moose.useClock( 0, '/##', 'init' )

        utils.dump("STEP"
                , [ "Simulating cable for {} sec".format(simTime)
                    , " simDt: %s" % self.simDt
                    ]
                )
        utils.verify( )
        moose.reinit( )
        self.setupSolver( )
        moose.start( simTime )

def main( args ):
    # d is depth of cable. 
    d = args['tree_depth']
    assert d > 0, "Cable depth can not be nagative"
    binCable = BinaryCable( depth = d )
    binCable.buildCable( args )
    table0 = binCable.recordAt( depth = 0, index = 0 )
    table1 = binCable.recordAt( depth = d-1, index = -1)
    print("[STIM] Simulating a cable with depth {}".format(d))
    binCable.simulate( simTime = args['run_time'], simDt = args['dt'] )
    #utils.plotTables( [ table0, table1 ]
    #        , file = args['output']
    #        , xscale = args['dt']
    #        )
    graphviz.writeGraphviz(__file__+".dot") #, compartment_shape='point')

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
            description = 'Rallpacks1: A cable with passive compartments'
            )
    parser.add_argument( '--tau'
            , default = 0.04
            , help = 'Time constant of membrane'
            )
    parser.add_argument( '--run_time'
            , default = 0.25
            , help = 'Simulation run time'
            )
    parser.add_argument( '--dt'
            , default = 5e-5
            , help = 'Step time during simulation'
            )
    parser.add_argument( '--Em'
            , default = -65e-3
            , help = 'Resting potential of membrane'
            )
    parser.add_argument( '--RA'
            , default = 1.0
            , help = 'Axial resistivity'
            )
    parser.add_argument( '--RM'
            , default = 4.0
            , help = 'Membrane resistivity.'
            )
    parser.add_argument( '--lambda'
            , default = 1e-3
            , help = 'Lambda, what else?'
            )
    parser.add_argument( '--x'
            , default = 1e-3
            , help = 'You should record membrane potential somewhere, right?'
            ) 
    parser.add_argument( '--length'
            , default = 1e-3
            , help = 'Length of the cable'
            )
    parser.add_argument( '--diameter'
            , default = 1e-6
            , help = 'Diameter of cable'
            )
    parser.add_argument( '--inj'
            , default = 1e-10
            , help = 'Current injected at one end of the cable'
            )
    parser.add_argument( '--tree_depth'
            , default = 10
            , help = 'Depth of binary tree.'
            )
    parser.add_argument( '--output'
            , default = None
            , help = 'Store simulation results to this file'
            )

    args = parser.parse_args()
    main( vars(args) )
