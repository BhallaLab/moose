This is a list of the files in this directory. 
The detailed account of the models here is provided in the MOOSE online
documentation at

https://moose.ncbs.res.in/readthedocs/install/user/py/cookbook/Rd.html

ex1_minimalModel.py: Bare Rdesigneur: single passive compartment
	No graphics. Suggestions: 
	- Modify to print out the parameters of the compartment.
	- Change default values.

ex2.0_currentPulse.py: Simulate and display current pulse to soma
	Introduces stimulus and plotting.
	Suggestion: Play with the stimulus parameters. Can you put in a
	current ramp? Plot it too.

ex2.1_vclamp.py: Show how to do voltage clamp (vclamp) stimulus and plotting,
	on the passive soma. Illustrates the capacitive transients.

ex3.0_squid_currentPulse.py: HH Squid model in a single compartment, given
	a current pulse to elicit action potentials. 
	Suggestion: Play with the stimulus parameters. 
	- Change injection current.
	- Put in a protocol to get rebound action potential.
	- Put in a current ramp, and run it for a different duration
	- Put in a frequency chirp, and see how the squid model is tuned to
	a certain frequency range.
	- Modify channel or passive parameters. See if it still fires. 
	Try the frequency chirp on the modified cell, is it retuned?

ex3.1.squid_vclamp.py: Voltage clamp on HH Squid model in a single compartment.
	Here we add a voltage clamp circuit to the HH Squid model, and 
	monitor the holding current.
	Suggestions: 
	- Monitor individual channel currents through additional plots.
	- Convert this into a voltage clamp series. Easiest way to do this is
	to complete the rdes.BuildModel, then delete the Function object 
	on the /model/elec/soma/vclamp. Now you can simply set the 'command'
	field of the vclamp in a for loop, going from -ve to +ve voltages.
	Remember, SI units. You may wish to capture the plot vectors each
	cycle. The plot vectors are accessed by something like
		moose.element( '/model/graphs/plot1' ).vector

ex3.2_squid_axon_propgn.py: Set up a long spiral axon with HH channels. 
	Inject current at 
	the soma in order to trigger periodic action potentials propagating 
	down the axon. Display using moogli as well as plotting the output 
	at a few points.

ex3.3_AP_collision.py: Same as 3.2: a long spiral axon. This time we deliver 
	current injection at two points, the soma and a point along the axon. 
	Watch how the AP is triggered bidirectionally and observe what happens 
	when two action potentials bump into each other.
	
ex3.4_myelinated_axon.py: Set up a long spiral myelinated axon with 
	HH channels, odd though this combination is. Inject current at the 
	soma. Display using moogli as well as plotting the output at a few 
	points.  Note that we get failure of propagation through nodes of 
	Ranvier on some of the somatic action potentials.

ex4.0_scaledSoma.py: This is very similar to ex3, but it introduces a 
	different way to define the soma and its diameter and length, in
	the cellProto argument. The diameter and length are optional.

ex4.1_ballAndStick.py: This shows how to make a ball-and-stick model of a 
	neuron using a cellProto definition line. Things to try:
	- Change length
	- Change number of segments. Explore what it does to accuracy. How
		will you know that you have an accurate model?

ex4.2_ballAndStickSpeed.py: This is very similar to 4.1, but it runs
	much longer to give you a chance to benchmark your system. It also
	illustrates how to generate an interesting sine-wave stimulus.
	Things to try:
	- How slow does it get if you turn on the 3-D moogli display?
	- Is it costlier to run 2 compartments for 1000 seconds, or 
		200 compartments for 10 seconds?

ex5.0_random_syn_input.py: Deliver Poisson (random) synaptic input to glu 
	synapse on soma.

ex5.1_periodic_syn_input.py: Deliver periodic synaptic input to glu synapse on
	soma.

ex6_chem_osc.py: Reaction system in a single compartment. This is a simple
	chemical oscillator system: 
	s ---a---> a  // s goes to a, catalyzed by a.
	s ---a---> b  // s goes to b, catalyzed by a.
	a ---b---> s  // a goes to s, catalyzed by b.
	b -------> s  // b is degraded irreversibly to s
	
