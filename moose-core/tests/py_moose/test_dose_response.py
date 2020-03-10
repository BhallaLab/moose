# -*- coding: utf-8 -*-
from __future__ import print_function, division

## Makes and plots the dose response curve for bistable models
## Author: Sahil Moza
## June 26, 2014
## Update:
## Friday 14 September 2018 05:48:42 PM IST
## Tunrned into a light-weight test by Dilawar Singh 

import os
import sys
import moose
import numpy as np

sdir_ = os.path.dirname( os.path.realpath( __file__ ) )
vals_ = [ ]

def setupSteadyState(simdt,plotDt):

    ksolve = moose.Ksolve( '/model/kinetics/ksolve' )
    stoich = moose.Stoich( '/model/kinetics/stoich' )
    stoich.compartment = moose.element('/model/kinetics')
    stoich.ksolve = ksolve
    stoich.path = "/model/kinetics/##"
    state = moose.SteadyState( '/model/kinetics/state' )
    moose.reinit()
    state.stoich = stoich
    state.showMatrices()
    state.convergenceCriterion = 1e-8
    return ksolve, state

def parseModelName(fileName):
    pos1=fileName.rfind('/')
    pos2=fileName.rfind('.')
    directory=fileName[:pos1]
    prefix=fileName[pos1+1:pos2]
    suffix=fileName[pos2+1:len(fileName)]
    return directory, prefix, suffix

# Solve for the steady state
def getState( ksolve, state, vol):
    scale = 1.0 / ( vol * 6.022e23 )
    moose.reinit()
    state.randomInit() # Removing random initial condition to systematically make Dose reponse curves.
    moose.start( 2.0 ) # Run the model for 2 seconds.
    a = moose.element( '/model/kinetics/a' ).conc
    vals_.append( a )
    state.settle()
    
    vector = []
    a = moose.element( '/model/kinetics/a' ).conc
    for x in ksolve.nVec[0]:
        vector.append( x * scale)
    failedSteadyState = any([np.isnan(x) for x in vector])
    if not (failedSteadyState):
        return state.stateType, state.solutionStatus, a, vector


def test_dose_response():
    # Setup parameters for simulation and plotting
    simdt= 1e-2
    plotDt= 1

    # Factors to change in the dose concentration in log scale
    factorExponent = 10  ## Base: ten raised to some power.
    factorBegin = -10
    factorEnd = 11
    factorStepsize = 2
    factorScale = 10.0 ## To scale up or down the factors

    # Load Model and set up the steady state solver.
    # model = sys.argv[1] # To load model from a file.
    model = os.path.join(sdir_, '..', 'data', '19085.cspace')
    modelPath, modelName, modelType = parseModelName(model)
    outputDir = modelPath
    
    modelId = moose.loadModel(model, 'model', 'ee')
    dosePath = '/model/kinetics/b/DabX' # The dose entity

    ksolve, state = setupSteadyState(simdt, plotDt)
    vol = moose.element('/model/kinetics' ).volume
    iterInit = 100
    solutionVector = []
    factorArr = []
    
    enz = moose.element(dosePath)
    init = float(enz.kcat) # Dose parameter
    
    # Change Dose here to .
    for factor in range(factorBegin, factorEnd, factorStepsize ):
        scale = factorExponent ** (factor/factorScale) 
        enz.kcat = init * scale     
        print( "scale={:.3f}\tkcat={:.3f}".format( scale, enz.kcat) )
        for num in range(iterInit):
            stateType, solStatus, a, vector = getState( ksolve, state, vol)
            if solStatus == 0:
                #solutionVector.append(vector[0]/sum(vector))
                solutionVector.append(a)
                factorArr.append(scale)   

    expected = (0.001411, 0.00092559)
    got = ( np.mean(vals_), np.std(vals_) )
    assert np.isclose(got, expected, atol=1e-4).all(), "Got %s, expected %s" % (got, expected)
    print( '[TEST1] Passed. Concentration of a is same' )
                
    joint = np.array([factorArr, solutionVector])
    joint = joint[:,joint[1,:].argsort()]
    got = np.mean( joint ), np.std( joint )
    expected = (1.2247, 2.46)
    # Close upto 2 decimal place is good enough.
    assert np.isclose(got, expected, atol=1e-2).all(), "Got %s, expected %s" % (got, expected)
    print( joint )

def main():
    test_dose_response()

if __name__ == '__main__':
    main()
