####################################################################
# Rdesigneur example 3.2
# Making a myelinated axon with a propagating action potential.
####################################################################
import numpy as np
import moose
import pylab
import rdesigneur as rd

numAxonSegments = 405
nodeSpacing = 100
comptLen = 10e-6
comptDia = 2e-6 # 2x usual
RM = 100.0 # 10x usual
RA = 5.0
CM = 0.001 # 0.1x usual

nodeDia = 1e-6
nodeRM = 1.0
nodeCM = 0.01

def makeAxonProto():
    axon = moose.Neuron( '/library/axon' )
    x = 0.0
    y = 0.0
    prev = rd.buildCompt( axon, 'soma', RM = RM, RA = RA, CM = CM, dia = 10e-6, x=0, dx=comptLen)
    theta = 0
    x = comptLen

    for i in range( numAxonSegments ):
        r = comptLen
        dx = comptLen * np.cos( theta )
        dy = comptLen * np.sin( theta )
        r = np.sqrt( x * x + y * y )
        theta += comptLen / r
        if i % nodeSpacing == 0:
            compt = rd.buildCompt( axon, 'axon' + str(i), RM = nodeRM, RA = RA, CM = nodeCM, x = x, y = y, dx = dx, dy = dy, dia = nodeDia )
        else:
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
        ['Na', '#', 'Gbar', '12000 * (dia < 1.5e-6)' ],
        ['K', '#', 'Gbar', '3600 * (dia < 1.5e-6)' ]],
    stimList = [['soma', '1', '.', 'inject', '(t>0.01 && t<0.2) * 1e-10' ]],
    plotList = [['soma,axon100,axon200,axon300,axon400', '1', '.', 'Vm', 'Membrane potential']],
    moogList = [['#', '1', '.', 'Vm', 'Vm (mV)']]
)

rdes.buildModel()
moose.reinit()
rdes.displayMoogli( 0.00005, 0.05, 0.0 )
