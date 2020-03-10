# Rdesigneur example 4.1
# Making an axon with a propagating action potential.
# Turned into a doctest

import matplotlib
matplotlib.use('Agg')
import numpy as np
import moose
import rdesigneur as rd

numAxonSegments = 200
comptLen = 10e-6
comptDia = 1e-6
RM = 1.0
RA = 10.0
CM = 0.01

expected = np.array([-0.065     , -0.06983536, -0.07267512, -0.07384024, -0.07422599,
           -0.07426641, -0.07057581, -0.06788169, -0.06541139, -0.06203896,
           -0.04844496,  0.02185485, -0.02780464, -0.07129777, -0.07606423,
           -0.07568317, -0.07514661, -0.07449245, -0.07373675, -0.07290267,
           -0.07201665, -0.07110338, -0.07018217, -0.06926533, -0.06835791,
           -0.06745804, -0.0665565 , -0.06563355, -0.06464782, -0.06349831,
           -0.06186082, -0.05798547,  0.00504934, -0.00513958, -0.04914883,
           -0.07588121, -0.07586509, -0.07538907, -0.07478491, -0.07407192,
           -0.0732706 ])

def makeAxonProto():
    axon = moose.Neuron( '/library/axon' )
    prev = rd.buildCompt( axon, 'soma', RM = RM, RA = RA, CM = CM, dia = 10e-6, x=0, dx=comptLen)
    theta = 0
    x = comptLen
    y = 0.0

    for i in range( numAxonSegments ):
        dx = comptLen * np.cos( theta )
        dy = comptLen * np.sin( theta )
        r = np.sqrt( x * x + y * y )
        theta += comptLen / r
        compt = rd.buildCompt( axon, 'axon' + str(i), RM = RM, RA = RA, CM = CM, x = x, y = y, dx = dx, dy = dy, dia = comptDia )
        moose.connect( prev, 'axial', compt, 'raxial' )
        prev = compt
        x += dx
        y += dy
    
    return axon

def test_axon_propagation():
    """ Test squid axon propagation.
    """
    moose.Neutral( '/library' )
    makeAxonProto()

    rdes = rd.rdesigneur(
        chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
        cellProto = [['elec','axon']],
        chanDistrib = [
            ['Na', '#', 'Gbar', '1200' ],
            ['K', '#', 'Gbar', '360' ]],
        stimList = [['soma', '1', '.', 'inject', '(t>0.005 && t<0.2) * 2e-11' ]],
        plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']],
        moogList = [['#', '1', '.', 'Vm', 'Vm (mV)']]
    )

    rdes.buildModel()
    moose.reinit()
    moose.start(0.04)
    data = moose.wildcardFind('/##[TYPE=Table]')[0].vector[::10]
    assert np.allclose(data, expected), "Error is %s" % (data-expected)
    print('Add done')
    return data

if __name__ == '__main__':
    test_axon_propagation()
