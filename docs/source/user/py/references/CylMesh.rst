CylMesh
-------

.. py:class:: CylMesh


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


   .. py:method:: setR0

      (*destination message field*)      Assigns field value.


   .. py:method:: getR0

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


   .. py:method:: setR1

      (*destination message field*)      Assigns field value.


   .. py:method:: getR1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCoords

      (*destination message field*)      Assigns field value.


   .. py:method:: getCoords

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumDiffCompts

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getTotLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: x0

      double (*value field*)      x coord of one end


   .. py:attribute:: y0

      double (*value field*)      y coord of one end


   .. py:attribute:: z0

      double (*value field*)      z coord of one end


   .. py:attribute:: r0

      double (*value field*)      Radius of one end


   .. py:attribute:: x1

      double (*value field*)      x coord of other end


   .. py:attribute:: y1

      double (*value field*)      y coord of other end


   .. py:attribute:: z1

      double (*value field*)      z coord of other end


   .. py:attribute:: r1

      double (*value field*)      Radius of other end


   .. py:attribute:: diffLength

      double (*value field*)      Length constant to use for subdivisionsThe system will attempt to subdivide using compartments oflength diffLength on average. If the cylinder has different enddiameters r0 and r1, it will scale to smaller lengthsfor the smaller diameter end and vice versa.Once the value is set it will recompute diffLength as totLength/numEntries


   .. py:attribute:: coords

      vector<double> (*value field*)      All the coords as a single vector: x0 y0 z0  x1 y1 z1  r0 r1 diffLength


   .. py:attribute:: numDiffCompts

      unsigned int (*value field*)      Number of diffusive compartments in model


   .. py:attribute:: totLength

      double (*value field*)      Total length of cylinder
