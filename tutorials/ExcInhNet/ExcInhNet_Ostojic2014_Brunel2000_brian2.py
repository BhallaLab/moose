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
    from brian2 import *   # importing brian also does:
except ImportError as e:
    print( "[INFO ] brian2 is not found." )
    quit()

#prefs.codegen.target='numpy'
#prefs.codegen.target='weave'
set_device('cpp_standalone')
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

# ###########################################
# Neuron model
# ###########################################

# equation: dv/dt=(1/taum)*(-(v-el))
# with spike when v>vt, reset to vr

el = -65.*mV          # Resting potential
vt = -45.*mV          # Spiking threshold
taum = 20.*ms         # Membrane time constant
vr = -55.*mV          # Reset potential
inp = 20.1*mV/taum    # input I/C to each neuron
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

P=NeuronGroup(N,model=eqs_neurons,\
    threshold='v>=vt',reset='v=vr',refractory=taur,method='euler')
# not distributing uniformly to ensure match with MOOSE
#Pe.v = uniform(el,vt+10*mV,NE)
#Pi.v = uniform(el,vt+10*mV,NI)
P.v = linspace(el/mV-20,vt/mV,N)*mV

# ###########################################
# Connecting the network 
# ###########################################

sparseness_e = fC*C/float(NE)
sparseness_i = (1-fC)*C/float(NI)
# Follow Dale's law -- exc (inh) neurons only have +ve (-ve) synapses
#  hence need to set w correctly (always set after creating connections
con = Synapses(P,P,'w:volt',pre='v_post+=w',method='euler')
# I don't use Brian's connect_random,
#  instead I use the same algorithm and seed as in the MOOSE version
#con_e.connect_random(sparseness=sparseness_e)
#con_i.connect_random(sparseness=sparseness_i)
## Connections from some Exc/Inh neurons to each neuron
random.seed(100) # set seed for reproducibility of simulations
conn_i = []
conn_j = []
for j in range(0,N):
    ## draw excC number of neuron indices out of NmaxExc neurons
    preIdxsE = random.sample(list(range(NE)),excC)
    ## draw inhC=C-excC number of neuron indices out of inhibitory neurons
    preIdxsI = random.sample(list(range(NE,N)),C-excC)
    ## connect these presynaptically to i-th post-synaptic neuron
    ## choose the synapses object based on whether post-syn nrn is exc or inh
    conn_i += preIdxsE
    conn_j += [j]*excC
    conn_i += preIdxsI
    conn_j += [j]*(C-excC)
con.connect(conn_i,conn_j)
con.delay = taudelay
con.w['i<NE'] = J
con.w['i>=NE'] = -g*J

# ###########################################
# Setting up monitors
# ###########################################

Nmon = N
sm = SpikeMonitor(P)

# Population monitor
popm = PopulationRateMonitor(P)

# voltage monitor
sm_vm = StateMonitor(P,'v',record=list(range(10))+list(range(NE,NE+10)))

# ###########################################
# Simulate
# ###########################################

print(("Setup complete, running for",simtime,"at dt =",dt,"s."))
t1 = time.time()
run(simtime,report='text')
device.build(directory='output', compile=True, run=True, debug=False)
print(('inittime + runtime, t = ', time.time() - t1))

#print "For g,J =",g,J,"mean exc rate =",\
#    sm_e.num_spikes/float(NE)/(simtime/second),'Hz.'
#print "For g,J =",g,J,"mean inh rate =",\
#    sm_i.num_spikes/float(NI)/(simtime/second),'Hz.'

# ###########################################
# Analysis functions
# ###########################################

tau=50e-3
sigma = tau/2.
# normalized Gaussian kernel, integral with dt is normed to 1
# to count as 1 spike smeared over a finite interval
norm_factor = 1./(sqrt(2.*pi)*sigma)
gauss_kernel = array([norm_factor*exp(-x**2/(2.*sigma**2))\
    for x in arange(-5.*sigma,5.*sigma+dt,dt)])
def rate_from_spiketrain(spikemon,fulltime,nrnidx=None):
    """
    Returns a rate series of spiketimes convolved with a Gaussian kernel;
    all times must be in SI units,
    remember to divide fulltime and dt by second
    """
    if nrnidx is None:
        spiketimes = spikemon.t # take spiketimes of all neurons
    else:
        # take spiketimes of only neuron index nrnidx
        spiketimes = spikemon.t[where(spikemon.i==nrnidx)[0]]
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
    plot(timeseries[:len(sm_vm.t)],sm_vm[i].v)

fig = figure()
# raster plots
subplot(231)
plot(sm.t,sm.i,',')
title(str(N)+" exc & inh neurons")
xlim([0,simtime/second])
xlabel("")

print("plotting firing rates")
subplot(232)
# firing rates
timeseries = arange(0,simtime/second+dt,dt)
num_to_plot = 10
#rates = []
for nrni in range(num_to_plot):
    rate = rate_from_spiketrain(sm,simtime/second,nrni)
    plot(timeseries[:len(rate)],rate)
    #print mean(rate),len(sm_e[nrni])
    #rates.append(rate)
title(str(num_to_plot)+" exc rates")
ylabel("Hz")
ylim(0,300)
subplot(235)
for nrni in range(NE,NE+num_to_plot):
    rate = rate_from_spiketrain(sm,simtime/second,nrni)
    plot(timeseries[:len(rate)],rate)
    #print mean(rate),len(sm_i[nrni])
    #rates.append(rate)
title(str(num_to_plot)+" inh rates")
ylim(0,300)
#print "Mean rate = ",mean(rates)
xlabel("Time (s)")
ylabel("Hz")

print("plotting pop firing rates")
# Population firing rates
subplot(233)
timeseries = arange(0,simtime/second,dt)
#plot(timeseries,popm_e.smooth_rate(width=50.*ms,filter="gaussian"),color='grey')
rate = rate_from_spiketrain(sm,simtime/second)/float(N)
plot(timeseries[:len(rate)],rate)
title("population rate")
ylabel("Hz")
xlabel("Time (s)")

fig.tight_layout()

show()
