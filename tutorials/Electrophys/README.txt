This is a list of files in the Electrophys tutorials directory

ephys1_cable.py:

This is a model of a simple uniform passive cable. It has two plots: one
of membrane potential Vm as a function of time, sampled at four locations 
along the cable, and the other of Vm as a function of position, sampled at
5 times (intervals of 5 ms) during the simulation.
The user can use the sliders to scale the following parameters up and down:
RM
RA
CM
length
diameter

In addition, the user can toggle between a long (steady-state) current
injection stimulus, and a brief pulse of 1 ms.

Finally, when the user hits "Quit" the time-series of the soma compartment
is printed out.

Things to do:
1. Vary diameter, length, RM, CM, RA. Check that the decay behaves as per
	cable theory. Get an intuition for these.
2. Check that lambda (length constant) is according to equations. Check
	how much decay there is for different L (electrotonic length).
2a. Note that if the cable is shorter than L, the signal doesn't decay so
	well. Why?
2b. Convince yourself that you'll never see a passive signal along a long 
	axon.
3. Examine propagation of the depolarization for a brief current pulse.
	Look for the three signatures of propagation along a dendrite.
4. Run a simulation of the long stimulus. When you hit Quit, the program will
	dump the soma potential charging time-course into a file. Check that 
	the tau (time-constant) of the soma is according to equations when the 
	L is very small, but deviates when it is longer. Use Rall's expression 
	for L as a function of the time-courses of soma depolarization.

------------------------------------------------------------------------

ephys2_Rall_law.py:

This explores the implication of Rall's Law and cable branching.
This is a model of a branched cell, compared with the model of a uniform
cylindrical cell. The sliders vary the parameters of the branches. Two 
time-points are displayed; 10 ms and 50 ms.

Things to do:
1. Vary all the passive parameters of branches, see how they affect the
	propagation past the branch point.
2. Match up the branched cell (blue plot/dots) to the cylinder (red line).
	See if the resultant parameters obey Rall's Law.

Todo: - Stimuli in any end or both dend ends.

------------------------------------------------------------------------

ephys3_synaptic_summation.py

This is a very general tutorial on neuronal summation. It implements a Y-branch
neuron, with synaptic inputs at the end of each branch and on the junction.
There are excitatory as well as inhibitory inputs, and many parameters of the
synapses as well as of the cell can be modified.
There are two plots built in: one is a time-series plot of membrane potential
(Vm) for the soma, the junction, and the two branches. The other is a plot of
Vm against position. 

There are six modes for modifying parameters: Gbar, Onset, Tau, Duration,
Freq and Elec. Each of these modes activates a set of six sliders, to specify
the parameters of the model.
There is a toggle for turning spiking on or off. The spiking is implemented as
a full Hodgkin-Huxley type model with Na and K channels, but the channel
kinetics are based on mammalian parameters.

Here are just some of the things to try with the model:

	- Show temporal summation of exc input with many spikes
	- Show sublinear summation and saturation of exc input
	- Show spatial summation of exc input
	- Show temporal summation of exc input with 2 inputs at different times
	- Show local inhibition
	- Show efficacy of inhibition as function of time-course
	- Show gating of excitation by junction inhibition
	- Show shunting inhibition
	- Turn on spiking and show AND and OR
	- Turn on spiking and show how different freq inputs cross threshold.
	- Show coincidence detection vs integration (change spiking props?)
	- Change dend length and show somatic effects at different distances.
	- Change dend dia and show somatic effects
	- Show truncation of EPSP by inhib input arriving at diff times.
	- Show EI balance and imbalance

------------------------------------------------------------------------

ephys4_seq_summation.py

This tutorial illustrates summation in space and time, specifically successive
inputs along a dendrite. It is designed to replicate Rall's analysis of
passive sequence selectivity (ref). There is also a trial implementation of
Branco et al's analysis of sequence selectivity as amplified by NMDA receptors.
As usual, there are two plots: a time-plot and a space plot.
Some things to try:

	- Establish delays and peak amplitude for individual synaptic inputs
	- Figure out how to line them up by adjusting onset delays
	- Come up with an estimate of selectivity. Hint: Define selectivity
		in terms of something easily measured in the graphs.
	- Examine how selectivity varies with dendritic geometry
	- Examine how selectivity changes when you add NMDAR.

------------------------------------------------------------------------

ephys5_channel_mixer.py

This demo illustrates how different classes  of ion channels affect neuronal 
spiking behaviour. The model consists of a single compartment spherical soma, 
with Na and K_DR set at a default level to get spiking.  The top graph shows 
the membrane potential, and the second graph shows the calcium concentration 
at the soma. The current injection begins at 50 ms, and lasts for 500 ms.
In each graph the solid blue plot indicates the current calculated response of 
the model to the specified channel modulation and current injection.  The red 
dotted plot indicates what would have happened had the modulated channel not 
been there at all.

The default channel density for all channels is 100 Mho/m^2, except K_AHP where
 the default is 10 Mho/m^2. There are sliders for modulating the levels of 
each of the channels.  The slider lets you modulate this channel density in a 
range from zero to 10x the default density.
There are two terms to modulate for the Calcium dynamics: the Ca_tau is the 
time-course for the pump to expel calcium influx, and the Ca_thickness is the 
thickness of the calcium shell. The maximum thickness is, of course, the 
radius of the soma, which is 5 microns. If we have a smaller value than this 
it means that the calcium flux only comes into a cylindrical shell and 
therefore gives rise to higher concentrations.

The simulation starts with nominal values for the Na and K_DR channels of 
100 Mho/m^2, and all other channels modulated down to zero.

Things to do:
    - Examine what each channel does on its own
    - Examine how to control the calcium influx
    - Once calcium influx is present, examine how it affects the K_Ca and 
	K_AHP channels, and how in turn the cell dynamics are affected by 
	calcium dynamics.
    - Can you get it to fire a burst of APs starting around 400ms?
    - Can you get it to fire just one action potential around 100 ms?
    - Can you get it to fire just one action potential around 400 ms?
    - Can you get it to fire with adaptation, that is, rate gets slower?
    - Can you get it to fire with facilitation, that is, rate gets faster?
    - It is a bit fiddly, as the simulation only runs 500 ms, but can you 
	get it to do a couple of bursts?

------------------------------------------------------------------------
Divisive normalization
	- Better display.
	* print peak in text box
	- Fix up toggles, including capacity for spiking
	- toggles for saving plots and for saving tabulated output
------------------------------------------------------------------------
Channel builder:
	- voltage step series
	- Set # of gates, powers
	- Set equations for alpha, beta or tau and minf
	- Show plots for minf and tau, alpha and beta
	- Optionally read in expt points for minf, tau or voltage step series.
	- Sliders for up to six params in each of these
	- Sample of effect on standard cell with Na and Kdr if chan added
	- Dump MOOSE prototype Python file, and ChannelML file.

------------------------------------------------------------------------

NMDA receptor and associativity:
        - Plot Vm and Ca
        - Give glu and NMDA input different time and different ampl
------------------------------------------------------------------------

AP propagation:
        - Vary dia
        - Vary RM, CM, RA
        - Vary channel densities
------------------------------------------------------------------------

Squid demo:
        - Already have it.

