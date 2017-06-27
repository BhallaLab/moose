
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
    Simulate pre and post Poisson firing for a synapse with
    Ca plasticity of Graupner and Brunel 2012.
    See the trace over time (lifetime) for the synaptic efficacy,
    similar to figure 2A of Higgins, Graupner, Brunel, 2014.

    Author: Aditya Gilra, NCBS, Bangalore, October, 2014.
    """

    numrepeats = 10         # repeat runtime for numrepeats

    frate = 1.0             # pre- and post-synaptic firing rate
                            # 1 Hz gives ~300s lifetime, ~0.2 efficacy (weight)
                            # 10 Hz gives ~10s lifetime, ~0.5 efficacy (weight)
                            # high firing rates make synaptic efficacy go to 0.5.
    runtime = 600.0/frate   # s

    #############################################
    # Ca Plasticity parameters: synapses (not for ExcInhNetBase)
    #############################################

    ## Cortical slice values -- Table Suppl 2 in Graupner & Brunel 2012
    ## Also used in Higgins et al 2014
    tauCa = 22.6936e-3      # s # Ca decay time scale
    tauSyn = 346.3615       # s # synaptic plasticity time scale
    ## in vitro values in Higgins et al 2014, faster plasticity
    CaPre = 0.56175         # mM
    CaPost = 1.2964         # mM
    ## in vivo values in Higgins et al 2014, slower plasticity
    #CaPre = 0.33705         # mM
    #CaPost = 0.74378        # mM
    delayD = 4.6098e-3      # s # CaPre is added to Ca after this delay
                            # proxy for rise-time of NMDA
    thetaD = 1.0            # mM # depression threshold for Ca
    thetaP = 1.3            # mM # potentiation threshold for Ca
    gammaD = 331.909        # factor for depression term
    gammaP = 725.085        # factor for potentiation term

    J = 5e-3 # V            # delta function synapse, adds to Vm
    weight = 0.43           # initial synaptic weight
                            # gammaP/(gammaP+gammaD) = eq weight w/o noise
                            # see eqn (22), noiseSD also appears
                            # but doesn't work here,
                            # weights away from 0.4 - 0.5 screw up the STDP rule!!

    bistable = True        # if bistable is True, use bistable potential for weights
    noisy = True           # use noisy weight updates given by noiseSD
    noiseSD = 3.3501        # if noisy, use noiseSD (3.3501 from Higgins et al 2014)

    ##########################################

    prePoisson = moose.RandSpike('/pre')
    prePoisson.rate = frate
    postPoisson = moose.RandSpike('/post')
    postPoisson.rate = frate

    syn = moose.GraupnerBrunel2012CaPlasticitySynHandler( '/syn' )
    syn.numSynapses = 1     # 1 synapse
                            # many pre-synaptic inputs can connect to a synapse

    # spikes from presynaptic Poisson generator
    moose.connect( prePoisson,'spikeOut', syn.synapse[0], 'addSpike')

    # post-synaptic spikes from postsynaptic Poisson generator
    moose.connect( postPoisson, 'spikeOut', syn, 'addPostSpike')

    syn.synapse[0].delay = 0.0
    syn.synapse[0].weight = 1.0 # starting weight to decay from
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
    syn.weightMax = 1.0     # bounds on the weight
    syn.weightMin = 0.

    syn.noisy = noisy
    syn.noiseSD = noiseSD
    syn.bistable = bistable

    # ###########################################
    # Setting up tables
    # ###########################################

    CaTable = moose.Table( '/plotCa', 1 )
    moose.connect( CaTable, 'requestOut', syn, 'getCa')
    WtTable = moose.Table( '/plotWeight', 1 )
    moose.connect( WtTable, 'requestOut', syn.synapse[0], 'getWeight')

    # ###########################################
    # Simulate
    # ###########################################

    dt = 1e-3 # s
    # moose simulation
    #moose.useClock( 0, '/syn', 'process' )
    #moose.useClock( 1, '/pre', 'process' )
    #moose.useClock( 1, '/post', 'process' )
    # I think MOOSE uses a different clock than 3 for Tables
    # anyway, we use the default one and set all clocks to dt below
    #moose.useClock( 3, '/plotCa', 'process' )
    #moose.useClock( 3, '/plotWeight', 'process' )
    ## use setClock to set the dt different from default dt
    for i in range(10):
        moose.setClock( i, dt )

    moose.seed(100)
    moose.reinit()

    # function to make the aPlus and aMinus settle to equilibrium values
    print(("Rates of pre- and post-syanptic neurons =",frate))
    WtSeries = []
    numsteps = int(runtime/dt)
    for i in range(numrepeats):
        syn.synapse[0].weight = 1.0 # starting weight to decay from
        syn.Ca = 0.0
        print(("Repeat number",i,"running..."))
        moose.start(runtime)
        WtSeries.append(WtTable.vector[i*numsteps:(i+1)*numsteps])
    WtSeries = array(WtSeries)
    WtMean = mean(WtSeries,axis=0)
    print("plotting...")

    # ###########################################
    # Plot the simulated weights and Ca vs time
    # ###########################################

    timeseries = linspace(0.0,runtime,numsteps)
    # Ca plots for the synapse
    # only the first repeat
    figure(facecolor='w')
    plot(timeseries,CaTable.vector[:numsteps],color='r')
    plot((timeseries[0],timeseries[-1]),(thetaP,thetaP),color='k',\
        linestyle='dashed',label='pot thresh')
    plot((timeseries[0],timeseries[-1]),(thetaD,thetaD),color='b',\
        linestyle='dashed',label='dep thresh')
    legend()
    xlabel('time (s)')
    ylabel('Ca (arb)')
    title("Ca conc in the synapse @ "+str(frate)+" Hz firing")

    # Weight plots for the synapse
    figure(facecolor='w')
    for i in range(numrepeats):
        plot(timeseries,WtSeries[i],alpha=0.5)
    plot(timeseries,WtMean,color='k',linewidth=2)
    xlabel('time (s)')
    ylabel('Efficacy')
    title("Efficacy of the synapse @ "+str(frate)+" Hz firing")

    show()

if __name__ == '__main__':
    main()
