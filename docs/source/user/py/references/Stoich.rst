Stoich
------

.. py:class:: Stoich


   .. py:method:: setPath

      (*destination message field*)      Assigns field value.


   .. py:method:: getPath

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKsolve

      (*destination message field*)      Assigns field value.


   .. py:method:: getKsolve

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDsolve

      (*destination message field*)      Assigns field value.


   .. py:method:: getDsolve

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompartment

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompartment

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumVarPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumBufPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumAllPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumProxyPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getPoolIdMap

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumRates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMatrixEntry

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getColumnIndex

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getRowStart

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getProxyPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStatus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: unzombify

      (*destination message field*)      Restore all zombies to their native state


   .. py:method:: buildXreacs

      (*destination message field*)      Build cross-reaction terms between current stoich and argument. This function scans the voxels at which there are junctions between different compartments, and orchestrates set up of interfaces between the Ksolves that implement the X reacs at those junctions.


   .. py:method:: filterXreacs

      (*destination message field*)      Filter cross-reaction terms on current stoichThis function clears out absent rate terms that would otherwise try to compute cross reactions where the junctions are not present.


   .. py:method:: scaleBufsAndRates

      (*destination message field*)      Args: voxel#, volRatio
      Handles requests for runtime volume changes in the specified voxel#, Used in adaptors changing spine vols.


   .. py:attribute:: path

      string (*value field*)      Wildcard path for reaction system handled by Stoich


   .. py:attribute:: ksolve

      Id (*value field*)      Id of Kinetic reaction solver class that works with this Stoich.  Must be of class Ksolve, or Gsolve (at present)  Must be assigned before the path is set.


   .. py:attribute:: dsolve

      Id (*value field*)      Id of Diffusion solver class that works with this Stoich. Must be of class Dsolve  If left unset then the system will be assumed to work in a non-diffusive, well-stirred cell. If it is going to be  used it must be assigned before the path is set.


   .. py:attribute:: compartment

      Id (*value field*)      Id of chemical compartment class that works with this Stoich. Must be derived from class ChemCompt. If left unset then the system will be assumed to work in a non-diffusive, well-stirred cell. If it is going to be  used it must be assigned before the path is set.


   .. py:attribute:: numVarPools

      unsigned int (*value field*)      Number of time-varying pools to be computed by the numerical engine


   .. py:attribute:: numBufPools

      unsigned int (*value field*)      Number of buffered pools to be computed by the numerical engine. Includes pools controlled by functions.


   .. py:attribute:: numAllPools

      unsigned int (*value field*)      Total number of pools handled by the numerical engine. This includes variable ones, buffered ones, and functions. It includes local pools as well as cross-solver proxy pools.


   .. py:attribute:: numProxyPools

      unsigned int (*value field*)      Number of pools here by proxy as substrates of a cross-compartment reaction.


   .. py:attribute:: poolIdMap

      vector<unsigned int> (*value field*)      Map to look up the index of the pool from its Id.poolIndex = poolIdMap[ Id::value() - poolOffset ] where the poolOffset is the smallest Id::value. poolOffset is passed back as the last entry of this vector. Any Ids that are not pools return EMPTY=~0.


   .. py:attribute:: numRates

      unsigned int (*value field*)      Total number of rate terms in the reaction system.


   .. py:attribute:: matrixEntry

      vector<int> (*value field*)      The non-zero matrix entries in the sparse matrix. Theircolumn indices are in a separate vector and the rowinformatino in a third


   .. py:attribute:: columnIndex

      vector<unsigned int> (*value field*)      Column Index of each matrix entry


   .. py:attribute:: rowStart

      vector<unsigned int> (*value field*)      Row start for each block of entries and column indices


   .. py:attribute:: status

      int (*value field*)      Status of Stoich in the model building process. Values are: -1: Reaction path not yet assigned.
       0: Successfully built stoichiometry matrix.
       1: Warning: Missing a reactant in a Reac or Enz.
       2: Warning: Missing a substrate in an MMenz.
       3: Warning: Missing substrates as well as reactants.
       4: Warning: Compartment not defined.
       8: Warning: Neither Ksolve nor Dsolve defined.
       16: Warning: No objects found on path.



   .. py:attribute:: proxyPools

      Id,vector<Id> (*lookup field*)      Return vector of proxy pools for X-compt reactions between current stoich, and the argument, which is a StoichId. The returned pools belong to the compartment handling the Stoich specified in the argument. If no pools are found, return an empty vector.
