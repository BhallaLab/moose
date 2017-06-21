#!/usr/bin/env python

"""cable.py: A passive cable of n compartments.

Last modified: Wed May 21, 2014  04:26AM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


import compartment as comp

import moose
import moose.utils as utils
import os
import pylab
import numpy as np

class PasiveCable( ):
    ''' Class representing a cable '''

    def __init__(self, args):
        ''' Initialize the cable '''
        self.length = float(args['length'])
        self.ncomp = int(args['ncomp'])
        self.diameter = float(args['diameter'])

        self.cablePath = '/cable'
        self.tablePath = '/data'
        moose.Neutral( self.cablePath )
        moose.Neutral( self.tablePath )
        # Store all moose-compartments in this list.
        self.cable = []
        # Keep all simulation data in this list of tables.
        self.tables = []
        # keep all stimulus in this list of tables.
        self.stimTables = []
        self.makeCable( args )

    def makeCable( self, args ):
        ''' Make a cable out of n compartments '''
        for i in range( self.ncomp ):
            compPath = '{}/comp{}'.format( self.cablePath, i)
            l = self.length / self.ncomp
            d = self.diameter
            c = comp.MooseCompartment( compPath, l, d, args )
            self.cable.append(c)
        self.connect( )
        utils.dump( "STEP"
                , "Passive cable is connected and ready for simulation." 
                )

    def connect( self ):
        ''' Connect the cable '''
        utils.dump('STEP', 'Connecting cable ...')
        for i, c1 in enumerate( self.cable[:-1] ):
            c2 = self.cable[i+1].mc_
            c1.mc_.connect('axial', c2, 'raxial')

    def setupDUT( self ):
        ''' Setup cable for recording '''

        # Create a pulse input
        moose.Neutral( self.tablePath )
        stim = moose.PulseGen( '{}/input'.format( self.tablePath) )
        stim.level[0] = 0.1e-9
        stim.width[0] = 0.25
        stim.delay[0] = 0.0

        # Inject the current from stim to first compartment.
        moose.connect( stim, 'output', self.cable[0].mc_, 'injectMsg' )
        
        # Fill the data from stim into table.
        inputTable = moose.Table( '{}/inputTable'.format( self.tablePath ) )
        self.stimTables.append( inputTable )
        moose.connect( inputTable, 'requestOut', stim, 'getOutputValue' )

    def setupHSolve(self, path='/hsolve'):
        """ Setup hsolve solver """
        hsolve = moose.HSolve( path )
        hsolve.dt = self.simDt
        moose.useClock(1, path, 'process')
        hsolve.target = self.cablePath


    def simulate( self, simTime, simDt = 1e-3, plotDt = None ):
        '''Simulate the cable '''

        if plotDt is None:
            plotDt = simDt / 2
        self.simDt = simDt
        self.plotDt = plotDt
        self.setupDUT( )
 
        # Setup clocks 
        utils.dump("STEP", "Setting up the clocks ... ")
        moose.setClock( 0, self.simDt )
        moose.setClock( 1, self.simDt )
        moose.setClock( 2, self.simDt )

        ## Use clocksc
        moose.useClock( 0, '/cable/##'.format(self.cablePath), 'process' )
        moose.useClock( 1, '/cable/##'.format(self.cablePath), 'init' )
        moose.useClock( 2, '{}/##'.format(self.tablePath), 'process' )

        utils.dump("STEP"
                , [ "Simulating cable for {} sec".format(simTime)
                    , " simDt: %s, plotDt: %s" % ( self.simDt, self.plotDt )
                    ]
                )
        self.setupHSolve( )
        moose.reinit( )
        utils.verify( )
        moose.start( simTime )

def main( args ):
    cableLength = float(args['length'])
    compNons = int(args['ncomp'])
    compartmentSize = cableLength / compNons
    cable = PasiveCable( args )
    first = 0
    last = int(int(args['x']) / compartmentSize) - 1
    table1 = utils.recordTarget('/data/table1', cable.cable[first].mc_, 'vm' )
    table2 = utils.recordTarget('/data/table2', cable.cable[last].mc_, 'vm' )

    simTime = args['run_time']
    sim_dt = args['dt']
    outputFile = args['output']

    cable.simulate( simTime, sim_dt )
    utils.plotTables([table1, table2]
            , xscale = sim_dt
            , file = outputFile
            )
    utils.saveTables( [table1, table2 ]
            , xscale = sim_dt
            , file = 'data/rallpack2_moose.dat'
            )
    #import moose.backend.spice as spice
    #spice.toSpiceNetlist( output = 'cable.spice' )

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
    parser.add_argument( '--ncomp'
            , default = 1000
            , help = 'No of compartment in cable'
            )
    parser.add_argument( '--output'
            , default = None
            , help = 'Store simulation results to this file'
            )
    args = parser.parse_args()
    main( vars(args) )

