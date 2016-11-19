## Makes and plots the dose response curve for bistable models
## Author: Sahil Moza
## June 26, 2014

import moose
import pylab
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
    moose.useClock(4, "/model/kinetics/##[]", "process")
    moose.setClock(4, float(simdt))
    moose.setClock(5, float(simdt))
    moose.useClock(5, '/model/kinetics/ksolve', 'process' )
    moose.useClock(8, '/model/graphs/#', 'process' )
    moose.setClock(8, float(plotDt))
 
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
      state.randomInit() # Removing random initial condition to systematically make Dose reponse curves.
      moose.start( 2.0 ) # Run the model for 2 seconds.
      state.settle()
      
      vector = []
      for x in ksolve.nVec[0]:
          vector.append( x * scale)
      moose.start( 10.0 ) # Run model for 10 seconds, just for display
      failedSteadyState = any([np.isnan(x) for x in vector])
      
      if not (failedSteadyState):
           return state.stateType, state.solutionStatus, vector


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
    iterInit = 1000
    solutionVector = []
    factorArr = []
    
    enz = moose.element(dosePath)
    init = enz.kcat # Dose parameter
    
    # Change Dose here to .
    for factor in range(factorBegin, factorEnd, factorStepsize ):
        scale = factorExponent ** (factor/factorScale) 
        enz.kcat = init * scale     
        print(factor)
        for num in range(iterInit):
            stateType, solStatus, vector = getState( ksolve, state, vol)
            if solStatus == 0:
                solutionVector.append(vector[0]/sum(vector))
                factorArr.append(scale)   
                
    joint = np.array([factorArr, solutionVector])
    joint = joint[:,joint[1,:].argsort()]
    
    # Plot dose response. 
    fig0 = plt.figure()
    pylab.semilogx(joint[0,:],joint[1,:],marker="o",label = 'concA')
    pylab.xlabel('Dose')
    pylab.ylabel('Response')
    pylab.suptitle('Dose-Reponse Curve for a bistable system')
    
    pylab.legend(loc=3)
    #plt.savefig(outputDir + "/" + modelName +"_doseResponse" + ".png")
    plt.show()
    #plt.close(fig0)
    quit()
    


if __name__ == '__main__':
     main()
