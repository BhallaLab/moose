## Makes and plots the dose response curve for bistable models
## Author: Sahil Moza
## June 26, 2014

import moose
import numpy as np
from matplotlib import pyplot as plt

def setupSteadyState(simdt,plotDt):

    ksolve = moose.Ksolve( '/model/kinetics/ksolve' )
    stoich = moose.Stoich( '/model/kinetics/stoich' )
    stoich.compartment = moose.element('/model/kinetics')

    stoich.ksolve = ksolve
    #ksolve.stoich = stoich
    stoich.path = "/model/kinetics/##"
    state = moose.SteadyState( '/model/kinetics/state' )
   
    #### Set clocks here
    #moose.useClock(4, "/model/kinetics/##[]", "process")
    #moose.setClock(4, float(simdt))
    #moose.setClock(5, float(simdt))
    #moose.useClock(5, '/model/kinetics/ksolve', 'process' )
    #moose.useClock(8, '/model/graphs/#', 'process' )
    #moose.setClock(8, float(plotDt))
 
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
      state.settle()
      
      vector = []
      a = moose.element( '/model/kinetics/a' ).conc
      #print a
      for x in ksolve.nVec[0]:
          vector.append( x * scale)
      moose.start( 10.0 ) # Run model for 10 seconds, just for display
      failedSteadyState = any([np.isnan(x) for x in vector])
      
      if not (failedSteadyState):
           return state.stateType, state.solutionStatus, a, vector


def main():
    # Setup parameters for simulation and plotting
    simdt= 1e-2
    plotDt= 1

    # Factors to change in the dose concentration in log scale
    factorExponent = 10  ## Base: ten raised to some power.
    factorBegin = -20
    factorEnd = 21
    factorStepsize = 1
    factorScale = 10.0 ## To scale up or down the factors

    # Load Model and set up the steady state solver.
    # model = sys.argv[1] # To load model from a file.
    model = './19085.cspace'
    modelPath, modelName, modelType = parseModelName(model)
    outputDir = modelPath
    
    modelId = moose.loadModel(model, 'model', 'ee')
    dosePath = '/model/kinetics/b/DabX' # The dose entity

    ksolve, state = setupSteadyState( simdt, plotDt)
    vol = moose.element( '/model/kinetics' ).volume
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
                
    joint = np.array([factorArr, solutionVector])
    joint = joint[:,joint[1,:].argsort()]
    
    # Plot dose response. Remove NaN from the values else plotting will fail.
    ax = plt.subplot()
    # plt.semilogx was failing. not sure why. That is why this convoluted
    # approach.
    ax.plot( joint[0,:], joint[1,:] , marker="o", label = 'concA')
    ax.set_xscale( 'log' )
    plt.xlabel('Dose')
    plt.ylabel('Response')
    plt.suptitle('Dose-Reponse Curve for a bistable system')
    
    plt.legend(loc=3)
    #plt.savefig(outputDir + "/" + modelName +"_doseResponse" + ".png")
    plt.show()
    quit()
    


if __name__ == '__main__':
     main()
