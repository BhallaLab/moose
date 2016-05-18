-----

# **Rdesigneur: Building multiscale models**

Upi Bhalla

Dec 28 2015.

-----

## Contents
	

## Introduction

**Rdesigneur** (Reaction Diffusion and Electrical SIGnaling in NEURons) is an
interface to the multiscale modeling capabilities in MOOSE. It is designed
to build models incorporating biochemical signaling pathways in 
dendrites and spines, coupled to electrical events in neurons. Rdesigneur
assembles models from predefined parts: it delegates the details to 
specialized model definition formats. Rdesigneur combines one or more of
the following cell parts to build models:

*	Neuronal morphology
*	Dendritic spines
*	Ion channels
*	Reaction systems

Rdesigneur's main role is to specify how these are put together, including 
assigning parameters to do so. Rdesigneur also helps with setting up the
simulation input and output.

## Quick Start
Here we provide a few use cases, building up from a minimal model to a 
reasonably complete multiscale model spanning chemical and electrical signaling.

### Bare Rdesigneur: single passive compartment
If we don't provide any arguments at all to the Rdesigneur, it makes a model
with a single passive electrical compartment in the MOOSE path 
`/model/elec/soma`. Here is how to do this:

	import moose
	import rdesigneur as rd
	rdes = rd.rdesigneur()
	rdes.buildModel()

To confirm that it has made a compartment with some default values we can add 
a line:
	
	moose.showfields( rdes.soma )

This should produce the output:

	[ /model[0]/elec[0]/soma[0] ]
	diameter         = 0.0005
	fieldIndex       = 0
	Ra               = 7639437.26841
	y0               = 0.0
	Rm               = 424413.177334
	index            = 0
	numData          = 1
	inject           = 0.0
	initVm           = -0.065
	Em               = -0.0544
	y                = 0.0
	numField         = 1
	path             = /model[0]/elec[0]/soma[0]
	dt               = 0.0
	tick             = -2
	z0               = 0.0
	name             = soma
	Cm               = 7.85398163398e-09
	x0               = 0.0
	Vm               = -0.06
	className        = ZombieCompartment
	idValue          = 465
	length           = 0.0005
	Im               = 1.3194689277e-08
	x                = 0.0005
	z                = 0.0


### Simulate and display current pulse to soma
A more useful script would run and display the model. Rdesigneur can help with
the stimulus and the plotting. This simulation has the same passive 
compartment, and current is injected as the simulation runs.
This script displays the membrane potential of the soma as it charges and 
discharges.

	import moose
	import rdesigneur as rd
	rdes = rd.rdesigneur(
		stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 2e-8']],
		plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']],
	)
	rdes.buildModel()
	moose.reinit()
	moose.start( 0.3 )
	rdes.display()

The *stimList* defines a stimulus. Each entry has five arguments:

	`[region_in_cell, region_expression, moose_object, parameter, expression_string]`

+	`region_in_cell` specifies the objects to stimulate. Here it is just the
	soma.
+	`region_expression` specifies a geometry based calculation to decide
	whether to apply the stimulus. The value must be >0 for the stimulus
	to be present. Here it is just 1.
	`moose_object` specifies the simulation object to operate upon during 
	the stimulus. Here the `.` means that it is the soma itself. In other
	models it might be a channel on the soma, or a synapse, and so on.
+	`parameter` specifies the simulation parameter on the moose object that
	the stimulus will modify. Here it is
	the injection current to the soma compartment.
+	`expression_string` calculates the value of the parameter, typically
	as a function of time. Here we use the function 
	`(t>0.1 && t<0.2) * 2e-8` which evaluates as 2e-8 between the times of
	0.1 and 0.2 seconds.

To summarise this, the *stimList* here means *inject a current of 20nA to the
soma between the times of 0.1 and 0.2 s*.

The *plotList* defines what to plot. It has a similar set of arguments:

	`[region_in_cell, region_expression, moose_object, parameter, title_of_plot]`
