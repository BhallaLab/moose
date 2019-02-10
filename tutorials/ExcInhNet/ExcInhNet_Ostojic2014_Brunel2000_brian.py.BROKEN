'''
The LIF network is based on:
Ostojic, S. (2014).
 Two types of asynchronous activity in networks of
 excitatory and inhibitory spiking neurons.
 Nat Neurosci 17, 594-600.
 
Key parameter to change is synaptic coupling J (mV).
Tested with Brian 1.4.1

Written by Aditya Gilra, CAMP 2014, Bangalore, 20 June, 2014.
Updated to match MOOSE implementation by Aditya Gilra, Jan, 2015.
Currently, simtime and dt are modified to compare across MOOSE, Brian1 and Brian2.
'''

#import modules and functions to be used
# 'from pylab import *' which imports:
# matplot like commands into the namespace, further
# also can use np. for numpy and mpl. for matplotlib
try:
    from brian import * # importing brian also does:
except Exception as e:
    print( "[INFO ] Could not import brain . Quitting ..." )
    quit()
    
import random
import time

np.random.seed(100) # set seed for reproducibility of simulations
random.seed(100) # set seed for reproducibility of simulations

# ###########################################
# Simulation parameters
# ###########################################

simdt = 0.01*ms
simtime = 10.0*second            # Simulation time
defaultclock.dt = simdt         # Brian's default sim time step
dt = defaultclock.dt/second     # convert to value in seconds

clocknrn = Clock(dt=simdt,order=0)
clocksyn = Clock(dt=simdt,order=1)

# ###########################################
# Neuron model
# ###########################################

# equation: dv/dt=(1/taum)*(-(v-el))
# with spike when v>vt, reset to vr

el = -65.*mV          # Resting potential
vt = -45.*mV          # Spiking threshold
taum = 20.*ms         # Membrane time constant
vr = -55.*mV          # Reset potential
inp = 20.1*mV/taum     # input I/C to each neuron
                      # same as setting el=-41 mV and inp=0
taur = 0.5*ms         # Refractory period
taudelay = 0.5*ms + dt*second      # synaptic delay

eqs_neurons='''
dv/dt=(1/taum)*(-(v-el))+inp : volt
'''

# ###########################################
# Network parameters: numbers
# ###########################################

N = 1000          # Total number of neurons
fexc = 0.8        # Fraction of exc neurons
NE = int(fexc*N)  # Number of excitatory cells
NI = N-NE         # Number of inhibitory cells 

# ###########################################
# Network parameters: synapses
# ###########################################

C = 100           # Number of incoming connections on each neuron (exc or inh)
fC = fexc         # fraction fC incoming connections are exc, rest inhibitory
excC = int(fC*C)  # number of exc incoming connections
J = 0.8*mV        # exc strength is J (in mV as we add to voltage)
                  # Critical J is ~ 0.45 mV in paper for N = 1000, C = 1000
g = 5.0           # -gJ is the inh strength. For exc-inh balance g>~f(1-f)=4

# ###########################################
# Initialize neuron (sub)groups
# ###########################################

neurons=NeuronGroup(N,model=eqs_neurons,\
    threshold='v>=vt',reset=vr,refractory=taur,clock=clocknrn)
Pe=neurons.subgroup(NE)
Pi=neurons.subgroup(NI)
# not distributing uniformly to ensure match with MOOSE
#Pe.v = uniform(el,vt+10*mV,NE)
#Pi.v = uniform(el,vt+10*mV,NI)
neurons.v = linspace(el/mV-20,vt/mV,N)*mV

# ###########################################
# Connecting the network 
# ###########################################

sparseness_e = fC*C/float(NE)
sparseness_i = (1-fC)*C/float(NI)
# Follow Dale's law -- exc (inh) neurons only have +ve (-ve) synapses.
con_e = Synapses(Pe,neurons,'',pre='v_post+=J',clock=clocksyn)
con_i = Synapses(Pi,neurons,'',pre='v_post+=-g*J',clock=clocksyn)
# I don't use Brian's connect_random,
#  instead I use the same algorithm and seed as in the MOOSE version
#con_e.connect_random(sparseness=sparseness_e)
#con_i.connect_random(sparseness=sparseness_i)
## Connections from some Exc/Inh neurons to each neuron
random.seed(100) # set seed for reproducibility of simulations
for i in range(0,N):
    ## draw excC number of neuron indices out of NmaxExc neurons
    preIdxs = random.sample(list(range(NE)),excC)
    ## connect these presynaptically to i-th post-synaptic neuron
    for synnum,preIdx in enumerate(preIdxs):
        con_e[preIdx,i]=True
    ## draw inhC=C-excC number of neuron indices out of inhibitory neurons
    preIdxs = random.sample(list(range(N-NE)),C-excC)
    ## connect these presynaptically to i-th post-synaptic neuron
    for synnum,preIdx in enumerate(preIdxs):
        con_i[preIdx,i]=True
con_e.delay = taudelay
con_i.delay = taudelay

