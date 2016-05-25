# Introduction

Neuronal models in NeuroML 1.8 format can be loaded and simulated in
the **MOOSE Graphical User Interface**. The GUI displays the
neurons in 3D, and allows visual selection and editing of neuronal
properties. Plotting and visualization of activity proceeds
concurrently with the simulation. Support for creating and editing
channels, morphology and networks is planned for the future.

# Neuronal models

Neurons are modeled as equivalent electrical circuits. The
morphology of a neuron can be broken into isopotential compartments
connected by axial resistances `R`~`a`~ denoting the cytoplasmic
resistance. In each compartment, the neuronal membrane is
represented as a capacitance `C`~`m`~ with a shunt leak resistance `R`~`m`~.
Electrochemical gradient (due to ion pumps) across the leaky
membrane causes a voltage drive `E`~`m`~, that hyperpolarizes the inside
of the cell membrane compared to the outside.

Each voltage dependent ion channel, present on the membrane, is
modeled as a voltage dependent conductance `G`~`k`~ with gating
kinetics, in series with an electrochemical voltage drive (battery)
`E`~`k`~, across the membrane capacitance `C`~`m`~, as in the figure below.

----

![**Equivalent circuit of neuronal compartments**](../../images/neuroncompartment.png)

----

Neurons fire action potentials / spikes (sharp rise and fall of
membrane potential `V`~`m`~) due to voltage dependent channels. These
result in opening of excitatory / inhibitory synaptic channels
(conductances with batteries, similar to voltage gated channels) on
other connected neurons in the network.

MOOSE can handle large networks of detailed neurons, each with
complicated channel dynamics. Further, MOOSE can integrate chemical
signaling with electrical activity. Presently, creating and
simulating these requires PyMOOSE scripting, but these will be
incorporated into the GUI in the future.

To understand channel kinetics and neuronal action potentials, run
the Squid Axon demo installed along with MOOSEGUI and consult its
help/tutorial.

