PsdMesh
-------

.. py:class:: PsdMesh


   .. py:method:: setThickness

      (*destination message field*)      Assigns field value.


   .. py:method:: getThickness

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNeuronVoxel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getElecComptMap

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getElecComptList

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStartVoxelInCompt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getEndVoxelInCompt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: psdList

      (*destination message field*)      Specifies the geometry of the spine,and the associated parent voxelArguments: disk params vector with 8 entries per psd, vector of Ids of electrical compts mapped to voxels, parent voxel index


   .. py:attribute:: thickness

      double (*value field*)      An assumed thickness for PSD. The volume is computed as thePSD area passed in to each PSD, times this value.defaults to 50 nanometres. For reference, membranes are 5 nm.


   .. py:attribute:: neuronVoxel

      vector<unsigned int> (*value field*)      Vector of indices of voxels on parent NeuroMesh, from which the respective spines emerge.


   .. py:attribute:: elecComptMap

      vector<Id> (*value field*)      Vector of Ids of electrical compartments that map to each voxel. This is necessary because the order of the IDs may differ from the ordering of the voxels. Note that there is always just one voxel per PSD.


   .. py:attribute:: elecComptList

      vector<Id> (*value field*)      Vector of Ids of all electrical compartments in this PsdMesh. Ordering is as per the tree structure built in the NeuroMesh, and may differ from Id order. Ordering matches that used for startVoxelInCompt and endVoxelInCompt


   .. py:attribute:: startVoxelInCompt

      vector<unsigned int> (*value field*)      Index of first voxel that maps to each electrical compartment. This is a trivial function in the PsdMesh, aswe have a single voxel per spine. So just a vector of its own indices.


   .. py:attribute:: endVoxelInCompt

      vector<unsigned int> (*value field*)      Index of end voxel that maps to each electrical compartment. Since there is just one voxel per electrical compartment in the spine, this is just a vector of index+1


.. py:class:: PulseGen

   PulseGen: general purpose pulse generator. This can generate any number of pulses with specified level and duration.

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: getOutputValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setBaseLevel

      (*destination message field*)      Assigns field value.


   .. py:method:: getBaseLevel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFirstLevel

      (*destination message field*)      Assigns field value.


   .. py:method:: getFirstLevel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFirstWidth

      (*destination message field*)      Assigns field value.


   .. py:method:: getFirstWidth

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFirstDelay

      (*destination message field*)      Assigns field value.


   .. py:method:: getFirstDelay

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSecondLevel

      (*destination message field*)      Assigns field value.


   .. py:method:: getSecondLevel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSecondWidth

      (*destination message field*)      Assigns field value.


   .. py:method:: getSecondWidth

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSecondDelay

      (*destination message field*)      Assigns field value.


   .. py:method:: getSecondDelay

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCount

      (*destination message field*)      Assigns field value.


   .. py:method:: getCount

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTrigMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getTrigMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLevel

      (*destination message field*)      Assigns field value.


   .. py:method:: getLevel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setWidth

      (*destination message field*)      Assigns field value.


   .. py:method:: getWidth

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDelay

      (*destination message field*)      Assigns field value.


   .. py:method:: getDelay

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: input

      (*destination message field*)      Handle incoming input that determines gating/triggering onset. Note that although this is a double field, the underlying field is integer. So fractional part of input will be truncated


   .. py:method:: levelIn

      (*destination message field*)      Handle level value coming from other objects


   .. py:method:: widthIn

      (*destination message field*)      Handle width value coming from other objects


   .. py:method:: delayIn

      (*destination message field*)      Handle delay value coming from other objects


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: output

      double (*source message field*)      Current output level.


   .. py:attribute:: outputValue

      double (*value field*)      Output amplitude


   .. py:attribute:: baseLevel

      double (*value field*)      Basal level of the stimulus


   .. py:attribute:: firstLevel

      double (*value field*)      Amplitude of the first pulse in a sequence


   .. py:attribute:: firstWidth

      double (*value field*)      Width of the first pulse in a sequence


   .. py:attribute:: firstDelay

      double (*value field*)      Delay to start of the first pulse in a sequence


   .. py:attribute:: secondLevel

      double (*value field*)      Amplitude of the second pulse in a sequence


   .. py:attribute:: secondWidth

      double (*value field*)      Width of the second pulse in a sequence


   .. py:attribute:: secondDelay

      double (*value field*)      Delay to start of of the second pulse in a sequence


   .. py:attribute:: count

      unsigned int (*value field*)      Number of pulses in a sequence


   .. py:attribute:: trigMode

      unsigned int (*value field*)      Trigger mode for pulses in the sequence.
       0 : free-running mode where it keeps looping its output
       1 : external trigger, where it is triggered by an external input (and stops after creating the first train of pulses)
       2 : external gate mode, where it keeps generating the pulses in a loop as long as the input is high.


   .. py:attribute:: level

      unsigned int,double (*lookup field*)      Level of the pulse at specified index


   .. py:attribute:: width

      unsigned int,double (*lookup field*)      Width of the pulse at specified index


   .. py:attribute:: delay

      unsigned int,double (*lookup field*)      Delay of the pulse at specified index
