#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

# This example illustrates how to set up a transport model with 
# four non-reacting molecules in a cylinder. 
# Molecule a and b have a positive motorConst so they are 
# are transported from soma (voxel 0) to the end of the cylinder.
# Molecules c and d have a negative motorConst so they are transported 
# from the end of the cylinder to the soma.
# Rate of all motors is 1e-6 microns/sec.
# Pools a and c start out with all molecules at the soma, b and d 
# start with all molecules at the end of the cylinder.
# Net effect is that only molecules a and d actually move. B and c
# stay put as their motors are pushing further toward their respective
# ends, and I assume all cells have sealed ends.

import math
import pylab
import numpy
import moose

import os
import signal 
PID = os.getpid()

def doNothing( *args ):
		pass

signal.signal( signal.SIGUSR1, doNothing )

def makeModel():
		# create container for model
		r0 = 1e-6	# m
		r1 = 1e-6	# m
		num = 25
		diffLength = 1e-6 # m
		len = num * diffLength	# m
		diffConst = 1e-12 # m^2/sec
		motorConst = 1e-6 # m/sec
		concA = 1 # millimolar

		model = moose.Neutral( 'model' )
		compartment = moose.CylMesh( '/model/compartment' )
		compartment.r0 = r0
		compartment.r1 = r1
		compartment.x0 = 0
		compartment.x1 = len
		compartment.diffLength = diffLength
		
		assert( compartment.numDiffCompts == num )

		# create molecules and reactions
		a = moose.Pool( '/model/compartment/a' )
		b = moose.Pool( '/model/compartment/b' )
		c = moose.Pool( '/model/compartment/c' )
		d = moose.Pool( '/model/compartment/d' )
                """
                r1 = moose.Reac( '/model/compartment/r1' )
                moose.connect( r1, 'sub', b, 'reac' )
                moose.connect( r1, 'sub', d, 'reac' )
                moose.connect( r1, 'prd', c, 'reac' )
                r1.Kf = 100 # 1/(mM.sec)
                r1.Kb = 0.01 # 1/sec
                """

		# Assign parameters
		a.diffConst = 0.0;
		b.diffConst = 0.0;
		#b.motorRate = motorRate
		c.diffConst = 0.0;
		d.diffConst = 0.0;
		#d.diffConst = diffConst;
		os.kill( PID, signal.SIGUSR1 )
                a.motorConst = motorConst
                b.motorConst = motorConst
                c.motorConst = -motorConst
                d.motorConst = -motorConst


		# Make solvers
		ksolve = moose.Ksolve( '/model/compartment/ksolve' )
		dsolve = moose.Dsolve( '/model/compartment/dsolve' )
		stoich = moose.Stoich( '/model/compartment/stoich' )
		stoich.compartment = compartment
		stoich.ksolve = ksolve
		stoich.dsolve = dsolve
		stoich.path = "/model/compartment/##"
                assert( dsolve.numPools == 4 )
		a.vec[0].concInit = concA * 1
		b.vec[num-1].concInit = concA * 2
		c.vec[0].concInit = concA * 3
		d.vec[num-1].concInit = concA * 4

def displayPlots():
		a = moose.element( '/model/compartment/a' )
		b = moose.element( '/model/compartment/b' )
		c = moose.element( '/model/compartment/c' )
		d = moose.element( '/model/compartment/d' )
                pos = numpy.arange( 0, a.vec.conc.size, 1 )
                pylab.plot( pos, a.vec.conc, label='a' )
                pylab.plot( pos, b.vec.conc, label='b' )
                pylab.plot( pos, c.vec.conc, label='c' )
                pylab.plot( pos, d.vec.conc, label='d' )
                pylab.legend()
                pylab.show()

def main():
		dt4 = 0.01
		dt5 = 0.01
                runtime = 10.0 # seconds
                # Set up clocks. The dsolver to know before assigning stoich
		moose.setClock( 4, dt4 )
		moose.setClock( 5, dt5 )

		makeModel()
		moose.useClock( 4, '/model/compartment/dsolve', 'process' )
                # Ksolve must be scheduled after dsolve.
		moose.useClock( 5, '/model/compartment/ksolve', 'process' )

		moose.reinit()
		moose.start( runtime ) # Run the model

		a = moose.element( '/model/compartment/a' )
		b = moose.element( '/model/compartment/b' )
		c = moose.element( '/model/compartment/c' )
		d = moose.element( '/model/compartment/d' )

                atot = sum( a.vec.conc )
                btot = sum( b.vec.conc )
                ctot = sum( c.vec.conc )
                dtot = sum( d.vec.conc )

                print 'tot = ', atot, btot, ctot, dtot, ' (b+c)=', btot+ctot
                displayPlots()
		moose.start( runtime ) # Run the model
                atot = sum( a.vec.conc )
                btot = sum( b.vec.conc )
                ctot = sum( c.vec.conc )
                dtot = sum( d.vec.conc )

                print 'tot = ', atot, btot, ctot, dtot, ' (b+c)=', btot+ctot

		quit()


# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