These mean the same thing as for the stimList except for the title of the plot.

The *rdes.display()* function causes the plots to be displayed.

![Plot for current input to passive compartment](../../images/rdes2_passive_squid.png)

When we run this we see an initial depolarization as the soma settles from its
initial -65 mV to a resting Em = -54.4 mV. These are the original HH values, see
the example above. At t = 0.1 seconds there is another depolarization due
to the current injection, and at t = 0.2 seconds this goes back to the resting
potential.

### HH Squid model in a single compartment
Here we put the Hodgkin-Huxley squid model channels into a passive compartment.
The HH channels are predefined as prototype channels for Rdesigneur,

	import moose
	import pylab
	import rdesigneur as rd
	rdes = rd.rdesigneur(
    	chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    	chanDistrib = [
        	['Na', 'soma', 'Gbar', '1200' ],
        	['K', 'soma', 'Gbar', '360' ]],
    	stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 1e-8' ]],
    	plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']]
	)
	
	rdes.buildModel()
	moose.reinit()
	moose.start( 0.3 )
	rdes.display()


Here we introduce two new model specification lines:

+	**chanProto**: This specifies which ion channels will be used in the 
	model.
	Each entry here has two fields: the source of the channel definition,
	and (optionally) the name of the channel.
	In this example we specify two channels, an Na and a K channel using
	the original Hodgkin-Huxley parameters. As the source of the channel
	definition we use the name of the  Python function that builds the 
	channel. The *make_HH_Na()* and *make_HH_K()* functions are predefined 
	but we can also specify our own functions for making prototypes.
	We could also have specified the channel prototype using the name
	of a channel definition file in ChannelML (a subset of NeuroML) format.
+	**chanDistrib**: This specifies  *where* the channels should be placed
	over the geometry of the cell. Each entry in the chanDistrib list 
	specifies the distribution of parameters for one channel using four 
	entries: 

	`[object_name, region_in_cell, parameter, expression_string]`

	In this case the job is almost trivial, since we just have a single 
	compartment named *soma*. So the line

	`['Na', 'soma', 'Gbar', '1200' ]`

	means *Put the Na channel in the soma, and set its maximal 
	conductance density (Gbar) to 1200 Siemens/m^2*. 

As before we apply a somatic current pulse. Since we now have HH channels in
the model, this generates action potentials.

![Plot for HH squid simulation ](../../images/rdes3_squid.png)


### Reaction system in a single compartment
Here we use the compartment as a place in which to embed a chemical model.
The chemical oscillator model is predefined in the rdesigneur prototypes.

	import moose
	import pylab
	import rdesigneur as rd
	rdes = rd.rdesigneur(
    		turnOffElec = True,
    		diffusionLength = 1e-3, # Default diffusion length is 2 microns
    		chemProto = [['makeChemOscillator()', 'osc']],
    		chemDistrib = [['osc', 'soma', 'install', '1' ]],
    		plotList = [['soma', '1', 'dend/a', 'conc', 'a Conc'],
        		['soma', '1', 'dend/b', 'conc', 'b Conc']]
	)
	rdes.buildModel()
	b = moose.element( '/model/chem/dend/b' )
	b.concInit *= 5
	moose.reinit()
	moose.start( 200 )
	
	rdes.display()

In this special case we set the turnOffElec flag to True, so that Rdesigneur 
only sets up chemical and not electrical calculations.  This makes the 
calculations much faster, since we disable electrical calculations and delink
chemical calculations from them.

We also have a line which sets the `diffusionLength` to 1 mm, so that it is 
bigger than the 0.5 mm squid axon segment in the default compartment. If you 
don't do this the system will subdivide the compartment into the default 
2 micron voxels for the purposes of putting in a reaction-diffusion system.
We discuss this case below.

Note how the *plotList* is done here. To remind you, each entry has five 
arguments

	[region_in_cell, region_expression, moose_object, parameter, title_of_plot]

