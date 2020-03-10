# -*- coding: utf-8 -*-
#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2018 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU General Public License version 3 or later.
## See the file COPYING.LIB for the full notice.
##
## This file makes u + t <=====> s where u is in a different compt from t,s
#########################################################################


import math
import numpy as np
import moose
import moose.fixXreacs as fixXreacs

def makeModel():
    # create container for model
    num = 1 # number of compartments
    model = moose.Neutral( '/model' )
    compartment = moose.CylMesh( '/model/compartment' )
    compartment.x1 = 1.0e-6 # Set it to a 1 micron single-voxel cylinder

    # create molecules and reactions
    u = moose.Pool( '/model/compartment/u' )
    #####################################################################
    # Put in endo compartment. Add molecule s
    endo = moose.EndoMesh( '/model/endo' )
    endo.isMembraneBound = True
    endo.surround = compartment
    rXfer = moose.Reac( '/model/endo/rXfer' )
    et = moose.Pool( '/model/endo/t' )
    es = moose.Pool( '/model/endo/s' )
    #####################################################################
    moose.connect( rXfer, 'sub', u, 'reac' )
    moose.connect( rXfer, 'sub', et, 'reac' )
    moose.connect( rXfer, 'prd', es, 'reac' )
    u.concInit = 1.0
    et.concInit = 1.0

    #####################################################################
    # [u0] = 1 in compt, [t0] = 1 in endo, [s0] = 0
    # [u] + [s]/8 = [u0]  ; [t] + [s] = [t0]; nu + ns = nu0, nt + ns = nt0
    # At equil, numKf*nu*nt = numKb*ns
    # Express all # in terms of ns.
    # nu = nu0-ns; nt = nt0-ns      Also, nu0 = 8*nt0
    # So  numKf*(nu0-ns)*(nt0-ns) = numKb*ns
    # Target level is nt = ns = nt0/2
    # So numKf*( 8nt0 - nt0/2)*nt0/2 = numKb*nt0/2
    # So 7.5*nt0 = numKb/numKf
    rXfer.numKb = 0.1
    rXfer.numKf = 0.1/(7.5 * et.nInit)
    #print( "Rates = {}, {}".format( rXfer.Kf, rXfer.Kb ))
    #####################################################################
    fixXreacs.fixXreacs( '/model' )
    #fixXreacs.restoreXreacs( '/model' )
    #fixXreacs.fixXreacs( '/model' )
    #####################################################################

    # Make solvers
    ksolve = moose.Ksolve( '/model/compartment/ksolve' )
    dsolve = moose.Dsolve( '/model/compartment/dsolve' )
    eksolve = moose.Ksolve( '/model/endo/ksolve' )
    edsolve = moose.Dsolve( '/model/endo/dsolve' )

    stoich = moose.Stoich( '/model/compartment/stoich' )
    stoich.compartment = compartment
    stoich.ksolve = ksolve
    stoich.dsolve = dsolve
    stoich.path = "/model/compartment/##"
    assert( dsolve.numPools == 1 )

    estoich = moose.Stoich( '/model/endo/stoich' )
    estoich.compartment = endo
    estoich.ksolve = eksolve
    estoich.dsolve = edsolve
    estoich.path = "/model/endo/##"
    assert( edsolve.numPools == 3 )

    edsolve.buildMeshJunctions( dsolve )

    plot1 = moose.Table2( '/model/plot1' )
    plot2 = moose.Table2( '/model/plot2' )
    plot3 = moose.Table2( '/model/plot3' )
    moose.connect( plot1, 'requestOut', u, 'getN' )
    moose.connect( plot2, 'requestOut', et, 'getN' )
    moose.connect( plot3, 'requestOut', es, 'getN' )
    plot4 = moose.Table2( '/model/plot4' )
    plot5 = moose.Table2( '/model/plot5' )
    plot6 = moose.Table2( '/model/plot6' )
    moose.connect( plot4, 'requestOut', u, 'getConc' )
    moose.connect( plot5, 'requestOut', et, 'getConc' )
    moose.connect( plot6, 'requestOut', es, 'getConc' )

def almostEq( a, b ):
    #print a, b, (a-b)/(a+b)
    return abs(a-b)/(a+b) < 5e-5

def test_xreacs7():
    runtime = 100
    makeModel()
    moose.reinit()
    moose.start( runtime )

    assert( almostEq( moose.element( 'model/endo/t' ).conc,
        moose.element( '/model/endo/s' ).conc ) )
    moose.delete( '/model' )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    test_xreacs7()
