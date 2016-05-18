#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################



import math
import matplotlib.pyplot as plt
import numpy
import moose

def makeModel():
		# create container for model
		model = moose.Neutral( 'model' )
		compt0 = moose.CubeMesh( '/model/compt0' )
		compt0.volume = 1e-15
		compt1 = moose.CubeMesh( '/model/compt1' )
		compt1.volume = 1e-16
		compt2 = moose.CubeMesh( '/model/compt2' )
		compt2.volume = 1e-17

                # Position containers so that they abut each other, with
                # compt1 in the middle.
                side = compt1.dy
                compt0.y1 += side
                compt0.y0 += side
                compt2.x1 += side
                compt2.x0 += side
                print('Volumes = ', compt0.volume, compt1.volume, compt2.volume)

		# create molecules and reactions
		a = moose.Pool( '/model/compt0/a' )
		b = moose.Pool( '/model/compt1/b' )
		c = moose.Pool( '/model/compt2/c' )
		reac0 = moose.Reac( '/model/compt1/reac0' )
		reac1 = moose.Reac( '/model/compt1/reac1' )

		# connect them up for reactions
		moose.connect( reac0, 'sub', a, 'reac' )
		moose.connect( reac0, 'prd', b, 'reac' )
		moose.connect( reac1, 'sub', b, 'reac' )
		moose.connect( reac1, 'prd', c, 'reac' )

		# Assign parameters
		a.concInit = 1
		b.concInit = 12.1
		c.concInit = 1
		reac0.Kf = 0.1
		reac0.Kb = 0.1
		reac1.Kf = 0.1
		reac1.Kb = 0.1

		# Create the output tables
		graphs = moose.Neutral( '/model/graphs' )
		outputA = moose.Table2 ( '/model/graphs/concA' )
		outputB = moose.Table2 ( '/model/graphs/concB' )
		outputC = moose.Table2 ( '/model/graphs/concC' )

		# connect up the tables
		moose.connect( outputA, 'requestOut', a, 'getConc' );
		moose.connect( outputB, 'requestOut', b, 'getConc' );
		moose.connect( outputC, 'requestOut', c, 'getConc' );

                # Build the solvers. No need for diffusion in this version.
                ksolve0 = moose.Ksolve( '/model/compt0/ksolve0' )
                ksolve1 = moose.Ksolve( '/model/compt1/ksolve1' )
                ksolve2 = moose.Ksolve( '/model/compt2/ksolve2' )
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
                stoich0.path = '/model/compt0/#'
                stoich1.path = '/model/compt1/#'
                stoich2.path = '/model/compt2/#'
                stoich1.buildXreacs( stoich0 )
                stoich1.buildXreacs( stoich2 )
                stoich0.filterXreacs()
                stoich1.filterXreacs()
                stoich2.filterXreacs()



def main():
    """
    This example illustrates a simple cross compartment reaction::

        a <===> b <===> c

    Here each molecule is in a different compartment.
    The initial conditions are such that the end conc on all compartments
    should be 2.0.
    The time course depends on which compartment the Reac object is 
    embedded in.
    The cleanest thing numerically and also conceptually is to have both 
    reactions in the same compartment, in this case the middle one
    (**compt1**).
    The initial conditions have a lot of **B**. The equilibrium with 
    **C** is fast and so **C** shoots up and passes **B**, peaking at 
    about (2.5,9). This is also just 
    about the crossover point.
    **A** starts low and slowly climbs up to equilibrate.

    If we put **reac0** in **compt0** and **reac1** in **compt1**, 
    it behaves the same
    qualitiatively but now the peak is at around (1, 5.2)

    This configuration of reactions makes sense from the viewpoint of 
    having the
    reactions always in the compartment with the smaller volume, which is
    important if we need to have junctions where many small voxels talk to
    one big voxel in another compartment.

    Note that putting the reacs in other compartments doesn't work and in
    some combinations (e.g., **reac0** in **compt0** and **reac1** in 
    **compt2**) give
    numerical instability.
    """
    simdt = 0.1
    plotdt = 0.1
    runtime = 100.0

    makeModel()

    # MOOSE autoschedules everything.
    moose.reinit()
    moose.start( runtime ) # Run the model for 100 seconds.
    print("All concs should converge to 2.0 even though vols differ:")
    for x in moose.wildcardFind( '/model/compt#/#[ISA=PoolBase]' ):
        print(x.name, x.conc)

    # FIXME: Plotting causes seg-fault.
    ## Iterate through all plots, dump their contents to data.plot.
    #for x in moose.wildcardFind( '/model/graphs/conc#' ):
    #    t = numpy.linspace( 0, runtime, x.vector.size ) # sec
    #    plt.plot( t, x.vector, label=x.name )
    #plt.legend()
    #plt.show()
    #quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
