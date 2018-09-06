"""test_connectionLists.py: 

Test connectionList in SparseMsg.

"""

from __future__ import print_function
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import moose
import numpy as np

params = {
        'Rm': 1e10,
        'Cm': 1e-11,
        'randInputRate': 10,
        'randRefractTime': 0.005,
        'numInputs': 100,
        'numInhib': 10,
        'inhibRefractTime': 0.005,
        'inhibThresh': -0.04,
        'numOutput': 10,
        'outputRefractTime': 0.005,
        'outputThresh': 5.00e6,
        'stimToInhProb': 0.01,
        'stimToInhSeed': 11234,
        'inhVreset': -0.06,
        'stimToOutProb': 0.05,
        'stimToOutSeed': 1234,
        'inhToOutProb': 0.5,
        'inhToOutSeed': 1234,
        'runtime': 2.0,
        'wtStimToOut': 0.001,
        'wtStimToInh': 0.002,
        'wtInhToOut': -0.002,
}


def makeGlobalBalanceNetwork():
    stim = moose.RandSpike( '/model/stim', params['numInputs'] )
    inhib = moose.LIF( '/model/inhib', params['numInhib'] )
    insyn = moose.SimpleSynHandler(inhib.path + '/syns', params['numInhib'])
    moose.connect( insyn, 'activationOut', inhib, 'activation', 'OneToOne' )
    output = moose.LIF( '/model/output', params['numOutput'] )
    outsyn = moose.SimpleSynHandler(output.path+'/syns',params['numOutput'])
    moose.connect(outsyn, 'activationOut', output, 'activation', 'OneToOne')
    outInhSyn = moose.SimpleSynHandler(output.path+'/inhsyns',params['numOutput'])
    moose.connect(outInhSyn, 'activationOut', output, 'activation', 'OneToOne')

    iv = moose.vec( insyn.path + '/synapse' )
    ov = moose.vec( outsyn.path + '/synapse' )
    oiv = moose.vec( outInhSyn.path + '/synapse' )

    temp = moose.connect( stim, 'spikeOut', iv, 'addSpike', 'Sparse' )
    inhibMatrix = moose.element( temp )
    inhibMatrix.setRandomConnectivity( 
            params['stimToInhProb'], params['stimToInhSeed'] )
    cl = inhibMatrix.connectionList

    # This can change when random-number generator changes.
    # This was before we used c++11 <random> to generate random numbers. This
    # test has changes on Tuesday 31 July 2018 11:12:35 AM IST
    #  expectedCl = [ 1,4,13,13,26,42,52,56,80,82,95,97,4,9,0,9,4,8,0,6,1,6,6,7]
    expectedCl=[0,6,47,50,56,67,98,2,0,3,5,4,8,3]

    assert list(cl) == expectedCl, "Expected %s, got %s" % (expectedCl, cl)

    temp = moose.connect( stim, 'spikeOut', ov, 'addSpike', 'Sparse' )
    excMatrix = moose.element( temp )
    excMatrix.setRandomConnectivity( 
            params['stimToOutProb'], params['stimToOutSeed'] )

    temp = moose.connect( inhib, 'spikeOut', oiv, 'addSpike', 'Sparse' )
    negFFMatrix = moose.element( temp )
    negFFMatrix.setRandomConnectivity( 
            params['inhToOutProb'], params['inhToOutSeed'] )

    # print("ConnMtxEntries: ", inhibMatrix.numEntries, excMatrix.numEntries, negFFMatrix.numEntries)
    got = (inhibMatrix.numEntries, excMatrix.numEntries, negFFMatrix.numEntries)
    expected = (7, 62, 55)
    assert expected == got, "Expected %s, Got %s" % (expected,got)

    cl = negFFMatrix.connectionList
    numInhSyns = [ ]
    niv = 0
    nov = 0
    noiv = 0
    for i in moose.vec( insyn ):
        niv += i.synapse.num
        numInhSyns.append( i.synapse.num )
        if i.synapse.num > 0:
            i.synapse.weight = params['wtStimToInh']

    #  expected = [2,1,0,0,2,0,3,1,1,2]
    expected = [1, 0, 1, 2, 1, 1, 0, 0, 1, 0]
    assert numInhSyns == expected, "Expected %s, got %s" % (expected,numInhSyns)

    for i in moose.vec( outsyn ):
        nov += i.synapse.num
        if i.synapse.num > 0:
            i.synapse.weight = params['wtStimToOut']
    for i in moose.vec( outInhSyn ):
        noiv += i.synapse.num
        #print i.synapse.num
        if i.synapse.num > 0:
            i.synapse.weight = params['wtInhToOut']
     
    print("SUMS: ", sum( iv.numField ), sum( ov.numField ), sum( oiv.numField ))
    assert [1, 64, 25] == [sum( iv.numField ), sum( ov.numField ), sum( oiv.numField )]
    print("SUMS2: ", niv, nov, noiv)
    assert [7, 62, 55] ==  [ niv, nov, noiv ]
    print("SUMS3: ", sum( insyn.vec.numSynapses ), sum( outsyn.vec.numSynapses ), sum( outInhSyn.vec.numSynapses ))
    assert [7,62,55] == [ sum( insyn.vec.numSynapses ), sum( outsyn.vec.numSynapses ), sum( outInhSyn.vec.numSynapses ) ]

    # print(oiv.numField)
    # print(insyn.vec[1].synapse.num)
    # print(insyn.vec.numSynapses)
    # print(sum( insyn.vec.numSynapses ))
    # niv = iv.numSynapses
    # ov = iv.numSynapses

    sv = moose.vec( stim )
    sv.rate = params['randInputRate']
    sv.refractT = params['randRefractTime']
    #moose.showfield( sv[7] )

    inhib.vec.thresh = params['inhibThresh']
    inhib.vec.Rm = params['Rm']
    inhib.vec.Cm = params['Cm']
    inhib.vec.vReset = params['inhVreset']
    inhib.vec.refractoryPeriod = params['inhibRefractTime']
    output.vec.thresh = params['outputThresh']
    output.vec.Rm = params['Rm']
    output.vec.Cm = params['Cm']
    output.vec.refractoryPeriod = params['outputRefractTime']
    otab = moose.Table( '/model/otab', params['numOutput'] )
    moose.connect( otab, 'requestOut', output, 'getVm', 'OneToOne' )
    itab = moose.Table( '/model/itab', params['numInhib'] )
    moose.connect( itab, 'requestOut', inhib, 'getVm', 'OneToOne' )
    return inhib, output

def makeRaster( network ):
    raster = moose.Table(network.path + '/raster', network.numData)
    moose.connect(network, 'spikeOut', raster, 'spike', 'OneToOne')
    return raster

def main():
    mod = moose.Neutral( '/model' )
    inhib, output = makeGlobalBalanceNetwork()
    iraster = makeRaster( inhib )
    oraster = makeRaster( output )
    moose.reinit()
    moose.start( params['runtime'] )
    for i in range( len(iraster.vec) ):
        print( iraster.vec[i].vector )

if __name__ == '__main__':
    main()
