SpineMesh
---------

.. py:class:: SpineMesh


   .. py:method:: getParentVoxel

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


   .. py:method:: spineList

      (*destination message field*)      Specifies the list of electrical compartments for the spine,and the associated parent voxelArguments: shaft compartments, head compartments, parent voxel index


   .. py:attribute:: parentVoxel

      vector<unsigned int> (*value field*)      Vector of indices of proximal voxels within this mesh.Spines are at present modeled with just one compartment,so each entry in this vector is always set to EMPTY == -1U


   .. py:attribute:: neuronVoxel

      vector<unsigned int> (*value field*)      Vector of indices of voxels on parent NeuroMesh, from which the respective spines emerge.


   .. py:attribute:: elecComptMap

      vector<Id> (*value field*)      Vector of Ids of electrical compartments that map to each voxel. This is necessary because the order of the IDs may differ from the ordering of the voxels. Note that there is always just one voxel per spine head.


   .. py:attribute:: elecComptList

      vector<Id> (*value field*)      Vector of Ids of all electrical compartments in this SpineMesh. Ordering is as per the tree structure built in the NeuroMesh, and may differ from Id order. Ordering matches that used for startVoxelInCompt and endVoxelInCompt


   .. py:attribute:: startVoxelInCompt

      vector<unsigned int> (*value field*)      Index of first voxel that maps to each electrical compartment. This is a trivial function in the SpineMesh, aswe have a single voxel per spine. So just a vector of its own indices.


   .. py:attribute:: endVoxelInCompt

      vector<unsigned int> (*value field*)      Index of end voxel that maps to each electrical compartment. Since there is just one voxel per electrical compartment in the spine, this is just a vector of index+1
