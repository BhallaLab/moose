MeshEntry
---------

.. py:class:: MeshEntry

   One voxel in a chemical reaction compartment

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:attribute:: mesh

      void (*shared message field*)      Shared message for updating mesh volumes and subdivisions,typically controls pool volumes


   .. py:method:: getVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDimensions

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getMeshType

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCoordinates

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNeighbors

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDiffusionArea

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDiffusionScaling

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: getVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: remeshOut

      double,unsigned int,unsigned int,vector<unsigned int>,vector<double> (*source message field*)      Tells the target pool or other entity that the compartment subdivision(meshing) has changed, and that it has to redo its volume and memory allocation accordingly.Arguments are: oldvol, numTotalEntries, startEntry, localIndices, volsThe vols specifies volumes of each local mesh entry. It also specifieshow many meshEntries are present on the local node.The localIndices vector is used for general load balancing only.It has a list of the all meshEntries on current node.If it is empty, we assume block load balancing. In this secondcase the contents of the current node go from startEntry to startEntry + vols.size().


   .. py:attribute:: remeshReacsOut

      void (*source message field*)      Tells connected enz or reac that the compartment subdivision(meshing) has changed, and that it has to redo its volume-dependent rate terms like numKf\_ accordingly.


   .. py:attribute:: volume

      double (*value field*)      Volume of this MeshEntry


   .. py:attribute:: dimensions

      unsigned int (*value field*)      number of dimensions of this MeshEntry


   .. py:attribute:: meshType

      unsigned int (*value field*)       The MeshType defines the shape of the mesh entry. 0: Not assigned 1: cuboid 2: cylinder 3. cylindrical shell 4: cylindrical shell segment 5: sphere 6: spherical shell 7: spherical shell segment 8: Tetrahedral


   .. py:attribute:: Coordinates

      vector<double> (*value field*)      Coordinates that define current MeshEntry. Depend on MeshType.


   .. py:attribute:: neighbors

      vector<unsigned int> (*value field*)      Indices of other MeshEntries that this one connects to


   .. py:attribute:: DiffusionArea

      vector<double> (*value field*)      Diffusion area for geometry of interface


   .. py:attribute:: DiffusionScaling

      vector<double> (*value field*)      Diffusion scaling for geometry of interface
