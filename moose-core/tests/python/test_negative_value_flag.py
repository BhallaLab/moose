# test_negative_value_flag.py ---
#
# Filename: test_negative_value_flag.py
# Description:
# Author: Upi Bhalla
# Maintainer: Dilawar Singh <dilawars@ncbs.res.in>
# Created: Sat Oct 04 12:14:15 2014 (+0530)

import moose
import numpy as np
import sys
import os

scriptDir = os.path.dirname( os.path.realpath( __file__ ) )

def main():
    """
    This example illustrates loading, running, and saving a kinetic
    model defined in kkit format. It uses a default kkit model but
    you can specify another using the command line
        ``python filename runtime solver``.
    We use the gsl solver here.
    The model already defines a couple of plots and sets the runtime 20 secs.
    """

    solver = "gsl"  # Pick any of gsl, gssa, ee..
    mfile = os.path.join( scriptDir, './genesis/acc11.g' )
    runtime = 1000.0
    if ( len( sys.argv ) >= 3 ):
        if sys.argv[1][0] == '/':
            mfile = sys.argv[1]
        else:
            mfile = sys.argv[1]
            runtime = float( sys.argv[2] )
    if ( len( sys.argv ) == 4 ):
            solver = sys.argv[3]

    modelId = moose.loadModel( mfile, 'model')
    moose.mooseaddChemSolver('model',solver)
    moose.element( '/model/kinetics/neuroNOS/nNOS.arg' ).concInit = 0.1
    moose.reinit()
    moose.start( runtime )

    # Display all plots.
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        t = np.arange( 0, x.vector.size, 1 ) * x.dt
        print( x.vector )
        if x.vector.size > 0:
            assert min( x.vector ) >= 0.0, min(x.vector)
        assert x.vector.size in [0, 10001], x.vector.size

    ########################################################
    # Run it again with negative values allowed
    moose.element( '/model/kinetics/stoich' ).allowNegative = True
    moose.reinit()
    moose.start( runtime )
    oneValIsBelowZero = False
    allVals = [ ]
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        t = np.arange( 0, x.vector.size, 1 ) * x.dt
        if x.vector.size > 0:
            allVals.append( min( x.vector ) )
            if min( x.vector ) <= 0.0:
                oneValIsBelowZero = True

        if allVals:
            assert oneValIsBelowZero, "No value is negative: %s" % allVals 
        assert x.vector.size in [0, 10001], x.vector.size

if __name__ == '__main__':
    main()
