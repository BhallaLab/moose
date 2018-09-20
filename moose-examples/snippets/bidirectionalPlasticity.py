#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import moose
import os
import sys
import pylab

def dumpPlots():
    fname = 'bidirectionalPlasticity.plot'
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/model/graphs/conc#/#,/model/moregraphs/conc#/#' ):
        moose.element( x[0] ).xplot( fname, x[0].name )

def displayPlots():
    clock = moose.element( '/clock' ) # look up global clock
    totR = moose.element( '/model/graphs/conc1/tot_PSD_R.Co' )
    PP1 = moose.element( '/model/moregraphs/conc4/PP1_active.Co' )
    Ca = moose.element( '/model/graphs/conc1/Ca.Co' )
    pylab.plot( pylab.linspace( 0, clock.currentTime, len( totR.vector )), totR.vector, label='membrane Receptor' )
    pylab.plot( pylab.linspace( 0, clock.currentTime, len( PP1.vector ) ), PP1.vector, label='active PP1' )
    pylab.plot( pylab.linspace( 0, clock.currentTime, len( Ca.vector ) ), Ca.vector, label='Ca' )
    pylab.legend()
    pylab.show()

def main():
    """
    This is a toy model of synaptic bidirectional plasticity. The model has
    a small bistable chemical switch, and a small set of reactions that
    decode calcium input. One can turn the switch on with short high
    calcium pulses (over 2 uM for about 10 sec). One can turn it back off
    again using a long, lower calcium pulse (0.2 uM, 2000 sec).
    """
    method = 'old_gssa'  # This is the Gillespie Stoichastic Systems Algorithm
    if ( len( sys.argv ) >= 2 ):
        method = sys.argv[1]
    if ( method == "gsl" ):
        method = "old_gsl"
    if ( method == "gssa" ):
        method = "old_gssa"
    # Load in the model and set up to use the specified method
    modelId = moose.loadModel( './stargazin_synapse.g', 'model', method )
    moose.start( 1000.0 ) # Run the model for 1000 seconds.
    Ca = moose.element( '/model/kinetics/BULK/Ca' )
    Ca.concInit = 2.0e-3         # Calcium stimulus for turnon
    moose.start( 20.0 ) # Run the model for 100 seconds.
    Ca.concInit = 0.08e-3        # Calcium back to baseline
    moose.start( 1000.0 ) # Let system settle for 1000 sec
    Ca.concInit = 0.2e-3        #         Calcium turnoff stimulus
    moose.start( 2000.0 ) # Run the model for 1000 seconds for turnoff
    Ca.concInit = 0.08e-3        # Calcium back to baseline
    moose.start( 2000.0 ) # Let system settle for 2000 sec

    displayPlots()
    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
