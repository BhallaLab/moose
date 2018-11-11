# -*- coding: utf-8 -*-
# This script is modified version of GraupnerBrunel2012 model by Aditya Gilra.
# Modification is following:
#  - Added global seed.
#  - Removed some messages.
#  - Added assertion.
#  
# NOTE: This script is used for testing random number generators on various
# platform. This should not be used in any tutorial or scientific demo.

import moose
print( 'Using moose from %s' % moose.__file__ )
import numpy as np

moose.seed( 10 )

def main():
    """
    Simulate a pseudo-STDP protocol and plot the STDP kernel
    that emerges from Ca plasticity of Graupner and Brunel 2012.

    Author: Aditya Gilra, NCBS, Bangalore, October, 2014.
    """

    # ###########################################
    # Neuron models
    # ###########################################

    ## Leaky integrate and fire neuron
    Vrest = -65e-3 # V      # resting potential
    Vt_base = -45e-3 # V    # threshold
    Vreset = -55e-3 # V     # in current steps, Vreset is same as pedestal
    R = 1e8 # Ohm
    tau = 10e-3 # s
    refrT = 2e-3 # s

    # ###########################################
    # Initialize neuron group
    # ###########################################

    ## two neurons: index 0 will be presynaptic, 1 will be postsynaptic
    network = moose.LIF( 'network', 2 );
    moose.le( '/network' )
    network.vec.Em = Vrest
    network.vec.thresh = Vt_base
    network.vec.refractoryPeriod = refrT
    network.vec.Rm = R
    network.vec.vReset = Vreset
    network.vec.Cm = tau/R
    network.vec.inject = 0.
    network.vec.initVm = Vrest

    tauCa = 20e-3     
    tauSyn = 150.0    
    CaPre = 1.0       
    CaPost = 2.0      
    delayD = 13.7e-3  
    thetaD = 1.0      
    thetaP = 1.3      
    gammaD = 200.0    
    gammaP = 321.808  
    J = 5e-3 # V      
    weight = 0.5      
    bistable = True   

    syn = moose.GraupnerBrunel2012CaPlasticitySynHandler( '/network/syn' )
    syn.numSynapses = 1   
    moose.connect( syn, 'activationOut', network.vec[1], 'activation' )

    # synapse from presynaptic neuron
    moose.connect( network.vec[0],'spikeOut', syn.synapse[0], 'addSpike')

    # post-synaptic spikes also needed for STDP
    moose.connect( network.vec[1], 'spikeOut', syn, 'addPostSpike')

    syn.synapse[0].delay = 0.0
    syn.synapse[0].weight = weight
    syn.CaInit = 0.0
    syn.tauCa = tauCa
    syn.tauSyn = tauSyn
    syn.CaPre = CaPre
    syn.CaPost = CaPost
    syn.delayD = delayD
    syn.thetaD = thetaD
    syn.thetaP = thetaP
    syn.gammaD = gammaD
    syn.gammaP = gammaP
    syn.weightScale = J        
    syn.weightMax = 1.0 
    syn.weightMin = 0.

    syn.noisy = True
    syn.noiseSD = 1.3333
    syn.bistable = bistable

    # ###########################################
    # Setting up tables
    # ###########################################

    Vms = moose.Table( '/plotVms', 2 )
    moose.connect( network, 'VmOut', Vms, 'input', 'OneToOne')
    spikes = moose.Table( '/plotSpikes', 2 )
    moose.connect( network, 'spikeOut', spikes, 'input', 'OneToOne')
    CaTable = moose.Table( '/plotCa', 1 )
    moose.connect( CaTable, 'requestOut', syn, 'getCa')
    WtTable = moose.Table( '/plotWeight', 1 )
    moose.connect( WtTable, 'requestOut', syn.synapse[0], 'getWeight')

    dt = 1e-3 
    moose.useClock( 0, '/network/syn', 'process' )
    moose.useClock( 1, '/network', 'process' )
    moose.useClock( 2, '/plotSpikes', 'process' )
    moose.useClock( 3, '/plotVms', 'process' )
    moose.useClock( 3, '/plotCa', 'process' )
    moose.useClock( 3, '/plotWeight', 'process' )
    moose.setClock( 0, dt )
    moose.setClock( 1, dt )
    moose.setClock( 2, dt )
    moose.setClock( 3, dt )
    moose.setClock( 9, dt )
    moose.reinit()

    # function to make the aPlus and aMinus settle to equilibrium values
    settletime = 10e-3 # s
    def reset_settle():
        """ Call this between every pre-post pair
        to reset the neurons and make them settle to rest.
        """
        syn.synapse[0].weight = weight
        syn.Ca = 0.0
        moose.start(settletime)
        # Ca gets a jump at pre-spike+delayD
        # So this event can occur during settletime
        # So set Ca and weight once more after settletime
        syn.synapse[0].weight = weight
        syn.Ca = 0.0

    # function to inject a sharp current pulse to make neuron spike
    # immediately at a given time step
    def make_neuron_spike(nrnidx,I=1e-7,duration=1e-3):
        """ Inject a brief current pulse to
        make a neuron spike
        """
        network.vec[nrnidx].inject = I
        moose.start(duration)
        network.vec[nrnidx].inject = 0.

    dwlist_neg = []
    ddt = 10e-3 # s
    # since CaPlasticitySynHandler is event based
    # multiple pairs are needed for Ca to be registered above threshold
    # Values from Fig 2, last line of legend
    numpairs = 60           # number of spike parts per deltat
    t_between_pairs = 1.0   # time between each spike pair
    t_extent = 100e-3       # s  # STDP kernel extent,
                            # t_extent > t_between_pairs/2 inverts pre-post pairing!
    # dt = tpost - tpre
    # negative dt corresponds to post before pre
    print('-----------------------------------------------')
    for deltat in np.arange(t_extent,0.0,-ddt):
        reset_settle()
        for i in range(numpairs):
            # post neuron spike
            make_neuron_spike(1)
            moose.start(deltat)
            # pre neuron spike after deltat
            make_neuron_spike(0)
            moose.start(t_between_pairs)  # weight changes after pre-spike+delayD
                                          # must run for at least delayD after pre-spike
        dw = ( syn.synapse[0].weight - weight ) / weight
        print(('post before pre, dt = %1.3f s, dw/w = %1.3f'%(-deltat,dw)))
        dwlist_neg.append(dw)

    print('-----------------------------------------------')
    # positive dt corresponds to pre before post
    dwlist_pos = []
    for deltat in np.arange(ddt,t_extent+ddt,ddt):
        reset_settle()
        for i in range(numpairs):
            # pre neuron spike
            make_neuron_spike(0)
            moose.start(deltat)
            # post neuron spike after deltat
            make_neuron_spike(1)
            moose.start(t_between_pairs)
        dw = ( syn.synapse[0].weight - weight ) / weight
        print(('pre before post, dt = %1.3f s, dw/w = %1.3f'%(deltat,dw)))
        dwlist_pos.append(dw)

    Vmseries0 = Vms.vec[0].vector
    numsteps = len(Vmseries0)

    for t in spikes.vec[0].vector:
        Vmseries0[int(t/dt)-1] = 30e-3 # V

    Vmseries1 = Vms.vec[1].vector

    for t in spikes.vec[1].vector:
        Vmseries1[int(t/dt)-1] = 30e-3 # V

    timeseries = np.linspace(0.,200*numsteps*dt,numsteps)

    # STDP curve
    up, sp = np.mean( dwlist_pos ), np.std( dwlist_pos )
    un, sn = np.mean( dwlist_neg ), np.std( dwlist_neg )

    expected = [0.32476025611655324, 0.22658173497286094,
            0.02706212384326734, -0.2176119329016457, -0.17349820098625146,
            -0.049000627347906, 0.10942145078777199, 0.015381955378225953,
            0.004742824127517586, -0.12298343312253879]
    assert np.isclose(dwlist_pos[1:], expected[1:]).all(), "Got %s \nexpected %s" % (dwlist_pos, expected)

    expected = [-0.07871282492831622, 0.11915009122888964,
            -0.028510348966579557, 0.11812233585111875, 0.05098143255634335,
            -0.2303047508248669, 0.18033418630802123, -0.019377885225611347,
            -0.06038610826728241, 0.06575882890278106]
    assert np.isclose(dwlist_neg[1:], expected[1:]).all(), "Got %s\nexpected %s" % (dwlist_neg,
            expected)


    got = (up, sp)
    expNew = (0.014485615086785508, 0.16206703949072981)
    assert np.isclose(got, expNew).all(), 'Expected: %s, Got: %s' % (str(expNew), str(got))

if __name__ == '__main__':
    main()
