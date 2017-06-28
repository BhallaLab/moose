********************************
Neuronal simulations in MOOSEGUI
********************************

Neuronal models in various formats can be loaded and simulated in the
**MOOSE Graphical User Interface**. The GUI displays the neurons in 3D,
and allows visual selection and editing of neuronal properties. Plotting
and visualization of activity proceed concurrently with the simulation.
Support for creating and editing channels, morphology, and networks is
planned for the future. MOOSEGUI uses SI units throughout.

moose-examples
^^^^^^^^^^^^^^^

-  **Cerebellar granule cell**

   **File -> Load ->**
   ~/moose/moose-examples/neuroml/GranuleCell/GranuleCell.net.xml

   This is a single compartment Cerebellar granule cell with a variety
   of channels `Maex, R. and De Schutter, E.,
   1997 <http://www.tnb.ua.ac.be/models/network.shtml>`_ (exported from
   http://www.neuroconstruct.org/). Click on its soma, and **See
   children** for its list of channels. Vary the Gbar of these
   channels to obtain regular firing, adapting and bursty behaviour (may
   need to increase tau of the Ca pool).


-  **Pyloric rhythm generator in the stomatogastric ganglion of lobster**

   **File -> Load ->**
   ~/moose/moose-examples/neuroml/pyloric/Generated.net.xml


-  **Purkinje cell**

   **File -> Load ->**
   ~/moose/moose-examples/neuroml/PurkinjeCell/Purkinje.net.xml

   This is a purely passive cell, but with extensive morphology [De
   Schutter, E. and Bower, J. M., 1994] (exported from
   http://www.neuroconstruct.org/). The channel specifications are in an
   obsolete ChannelML format which MOOSE does not support.


-  **Olfactory bulb subnetwork**

   **File -> Load ->**
   ~/moose/moose-examples/neuroml/OlfactoryBulb/numgloms2_seed100.0_decimated.xml

   This is a pruned and decimated version of a detailed network model
   of the Olfactory bulb [Gilra A. and Bhalla U., in preparation]
   without channels and synaptic connections. We hope to post the
   ChannelML specifications of the channels and synapses soon.


-  **All channels cell**

   **File -> Load ->**
   ~/moose/moose-examples/neuroml/allChannelsCell/allChannelsCell.net.xml

   This is the Cerebellar granule cell as above, but with loads of
   channels from various cell types (exported from
   http://www.neuroconstruct.org/). Play around with the channel
   properties to see what they do. You can also edit the ChannelML files
   in ~/moose/moose-examples/neuroml/allChannelsCell/cells_channels/ to
   experiment further.


-  **NeuroML python scripts**
   In directory ~/moose/moose-examples/neuroml/GranuleCell, you can run
   python FvsI_Granule98.py which plots firing rate vs injected
   current for the granule cell. Consult this python script to see how
   to read in a NeuroML model and to set up simulations. There are ample
   snippets in ~/moose/moose-examples/snippets too.