The change from the earlier usage is that the `moose_object` now refers to 
a chemical entity, in this example the molecule *dend/a*. The simulator 
builds a default chemical compartment named *dend* to hold the reactions 
defined in the *chemProto*. What we do in this plot is to select molecule *a*
sitting in *dend*, and plot its concentration. Then we do this again for
molecule *b*. 

After the model is built, we add a couple of lines to change the 
initial concentration of the molecular pool *b*. Note its full path within 
MOOSE: */model/chem/dend/b*. It is scaled up 5x to give rise to slowly 
decaying oscillations.

![Plot for single-compartment reaction simulation ](../../images/rdes4_osc.png)

### Reaction-diffusion system

In order to see what a reaction-diffusion system looks like, delete the
`diffusionLength` expression in the previous example and add a couple of lines
to set up 3-D graphics for the reaction-diffusion product: 

	import moose
	import pylab
	import rdesigneur as rd
	rdes = rd.rdesigneur(
    		turnOffElec = True,
    		chemProto = [['makeChemOscillator()', 'osc']],
    		chemDistrib = [['osc', 'soma', 'install', '1' ]],
    		plotList = [['soma', '1', 'dend/a', 'conc', 'Concentration of a'],
        		['soma', '1', 'dend/b', 'conc', 'Concentration of b']],
    		moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]
	)

	rdes.buildModel()
	bv = moose.vec( '/model/chem/dend/b' )
	bv[0].concInit *= 2
	bv[-1].concInit *= 2
	moose.reinit()

	rdes.displayMoogli( 1, 400, 0.001 )



This is the line we deleted. 

    	diffusionLength = 1e-3,

With this change we permit
*rdesigneur* to use the default diffusion length of 2 microns. 
The 500-micron axon segment is now subdivided into 250 voxels, each of 
which has a reaction system and diffusing molecules. To make it more 
picturesque, we have added a line after the plotList, to display the outcome 
in 3-D:

	moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]

This line says: take the model compartments defined by `soma` as the region
to display, do so throughout the the geometry (the `1` signifies this), and
over this range find the chemical entity defined by `dend/a`. For each `a`
molecule, find the `conc` and dsiplay it. There are two optional arguments, 
`0` and `360`, which specify the low and high value of the displayed variable.

In order to initially break the symmetry of the system, we change the initial
concentration of molecule b at each end of the cylinder:

	bv[0].concInit *= 2
	bv[-1].concInit *= 2

If we didn't do this the entire system would go through a few cycles of 
decaying oscillation and then reach a boring, spatially uniform, steady state.
Try putting an initial symmetry break elsewhere to see what happens.

To display the concenctration changes in the 3-D soma as the simulation runs,
we use the line

	`rdes.displayMoogli( 1, 400, 0.001 )`

The arguments mean: *displayMoogli( frametime, runtime, rotation )*
Here, 

	frametime = time by which simulation advances between display updates
	runtime = Total simulated time
	rotation = angle by which display rotates in each frame, in radians.

When we run this, we first get a 3-D display with the oscillating 
reaction-diffusion system making its way inward from the two ends. After the
simulation ends the plots for all compartments for the whole run come up.


![Display for oscillatory reaction-diffusion simulation ](../../images/rdes5_reacdiff.png)

### Primer on using the 3-D MOOGLI display
Here is a short primer on the 3-D display controls.

- *Roll, pitch, and yaw*: Use the letters *r*, *p*, and *y*. To rotate 
backwards, use capitals.
- *Zoom out and in*: Use the *,* and *.* keys, or their upper-case equivalents,
*<* and *>*. Easier to remember if you think in terms of the upper-case.
- *Left/right/up/down*: Arrow keys.
- *Quit*: control-q or control-w.
- You can also use the mouse or trackpad to control most of the above. 
- By default rdesigneur gives Moogli a small rotation each frame. It is the
*rotation* argument in the line:

	`displayMoogli( frametime, runtime, rotation )`

