####################################################################
# Rdesigneur example 3.1
# Making an axon with a propagating action potential.
####################################################################
import numpy as np
import moose
import pylab
import rdesigneur as rd

numAxonSegments = 200
comptLen = 10e-6
comptDia = 1e-6
RM = 1.0
RA = 10.0
CM = 0.01

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

rdes.displayMoogli( 0.00005, 0.04, 0.0 )
