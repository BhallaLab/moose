# __DEPRECATED__ __BROKEN__
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

# This example illustrates how to set up a Turing pattern in 1-D using
# reaction diffusion calculations. The runtime is kept short so that the
# pattern doesn't make it all the way to the end of the system.

import math
import pylab
import numpy
import moose
import proto18

def loadElec():
	library = moose.Neutral( '/library' )
	moose.setCwe( '/library' )
	proto18.make_Ca()
	proto18.make_Ca_conc()
	proto18.make_K_AHP()
	proto18.make_K_C()
	proto18.make_Na()
	proto18.make_K_DR()
	proto18.make_K_A()
	proto18.make_glu()
	proto18.make_NMDA()
	proto18.make_Ca_NMDA()
	proto18.make_NMDA_Ca_conc()
	proto18.make_axon()
	model = moose.element( '/model' )
	cellId = moose.loadModel( 'ca1_asym.p', '/model/elec', "Neutral" )
	return cellId

def makeChemModel( cellId ):
		# create container for model
		r0 = 1e-6	# m
		r1 = 1e-6	# m
		num = 2800
		diffLength = 1e-6 # m
		diffConst = 5e-12 # m^2/sec
		motorRate = 1e-6 # m/sec
		concA = 1 # millimolar

		model = moose.element( '/model' )
		compartment = moose.NeuroMesh( '/model/compartment' )
                # FIXME: No attribute cell
                compartment.cell =  cellId
		compartment.diffLength = diffLength
                print "cell NeuroMesh parameters: numSeg and numDiffCompt: ", compartment.numSegments, compartment.numDiffCompts
		
                print "compartment.numDiffCompts == num: ", compartment.numDiffCompts, num
		assert( compartment.numDiffCompts == num )

		# create molecules and reactions
		a = moose.Pool( '/model/compartment/a' )
		b = moose.Pool( '/model/compartment/b' )
		s = moose.Pool( '/model/compartment/s' )
                e1 = moose.MMenz( '/model/compartment/e1' )
                e2 = moose.MMenz( '/model/compartment/e2' )
                e3 = moose.MMenz( '/model/compartment/e3' )
                r1 = moose.Reac( '/model/compartment/r1' )
                moose.connect( e1, 'sub', s, 'reac' )
                moose.connect( e1, 'prd', a, 'reac' )
                moose.connect( a, 'nOut', e1, 'enzDest' )
                e1.Km = 1
                e1.kcat = 1

                moose.connect( e2, 'sub', s, 'reac' )
                moose.connect( e2, 'prd', b, 'reac' )
                moose.connect( a, 'nOut', e2, 'enzDest' )
                e2.Km = 1
                e2.kcat = 0.5

                moose.connect( e3, 'sub', a, 'reac' )
                moose.connect( e3, 'prd', s, 'reac' )
                moose.connect( b, 'nOut', e3, 'enzDest' )
                e3.Km = 0.1
                e3.kcat = 1

                moose.connect( r1, 'sub', b, 'reac' )
                moose.connect( r1, 'prd', s, 'reac' )
                r1.Kf = 0.3 # 1/sec
                r1.Kb = 0 # 1/sec

		# Assign parameters
		a.diffConst = diffConst/10
		b.diffConst = diffConst
		s.diffConst = 0
                #b.motorConst = motorRate

		# Make solvers
		ksolve = moose.Ksolve( '/model/compartment/ksolve' )
		dsolve = moose.Dsolve( '/model/compartment/dsolve' )
		stoich = moose.Stoich( '/model/compartment/stoich' )
		stoich.compartment = compartment
		#ksolve.numAllVoxels = compartment.numDiffCompts
		stoich.ksolve = ksolve
		stoich.dsolve = dsolve
		stoich.path = "/model/compartment/##"
                assert( dsolve.numPools == 3 )
		a.vec.concInit = [0.1]*num
                a.vec[0].concInit += 0.5
                a.vec[400].concInit += 0.5
                a.vec[800].concInit += 0.5
                a.vec[1200].concInit += 0.5
                a.vec[1600].concInit += 0.5
                a.vec[2000].concInit += 0.5
                a.vec[2400].concInit += 0.5
                #a.vec[num/2].concInit -= 0.1
		b.vec.concInit = [0.1]*num
		s.vec.concInit = [1]*num

def displayPlots( num ):
		a = moose.element( '/model/compartment/a' )
		b = moose.element( '/model/compartment/b' )
                print '/newplot\n/plotname a' + str(num)
                for x in a.vec.conc:
                    print x
                print '/newplot\n/plotname b' + str(num)
                for y in b.vec.conc:
                    print y
                """
                print '/newplot\n/plotname bvol'
                for z in a.vec.volume:
                    print z * 1e18
                print '/newplot\n/plotname aInit'
                for x in a.vec.concInit:
                    print x
                pos = numpy.arange( 0, a.vec.conc.size, 1.0 )
                aconc = numpy.array( a.vec.conc )
                bconc = numpy.array( b.vec.conc )
                print "pos a, b = ", pos, b.vec.conc.size
                pylab.plot( pos[:100], aconc[:100], label='a' )
                pylab.plot( pos[:100], bconc[:100], label='b' )
                #pylab.plot( pos, a.vec.conc, label='a' )
                #pylab.plot( pos, b.vec.conc, label='b' )
                print "plotting done"
                pylab.legend()
                print "legend done"
                pylab.show()
                print "show done"
                """

def main():
                runtime = 400
		dt4 = 0.02  # for the diffusion
		dt5 = 0.2   # for the reaction
                # Set up clocks. The dsolver to know before assigning stoich
		moose.setClock( 4, dt4 )
		moose.setClock( 5, dt5 )
                model = moose.Neutral( '/model' )
                cellId = loadElec()
		makeChemModel( cellId )
		moose.useClock( 4, '/model/compartment/dsolve', 'process' )
                # Ksolve must be scheduled after dsolve.
		moose.useClock( 5, '/model/compartment/ksolve', 'process' )
                print "finished loading"
		moose.reinit()
                for i in range( 10 ):
		    moose.start( runtime / 10 ) # Run the model for 10 seconds.
                    # print 'done', i
                    displayPlots( i )
                print "finished running"

                """
		a = moose.element( '/model/compartment/a' )
		b = moose.element( '/model/compartment/b' )
		s = moose.element( '/model/compartment/s' )

                atot = sum( a.vec.conc )
                btot = sum( b.vec.conc )
                stot = sum( s.vec.conc )
                print "a = ", a.vec.conc
                print "b = ", b.vec.conc
                print "s = ", s.vec.conc

                print 'tot = ', atot, btot, atot + btot + stot
                displayPlots()
                """

                """
		dsolve = moose.element( '/model/compartment/dsolve' )
                
                print '**************** dsolve.nvecs'
                x = dsolve.nVec[0]
                print dsolve.numPools, x, sum(x)
                print dsolve.nVec[1], sum( dsolve.nVec[1] )
                print dsolve.nVec[2], sum( dsolve.nVec[2] )
                print dsolve.nVec[3], sum( dsolve.nVec[3] )
                """

		quit()


# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
