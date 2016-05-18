#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import moose
import pylab
import numpy
import sys

def deq( a, b ):
    eps1 = 1e-9
    eps2 = 1e-20
    return ( abs (a-b) < eps1 * (abs(a) + abs(b)) + eps2 )

def main():
        """
        This example illustrates loading and running a reaction system that
        spans two volumes, that is, is in different compartments. It uses a
        kkit model file. You can tell if it is working if you see nice 
        relaxation oscillations.
        """
        # the kkit reader doesn't know how to do multicompt solver setup.
        solver = "ee"  
	mfile = '../genesis/OSC_diff_vols.g'
	runtime = 3000.0
        simDt = 1.0
	modelId = moose.loadModel( mfile, 'model', solver )
        #moose.delete( '/model/kinetics/A/Stot' )
        compt0 = moose.element( '/model/kinetics' )
        compt1 = moose.element( '/model/compartment_1' )
        assert( deq( compt0.volume, 2e-20 ) )
        assert( deq( compt1.volume, 1e-20 ) )
        dy = compt0.dy
        compt1.y1 += dy
        compt1.y0 = dy
        assert( deq( compt1.volume, 1e-20 ) )
        # We now have two cubes adjacent to each other. Compt0 has 2x vol.
        # Compt1 touches it.
        stoich0 = moose.Stoich( '/model/kinetics/stoich' )
        stoich1 = moose.Stoich( '/model/compartment_1/stoich' )
        ksolve0 = moose.Ksolve( '/model/kinetics/ksolve' )
        ksolve1 = moose.Ksolve( '/model/compartment_1/ksolve' )
        stoich0.compartment = compt0
        stoich0.ksolve = ksolve0
        stoich0.path = '/model/kinetics/##'
        stoich1.compartment = compt1
        stoich1.ksolve = ksolve1
        stoich1.path = '/model/compartment_1/##'
        #stoich0.buildXreacs( stoich1 )
        print ksolve0.numLocalVoxels, ksolve0.numPools, stoich0.numAllPools
        assert( ksolve0.numLocalVoxels == 1 )
        assert( ksolve0.numPools == 7 )
        assert( stoich0.numVarPools == 5 )
        assert( stoich0.numBufPools == 1 )
        assert( stoich0.numProxyPools == 1 )
        assert( stoich0.numAllPools == 7 )
        print len( stoich0.proxyPools[stoich1] ),
        print len( stoich1.proxyPools[stoich0] )
        assert( len( stoich0.proxyPools[stoich1] ) == 1 )
        assert( len( stoich1.proxyPools[stoich0] ) == 1 )
        print ksolve1.numLocalVoxels, ksolve1.numPools, stoich1.numAllPools
        assert( ksolve1.numLocalVoxels == 1 )
        assert( ksolve1.numPools == 6 )
        assert( stoich1.numAllPools == 6 )
        stoich0.buildXreacs( stoich1 )
        print moose.element( '/model/kinetics/endo' )
        print moose.element( '/model/compartment_1/exo' )
        moose.le( '/model/compartment_1' )
	moose.reinit()
	moose.start( runtime ) 

	# Display all plots.
	for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
            t = numpy.arange( 0, x.vector.size, 1 ) * simDt
            pylab.plot( t, x.vector, label=x.name )
        pylab.legend()
        pylab.show()

	#quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
