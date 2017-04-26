Dsolve
------

.. py:class:: Dsolve


   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: setStoich

      (*destination message field*)      Assigns field value.


   .. py:method:: getStoich

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPath

      (*destination message field*)      Assigns field value.


   .. py:method:: getPath

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompartment

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompartment

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumVoxels

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumAllVoxels

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNVec

      (*destination message field*)      Assigns field value.


   .. py:method:: getNVec

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumPools

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffVol1

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffVol1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffVol2

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffVol2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: buildMeshJunctions

      (*destination message field*)      Builds junctions between mesh on current Dsolve, and another Dsolve. The meshes have to be compatible.


   .. py:method:: buildNeuroMeshJunctions

      (*destination message field*)      Builds junctions between NeuroMesh, SpineMesh and PsdMesh


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: stoich

      Id (*value field*)      Stoichiometry object for handling this reaction system.


   .. py:attribute:: path

      string (*value field*)      Path of reaction system. Must include all the pools that are to be handled by the Dsolve, can also include other random objects, which will be ignored.


   .. py:attribute:: compartment

      Id (*value field*)      Reac-diff compartment in which this diffusion system is embedded.


   .. py:attribute:: numVoxels

      unsigned int (*value field*)      Number of voxels in the core reac-diff system, on the current diffusion solver.


   .. py:attribute:: numAllVoxels

      unsigned int (*value field*)      Number of voxels in the core reac-diff system, on the current diffusion solver.


   .. py:attribute:: numPools

      unsigned int (*value field*)      Number of molecular pools in the entire reac-diff system, including variable, function and buffered.


   .. py:attribute:: nVec

      unsigned int,vector<double> (*lookup field*)      vector of # of molecules along diffusion length, looked up by pool index


   .. py:attribute:: diffVol1

      unsigned int,double (*lookup field*)      Volume used to set diffusion scaling: firstVol[ voxel# ] Particularly relevant for diffusion between PSD and head.


   .. py:attribute:: diffVol2

      unsigned int,double (*lookup field*)      Volume used to set diffusion scaling: secondVol[ voxel# ] Particularly relevant for diffusion between spine and dend.


   .. py:attribute:: diffScale

      unsigned int,double (*lookup field*)      Geometry term to set diffusion scaling: diffScale[ voxel# ] Here the scaling term is given by cross-section area/length Relevant for diffusion between spine head and dend, or PSD.
