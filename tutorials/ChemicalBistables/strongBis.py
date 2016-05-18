#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import moose
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pylab
import numpy
import sys

def main():
	"""
	This example illustrates loading, and running a kinetic model 
	for a bistable system, defined in kkit format. 
	Defaults to the deterministic gsl method, you can pick the 
	stochastic one by 

	    ``python filename gssa``

	The model starts out equally poised between sides **b** and **c**. 
	Then there is a small molecular 'tap' to push it over to **b**.
	Then we apply a moderate push to show that it is now very stably in
	this state. it takes a strong push to take it over to **c**.
	Then it takes a strong push to take it back to **b**.
	This is a good model to use as the basis for running stochastically
	and examining how state stability is affected by changing volume.
	"""

	solver = "gsl"  # Pick any of gsl, gssa, ee..
	#solver = "gssa"  # Pick any of gsl, gssa, ee..
	#moose.seed( 1234 ) # Needed if stochastic.
	mfile = '../../genesis/M1719.g'
	runtime = 100.0
	if ( len( sys.argv ) >= 2 ):
                solver = sys.argv[1]
	modelId = moose.loadModel( mfile, 'model', solver )
        # Increase volume so that the stochastic solver gssa 
        # gives an interesting output
        compt = moose.element( '/model/kinetics' )
        compt.volume = 0.2e-19 
        r = moose.element( '/model/kinetics/equil' )

	moose.reinit()
	moose.start( runtime ) 
        r.Kf *= 1.1 # small tap to break symmetry
	moose.start( runtime/10 ) 
        r.Kf = r.Kb
	moose.start( runtime ) 

        r.Kb *= 2.0 # Moderate push does not tip it back.
	moose.start( runtime/10 ) 
        r.Kb = r.Kf
	moose.start( runtime ) 

        r.Kb *= 5.0 # Strong push does tip it over
	moose.start( runtime/10 ) 
        r.Kb = r.Kf
	moose.start( runtime ) 
        r.Kf *= 5.0 # Strong push tips it back.
	moose.start( runtime/10 ) 
        r.Kf = r.Kb
	moose.start( runtime ) 


	# Display all plots.
        img = mpimg.imread( 'strongBis.png' )
        fig = plt.figure( figsize=(12, 10 ) )
        png = fig.add_subplot( 211 )
        imgplot = plt.imshow( img )
        ax = fig.add_subplot( 212 )
	x = moose.wildcardFind( '/model/#graphs/conc#/#' )
        dt = moose.element( '/clock' ).tickDt[18]
        t = numpy.arange( 0, x[0].vector.size, 1 ) * dt
        ax.plot( t, x[0].vector, 'r-', label=x[0].name )
        ax.plot( t, x[1].vector, 'g-', label=x[1].name )
        ax.plot( t, x[2].vector, 'b-', label=x[2].name )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'Time (seconds)' )
        pylab.legend()
        pylab.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()