These controls operate over and above this rotation, but the rotation 
continues. If you set the rotation to zero you can, with a suitable flick of
the mouse, get the image to rotate in any direction you choose as long as the
window is updating.

### Make a toy multiscale model with electrical and chemical signaling.
Now we put together chemical and electrical models. In this toy model we have an
HH-squid type single compartment electrical model, cohabiting with a chemical
oscillator. The chemical oscillator regulates K+ channel amounts, and the
average membrane potential regulates the amounts of a reactant in the 
chemical oscillator. This is a recipe for some strange firing patterns.

	import moose
	import pylab
	import rdesigneur as rd
	rdes = rd.rdesigneur(
        	# We want just one compartment so we set diffusion length to be
        	# bigger than the 0.5 mm HH axon compartment default. 
    			diffusionLength = 1e-3,
    			chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    			chanDistrib = [
        			['Na', 'soma', 'Gbar', '1200' ],
        			['K', 'soma', 'Gbar', '360' ]],
    		chemProto = [['makeChemOscillator()', 'osc']],
    		chemDistrib = [['osc', 'soma', 'install', '1' ]],
       		# These adaptor parameters give interesting-looking but
       		# not particularly physiological behaviour.
    		adaptorList = [
        		[ 'dend/a', 'conc', 'Na', 'modulation', 1, -5.0 ],
        		[ 'dend/b', 'conc', 'K', 'modulation', 1, -0.2],
        		[ 'dend/b', 'conc', '.', 'inject', -1.0e-7, 4e-7 ],
        		[ '.', 'Vm', 'dend/s', 'conc', 2.5, 20.0 ]
    		],
    		plotList = [['soma', '1', 'dend/a', 'conc', 'a Conc'],
        		['soma', '1', 'dend/b', 'conc', 'b Conc'],
        		['soma', '1', 'dend/s', 'conc', 's Conc'],
        		['soma', '1', 'Na', 'Gk', 'Na Gk'],
        		['soma', '1', '.', 'Vm', 'Membrane potential']
		]
	)

	rdes.buildModel()
	moose.reinit()
	moose.start( 250 ) # Takes a few seconds to run this.

	rdes.display()

We've already modeled the HH squid model and the oscillator individually,
and you should recognize the parts of those models above.
The new section that makes this work the *adaptorList* which specifies how 
the electrical and chemical parts talk to each other. This entirely
fictional set of interactions goes like this:

	[ 'dend/a', 'conc', 'Na', 'modulation', 1, -5.0 ]

+	*dend/a*: The originating variable comes from the 'a' pool on the
	'dend' compartment.

	*conc*: This is the originating variable name on the 'a' pool.

	*Na*: This is the target variable

	*modulation*: scale the Gbar of Na up and down. Use 'modulation'
	rather than direct assignment of Gbar since Gbar is different for
	each differently-sized compartment. 

	*1*: This is the initial offset

	*-5.0*: This is the scaling from the input to the parameter updated
	in the simulation.

A similar set of adaptor entries couple the molecule  *dend/b* to the 
K channel, *dend/b* again to the current injection into the soma, and the 
membrane potential to the concentration of *dend/s*. 


![Plot for toy multiscale model ](../../images/rdes6_multiscale.png)

