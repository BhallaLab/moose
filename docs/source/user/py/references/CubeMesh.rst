CubeMesh
--------

.. py:class:: CubeMesh


   .. py:method:: setIsToroid

      (*destination message field*)      Assigns field value.


   .. py:method:: getIsToroid

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPreserveNumEntries

      (*destination message field*)      Assigns field value.


   .. py:method:: getPreserveNumEntries

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAlwaysDiffuse

      (*destination message field*)      Assigns field value.


   .. py:method:: getAlwaysDiffuse

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setX0

      (*destination message field*)      Assigns field value.


   .. py:method:: getX0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setY0

      (*destination message field*)      Assigns field value.


   .. py:method:: getY0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZ0

      (*destination message field*)      Assigns field value.


   .. py:method:: getZ0

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setX1

      (*destination message field*)      Assigns field value.


   .. py:method:: getX1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setY1

      (*destination message field*)      Assigns field value.


   .. py:method:: getY1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZ1

      (*destination message field*)      Assigns field value.


   .. py:method:: getZ1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDx

      (*destination message field*)      Assigns field value.


   .. py:method:: getDx

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDy

      (*destination message field*)      Assigns field value.


   .. py:method:: getDy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDz

      (*destination message field*)      Assigns field value.


   .. py:method:: getDz

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNx

      (*destination message field*)      Assigns field value.


   .. py:method:: getNx

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNy

      (*destination message field*)      Assigns field value.


   .. py:method:: getNy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNz

      (*destination message field*)      Assigns field value.


   .. py:method:: getNz

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCoords

      (*destination message field*)      Assigns field value.


   .. py:method:: getCoords

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMeshToSpace

      (*destination message field*)      Assigns field value.


   .. py:method:: getMeshToSpace

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSpaceToMesh

      (*destination message field*)      Assigns field value.


   .. py:method:: getSpaceToMesh

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSurface

      (*destination message field*)      Assigns field value.


   .. py:method:: getSurface

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: isToroid

      bool (*value field*)      Flag. True when the mesh should be toroidal, that is,when going beyond the right face brings us around to theleft-most mesh entry, and so on. If we have nx, ny, nzentries, this rule means that the coordinate (x, ny, z)will map onto (x, 0, z). Similarly,(-1, y, z) -> (nx-1, y, z)Default is false


   .. py:attribute:: preserveNumEntries

      bool (*value field*)      Flag. When it is true, the numbers nx, ny, nz remainunchanged when x0, x1, y0, y1, z0, z1 are altered. Thusdx, dy, dz would change instead. When it is false, thendx, dy, dz remain the same and nx, ny, nz are altered.Default is true


   .. py:attribute:: alwaysDiffuse

      bool (*value field*)      Flag. When it is true, the mesh matches up sequential mesh entries for diffusion and chmestry. This is regardless of spatial location, and is guaranteed to set up at least the home reaction systemDefault is false


   .. py:attribute:: x0

      double (*value field*)      X coord of one end


   .. py:attribute:: y0

      double (*value field*)      Y coord of one end


   .. py:attribute:: z0

      double (*value field*)      Z coord of one end


   .. py:attribute:: x1

      double (*value field*)      X coord of other end


   .. py:attribute:: y1

      double (*value field*)      Y coord of other end


   .. py:attribute:: z1

      double (*value field*)      Z coord of other end


   .. py:attribute:: dx

      double (*value field*)      X size for mesh


   .. py:attribute:: dy

      double (*value field*)      Y size for mesh


   .. py:attribute:: dz

      double (*value field*)      Z size for mesh


   .. py:attribute:: nx

      unsigned int (*value field*)      Number of subdivisions in mesh in X


   .. py:attribute:: ny

      unsigned int (*value field*)      Number of subdivisions in mesh in Y


   .. py:attribute:: nz

      unsigned int (*value field*)      Number of subdivisions in mesh in Z


   .. py:attribute:: coords

      vector<double> (*value field*)      Set all the coords of the cuboid at once. Order is:x0 y0 z0   x1 y1 z1   dx dy dzWhen this is done, it recalculates the numEntries since dx, dy and dz are given explicitly.As a special hack, you can leave out dx, dy and dz and use a vector of size 6. In this case the operation assumes that nx, ny and nz are to be preserved and dx, dy and dz will be recalculated.


   .. py:attribute:: meshToSpace

      vector<unsigned int> (*value field*)      Array in which each mesh entry stores spatial (cubic) index


   .. py:attribute:: spaceToMesh

      vector<unsigned int> (*value field*)      Array in which each space index (obtained by linearizing the xyz coords) specifies which meshIndex is present.In many cases the index will store the EMPTY flag if there isno mesh entry at that spatial location


   .. py:attribute:: surface

      vector<unsigned int> (*value field*)      Array specifying surface of arbitrary volume within the CubeMesh. All entries must fall within the cuboid. Each entry of the array is a spatial index obtained by linearizing the ix, iy, iz coordinates within the cuboid. So, each entry == ( iz * ny + iy ) * nx + ixNote that the voxels listed on the surface are WITHIN the volume of the CubeMesh object
