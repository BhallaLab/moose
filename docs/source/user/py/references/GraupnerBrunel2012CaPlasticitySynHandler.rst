GraupnerBrunel2012CaPlasticitySynHandler
----------------------------------------

.. py:class:: GraupnerBrunel2012CaPlasticitySynHandler

   The GraupnerBrunel2012CaPlasticitySynHandler handles synapseswith Ca-based plasticity as per Higgins et al. 2014 and Graupner and Brunel 2012.Note 1:   Here, Ca ('chemical Ca') is updated only at each pre-spike, pre-spike+delayD and post-spike!   So it is inaccurate to use it for say Ca-dependent K channels in the electrical compartment,   for which you use are advised to use the CaPool i.e. 'electrical Ca'.Note 2:   Ca here is post-synaptic 'chemical Ca' common for all synapses in this SynHandler,   so weights of all pre-synapses connected to this SynHandler get updated   at each pre-spike, pre-spike+delayD and post-spike!   So if all pre-synaptic weights start out the same, they remain the same!!   If you want to consider each pre-synapse independently,   have independent SynHandlers for each synapse.   If these SynHandlers are in the same electrical compartment,   you're essentially assuming these are on different spines,   with their own 'chemical Ca' which won't match the   'electrical Ca' of the compartment (=dendrite).   If you put each SynHandler with a single synapse   in its own electrical compartment (=spine),   only then can you have an 'electrical Ca'   corresponding to the 'chemical Ca'.Three priority queues are used to manage pre, post, and pre+delayD spikes.

   .. py:method:: setNumSynapse

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSynapse

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:method:: addPostSpike

      (*destination message field*)      Handles arriving spike messages from post-synaptic neuron, inserts into postEvent queue.


   .. py:method:: setCa

      (*destination message field*)      Assigns field value.


   .. py:method:: getCa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaInit

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaInit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauCa

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauCa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTauSyn

      (*destination message field*)      Assigns field value.


   .. py:method:: getTauSyn

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaPre

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaPre

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaPost

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaPost

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDelayD

      (*destination message field*)      Assigns field value.


   .. py:method:: getDelayD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThetaP

      (*destination message field*)      Assigns field value.


   .. py:method:: getThetaP

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThetaD

      (*destination message field*)      Assigns field value.


   .. py:method:: getThetaD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGammaP

      (*destination message field*)      Assigns field value.


   .. py:method:: getGammaP

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGammaD

      (*destination message field*)      Assigns field value.


   .. py:method:: getGammaD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightMax

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightMax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightMin

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightMin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWeightScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getWeightScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNoisy

      (*destination message field*)      Assigns field value.


   .. py:method:: getNoisy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNoiseSD

      (*destination message field*)      Assigns field value.


   .. py:method:: getNoiseSD

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setBistable

      (*destination message field*)      Assigns field value.


   .. py:method:: getBistable

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: Ca

      double (*value field*)      Ca is a post-synaptic decaying variable as a proxy for Ca concentrationand receives an impulse whenever a pre- or post- spike occurs.Caution: Ca is updated via an event-based rule, so it is only updated and validwhen a pre- or post- spike has occured, or at time delayD after a pre-spike.Do not use it to control a Ca dependent current, etc.See notes in the class Description: all pre-synapses get updated via the same post-synaptic Ca.


   .. py:attribute:: CaInit

      double (*value field*)      CaInit is the initial value for Ca


   .. py:attribute:: tauCa

      double (*value field*)      tauCa is the time constant for decay of Ca


   .. py:attribute:: tauSyn

      double (*value field*)      tauSyn is the time constant for synaptic weight evolution equation


   .. py:attribute:: CaPre

      double (*value field*)      CaPre is added to Ca on every pre-spike


   .. py:attribute:: CaPost

      double (*value field*)      CaPost is added to Ca on every post-spike


   .. py:attribute:: delayD

      double (*value field*)      Time delay D after pre-spike, when Ca is increased by Capre. delayD represents NMDA rise time.


   .. py:attribute:: thetaP

      double (*value field*)      Potentiation threshold for CaUser must ensure thetaP>thetaD, else simulation results will be wrong.


   .. py:attribute:: thetaD

      double (*value field*)      Depression threshold for CaUser must ensure thetaP>thetaD, else simulation results will be wrong.


   .. py:attribute:: gammaP

      double (*value field*)      gammaP is the potentiation factor for synaptic weight increase if Ca>thetaP


   .. py:attribute:: gammaD

      double (*value field*)      gammaD is the depression factor for synaptic weight decrease if Ca>thetaD


   .. py:attribute:: weightMax

      double (*value field*)      An upper bound on the weight


   .. py:attribute:: weightMin

      double (*value field*)      A lower bound on the weight


   .. py:attribute:: weightScale

      double (*value field*)      Scale all pre-synaptic weights by weightScale before adding to activation (default 1.0)In the terminology of the paper Higgins et al 2012, weight is synaptic efficacy,while weightScale*weight is what finally is added to activation variable.


   .. py:attribute:: noisy

      bool (*value field*)      If true, turn noise on as per noiseSD


   .. py:attribute:: noiseSD

      double (*value field*)      Standard deviation of noise added to Ca


   .. py:attribute:: bistable

      bool (*value field*)      If true, the synapse is bistable as in GraupnerBrunel2012 paper.The effect of potential on the weight update is usually ignorable if Ca is above thetaP and thetaD most of the time.
