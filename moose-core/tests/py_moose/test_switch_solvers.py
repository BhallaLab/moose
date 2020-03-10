#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import os
import moose
print("[INFO ] using moose from %s" % moose.__file__)
import numpy as np 

sdir_ = os.path.dirname(os.path.realpath(__file__))


def runAndSavePlots( name ):
    runtime = 20.0
    moose.reinit()
    moose.start( runtime )
    pa = moose.Neutral( '/model/graphs/' + name )
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        if ( x.tick != -1 ):
            tabname = '/model/graphs/' + name + '/' + x.name + '.' + name
            y = moose.Table( tabname )
            y.vector = x.vector
            y.tick = -1

# Takes args ee, gsl, or gssa
def switchSolvers( solver ):
        if ( moose.exists( 'model/kinetics/stoich' ) ):
            moose.delete( '/model/kinetics/stoich' )
            moose.delete( '/model/kinetics/ksolve' )
        compt = moose.element( '/model/kinetics' )
        if ( solver == 'gsl' ):
            ksolve = moose.Ksolve( '/model/kinetics/ksolve' )
        if ( solver == 'gssa' ):
            ksolve = moose.Gsolve( '/model/kinetics/ksolve' )
        if ( solver != 'ee' ):
            stoich = moose.Stoich( '/model/kinetics/stoich' )
            stoich.compartment = compt
            stoich.ksolve = ksolve
            stoich.path = "/model/kinetics/##"

def test_switch_solvers():
    """
    At zero order, you can select the solver you want to use within the
    function moose.loadModel( filename, modelpath, solver ).
    Having loaded in the model, you can change the solver to use on it.
    This example illustrates how to assign and change solvers for a
    kinetic model. This process is necessary in two situations:

    * If we want to change the numerical method employed, for example,
      from deterministic to stochastic.
    * If we are already using a solver, and we have changed the reaction
      network by adding or removing molecules or reactions.

    Note that we do not have to change the solvers if the volume or
    reaction rates change.
    In this example the model is loaded in with a gsl solver. The
    sequence of solver calculations is:

    #. gsl
    #. ee
    #. gsl
    #. gssa
    #. gsl

    If you're removing the solvers, you just delete the stoichiometry
    object and the associated ksolve/gsolve. Should there be diffusion
    (a dsolve)then you should delete that too. If you're
    building the solvers up again, then you must do the following
    steps in order:

    #. build up the ksolve/gsolve and stoich (any order)
    #. Assign stoich.ksolve
    #. Assign stoich.path.

    See the Reaction-diffusion section should you want to do diffusion
    as well.

    """

    solver = "gsl"  # Pick any of gsl, gssa, ee..
    mfile = os.path.join(sdir_, '../data/kkit_objects_example.g')
    modelId = moose.loadModel( mfile, 'model', solver )
    # Increase volume so that the stochastic solver gssa
    # gives an interesting output
    compt = moose.element( '/model/kinetics' )
    compt.volume = 1e-19
    runAndSavePlots( 'gsl' )
    #########################################################
    switchSolvers( 'ee' )
    runAndSavePlots( 'ee' )
    #########################################################
    switchSolvers( 'gsl' )
    runAndSavePlots( 'gsl2' )
    #########################################################
    switchSolvers( 'gssa' )
    runAndSavePlots( 'gssa' )
    #########################################################
    switchSolvers( 'gsl' )
    runAndSavePlots( 'gsl3' )
    #########################################################

    # Display all plots.
    plotdt = moose.element( '/clock' ).tickDt[18]
    conc = []
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        conc.append(x.vector)
    conc = np.array(conc)
    assert conc.mean() > 0.0

    data = []
    for x in moose.wildcardFind( '/model/graphs/gsl/#' ):
        data.append(x.vector)
    gsl = np.array(data)
    assert abs(conc - gsl).sum() < 0.25

    data=[]
    for x in moose.wildcardFind( '/model/graphs/ee/#' ):
        data.append(x.vector)
    ee = np.array(data)
    assert abs(conc-ee).sum() < 0.2

    data=[]
    for x in moose.wildcardFind( '/model/graphs/gsl2/#' ):
        data.append(x.vector)
    gsl2 = np.array(data)
    assert abs(conc-gsl2).sum() == 0.0   # these are the same.

    data=[]
    for x in moose.wildcardFind( '/model/graphs/gssa/#' ):
        data.append(x.vector)
    gssa = np.array(data)
    assert abs(conc - gssa).sum() < 0.15, (conc - gssa).sum()
    assert gssa.shape == conc.shape == gsl.shape == ee.shape 

    print('all done')

def main():
    test_switch_solvers()

if __name__ == '__main__':
    main()
