#########################################################################
# crossComptOscillator.py --- 
# 
# Filename:  crossComptOscillator.py
# Author: Upinder S. Bhalla
# Maintainer: 
# Created: Oct  12 16:26:05 2014 (+0530)
# Version: 
# Last-Updated: May 15 2017
#           By: 
#     Update #: 
# URL: 
# Keywords: 
# Compatibility: 
# 
# 
# Commentary: 
# 
# 
# 
# 
# Change log: Indentation clean up
#
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

def displayPlots():
    for x in moose.wildcardFind( '/model/graphs/#' ):
        t = numpy.arange( 0, x.vector.size, 1 ) #sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()

def getState( ksolve, state ):
    state.randomInit()
    moose.start( 0.1 ) # Run the model for 2 seconds.
    state.settle()
    '''
    scale = 1.0 / ( 1e-15 * 6.022e23 )
    for x in ksolve.nVec[0]:
            print x * scale,
    # print ksolve.nVec[0]
    print state.nIter, state.status, state.stateType, state.nNegEigenvalues, state.nPosEigenvalues, state.solutionStatus
     '''
    moose.start( 20.0 ) # Run model for 10 seconds, just for display


def main():
    """ This example sets up the kinetic solver and steady-state finder, on a bistable model.
    """
    # The wildcard uses # for single level, and ## for recursive.
    #compartment = makeModel()

    moose.loadModel( '../genesis/M1719.cspace', '/model', 'ee' )
    compartment = moose.element( 'model/kinetics' )
    compartment.name = 'compartment'
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    #ksolve.stoich = stoich
    stoich.path = "/model/compartment/##"
    state = moose.SteadyState( '/model/compartment/state' )

    moose.reinit()
    state.stoich = stoich
    #state.showMatrices()
    state.convergenceCriterion = 1e-7

    moose.le( '/model/graphs' )
    a = moose.element( '/model/compartment/a' )
    b = moose.element( '/model/compartment/b' )
    c = moose.element( '/model/compartment/c' )

    for i in range( 0, 100 ):
        getState( ksolve, state )
    
    moose.start( 100.0 ) # Run the model for 100 seconds.

    b = moose.element( '/model/compartment/b' )
    c = moose.element( '/model/compartment/c' )

    # move most molecules over to b
    b.conc = b.conc + c.conc * 0.95
    c.conc = c.conc * 0.05
    moose.start( 100.0 ) # Run the model for 100 seconds.

    # move most molecules back to a
    c.conc = c.conc + b.conc * 0.95
    b.conc = b.conc * 0.05
    moose.start( 100.0 ) # Run the model for 100 seconds.

    # Iterate through all plots, dump their contents to data.plot.
    displayPlots()

    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
