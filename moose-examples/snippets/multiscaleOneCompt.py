#########################################################################
## multiscaleOneCompt.py --- 
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU General Public License version 2 or later.
## See the file COPYING.LIB for the full notice.
#########################################################################

import sys
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'
import math
import numpy
import pylab
import matplotlib.pyplot as plt
import moose
import proto18

#EREST_ACT = -70e-3

def loadElec():
    library = moose.Neutral( '/library' )
    moose.setCwe( '/library' )
    proto18.make_Ca()
    proto18.make_Ca_conc()
    proto18.make_Na()
    proto18.make_K_DR()
    proto18.make_K_A()
    # Disable all the prototypes.
    for x in moose.wildcardFind( "/library/##" ):
        x.tick = -1
    model = moose.Neutral( '/model' )
    cellId = moose.loadModel( 'soma.p', '/model/elec', "Neutral" )
    moose.setCwe( '/' )
    '''
    hsolve = moose.HSolve( '/model/elec/hsolve' )
    hsolve.dt = 50.0e-6
    hsolve.target = '/model/elec/soma'
    moose.reinit()
    '''
    return cellId

def loadChem():
    chem = moose.Neutral( '/model/chem' )
    modelId = moose.loadModel( '../genesis/chanPhosphByCaMKII.g', '/model/chem', 'gsl' )
    nmstoich = moose.element( '/model/chem/kinetics/stoich' )

def makeModel():
    loadElec()
    loadChem()
    makeAdaptors()

def makeAdaptors():
    ##################################################################
    # set up adaptor for elec model Ca -> chem model Ca
    # Here it is easy because we don't have to deal with different
    # sizes of electrical and chemical compartments. 
    adaptCa = moose.Adaptor( '/model/chem/kinetics/adaptCa' )
    chemCa = moose.element( '/model/chem/kinetics/Ca' )
    elecCa = moose.element( '/model/elec/soma/Ca_conc' )
    moose.connect( elecCa, 'concOut', adaptCa, 'input' )
    moose.connect( adaptCa, 'output', chemCa, 'setConc' )
    adaptCa.inputOffset = 0.0    # 
    adaptCa.outputOffset = 0.00008    # 80 nM offset in chem.
    adaptCa.scale = 0.0008   

    # set up adaptor for chem model chan -> elec model chan.
    adaptChan = moose.Adaptor( '/model/chem/kinetics/adaptChan' )
    chemChan = moose.element( '/model/chem/kinetics/chan' )
    elecChan = moose.element( '/model/elec/soma/K_A' )
    # The Adaptor has to request the output conc of the chemical pool,
    # since there isn't an output message to deliver this value.
    moose.connect( adaptChan, 'requestOut', chemChan, 'getConc' )
    moose.connect( adaptChan, 'output', elecChan, 'setGbar' )
    adaptChan.inputOffset = 0.0    # 
    adaptChan.outputOffset = 0.0
    adaptChan.scale = 1e-5    #

def addPlot( objpath, field, plot, tick ):
    if moose.exists( objpath ):
        tab = moose.Table( '/graphs/' + plot )
        obj = moose.element( objpath )
        moose.connect( tab, 'requestOut', obj, field )
        tab.tick = tick
        return tab
    else:
        print "failed in addPlot(", objpath, field, plot, tick, ")"
        return 0

def main():
    """
    This example builds a simple multiscale model involving 
    electrical and chemical signaling, but without spatial dimensions.
    The electrical cell model is in a single compartment and has
    voltage-gated channels, including a voltage-gated Ca channel for
    Ca influx, and a K_A channel which is regulated by the chemical 
    pathways.

    The chemical model has calcium activating Calmodulin which activates
    CaM-Kinase II. The kinase phosphorylates the K_A channel to inactivate
    it.

    The net effect of the multiscale activity is a positive feedback
    loop where activity increases Ca, which activates the kinase, 
    which reduces K_A, leading to increased excitability of the cell. 

    In this example this results
    in a bistable neuron. In the resting state the cell does not fire,
    but if it is activated by a current pulse the cell will continue to
    fire even after the current is turned off. Application of an
    inhibitory current restores the cell to its silent state.

    Both the electrical and chemical models are loaded in from model
    description files, and these files could be replaced if one wished
    to define different models. However, there
    are model-specific Adaptor objects needed to map activity between the
    models of the two kinds. The Adaptors connect specific model entities
    between the two models. Here one Adaptor connects the electrical 
    Ca_conc object to the chemical Ca pool. The other Adaptor connects
    the chemical pool representing the K_A channel to its conductance 
    term in the electrical model.
    """

    runtime = 4
    chemDt = 0.005
    ePlotDt = 0.5e-3
    cPlotDt = 0.0025

    makeModel()

    moose.setClock( 8, ePlotDt )
    moose.setClock( 18, cPlotDt )
    for i in range( 10, 18 ):
        moose.setClock( i, chemDt )
    graphs = moose.Neutral( '/graphs' )
    caplot = addPlot( '/model/elec/soma/Ca_conc', 'getCa', 'somaCa', 8 )
    vmplot = addPlot( '/model/elec/soma', 'getVm', 'somaVm', 8 )
    ikplot = addPlot( '/model/elec/soma/K_A', 'getIk', 'KAIk', 8 )
    addPlot( '/model/chem/kinetics/chan', 'getConc', 'chan', 18 )
    addPlot( '/model/chem/kinetics/Ca', 'getConc', 'Ca', 18 )
    addPlot( '/model/chem/kinetics/CaM', 'getConc', 'CaM', 18 )
    addPlot( '/model/chem/kinetics/Ca_CaM_CaMKII', 'getConc', 'enz', 18 )
    hsolve = moose.HSolve( '/model/elec/hsolve' )
    hsolve.dt = 50.0e-6
    hsolve.target = '/model/elec/soma'
    moose.reinit()
    moose.element( '/model/elec/soma' ).inject = 0e-12
    moose.start( runtime )
    moose.element( '/model/elec/soma' ).inject = 1e-12
    moose.start( runtime )
    moose.element( '/model/elec/soma' ).inject = 0e-12
    moose.start( runtime )
    moose.element( '/model/elec/soma' ).inject = -1e-12
    moose.start( runtime )
    moose.element( '/model/elec/soma' ).inject = 0e-12
    moose.start( runtime )
    fig = plt.figure( figsize = (12,10) )
    t = numpy.arange( 0, caplot.vector.size, 1 ) * caplot.dt
    p1 = fig.add_subplot( 411 )
    p1.plot( t, caplot.vector, label="Ca elec" )
    p1.legend()
    p2 = fig.add_subplot( 412 )
    p2.plot( t, vmplot.vector, label="Vm" )
    p2.legend()
    p3 = fig.add_subplot( 413 )
    p3.plot( t, ikplot.vector, label="Ik for K_A" )
    p3.legend()
    p4 = fig.add_subplot( 414 )
    for x in moose.wildcardFind( '/graphs/#[FIELD(tick)=18]' ):
        t = numpy.arange( 0, x.vector.size, 1 ) * x.dt
        p4.plot( t, x.vector, label=x.name )
    p4.legend()
    plt.show()
    quit()

if __name__ == '__main__':
	main()
