# -*- coding: utf-8 -*-

import math
import pylab
import numpy
import moose
import moose.utils as mu

try:
    from collections import OrderedDict
    records_ = OrderedDict()
except:
    records_ = {}

def main():
    compartment = makeModel()
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    stoich.path = "/model/compartment/##"
    state = moose.SteadyState( '/model/compartment/state' )

    moose.reinit()
    state.stoich = stoich
    state.convergenceCriterion = 1e-6
    moose.seed( 111 ) # Used when generating the samples in state space
    moose.reinit()
    moose.start(10)
    plot( 10)

def add_table(elem):
    global records_
    tab = moose.Table2('%s/tab' % elem.path)
    moose.connect(tab, 'requestOut', elem, 'getConc')
    records_[elem.name] = tab

def plot( numPlots ):
    global records_ 
    toPlots = list(records_.keys())[0:numPlots]
    newPlots = {}
    for i in toPlots:
        newPlots[i] = records_[i]
    mu.plotRecords(newPlots, subplot = True)

def makeModel():
    """ This function creates a bistable reaction system using explicit
    MOOSE calls rather than load from a file.
    The reaction is::

        a ---b---> 2b    # b catalyzes a to form more of b.
        2b ---c---> a    # c catalyzes b to form a.
        a <======> 2b    # a interconverts to b.

    """
    # create container for model
    model = moose.Neutral( 'model' )
    compartment = moose.CubeMesh( '/model/compartment' )
    compartment.volume = 1e-15
    # the mesh is created automatically by the compartment
    mesh = moose.element( '/model/compartment/mesh' ) 

    # create molecules and reactions
    size = 100
    for i in range(size):
        a = moose.Pool( '/model/compartment/a%s' % i )
        b = moose.Pool( '/model/compartment/b%s'  % i )
        c = moose.Pool( '/model/compartment/c%s' % i )
        enz1 = moose.Enz( '%s/enz1' % a.path )
        enz2 = moose.Enz( '%s/enz2' % c.path )
        cplx1 = moose.Pool( '%s/cplx' % enz1.path )
        cplx2 = moose.Pool( '%s/cplx' % enz2.path )
        reac = moose.Reac( '/model/compartment/reac%s' % i)

        # connect them up for reactions
        moose.connect( enz1, 'sub', a, 'reac' )
        moose.connect( enz1, 'prd', b, 'reac' )
        moose.connect( enz1, 'prd', b, 'reac' ) # Note 2 molecules of b.
        moose.connect( enz1, 'enz', b, 'reac' )
        moose.connect( enz1, 'cplx', cplx1, 'reac' )

        moose.connect( enz2, 'sub', b, 'reac' )
        moose.connect( enz2, 'sub', b, 'reac' ) # Note 2 molecules of b.
        moose.connect( enz2, 'prd', a, 'reac' )
        moose.connect( enz2, 'enz', c, 'reac' )
        moose.connect( enz2, 'cplx', cplx2, 'reac' )

        moose.connect( reac, 'sub', a, 'reac' )
        moose.connect( reac, 'prd', b, 'reac' )
        moose.connect( reac, 'prd', b, 'reac' ) # Note 2 order in b.
        add_table(a)
        add_table(b)
        add_table(c)
        # Assign parameters
        a.concInit = 1
        b.concInit = 0
        c.concInit = 0.01
        enz1.kcat = 0.4
        enz1.Km = 4
        enz2.kcat = 0.6
        enz2.Km = 0.01
        reac.Kf = 0.001
        reac.Kb = 0.01
    return compartment

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