Read more about compartmental modeling in the first few chapters
of the
[Book of Genesis](http://www.genesis-sim.org/GENESIS/iBoG/iBoGpdf/index.html).

Models can be defined in [NeuroML](http://www.neuroml.org), an XML
format which is well supported across simulators. Channels,
neuronal morphology (compartments), and networks can be specified
using various levels of NeuroML, namely ChannelML, MorphML and
NetworkML. Importing of cell models in the
[GENESIS](http://www.genesis-sim.org/GENESIS) `.p` format is
supported for backwards compatibitility.

# Neuronal simulations in MOOSEGUI

## Quick start

-   MOOSEGUI provides a few neuronal models in moose/Demos directory in
    user's home folder. For example, *File->Load*
    `~/moose/Demos/neuroml/PurkinjeCellPassive/PurkinjePassive.net.xml`, which is a model of the purkinje cell. A 3D rendering of the neuron appears in **`Editor`** tab.
-   Click and drag to rotate, scroll wheel to zoom, and arrow
    keys to pan the 3D rendering.
-   Click to select a compartment on the 3D model. The selected compartment is colored green. 
-   An editor will appear on the right hand side where the properties of the compartment can be edited.
-   The 3D view of the model provided by the editor allows only editing of the compartment parameters. 
-   In the **`Run`** tab you can see two subwindows. The one on the left provides a dynamic visualization of the compartment Vm as the simulation progresses. The one on the right is the plot window where you can plot the Vm of the various compartments.
-   Press `Ctrl` and click and drag a compartment from the visualizer to the plot window.
-   Run the model using **`Run`** button. You can see the colors of the compartments changing as the simulation progresses. The graphs gets updated simultaneously with the visualizer.

### Editor View
![**Editor View**](../../images/NeurokitEditor.png)


### Run View
![**Run View**](../../images/NeurokitRunner.png)

## Modeling details

MOOSE uses SI units throughout.

Some salient properties of neuronal building blocks in MOOSE are
described below. Variables that are updated at every simulation
time step are are listed **dynamical**. Rest are parameters.

-   **Compartment**  
    When you select a compartment, you can view and edit its
    properties in the right pane. `V`~`m`~ and `I`~`m`~ are plot-able.
    
    -   **`V`~`m`~** : **dynamical** membrane potential (across `C`~`m`~) in Volts.
    -   **`C`~`m`~** : membrane capacitance in Farads.
    -   **`E`~`m`~** : membrane leak potential in Volts due to the electrochemical
        gradient setup by ion pumps.
    -   **`I`~`m`~** : **dynamical** current in Amperes across the membrane via leak
        resistance `R`~`m`~.
    -   **`inject`** : current in Amperes injected externally into the compartment.
    -   **`initVm`** : initial `V`~`m`~ in Volts.
    -   **`R`~`m`~** : membrane leak resistance in Ohms due to leaky channels.
    -   **`diameter`** : diameter of the compartment in metres.
    -   **`length`** : length of the compartment in metres.
    
    After selecting a compartment, you can click **`See children`** on
    the right pane to list its membrane channels, Ca pool, etc.

-   **HHChannel**  
    Hodgkin-Huxley channel with voltage dependent dynamical gates.
    
    -   **`Gbar`** : peak channel conductance in Siemens.
    -   **`E`~`k`~** : reversal potential of the channel, due to electrochemical
        gradient of the ion(s) it allows.
    -   **`G`~`k`~** : **dynamical** conductance of the channel in Siemens.
        
        > G~k~(t) = Gbar × X(t)^Xpower^ × Y(t)^Ypower^ × Z(t)^Zpower^
        
    -   **`I`~`k`~** : **dynamical** current through the channel into the neuron in
        Amperes.
        
        > I~k~(t) = G~k~(t) × (E~k~-V~m~(t))
        
    -   **`X`**, **`Y`**, **`Z`** : **dynamical** gating variables (range `0.0`
        to `1.0`) that may turn on or off as voltage increases with different time
        constants.
        
        > dX(t)/dt = X~inf~/τ - X(t)/τ
        
        Here, `X`~`inf`~ and `τ` are typically
        sigmoidal/linear/linear-sigmoidal functions of membrane potential
        `V`~`m`~, which are described in a ChannelML file and presently not
        editable from MOOSEGUI. Thus, a gate may open `(X`~`inf`~`(V`~`m`~`) → 1)` or
        close `(X`~`inf`~`(V`~`m`~`) → 0)` on increasing `V`~`m`~, with time constant
        `τ(V`~`m`~`)`.
    -   **`Xpower`**, **`Ypower`**, **`Zpower`** : powers to which gates are raised in the
        `G`~`k`~`(t)` formula above.

-   **HHChannel2D**  
    The Hodgkin-Huxley channel2D can have the usual voltage
    dependent dynamical gates, and also gates that dependent on voltage
    and an ionic concentration, as for say Ca-dependent K conductance.
    It has the properties of HHChannel above, and a few more like
    `Xindex` as in the
    [GENESIS tab2Dchannel reference](http://www.genesis-sim.org/GENESIS/Hyperdoc/Manual-26.html#ss26.61).

-   **CaConc**  
    This is a pool of Ca ions in each compartment, in a shell
    volume under the cell membrane. The dynamical Ca concentration
    increases when Ca channels open, and decays back to resting with a
    specified time constant τ. Its concentration controls Ca-dependent
    K channels, etc.
    -   `Ca` : **dynamical** Ca concentration in the pool in units `mM` ( i.e.,
        `mol/m`^`3`^).
        
        > d[Ca^2+^]/dt = B × I~Ca~ - [Ca^2+^]/τ
        
    -   `CaBasal`/`Ca_base` : Base Ca concentration to which the Ca decays
    -   `tau` : time constant with which the Ca concentration decays to the
        base Ca level.
    -   `B` : constant in the `[Ca`^`2+`^`]` equation above.
    -   `thick` : thickness of the Ca shell within the cell membrane which is
        used to calculate `B` (see Chapter 19 of
        [Book of GENESIS](http://www.genesis-sim.org/GENESIS/iBoG/iBoGpdf/index.html).)


## Demos

-   **Cerebellar granule cell**  
    **`File -> Load -> `**
    `~/moose/Demos/neuroml/GranuleCell/GranuleCell.net.xml`  
    This is a single compartment Cerebellar granule cell with a variety of
    channels
    [Maex, R. and De Schutter, E., 1997](http://www.tnb.ua.ac.be/models/network.shtml)
    (exported from <http://www.neuroconstruct.org/>). Click on
    its soma, and **See children** for its list of channels. Vary the
    `Gbar` of these channels to obtain regular firing, adapting and
    bursty behaviour (may need to increase tau of the Ca pool).
    
-   **Purkinje cell**  
    **`File -> Load -> `**
    `~/moose/Demos/neuroml/PurkinjeCell/Purkinje.net.xml`  
    This is a purely passive cell, but with extensive morphology
    [De Schutter, E. and Bower, J. M., 1994] (exported from
    <http://www.neuroconstruct.org/>). The channel
    specifications are in an obsolete ChannelML format which MOOSE does
    not support.
    
-   **Olfactory bulb subnetwork**  
    **`File -> Load -> `**
    `~/moose/Demos/neuroml/OlfactoryBulb/numgloms2_seed100.0_decimated.xml`  
    This is a pruned and decimated version of a detailed network
    model of the Olfactory bulb [Gilra A. and Bhalla U., in
    preparation] without channels and synaptic connections. We hope to
    post the ChannelML specifications of the channels and synapses
    soon.
    
-   **All channels cell**  
    **`File -> Load -> `**
    `~/moose/Demos/neuroml/allChannelsCell/allChannelsCell.net.xml`  
    This is the Cerebellar granule cell as above, but with loads of
    channels from various cell types (exported from
    <http://www.neuroconstruct.org/>). Play around with the
    channel properties to see what they do. You can also edit the
    ChannelML files in
    `~/moose/Demos/neuroml/allChannelsCell/cells_channels/` to
    experiment further.
    
-   **NeuroML python scripts**  
    In directory `~/moose/Demos/neuroml/GranuleCell`, you can run
    `python FvsI_Granule98.py` which plots firing rate vs injected
    current for the granule cell. Consult this python script to see how
    to read in a NeuroML model and to set up simulations. There are
    ample snippets in `~/moose/Demos/snippets` too.
