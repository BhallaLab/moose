#!/usr/bin/env python

"""rallpacks_cable_hhchannel.py: 

    A cable with 1000 compartments with HH-type channels in it.

Last modified: Wed May 21, 2014  09:51AM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import moose
from moose import utils

import os
import numpy as np
import matplotlib.pyplot as plt
import compartment as comp


EREST_ACT = -65e-3
per_ms = 1e3
dt = 5e-5
cable = []

def alphaM(A, B, V0, v):
    '''Compute alpha_m at point v

    aplha_m = A(v - v0 ) / (exp((v-V0)/B) - 1) 
    '''
    return (A*(v-V0) / (np.exp((v - V0)/B) -1 ))

def alphaN(A, B, V0, v):
    '''Compute alpha_n at point v 
    aplha_n = A(v-V0) / (exp((v-V0)/B) -1 )
    '''
    return alphaM(A, B, V0, v)

def betaM(A, B, V0, v):
    '''Compute beta_m at point v
    '''
    return (A * np.exp((v-V0)/B))

def betaN(A, B, V0, v):
    return betaM(A, B, V0, v)

def alphaH(A, B, V0, v):
    '''Compute alpha_h at point v 
    '''
    return (A * np.exp(( v - V0) / B))

def behaH(A, B, V0, v):
    '''Compute beta_h at point v 
    '''
    return (A * np.exp((v-V0)/B) + 1)

def createChannel(species, path, **kwargs):
    """Create a channel """
    if species == 'na':
        return sodiumChannel( path, **kwargs)
    elif species == 'ca':
        channel.Xpower = 4
    else:
        utils.dump("FATAL", "Unsupported channel type: {}".format(species))
        raise RuntimeError("Unsupported species of chanel")

def create_na_chan(parent='/library', name='na', vmin=-110e-3, vmax=50e-3, vdivs=3000):
    """Create a Hodhkin-Huxley Na channel under `parent`.
    
    vmin, vmax, vdivs: voltage range and number of divisions for gate tables
    
    """
    na = moose.HHChannel('%s/%s' % (parent, name))
    na.Xpower = 3
    na.Ypower = 1
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT
    m_alpha = per_ms * (25 - v * 1e3) / (10 * (np.exp((25 - v * 1e3) / 10) - 1))
    m_beta = per_ms * 4 * np.exp(- v * 1e3/ 18)
    m_gate = moose.element('%s/gateX' % (na.path))
    m_gate.min = vmin
    m_gate.max = vmax
    m_gate.divs = vdivs
    m_gate.tableA = m_alpha
    m_gate.tableB = m_alpha + m_beta
    h_alpha = per_ms * 0.07 * np.exp(-v / 20e-3)
    h_beta = per_ms * 1/(np.exp((30e-3 - v) / 10e-3) + 1)
    h_gate = moose.element('%s/gateY' % (na.path))
    h_gate.min = vmin
    h_gate.max = vmax
    h_gate.divs = vdivs
    h_gate.tableA = h_alpha
    h_gate.tableB = h_alpha + h_beta
    return na

def create_k_chan(parent='/library', name='k', vmin=-120e-3, vmax=40e-3, vdivs=3000):
    """Create a Hodhkin-Huxley K channel under `parent`.
    
    vmin, vmax, vdivs: voltage range and number of divisions for gate tables
    
    """
    k = moose.HHChannel('%s/%s' % (parent, name))
    k.Xpower = 4
    v = np.linspace(vmin, vmax, vdivs+1) - EREST_ACT
    n_alpha = per_ms * (10 - v * 1e3)/(100 * (np.exp((10 - v * 1e3)/10) - 1))
    n_beta = per_ms * 0.125 * np.exp(- v * 1e3 / 80)
    n_gate = moose.element('%s/gateX' % (k.path))
    n_gate.min = vmin
    n_gate.max = vmax
    n_gate.divs = vdivs
    n_gate.tableA = n_alpha
    n_gate.tableB = n_alpha + n_beta
    return k
    
def creaetHHComp(parent='/library', name='hhcomp', diameter=1e-6, length=1e-6):
    """Create a compartment with Hodgkin-Huxley type ion channels (Na and
    K).

    Returns a 3-tuple: (compartment, nachannel, kchannel)

    """
    compPath = '{}/{}'.format(parent, name)
    mc = comp.MooseCompartment( compPath, length, diameter, {})
    c = mc.mc_
    sarea = mc.surfaceArea

    if moose.exists('/library/na'):
        moose.copy('/library/na', c.path, 'na')
    else:
        create_na_chan(parent = c.path)

    na = moose.element('%s/na' % (c.path))

    # Na-conductance 120 mS/cm^2
    na.Gbar = 120e-3 * sarea * 1e4 
    na.Ek = 115e-3 + EREST_ACT

    moose.connect(c, 'channel', na, 'channel')
    if moose.exists('/library/k'):
        moose.copy('/library/k', c.path, 'k')
    else:
        create_k_chan(parent = c.path)

    k = moose.element('%s/k' % (c.path))
    # K-conductance 36 mS/cm^2
    k.Gbar = 36e-3 * sarea * 1e4 
    k.Ek = -12e-3 + EREST_ACT
    moose.connect(c, 'channel', k, 'channel')
    return (c, na, k)

def makeCable(args):
    global cable
    ncomp = args['ncomp']
    moose.Neutral('/cable')
    for i in range( ncomp ):
        compName = 'hhcomp{}'.format(i)
        hhComp = creaetHHComp( '/cable', compName )
        cable.append( hhComp[0] )

    # connect the cable.
    for i, hhc in enumerate(cable[0:-1]):
        hhc.connect('axial', cable[i+1], 'raxial')


def setupDUT( dt ):
    global cable
    comp = cable[0]
    data = moose.Neutral('/data')
    pg = moose.PulseGen('/data/pg')
    pg.firstWidth = 25e-3
    pg.firstLevel = 1e-10
    moose.connect(pg, 'output', comp, 'injectMsg')
    setupClocks( dt )
    
def setupClocks( dt ):
    moose.setClock(0, dt)
    moose.setClock(1, dt)

def setupSolver( hsolveDt ):
    hsolvePath = '/hsolve'
    hsolve = moose.HSolve( hsolvePath )
    hsolve.dt = hsolveDt
    hsolve.target = '/cable'
    moose.useClock(1, hsolvePath, 'process')

def simulate( runTime, dt):
    """ Simulate the cable """
    moose.useClock(0, '/cable/##', 'process')
    moose.useClock(0, '/cable/##', 'init')
    moose.useClock(1, '/##', 'process')
    moose.reinit()
    setupSolver( hsolveDt = dt )
    utils.verify()
    moose.start( runTime )

def main(args):
    global cable
    dt = args['dt']
    makeCable(args)
    setupDUT( dt )
    table0 = utils.recordAt( '/table0', cable[0], 'vm')
    table1 = utils.recordAt( '/table1', cable[-1], 'vm')
    simulate( args['run_time'], dt )
    utils.plotTables( [ table0, table1 ], file = args['output'], xscale = dt )

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
            description = 'Rallpacks3: A cable with n compartment with HHChannel'
            )
    parser.add_argument( '--tau'
            , default = 0.04
            , type = float
            , help = 'Time constant of membrane'
            )
    parser.add_argument( '--run_time'
            , default = 0.25
            , type = float
            , help = 'Simulation run time'
            )
    parser.add_argument( '--dt'
            , default = 5e-5
            , type = float
            , help = 'Step time during simulation'
            )
    parser.add_argument( '--Em'
            , default = -65e-3
            , type = float
            , help = 'Resting potential of membrane'
            )
    parser.add_argument( '--RA'
            , default = 1.0
            , type = float
            , help = 'Axial resistivity'
            )
    parser.add_argument( '--lambda'
            , default = 1e-3
            , type = float
            , help = 'Lambda, what else?'
            )
    parser.add_argument( '--x'
            , default = 1e-3
            , type = float
            , help = 'You should record membrane potential somewhere, right?'
            ) 
    parser.add_argument( '--length'
            , default = 1e-3
            , type = float
            , help = 'Length of the cable'
            )
    parser.add_argument( '--diameter'
            , default = 1e-6
            , type = float
            , help = 'Diameter of cable'
            )
    parser.add_argument( '--inj'
            , default = 1e-10
            , type = float
            , help = 'Current injected at one end of the cable'
            )
    parser.add_argument( '--ncomp'
            , default = 1000
            , type = int
            , help = 'No of compartment in cable'
            )
    parser.add_argument( '--output'
            , default = None
            , type = str
            , help = 'Store simulation results to this file'
            )
    args = parser.parse_args()
    main( vars(args) )