ex7.0_spatial_chem_osc.py: Oscillating reaction-diffusion system. This is the 
	same oscillator as in ex6, but now in a length of dendrite so we get 
	travelling waves.
	Suggestions:
	- Play with the diffusionLength term. See what happens if it is too
	sparse or too fine.
	- See how to give different initial conditions. For example, what if 
	the oscillations start from the middle?
	- Replace the built in chemical oscillator with just a simple diffusing
	molecule concentrated in the middle or one end. Convince yourself 
	that diffusion happens as you expect.
	- Put instead an initial linear gradient of molecules. 
	- Elaborate the single molecule with a reaction so that there is a
	buffered source of molecules at one end, and a sink at the other. Does
	this system make sense?

ex7.1_diffusive_gradient.py: Simple diffusion of a molecule that starts out 
	bunched up to the left of a cylinder, and spreads out over time.
	Suggestions:
	- Use different diffusion constants.
	- Compare to analytical solution

ex7.2_CICR.py: Calcium-induced Calcium release model. Demonstrates the use of
	endo compartments. Endo-compartments, as the name suggests, represent
	compartments that sit within other cellular compartments. If the 
	surround compartment is subdivided into N voxels, so is the endo-
	compartment. Here we use the endo-compartment to represent the 
	endoplasmic reticulum. We also introduce ConcChans in the chemical 
	model, which act as membrane pores whose permeability scales with 
	number of channels in the open state. Here the IP3R opens due to 
	various chemical steps, and the whole system shows propagating-wave
	oscillations.

ex7.3_simple_transport.py: Model of a single molecule undergoing molecular
	transport along a cylinder. All the molecules start out in a single 
	voxel on the left, the spread and move along the cylinder, and 
	eventually they all end up on the right. Note the formulation for
	transport is to compute the number of moving molecules from the 
	product of the motor constant with the number in the local compartment:
	Flux = conc * motorConst / spacing
	This is not a pure shift register, which would have just assigned the
	local conc to the next voxel. This is why there is a spread.
	Suggestions:
		- Play with different motor rates.
		- Change the diffConst of A to a nonzero value.
		- Consider how you could avoid the buildup in the last voxel.
		- Consider how to achieve a nice exponential falloff over a
			much longer range than possible with diffusion.

ex7.4_travelling_osc.py: Another transport model. Here the entire system is the
	simple oscillator model, and instead of diffusion the molecule a is
	transported to the right. The oscillatory zone slowly moves to the
	right, with an amplification in the last compartment due to end-effects.
	Suggestions:
		- What happens if all molecules undergo transport?
		- What happens if b is transported opposite to a?
		- What happens if there is also diffusion?

ex7.5_bidirectional_transport.py: Very similar system to ex7.4, but now 
	b and s are transported
	the other way from a. The oscillations propagate to the right, but the
	zone of oscillations gets eroded from right to left.
	Suggestions:
		- There is a model for formation of vertebral somites, in
		which an oscillator at one end is coupled to the outgrowing
		cells. The departing cells retain a signal that is 'frozen' 
		in the oscillation phase they saw. Can you model this using
		the MOOSE transport equations? Hint: probably not, but it is 
		instructive to try. You need a bistable or non-distorting
		tranport system to be able to retain the signal shape as 
		it propagates.

ex7.6_French_flag (pending): Here we show how to achieve local developmental 
	patterning due to molecular gradients. We set up the gradients using 
	two counter-propagating transport sytems, and use a 3-component 
	winner-take-all chemical system to get nice sharp zones.
	Suggestions:
		- Contrast winner-take-all with a simple molecular cooperative
		system for obtaining 3 zones.