# ###########################################
# Setting up monitors
# ###########################################

Nmon = N
Nmon_exc = int(fexc*Nmon)
Pe_mon = Pe.subgroup(Nmon_exc)
sm_e = SpikeMonitor(Pe_mon)
Pi_mon = Pi.subgroup(Nmon-Nmon_exc)
sm_i = SpikeMonitor(Pi_mon)

# Population monitor
popm_e = PopulationRateMonitor(Pe,bin=1.*ms)
popm_i = PopulationRateMonitor(Pi,bin=1.*ms)

# voltage monitor
sm_e_vm = StateMonitor(Pe,'v',record=list(range(10)),clock=clocknrn)

# ###########################################
# Simulate
# ###########################################

print(("Setup complete, running for",simtime,"at dt =",dt,"s."))
t1 = time.time()
run(simtime,report='text')
print(('inittime + runtime, t = ', time.time() - t1))

print(("For g,J =",g,J,"mean exc rate =",\
    sm_e.nspikes/float(Nmon_exc)/(simtime/second),'Hz.'))
print(("For g,J =",g,J,"mean inh rate =",\
    sm_i.nspikes/float(Nmon-Nmon_exc)/(simtime/second),'Hz.'))

# ###########################################
# Analysis functions
# ###########################################

def rate_from_spiketrain(spiketimes,fulltime,dt,tau=50e-3):
    """
    Returns a rate series of spiketimes convolved with a Gaussian kernel;
    all times must be in SI units,
    remember to divide fulltime and dt by second
    """
    sigma = tau/2.
    # normalized Gaussian kernel, integral with dt is normed to 1
    # to count as 1 spike smeared over a finite interval
    norm_factor = 1./(sqrt(2.*pi)*sigma)
    gauss_kernel = array([norm_factor*exp(-x**2/(2.*sigma**2))\
        for x in arange(-5.*sigma,5.*sigma+dt,dt)])
    kernel_len = len(gauss_kernel)
    # need to accommodate half kernel_len on either side of fulltime
    rate_full = zeros(int(fulltime/dt)+kernel_len)
    for spiketime in spiketimes:
        idx = int(spiketime/dt)
        rate_full[idx:idx+kernel_len] += gauss_kernel
    # only the middle fulltime part of the rate series
    # This is already in Hz,
    # since should have multiplied by dt for above convolution
    # and divided by dt to get a rate, so effectively not doing either.
    return rate_full[kernel_len/2:kernel_len/2+int(fulltime/dt)]

# ###########################################
# Make plots
# ###########################################

fig = figure()
# Vm plots
timeseries = arange(0,simtime/second+dt,dt)
for i in range(3):
    plot(timeseries[:len(sm_e_vm[i])],sm_e_vm[i])

fig = figure()
# raster plots
subplot(231)
raster_plot(sm_e,ms=1.)
title(str(Nmon_exc)+" exc neurons")
xlabel("")
xlim([0,simtime/ms])
subplot(234)
raster_plot(sm_i,ms=1.)
title(str(Nmon-Nmon_exc)+" inh neurons")
subplot(232)

# firing rates
timeseries = arange(0,simtime/second+dt,dt)
num_to_plot = 10
#rates = []
for nrni in range(num_to_plot):
    rate = rate_from_spiketrain(sm_e[nrni],simtime/second,dt)
    plot(timeseries[:len(rate)],rate)
    #print mean(rate),len(sm_e[nrni])
    #rates.append(rate)
title(str(num_to_plot)+" exc rates")
ylabel("Hz")
ylim(0,300)
subplot(235)
for nrni in range(num_to_plot):
    rate = rate_from_spiketrain(sm_i[nrni],simtime/second,dt)
    plot(timeseries[:len(rate)],rate)
    #print mean(rate),len(sm_i[nrni])
    #rates.append(rate)
title(str(num_to_plot)+" inh rates")
ylim(0,300)
#print "Mean rate = ",mean(rates)
xlabel("Time (s)")
ylabel("Hz")

# Population firing rates
subplot(233)
timeseries = arange(0,simtime/second,dt)
allspikes = []
for nrni in range(NE):
    allspikes.extend(sm_e[nrni])
#plot(timeseries,popm_e.smooth_rate(width=50.*ms,filter="gaussian"),color='grey')
rate = rate_from_spiketrain(allspikes,simtime/second,dt)/float(NE)
plot(timeseries[:len(rate)],rate)
title("Exc population rate")
ylabel("Hz")
subplot(236)
timeseries = arange(0,simtime/second,dt)
allspikes = []
for nrni in range(NI):
    allspikes.extend(sm_i[nrni])
#plot(timeseries,popm_i.smooth_rate(width=50.*ms,filter="gaussian"),color='grey')
rate = rate_from_spiketrain(allspikes,simtime/second,dt)/float(NI)
plot(timeseries[:len(rate)],rate)
title("Inh population rate")
xlabel("Time (s)")
ylabel("Hz")

fig.tight_layout()

show()
