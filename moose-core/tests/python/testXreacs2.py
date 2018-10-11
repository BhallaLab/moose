# -*- coding: utf-8 -*-
import os
mpl.rcParams['text.usetex'] = False
import moose
import numpy as np
import moose.fixXreacs as fixXreacs

def countCrossings( plot, thresh ):
    vec = moose.element( plot ).vector
    #print (vec[:-1] < thresh)
    return sum( (vec[:-1] < thresh) * (vec[1:] >= thresh ) )

def main( standalone = False ):
        mfile = os.path.join( os.path.dirname( __file__), 'OSC_diff_vols.g' )
        runtime = 4000.0
        modelId = moose.loadModel( mfile, 'model', 'ee' )
        kin = moose.element( '/model/kinetics' )
        compt1 = moose.element( '/model/compartment_1' )
        compt1.x1 += kin.x1
        compt1.x0 += kin.x1
        fixXreacs.fixXreacs( '/model' )
        #fixXreacs.restoreXreacs( '/model' )
        #fixXreacs.fixXreacs( '/model' )

        ks1 = moose.Ksolve( '/model/kinetics/ksolve' )
        ds1 = moose.Dsolve( '/model/kinetics/dsolve' )
        s1 = moose.Stoich( '/model/kinetics/stoich' )
        s1.compartment = moose.element( '/model/kinetics' )
        s1.ksolve = ks1
        s1.dsolve = ds1
        s1.path = '/model/kinetics/##'

        ks2 = moose.Ksolve( '/model/compartment_1/ksolve' )
        ds2 = moose.Dsolve( '/model/compartment_1/dsolve' )
        s2 = moose.Stoich( '/model/compartment_1/stoich' )
        s2.compartment = moose.element( '/model/compartment_1' )
        s2.ksolve = ks2
        s2.dsolve = ds2
        s2.path = '/model/compartment_1/##'

        ds2.buildMeshJunctions( ds1 )

        moose.reinit()
        moose.start( runtime )
        # I don't have an analytic way to assess oscillations
        assert( countCrossings( '/model/graphs/conc2/M.Co', 0.001 ) == 4 )

        if standalone:
            # Display all plots.
            for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
                t = np.arange( 0, x.vector.size, 1 ) * x.dt
                plt.plot( t, x.vector, label=x.name )
            plt.legend()
            plt.show()
        moose.delete( '/model' )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main( standalone = True )