ex8.0_multiscale_KA_phosph.py: Multiscale model that is bistable. It switches
	between a resting, non-spiking state, and an active, tonically spiking
	state. In the active state, spiking -> Ca influx -> chemical activity->
	phosphorylation and inactivation of KA -> depolarization -> spiking.
	This example illustrates rather a lot of new things: adaptors,
	wave displays of values varying in space and time, and including a 
	moderately involved chemical model into a simulation.
	Suggestions:
	- Vary the adaptor settings, which couple electrical to chemical
	signaling and vice versa.
	- Play with the channel densities
	- Open the chem model in moosegui and vary its parameters too.

ex8.1_synTrigCICR.py: Multiscale model of CICR in dendrite triggered by 
	synaptic input. Models Ca events in dendrite, ER inside dendrite, 
	and spine. The signaling is a slight change from the toy model used
	in ex7.2_CICR.py. Note specially how the range of CICR wave propagation
	is limited by the level of IP3 in the dendrite.
	Suggestions:
	- Vary IP3 profile. Display it as a wave plot.
	- Vary diffusion constant of Ca. What effect does it have on 
	propagation and initiation?
	- Try to change the width of the CICR wave, and its propagation speed.

ex8.2_multiscale_gluR_phosph_3compt.py: Multiscale model across PSD, spine head,
	and dendrite. This is bistable as long as periodic synaptic input keeps
	coming. This is how it works:
	At baseline, we just have small EPSPs and little Ca influx. A burst of
	strong synaptic input causes Ca entry into the spine via NMDAR. This 
	triggers activation of CaMKII and its translocation to the PSD, where 
	it phosphorylates and increases the conductance of gluR. Now that gluR
	has a greater weight, just baseline input keeps Ca trickling in enough
	to keep the CaMKII active. In the meantime Ca diffuses into the 
	dendrite and spreads. Lots going on. 
	Here are the reactions:
	Ca+CaM <===> Ca_CaM;    Ca_CaM + CaMKII <===> Ca_CaM_CaMKII (all in 
	spine head, except that the Ca_CaM_CaMKII translocates to the PSD)
	chan ------Ca_CaM_CaMKII-----> chan_p; chan_p ------> chan  (all in PSD)
	Suggestions:
	- Add GABAR using make_GABA(), put it on soma or dendrite. Stimulate it
	after 20 s to see if you can turn off the sustained activation
	- Replace the 'periodicsyn' in stimList with 'randsyn'. This gives 
	Poisson activity at the specified mean frequency. Does the switch 
	remain reliable?
	- What are the limits of various parameters for this switching? You 
	could try basal synaptic rate, burst rate, the various scaling factors
	for the adaptors, the densities of various channels, synaptic weight,
	and so on.
	- In real life an individual synaptic EPSP is tiny, under a millivolt. 
	How many synapses would you need to achieve this kind of switching? 
	You can play with # of synapses by altering the spacing between 
	spines as the third argument of spineDistrib.

ex8.3_spine_vol_change.py: Multiscale model demonstrating spine geometry 
	changing due to signaling events in a multiscale model. 
	The reactions are the same as in 8.2:
	Ca+CaM <===> Ca_CaM;    Ca_CaM + CaMKII <===> Ca_CaM_CaMKII (all in 
	spine head, except that the Ca_CaM_CaMKII translocates to the PSD)
	chan ------Ca_CaM_CaMKII-----> chan_p; chan_p ------> chan  (all in PSD)
	Here the chan_p maps through an adaptor to the psdArea. There are many
	plots here to show the effects on max channel conductance, on 
	concentrations of molecules, and on passive electrical properties of 
	the spine. The diffusion properties also change but are harder to
	visualize.
	Suggestions:
	- The Spine class (instance: spine) manages several possible scaling 
	targets on the spine geometry: shaftLength, shaftDiameter, 
	headLength, headDiameter, psdArea, headVolume, totalLength. Try them
	out. Think about mechanisms by which molecular concentrations might
	affect each.
	- When volume changes, we assume that the molecular numbers stay 
	fixed, so concentration changes. Except for buffered molecules, where
	we assume concentration remains fixed. Use this to design a bistable
	simply relying on molecules and spine geometry terms. 
	- Even more interesting, use it to design an oscillator.