### Morphology: Load .swc morphology file and view it
Here we build a passive model using a morphology file in the .swc file format
(as used by NeuroMorpho.org). The morphology file is predefined for Rdesigneur
and resides in the 
directory `./cells`. We apply a somatic current pulse, and view
the somatic membrane potential in a plot, as before. 
To make things interesting we display the morphology in 3-D upon which we
represent the membrane potential as colors.

	import moose
	import rdesigneur as rd
	rdes = rd.rdesigneur(
		cellProto = [['./cells/h10.CNG.swc', 'elec']],
		stimList = [['soma', '1', '.', 'inject', 't * 25e-9' ]], 
		plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
			['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
		moogList = [['#', '1', '.', 'Vm', 'Soma potential']]
	)

	rdes.buildModel()

	moose.reinit()
	rdes.displayMoogli( 0.0002, 0.1 )

Here the new concept is the cellProto line, which loads in the specified cell
model:

	`[ filename, cellname ]`

The system recognizes the filename extension and builds a model from the swc
file. It uses the cellname **elec** in this example.

We use a similar line as in the reaction-diffusion example, to build up a 
Moogli display of the cell model:

	`moogList = [['#', '1', '.', 'Vm', 'Soma potential']]`

Here we have:

	*#*: the path to use for selecting the compartments to display. 
	This wildcard means use all compartments.
	*1*: The expression to use for the compartments. Again, `1` means use
	all of them.
	*.*: Which object in the compartment to display. Here we are using the
	compartment itself, so it is just a dot.
	*Vm*: Field to display
	*Soma potential*: Title for display.

![3-D display for passive neuron](../../images/rdes7_passive.png)

### Build an active neuron model by putting channels into a morphology file
We load in a morphology file and distribute voltage-gated ion channels over 
the neuron. Here the voltage-gated channels are obtained from a number of 
channelML files, located in the `./channels` subdirectory. Since we have a 
spatially extended neuron, we need to specify the spatial distribution of 
channel densities too. 


	import moose
	import rdesigneur as rd
	rdes = rd.rdesigneur(
    	chanProto = [
        	['./chans/hd.xml'],
        	['./chans/kap.xml'],
        	['./chans/kad.xml'],
        	['./chans/kdr.xml'],
        	['./chans/na3.xml'],
        	['./chans/nax.xml'],
        	['./chans/CaConc.xml'],
        	['./chans/Ca.xml']
    	],
    	cellProto = [['./cells/h10.CNG.swc', 'elec']],
    	chanDistrib = [ \
        	["hd", "#dend#,#apical#", "Gbar", "50e-2*(1+(p*3e4))" ],
        	["kdr", "#", "Gbar", "p < 50e-6 ? 500 : 100" ],
        	["na3", "#soma#,#dend#,#apical#", "Gbar", "850" ],
        	["nax", "#soma#,#axon#", "Gbar", "1250" ],
        	["kap", "#axon#,#soma#", "Gbar", "300" ],
        	["kap", "#dend#,#apical#", "Gbar",
            	"300*(H(100-p*1e6)) * (1+(p*1e4))" ],
        	["Ca_conc", "#", "tau", "0.0133" ],
        	["kad", "#soma#,#dend#,#apical#", "Gbar", "50" ],
        	["Ca", "#", "Gbar", "50" ]
    	],
    	stimList = [['soma', '1', '.', 'inject', '(t>0.02) * 1e-9' ]],
    	plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
            	['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
    	moogList = [['#', '1', 'Ca_conc', 'Ca', 'Calcium conc (uM)', 0, 120],
        	['#', '1', '.', 'Vm', 'Soma potential']]
	)
	
	rdes.buildModel()
	
	moose.reinit()
	rdes.displayMoogli( 0.0002, 0.052 )


Here we make more extensive use of two concepts which we've already seen from 
the single compartment squid model:

1. *chanProto*: This defines numerous channels, each of which is of the form:

	`[ filename ]`

	or 

	`[ filename, channelname ]`

If the *channelname* is not specified the system uses the last part of the
channel name, before the filetype suffix.

2. *chanDistrib*: This defines the spatial distribution of each channel type.
Each line is of a form that should be familiar now:

	`[channelname, region_in_cell, parameter, expression_string]`

- The *channelname* is the name of the prototype from *chanproto*. This is 
usually an ion channel, but in the example above you can also see a calcium 
concentration pool defined.
- The *region_in_cell* is typically defined using wildcards, so that it
generalizes to any cell morphology.
For example, the plain wildcard `#` means to consider 
all cell compartments. The wildcard `#dend#` means to consider all compartments with the string `dend`
somewhere in the name. Wildcards can be comma-separated, so 
`#soma#,#dend#` means consider all compartments with either soma or dend in
their name. The naming in MOOSE is defined by the model file. Importantly,
in **.swc** files MOOSE generates names that respect the classification of 
compartments into axon, soma, dendrite, and apical dendrite compartments 
respectively. SWC files generate compartment names such as:

		soma_<number>
		dend_<number>
		apical_<number>
		axon_<number>

where the number is automatically assigned by the reader. In order to 
select all dendritic compartments, for example, one would use *"#dend#"*
where the *"#"* acts as a wildcard to accept any string.
- The *parameter* is usually Gbar, the channel conductance density in *S/m^2*.
If *Gbar* is zero or less, then the system economizes by not incorporating this
channel mechanism in this part of the cell. Similarly, for calcium pools, if 
the *tau* is below zero then the calcium pool object is simply not inserted 
into this part of the cell.
- The *expression_string* defines the value of the parameter, such as Gbar.
This is typically a function of position in the cell. The expression evaluator 
knows about several parameters of cell geometry. All units are in metres: 

+ *x*, *y* and *z* coordinates.
+ *g*, the geometrical distance from the soma
+ *p*, the path length from the soma, measured along the dendrites. 
+ *dia*, the diameter of the dendrite.
+ *L*, The electrotonic length from the soma (no units).

Along with these geometrical arguments, we make liberal use of the Heaviside 
function H(x) to set up the channel distributions. The expression evaluator
also knows about pretty much all common algebraic, trignometric, and logarithmic
functions, should you wish to use these.

Also note the two Moogli displays. The first is the calcium 
concentration. The second is the membrane potential in each compartment. Easy!

![3-D display for active neuron](../../images/rdes8_active.png)

### Build a spiny neuron from a morphology file and put active channels in it.
This model is one step elaborated from the previous one, in that we now also
have dendritic spines. MOOSE lets one decorate a bare neuronal morphology file 
with dendritic spines, specifying various geometric parameters of their
location. As before, we use an swc file for the morphology, and the same 
ion channels and distribution.

	import moose
	import pylab
	import rdesigneur as rd
	rdes = rd.rdesigneur(
    	chanProto = [
        	['./chans/hd.xml'],
        	['./chans/kap.xml'],
        	['./chans/kad.xml'],
        	['./chans/kdr.xml'],
        	['./chans/na3.xml'],
        	['./chans/nax.xml'],
        	['./chans/CaConc.xml'],
        	['./chans/Ca.xml']
    	],
    	cellProto = [['./cells/h10.CNG.swc', 'elec']],
    	spineProto = [['makeActiveSpine()', 'spine']],
    	chanDistrib = [
        	["hd", "#dend#,#apical#", "Gbar", "50e-2*(1+(p*3e4))" ],
        	["kdr", "#", "Gbar", "p < 50e-6 ? 500 : 100" ],
        	["na3", "#soma#,#dend#,#apical#", "Gbar", "850" ],
        	["nax", "#soma#,#axon#", "Gbar", "1250" ],
        	["kap", "#axon#,#soma#", "Gbar", "300" ],
        	["kap", "#dend#,#apical#", "Gbar",
            	"300*(H(100-p*1e6)) * (1+(p*1e4))" ],
        	["Ca_conc", "#", "tau", "0.0133" ],
        	["kad", "#soma#,#dend#,#apical#", "Gbar", "50" ],
        	["Ca", "#", "Gbar", "50" ]
    	],
    	spineDistrib = [['spine', '#dend#,#apical#', '20e-6', '1e-6']],
    	stimList = [['soma', '1', '.', 'inject', '(t>0.02) * 1e-9' ]],
    	plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
            	['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
    	moogList = [['#', '1', 'Ca_conc', 'Ca', 'Calcium conc (uM)', 0, 120],
        	['#', '1', '.', 'Vm', 'Soma potential']]
	)
	
	rdes.buildModel()
	
	moose.reinit()
	rdes.displayMoogli( 0.0002, 0.023 )


Spines are set up in a familiar way: we first define one (or more) prototype
spines, and then distribute these around the cell. Here is the prototype
string:

    	[spine_proto, spinename]

*spineProto*: This is typically a function. One can define one's own,
but there are several predefined ones in rdesigneur. All these define a 
spine with the following parameters:

- head diameter 0.5 microns
- head length 0.5 microns
- shaft length 1 micron
- shaft diameter of 0.2 microns
- RM = 1.0 ohm-metre square
- RA = 1.0 ohm-meter
- CM = 0.01 Farads per square metre.
	
Here are the predefined spine prototypes:

- *makePassiveSpine()*: This just makes a passive spine with the 
default parameters
- *makeExcSpine()*: This makes a spine with NMDA and glu receptors,
and also a calcium pool. The NMDA channel feeds the Ca pool.
- *makeActiveSpine()*: This adds a Ca channel to the exc_spine.
and also a calcium pool.

The spine distributions are specified in a familiar way for the first few 
arguments, and then there are multiple (optional) spine-specific parameters:

*[spinename, region_in_cell, spacing, spacing_distrib, size, size_distrib, angle, angle_distrib ]*

Only the first two arguments are mandatory.

- *spinename*: The prototype name
- *region_in_cell*: Usual wildcard specification of names of compartments in which to put the spines.
- *spacing*: Math expression to define spacing between spines. In the current implementation this evaluates to `1/probability_of_spine_per_unit_length`.
Defaults to 10 microns. Thus, there is a 10% probability of a spine insertion in every micron. This evaluation method has the drawback that it is possible to space spines rather too close to each other. If spacing is zero or less, no spines are inserted.
- *spacing_distrib*: Math expression for distribution of spacing. In the current implementation, this specifies the interval at which the system samples from the spacing probability above. Defaults to 1 micron.
- *size*: Linear scale factor for size of spine. All dimensions are scaled by this factor. The default spine head here is 0.5 microns in diameter and length. If the scale factor were to be 2, the volume would be 8 times as large. Defaults to 1.0.
- *size_distrib*: Range for size of spine. A random number R is computed in the range 0 to 1, and the final size used is `size + (R - 0.5) * size_distrib`. Defaults to 0.5
- *angle*: This specifies the initial angle at which the spine sticks out of the dendrite. If all angles were zero, they would all point away from the soma. Defaults to 0 radians.
- *angle_distrib*: Specifies a random number to add to the initial angle. Defaults to 2 PI radians, so the spines come out in any direction.

One may well ask why we are not using a Python dictionary to handle all 
these parameters. Short answer is: terseness. Longer answer is that the 
rdesigneur format is itself meant to be an intermediate form for an 
eventual high-level, possibly XML-based multiscale modeling format.

![3-D display for spiny active neuron](../../images/rdes9_spiny_active.png)

### Build a spiny neuron from a morphology file and put a reaction-diffusion system in it.
Rdesigneur is specially designed to take reaction systems with a dendrite,
a spine head, and a spine PSD compartment, and embed these systems into 
neuronal morphologies. This example shows how this is done.

The dendritic molecules diffuse along the dendrite
in the region specified by the *chemDistrib* keyword. In this case they are
placed on all apical and basal dendrites, but only at distances over 
500 microns from the soma. The spine head and PSD 
reaction systems are inserted only into spines within this same *chemDistrib*
zone. Diffusion coupling between dendrite, and each spine head and PSD is also 
set up.
It takes a predefined chemical model file for Rdesigneur, which resides 
in the `./chem` subdirectory. As in an earlier example, we turn off the 
electrical calculations here as they are not needed. 
Here we plot out the number of receptors on every single spine as a function
of time.

(Documentation still to come here)

### Make a full multiscale model with complex spiny morphology and electrical and chemical signaling.

(Documentation still to come here)
