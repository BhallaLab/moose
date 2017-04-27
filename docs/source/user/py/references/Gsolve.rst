Gsolve
------

.. py:class:: Gsolve


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:attribute:: init

      void (*shared message field*)      Shared message for initProc and initReinit. This is used when the system has cross-compartment reactions.


   .. py:attribute:: xCompt

      void (*shared message field*)      Shared message for pool exchange for cross-compartment reactions. Exchanges latest values of all pools that participate in such reactions.


   .. py:method:: setStoich

      (*destination message field*)      Assigns field value.


   .. py:method:: getStoich

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumLocalVoxels

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNVec

      (*destination message field*)      Assigns field value.


   .. py:method:: getNVec

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumAllVoxels

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumAllVoxels

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumPools

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: voxelVol

      (*destination message field*)      Handles updates to all voxels. Comes from parent ChemCompt object.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: initProc

      (*destination message field*)      Handles initProc call from Clock


   .. py:method:: initReinit

      (*destination message field*)      Handles initReinit call from Clock


   .. py:method:: xComptIn

      (*destination message field*)      Handles arriving pool 'n' values used in cross-compartment reactions.


   .. py:method:: setUseRandInit

      (*destination message field*)      Assigns field value.


   .. py:method:: getUseRandInit

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setUseClockedUpdate

      (*destination message field*)      Assigns field value.


   .. py:method:: getUseClockedUpdate

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumFire

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: xComptOut

      Id,vector<double> (*source message field*)      Sends 'n' of all molecules participating in cross-compartment reactions between any juxtaposed voxels between current compt and another compartment. This includes molecules local to this compartment, as well as proxy molecules belonging elsewhere. A(t+1) = (Alocal(t+1) + AremoteProxy(t+1)) - Alocal(t) A(t+1) = (Aremote(t+1) + Aproxy(t+1)) - Aproxy(t) Then we update A on the respective solvers with: Alocal(t+1) = Aproxy(t+1) = A(t+1) This is equivalent to sending dA over on each timestep.


   .. py:attribute:: stoich

      Id (*value field*)      Stoichiometry object for handling this reaction system.


   .. py:attribute:: numLocalVoxels

      unsigned int (*value field*)      Number of voxels in the core reac-diff system, on the current solver.


   .. py:attribute:: numAllVoxels

      unsigned int (*value field*)      Number of voxels in the entire reac-diff system, including proxy voxels to represent abutting compartments.


   .. py:attribute:: numPools

      unsigned int (*value field*)      Number of molecular pools in the entire reac-diff system, including variable, function and buffered.


   .. py:attribute:: useRandInit

      bool (*value field*)      Flag: True when using probabilistic (random) rounding.
       Default: True.
       When initializing the mol# from floating-point Sinit values, we have two options. One is to look at each Sinit, and round to the nearest integer. The other is to look at each Sinit, and probabilistically round up or down depending on the  value. For example, if we had a Sinit value of 1.49,  this would always be rounded to 1.0 if the flag is false, and would be rounded to 1.0 and 2.0 in the ratio 51:49 if the flag is true.


   .. py:attribute:: useClockedUpdate

      bool (*value field*)      Flag: True to cause all reaction propensities to be updated on every clock tick.
      Default: False.
      This flag should be set when the reaction system includes a function with a dependency on time or on external events. It has a significant speed penalty so the flag should not be set unless there are such functions.


   .. py:attribute:: nVec

      unsigned int,vector<double> (*lookup field*)      vector of pool counts


   .. py:attribute:: numFire

      unsigned int,vector<unsigned int> (*lookup field*)      Vector of the number of times each reaction has fired.Indexed by the voxel number.Zeroed out at reinit.