ex9.0_load_neuronal_morphology_file.py: Load .swc morphology file and view it.
	Illustrates loading a file in rdesigneur. You've already seen how to
	make a moogli viewer, this utilizes it for something more complicated.
	Suggestion: 
	- The tutorial directory already has a number of pre-loaded files from
	NeuroMorpho. Pass them in to ex9.0 on the command line:
	python ex9.0_load_neuronal_morphology_file.py <morpho.swc>
	- Grab other morphology files from NeuroMorpho.org,  try them out.

ex9.1_chans_in_neuronal_morph.py: Build an active neuron model by putting 
	channels into a morphology file. 
	Same as above except now we distribute ion channels over different
	parts of the cell morphology. In addition we illustrate how to  use
	Moogli to display both voltage and Ca influx. The units of Ca are
	arbitrary.
	Suggestion: 
	- Try another morphology file. 
	- Try different channel distributions by editing the chanDistrib lines.
        - There are numerous predefined channels available within Rdesigneur.
	  These can be set up using the following chanProto options:
		['make_HH_Na()', 'HH_Na']
		['make_HH_K_DR()', 'HH_K']
		['make_Na()', 'Na']
		['make_K_DR()', 'K_DR']
		['make_K_A()', 'K_A']
		['make_K_AHP()', 'K_AHP']
		['make_K_C()', 'K_C']
		['make_Ca()', 'Ca']
		['make_Ca_conc()', 'Ca_conc']
		['make_glu()', 'glu']
		['make_GABA()', 'GABA']
	  Then the chanDistrib can refer to these channels instead.
	- Deliver stimuli on the dendrites rather than the soma.

ex9.2_spines_in_neuronal_morpho.py: Add spines to a neuron built from a 
	morphology file and put active channels in it.
	Same as ex9.1 except now we also insert spines, using the spineDistrib
	keyword.
	Suggestions: 
	- Try different spine settings. Warning: if you put in too many spines
	it will take much longer to load and run!
	- Try different spine geometry layouts. 
	- See if you can deliver the current injection to the spine. Hint: the
	name of the spine compartments is 'head#' where # is the index of the
	spine.


ex9.3_spiral_spines.py: Just for fun. Illustrates how to place spines in a
spiral around the dendrite. For good measure the spines get bigger the further
they are from the soma. Note that the uniform spacing of spines is signified 
by the negative minSpacing term, the fourth argument to spineDistrib.
	Suggestions:
	- Play with expressions for spine size and angular placement.
	- See what happens if the segment size gets smaller than the
  	spine spacing.
	
To come:
rdes_ex10.py: Build a spiny neuron, and insert the oscillatory chemical model
	into it. Unfortunately Moogli cannot yet plot the chemical waves 
	going up and down in the dendrites. This example is not yet
	displayed on the web documentation.


rdes_ex11.py: Build a cylindrical dend. Put in ion channels
	for activity including VGCC-driven Ca influx. Put in an adaptor to
	take elec Ca and control chem Ca with that. Put in a reaction so Ca
	turns on an enzyme that phosphorylates a chem pool that represents 
	KA and disables it. Put in an adaptor from the dephosphorylated KA 
	to the electrical KA to control its conductance. Together, this should
	flip into a state of sustained activity when the Ca gets high enough.
	Plot Vm, Ca, KA

rdes_ex12.py: Build a cylindrical dend with a spine on it. Put in ion channels
	for activity including NMDAR-driven Ca influx. Put in a reaction to
	take Ca in the spine to activate CaMKII, and CaMKII to control a 
	reaction that phosphorylates stargazin and expands the spine. 
	See how the conductance etc increases.
	Use Moogull to plot spine as it expands, with colour displaying the Ca.
	- Implement importing of proto python file and using simple syntax to
	call the creation functions
	- Implement synaptic input as synUniform and synPoisson. Or put in as
	chan prototypes?

