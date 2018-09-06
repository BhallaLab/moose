This tutorial covers chemical bistable systems. 

1. simpleBis.py
This can be run using the form
python simpleBis.py [gsl|gssa|ee]
where the optional argument specifies the numerical engine to use. It defaults
to gsl, a deterministic solver.

This example shows the key property of a chemical bistable system: it has
two stable states. 
Here we start out with the system settling rather quickly to the first stable
state, where molecule A is high (blue) and the complementary molecule B (green)
is low. At t = 100s, we deliver a perturbation, which is to move 90% of the
A molecules into B. This triggers a state flip, which settles into a distinct
stable state where there is more of B than of A. At t = 200s we reverse the
flip by moving 99% of B molecules back to A. 

If we run the simulation with the gssa option
python simpleBis.py gssa

we see exactly the same sequence of events, except now the switch is noisy.
The calculations are now run with the Gillespie Stochastic Systems Algorithm
(gssa) which incorporates probabilistic reaction events. The switch still
switches but one can see that it might flip spontaneously once in a while.

Things to do:
1. There are sliders to control the enzyme rates and the concentrations. 
Play with them. Observe when the states flip
2. Establish the bistability range for the enzyme rates and the concentrations,
	that is the range in which the system is able to hold two states.
	There will be an upper and lower limit for most of the parameters.
3. It is interesting also to see how one parameter can compensate for 
	another in defining the bistable range.
4. Try re-running using the gssa, to see how well it tracks the deterministic
	case. See if the state flip happens around the same point.
5. Modify a copy of this to run using gssa, with an assigned volume, but 
	without any triggers for state flips. Find out its persistence time
	in each (upper and lower) state.

-----------------------------------------------------------------------------
2. BistabilityStates.py
This uses a similar model to SimpleBis.py. Here we simply run the model out
for 500 seconds, which reaches steady-state in most cases. 
The example lets one buffer a couple of key enzymes, as a way to building up 
a dose-response curve, which in turn is a way to estimate the steady states 
of the system.

Things to do:
1. As before, there are sliders to control the enzyme rates and the 
concentrations. In addition there is a radio button which lets one buffer
molecule B or molecule C. Play with them and see what happens.

2. Buffer B, systematically vary it, and measure C. This is one of the 
	dose-response curves.
	Now buffer C instead, and vary B. This is the other dose-response
	curve. 
	Plot them on the same axis (e.g., B vs C) and see what happens.

3. Change kcat2 to a higher value. Repeat step 2. See if you end up with only
	one stable point.

4. Buffer A. Observe how the simulation is prone to get stuck. Figure out why.

-----------------------------------------------------------------------------
3. scaleVolumes.py
This script runs exactly the same model as in simpleBis.py, but it 
automatically scales the volumes from 1e-19 down to smaller values.
Run using:
python scaleVolumes.py

Note how the simulation successively becomes moisier, until
at very small volumes there are spontaneous state transitions.

-----------------------------------------------------------------------------
4. strongBis.py
This example illustrates a particularly strong, that is, parametrically
robust bistable system. The model topology is symmetric between
molecules b and c. We have both positive feedback of molecules **b** and **c**
onto themselves, and also inhibition of **b** by **c** and vice versa. 
Open the python file to see what is happening.
The simulation starts at a symmetric point and the model settles at 
precisely the balance point where **a**, **b**, and **c** are at the same 
concentration.
At t= 100 we apply a small molecular 'tap' to push it over to a state 
where **c** is larger. This is stable.
At t = 210 we apply a moderate push to show that it is now very stably in
this state, and the system rebounds to its original levels.
At t = 320 we apply a strong push to take it over to a state where **b**
is larger. 
At t = 430 we give it a strong push to take it back to the **c** dominant
state.

Things to do:
1 Run it in stochastic mode
2 Modify it to run in stochastic mode with assigned volume, without the 
	triggers. See how long it stays in a given state, as a func of volume.
3 Compare with simpleBis.
4. Do 2, but now make the system asymmetric. See how this affects the state
	timing.

-----------------------------------------------------------------------------
5. mapkFB.py

This example illustrates loading, and running a kinetic model for a much 
more complex bistable positive feedback system, defined in kkit format.
This is based on Bhalla, Ram and Iyengar, Science 2002.

The core of this model is a positive feedback loop comprising of
the MAPK cascade, PLA2, and PKC. It receives PDGF and Ca2+ as
inputs.

This model is quite a large one and due to some stiffness in its
equations, it takes about 30 seconds to execute. Note that this is still
200 times faster than the events it models.

The simulation illustrated here shows how the model starts out in
a state of low activity. It is induced to 'turn on' when a
a PDGF stimulus is given for 400 seconds, starting at t = 500s.
After it has settled to the new 'on' state, the model is made to
'turn off'
by setting the system calcium levels to zero. This inhibition starts at 
t = 2900 and goes on for 500 s. 
Note that this is a somewhat unphysiological manipulation!
Following this the model settles back to the same 'off' state it was in 
originally.

-----------------------------------------------------------------------------
6. propagationBis.py
All the above models have been well-mixed, that is point or non-spatial
models. Bistables do interesting things when they are dispersed in space.
This is illustrated in this example. Here we have a tapering cylinder,
that is a pseudo 1-dimensional reaction-diffusion system. Every point in
this cylinder has the bistable system from the strongBis example.

The example has two stages. First it starts out with the model in the 
unstable transition point, and introduces a small symmetry-breaking
perturbation at one end. This rapidly propagates through the entire
length model, leaving molecule **b** at a higher value than **c**.

At t = 100 we carry out a different manipulation. We flip the concentrations
of molecules b and c for the left half of the model, and then just let it run.
Now we have opposing bistable states on either half. In the middle, the two
systems battle it out. Molecule **c** from the left side diffuses over to the 
right, and tries to inhibit **b**, and vice versa.
However we have a small asymmetry due to the tapering of the cylinder. As there
is a slightly larger volume on the left, the transition point gradually 
advances to the right, as molecule **b** yields to the slightly larger
amounts of molecule **c**.

Things to do:
1 Confirm that the movement is due to the tapering. Make a modified copy with
	the taper in the other direction. 
2 Try with different taper steepness.
3 See what happens at higher or lower diffusion constants.
4 Come up with a way to persuade the state flip to move even if there is no
	taper.

-----------------------------------------------------------------------------
7. findSteadyState.py
This is an example of how to use an internal MOOSE solver to find steady
states of a system very rapidly. The method starts from a random position
in state space that obeys mass conservation. It then finds the nearest
steady state and reports it. If it does this enough times it should find
all the steady states. 
We illustrate this process for 50 attempts to find the steady states.
It does find all of them. Each time it plots and prints the values, though
the plotting is not necessary.
The printout shows the concentrations of all molecules in the first 5 columns.
Then it prints the type of solution, and the numbers of negative and positive 
eigenvalues. In all cases the calculations are successful, though it takes
different numbers of iterations to arrive at the steady state. In some models
it would be necessary to put a cap on the number of iterations, if the 
system is not able to find a steady state.

In this example we run the bistable model using the ODE solver right at 
the end, and manually enforce transitions to show where the target steady 
states are.

Things to do:
1 Does it work for other models?
2 What happens if the system has only one steady state?
3 What happens if the system is an oscillator?

-----------------------------------------------------------------------------
8. doseResponse.py
This example generates a doseResponse plot for a bistable system, against
a control parameter (dose) that takes the system in and out again from the 
bistable regime. Like the previous example, it uses the steady-state solver 
to find the stable points for each value of the control parameter.
Unfortunately it doesn't work right now. Seems like the kcat scaling
isn't being registered.
