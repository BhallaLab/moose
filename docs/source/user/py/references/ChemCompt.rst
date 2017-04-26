ChemCompt
---------

.. py:class:: ChemCompt

   Pure virtual base class for chemical compartments

   .. py:method:: setVolume

      (*destination message field*)      Assigns field value.


   .. py:method:: getVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getVoxelVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getVoxelMidpoint

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setOneVoxelVolume

      (*destination message field*)      Assigns field value.


   .. py:method:: getOneVoxelVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumDimensions

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStencilRate

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStencilIndex

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: buildDefaultMesh

      (*destination message field*)      Tells ChemCompt derived class to build a default mesh with thespecified volume and number of meshEntries.


   .. py:method:: setVolumeNotRates

      (*destination message field*)      Changes volume but does not notify any child objects.Only works if the ChemCompt has just one voxel.This function will invalidate any concentration term inthe model. If you don't know why you would want to do this,then you shouldn't use this function.


   .. py:method:: resetStencil

      (*destination message field*)      Resets the diffusion stencil to the core stencil that only includes the within-mesh diffusion. This is needed prior to building up the cross-mesh diffusion through junctions.


   .. py:method:: setNumMesh

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumMesh

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:attribute:: voxelVolOut

      vector<double> (*source message field*)      Sends updated voxel volume out to Ksolve, Gsolve, and Dsolve.Used to request a recalculation of rates and of initial numbers.


   .. py:attribute:: volume

      double (*value field*)      Volume of entire chemical domain.Assigning this only works if the chemical compartment hasonly a single voxel. Otherwise ignored.This function goes through all objects below this on thetree, and rescales their molecule #s and rates as per thevolume change. This keeps concentration the same, and alsomaintains rates as expressed in volume units.


   .. py:attribute:: voxelVolume

      vector<double> (*value field*)      Vector of volumes of each of the voxels.


   .. py:attribute:: voxelMidpoint

      vector<double> (*value field*)      Vector of midpoint coordinates of each of the voxels. The size of this vector is 3N, where N is the number of voxels. The first N entries are for x, next N for y, last N are z.


   .. py:attribute:: numDimensions

      unsigned int (*value field*)      Number of spatial dimensions of this compartment. Usually 3 or 2


   .. py:attribute:: oneVoxelVolume

      unsigned int,double (*lookup field*)      Volume of specified voxel.


   .. py:attribute:: stencilRate

      unsigned int,vector<double> (*lookup field*)      vector of diffusion rates in the stencil for specified voxel.The identity of the coupled voxels is given by the partner field 'stencilIndex'.Returns an empty vector for non-voxelized compartments.


   .. py:attribute:: stencilIndex

      unsigned int,vector<unsigned int> (*lookup field*)      vector of voxels diffusively coupled to the specified voxel.The diffusion rates into the coupled voxels is given by the partner field 'stencilRate'.Returns an empty vector for non-voxelized compartments.
