
#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

import moose
from pylab import *

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

    #############################################
    # Ca Plasticity parameters: synapses (not for ExcInhNetBase)
    #############################################

    ### Cortical slice values -- Table Suppl 2 in Graupner & Brunel 2012
    ### Also used in Higgins et al 2014
    #tauCa = 22.6936e-3      # s # Ca decay time scale
    #tauSyn = 346.3615       # s # synaptic plasticity time scale
    ### in vitro values in Higgins et al 2014, faster plasticity
    #CaPre = 0.56175         # mM
    #CaPost = 1.2964         # mM
    ### in vivo values in Higgins et al 2014, slower plasticity
    ##CaPre = 0.33705         # mM
    ##CaPost = 0.74378        # mM
    #delayD = 4.6098e-3      # s # CaPre is added to Ca after this delay
                            ## proxy for rise-time of NMDA
    #thetaD = 1.0            # mM # depression threshold for Ca
    #thetaP = 1.3            # mM # potentiation threshold for Ca
    #gammaD = 331.909        # factor for depression term
    #gammaP = 725.085        # factor for potentiation term

    #J = 5e-3 # V            # delta function synapse, adds to Vm
    #weight = 0.43           # initial synaptic weight
                            ## gammaP/(gammaP+gammaD) = eq weight w/o noise
                            ## see eqn (22), noiseSD also appears
                            ## but doesn't work here,
                            ## weights away from 0.4 - 0.5 screw up the STDP rule!!

    #bistable = True        # if bistable is True, use bistable potential for weights
    #noisy = False          # use noisy weight updates given by noiseSD
    #noiseSD = 3.3501        # if noisy, use noiseSD (3.3501 from Higgins et al 2014)

    ########################################

    ## DP STDP curve (Fig 2C) values -- Table Suppl 1 in Graupner & Brunel 2012
    tauCa = 20e-3           # s # Ca decay time scale
    tauSyn = 150.0          # s # synaptic plasticity time scale
    CaPre = 1.0             # arb
    CaPost = 2.0            # arb
    delayD = 13.7e-3        # s # CaPre is added to Ca after this delay
                            # proxy for rise-time of NMDA
    thetaD = 1.0            # mM # depression threshold for Ca
    thetaP = 1.3            # mM # potentiation threshold for Ca
    gammaD = 200.0          # factor for depression term
    gammaP = 321.808        # factor for potentiation term

    J = 5e-3 # V            # delta function synapse, adds to Vm
    weight = 0.5            # initial synaptic weight
                            # gammaP/(gammaP+gammaD) = eq weight w/o noise
                            # see eqn (22), noiseSD also appears
                            # but doesn't work here,
                            # weights away from 0.4 - 0.5 screw up the STDP rule!!

    bistable = True        # if bistable is True, use bistable potential for weights
    noisy = False          # use noisy weight updates given by noiseSD
    noiseSD = 2.8284        # if noisy, use noiseSD (3.3501 in Higgins et al 2014)

    ##########################################

    syn = moose.GraupnerBrunel2012CaPlasticitySynHandler( '/network/syn' )
    syn.numSynapses = 1     # 1 synapse
                            # many pre-synaptic inputs can connect to a synapse
    # synapse onto postsynaptic neuron
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
    syn.weightScale = J        # weight ~1, weightScale ~ J
                               # weight*weightScale is activation,
                               # i.e. delta-fn added to postsynaptic Vm

    syn.weightMax = 1.0        # bounds on the weight
    syn.weightMin = 0.

    syn.noisy = noisy
    syn.noiseSD = noiseSD
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

    # ###########################################
    # Simulate the STDP curve with spaced pre-post spike pairs
    # ###########################################

    dt = 1e-3 # s
    # moose simulation
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
    settletime = 100e-3 # s
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
    ddt = 2e-3 # s
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
    for deltat in arange(t_extent,0.0,-ddt):
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
    for deltat in arange(ddt,t_extent+ddt,ddt):
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
    print('-----------------------------------------------')
    print(('Each of the above pre-post pairs was repeated',\
            numpairs,'times, with',t_between_pairs,'s between pairs.'))
    print()
    print('Due to event based updates, Ca decays suddenly at events:')
    print('pre-spike, pre-spike + delayD, and post-spike;')
    print('apart from the usual CaPre and CaPost jumps at')
    print('pre-spike + delayD and post-spike respectively.')
    print('Because of the event based update, multiple pre-post pairs are used.')
    print()
    print('If you reduce the t_between_pairs,')
    print(' you\'ll see potentiation for the LTD part without using any triplet rule!')
    print()
    print("If you turn on noise, the weights fluctuate too much,")
    print(" not sure if there's a bug in my noise implementation.")
    print('-----------------------------------------------')

    # ###########################################
    # Plot the simulated Vm-s and STDP curve
    # ###########################################

    # insert spikes so that Vm reset doesn't look weird
    Vmseries0 = Vms.vec[0].vector
    numsteps = len(Vmseries0)
    for t in spikes.vec[0].vector:
        Vmseries0[int(t/dt)-1] = 30e-3 # V
    Vmseries1 = Vms.vec[1].vector
    for t in spikes.vec[1].vector:
        Vmseries1[int(t/dt)-1] = 30e-3 # V

    timeseries = linspace(0.,1000*numsteps*dt,numsteps)
    # Voltage plots
    figure(facecolor='w')
    plot(timeseries,Vmseries0,color='r') # pre neuron's vm
    plot(timeseries,Vmseries1,color='b') # post neuron's vm
    xlabel('time (ms)')
    ylabel('Vm (V)')
    title("pre (r) and post (b) neurons' Vm")

    # Ca plots for the synapse
    figure(facecolor='w')
    plot(timeseries,CaTable.vector[:len(timeseries)],color='r')
    plot((timeseries[0],timeseries[-1]),(thetaP,thetaP),color='k',\
        linestyle='dashed',label='pot thresh')
    plot((timeseries[0],timeseries[-1]),(thetaD,thetaD),color='b',\
        linestyle='dashed',label='dep thresh')
    legend()
    xlabel('time (ms)')
    ylabel('Ca (arb)')
    title("Ca conc in the synapse")

    # Weight plots for the synapse
    figure(facecolor='w')
    plot(timeseries,WtTable.vector[:len(timeseries)],color='r')
    xlabel('time (ms)')
    ylabel('Efficacy')
    title("Efficacy of the synapse")

    # STDP curve
    fig = figure(facecolor='w')
    ax = fig.add_subplot(111)
    ax.plot(arange(-t_extent,0,ddt)*1000,array(dwlist_neg),'.-r')
    ax.plot(arange(ddt,(t_extent+ddt),ddt)*1000,array(dwlist_pos),'.-b')
    xmin,xmax = ax.get_xlim()
    ymin,ymax = ax.get_ylim()
    ax.set_xticks([xmin,0,xmax])
    ax.set_yticks([ymin,0,ymax])
    ax.plot((0,0),(ymin,ymax),linestyle='dashed',color='k')
    ax.plot((xmin,xmax),(0,0),linestyle='dashed',color='k')
    ax.set_xlabel('$t_{post}-t_{pre}$ (ms)')
    ax.set_ylabel('$\Delta w / w$')
    fig.tight_layout()
    #fig.subplots_adjust(hspace=0.3,wspace=0.5) # use after tight_layout()

    show()

if __name__ == '__main__':
    main()
