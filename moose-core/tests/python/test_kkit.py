# -*- coding: utf-8 -*-
import matplotlib
# Tests may be run over ssh without -X e.g. on travis.
matplotlib.use( 'Agg' )

import matplotlib.pyplot as plt
import numpy
import sys
import os
import moose

scriptdir = os.path.dirname( os.path.realpath( __file__ ) )
print( 'Script dir %s' % scriptdir )

def main():
        """ This example illustrates loading, running, and saving a kinetic model
        defined in kkit format. It uses a default kkit model but you can specify another using the command line ``python filename runtime solver``. We use the gsl solver here. The model already defines a couple of plots and sets the runtime to 20 seconds.
        """
        solver = "gsl"  # Pick any of gsl, gssa, ee..
        mfile = os.path.join( scriptdir, 'genesis/kkit_objects_example.g' )
        runtime = 20.0
        if ( len( sys.argv ) >= 3 ):
            if sys.argv[1][0] == '/':
                mfile = sys.argv[1]
            else:
                mfile = './genesis/' + sys.argv[1]
                runtime = float( sys.argv[2] )
        if ( len( sys.argv ) == 4 ):
                solver = sys.argv[3]
        modelId = moose.loadModel( mfile, 'model', solver )
        # Increase volume so that the stochastic solver gssa
        # gives an interesting output
        #compt = moose.element( '/model/kinetics' )
        #compt.volume = 1e-19

        moose.reinit()
        moose.start( runtime )

        # Report parameters
        '''
        for x in moose.wildcardFind( '/model/kinetics/##[ISA=PoolBase]' ):
                print x.name, x.nInit, x.concInit
        for x in moose.wildcardFind( '/model/kinetics/##[ISA=ReacBase]' ):
                print x.name, 'num: (', x.numKf, ', ',  x.numKb, '), conc: (', x.Kf, ', ', x.Kb, ')'
        for x in moose.wildcardFind('/model/kinetics/##[ISA=EnzBase]'):
                print x.name, '(', x.Km, ', ',  x.numKm, ', ', x.kcat, ')'
                '''

        # Display all plots.
        for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
            t = numpy.arange( 0, x.vector.size, 1 ) * x.dt
            plt.plot( t, x.vector, label=x.name )

        vals = x.vector
        stats = [ vals.min(), vals.max( ), vals.mean(), vals.std( ) ]
        expected = [ 0.0, 0.00040464 , 0.0001444 , 0.00013177 ]
        assert numpy.allclose(stats, expected, rtol=1e-4) , 'Got %s expected %s' % (stats, expected )
        plt.legend()
        plt.savefig( '%s.png' % sys.argv[0] )
        print( 'Wrote results to %s.png' % sys.argv[0] )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
