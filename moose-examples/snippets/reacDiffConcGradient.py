#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################



import math
import pylab
import numpy
import moose

diffConst = 1e-12

def makeCyl( num, concInit, radius, x0, x1 ):
		compt = moose.CylMesh( '/model/compt' + num )
                compt.x0 = x0
                compt.x1 = x1
                compt.y0 = 0
                compt.y1 = 0
                compt.z0 = 0
                compt.z1 = 0
                compt.r0 = radius
                compt.r1 = radius
                compt.diffLength = x1-x0
                a = moose.Pool( compt.path + '/a' )
                b = moose.Pool( compt.path + '/b' + num )
		reac = moose.Reac( compt.path + '/reac' )
		moose.connect( reac, 'sub', a, 'reac' )
		moose.connect( reac, 'prd', b, 'reac' )
                a.diffConst = diffConst
                a.concInit = concInit
                b.concInit = concInit
		reac.Kf = 0.1
		reac.Kb = 0.1
                return a, b, compt

def makeModel():
                radius = 1e-6
                len0 = 4e-6
                len1 = 2e-6
                len2 = 1e-6
		# create container for model
		model = moose.Neutral( 'model' )
                a0, b0, compt0 = makeCyl( '0', 1, radius, -len0, 0 )
                a1, b1, compt1 = makeCyl( '1', 2, radius, 0, len1 )
                a2, b2, compt2 = makeCyl( '2', 6, radius, len1, len1 + len2 )

                print('Volumes = ', compt0.volume, compt1.volume, compt2.volume)

		# create molecules and reactions
		reac0 = moose.Reac( '/model/compt1/reac0' )
		reac1 = moose.Reac( '/model/compt1/reac1' )

		# connect them up for reactions
		moose.connect( reac0, 'sub', b0, 'reac' )
		moose.connect( reac0, 'prd', b1, 'reac' )
		moose.connect( reac1, 'sub', b1, 'reac' )
		moose.connect( reac1, 'prd', b2, 'reac' )

		# Assign parameters
		reac0.Kf = 0.5
		reac0.Kb = 0.05
		reac1.Kf = 0.5
		reac1.Kb = 0.05

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		outputA0 = moose.Table2 ( '/model/graphs/concA0' )
		outputA1 = moose.Table2 ( '/model/graphs/concA1' )
		outputA2 = moose.Table2 ( '/model/graphs/concA2' )

		# connect up the tables
		moose.connect( outputA0, 'requestOut', a0, 'getConc' );
		moose.connect( outputA1, 'requestOut', a1, 'getConc' );
		moose.connect( outputA2, 'requestOut', a2, 'getConc' );

                # Build the solvers. No need for diffusion in this version.
                ksolve0 = moose.Ksolve( '/model/compt0/ksolve0' )
                ksolve1 = moose.Ksolve( '/model/compt1/ksolve1' )
                ksolve2 = moose.Ksolve( '/model/compt2/ksolve2' )
                dsolve0 = moose.Dsolve( '/model/compt0/dsolve0' )
                dsolve1 = moose.Dsolve( '/model/compt1/dsolve1' )
                dsolve2 = moose.Dsolve( '/model/compt2/dsolve2' )
                stoich0 = moose.Stoich( '/model/compt0/stoich0' )
                stoich1 = moose.Stoich( '/model/compt1/stoich1' )
                stoich2 = moose.Stoich( '/model/compt2/stoich2' )

                # Configure solvers
                stoich0.compartment = compt0
                stoich1.compartment = compt1
                stoich2.compartment = compt2
                stoich0.ksolve = ksolve0
                stoich1.ksolve = ksolve1
                stoich2.ksolve = ksolve2
                stoich0.dsolve = dsolve0
                stoich1.dsolve = dsolve1
                stoich2.dsolve = dsolve2
                stoich0.path = '/model/compt0/#'
                stoich1.path = '/model/compt1/#'
                stoich2.path = '/model/compt2/#'
                dsolve1.buildMeshJunctions( dsolve0 )
                dsolve1.buildMeshJunctions( dsolve2 )
                stoich1.buildXreacs( stoich0 )
                stoich1.buildXreacs( stoich2 )
                stoich0.filterXreacs()
                stoich1.filterXreacs()
                stoich2.filterXreacs()



def main():
    """
    This example shows how to maintain a conc gradient against diffusion ::

         compt0      compt1      compt 2
           a ......... a .......... a           [Diffusion between compts]
           |\          |\           |\
           |           |            |           [Reacs within compts]
          \|          \|           \|
           b0 <------->b1 <--------b2           [Reacs between compts]

          4x          2x           1x           [Ratios of vols of compts]

    If there is no diffusion then the ratio of concs should be 1:10:100
    If there is no x-compt reac, then clearly the concs should all be 
    the same, in this case they should be 2.0.
    If both are happening then the final concs are 1.4, 2.5, 3.4.
    """
    simdt = 0.1
    plotdt = 0.1
    runtime = 100.0

    makeModel()

    # MOOSE autoschedules everything.
    moose.reinit()
    moose.start( runtime ) # Run the model for 100 seconds.
    initTot = 0
    tot = 0
    for x in moose.wildcardFind( '/model/compt#/#[ISA=PoolBase]' ):
        print(x.name, x.conc)
        tot += x.n
        initTot += x.nInit
    print("Totals: expected = ", initTot, ", got: ", tot)

    # Iterate through all plots, dump their contents to data.plot.
    for x in moose.wildcardFind( '/model/graphs/conc#' ):
        t = numpy.linspace( 0, runtime, x.vector.size ) # sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()
    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
