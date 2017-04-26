Ksolve
------

.. py:class:: Ksolve


   .. py:attribute:: xCompt

      void (*shared message field*)      Shared message for pool exchange for cross-compartment reactions. Exchanges latest values of all pools that participate in such reactions.


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit. These are used for all regular Ksolve calculations including interfacing with the diffusion calculations by a Dsolve.


   .. py:attribute:: init

      void (*shared message field*)      Shared message for initProc and initReinit. This is used when the system has cross-compartment reactions.


   .. py:method:: setMethod

      (*destination message field*)      Assigns field value.


   .. py:method:: getMethod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setEpsAbs

      (*destination message field*)      Assigns field value.


   .. py:method:: getEpsAbs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setEpsRel

      (*destination message field*)      Assigns field value.


   .. py:method:: getEpsRel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompartment

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompartment

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


   .. py:method:: getEstimatedDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStoich

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: voxelVol

      (*destination message field*)      Handles updates to all voxels. Comes from parent ChemCompt object.


   .. py:method:: xComptIn

      (*destination message field*)      Handles arriving pool 'n' values used in cross-compartment reactions.


   .. py:method:: process

      (*destination message field*)      Handles process call from Clock


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call from Clock


   .. py:method:: initProc

      (*destination message field*)      Handles initProc call from Clock


   .. py:method:: initReinit

      (*destination message field*)      Handles initReinit call from Clock


   .. py:attribute:: xComptOut

      Id,vector<double> (*source message field*)      Sends 'n' of all molecules participating in cross-compartment reactions between any juxtaposed voxels between current compt and another compartment. This includes molecules local to this compartment, as well as proxy molecules belonging elsewhere. A(t+1) = (Alocal(t+1) + AremoteProxy(t+1)) - Alocal(t) A(t+1) = (Aremote(t+1) + Aproxy(t+1)) - Aproxy(t) Then we update A on the respective solvers with: Alocal(t+1) = Aproxy(t+1) = A(t+1) This is equivalent to sending dA over on each timestep.


   .. py:attribute:: method

      string (*value field*)      Integration method, using GSL. So far only explict. Options are:rk5: The default Runge-Kutta-Fehlberg 5th order adaptive dt methodgsl: alias for the aboverk4: The Runge-Kutta 4th order fixed dt methodrk2: The Runge-Kutta 2,3 embedded fixed dt methodrkck: The Runge-Kutta Cash-Karp (4,5) methodrk8: The Runge-Kutta Prince-Dormand (8,9) method


   .. py:attribute:: epsAbs

      double (*value field*)      Absolute permissible integration error range.


   .. py:attribute:: epsRel

      double (*value field*)      Relative permissible integration error range.


   .. py:attribute:: compartment

      Id (*value field*)      Compartment in which the Ksolve reaction system lives.


   .. py:attribute:: numLocalVoxels

      unsigned int (*value field*)      Number of voxels in the core reac-diff system, on the current solver.


   .. py:attribute:: numAllVoxels

      unsigned int (*value field*)      Number of voxels in the entire reac-diff system, including proxy voxels to represent abutting compartments.


   .. py:attribute:: numPools

      unsigned int (*value field*)      Number of molecular pools in the entire reac-diff system, including variable, function and buffered.


   .. py:attribute:: estimatedDt

      double (*value field*)      Estimated timestep for reac system based on Euler error


   .. py:attribute:: stoich

      Id (*value field*)      Id for stoichiometry object tied to this Ksolve


   .. py:attribute:: nVec

      unsigned int,vector<double> (*lookup field*)      vector of pool counts. Index specifies which voxel.
