Spine
-----

.. py:class:: Spine

   Spine wrapper, used to change its morphology typically by a message from an adaptor. The Spine class takes care of a lot of resultant scaling to electrical, chemical, and diffusion properties.

   .. py:method:: setShaftLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getShaftLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setShaftDiameter

      (*destination message field*)      Assigns field value.


   .. py:method:: getShaftDiameter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setHeadLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getHeadLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setHeadDiameter

      (*destination message field*)      Assigns field value.


   .. py:method:: getHeadDiameter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPsdArea

      (*destination message field*)      Assigns field value.


   .. py:method:: getPsdArea

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setHeadVolume

      (*destination message field*)      Assigns field value.


   .. py:method:: getHeadVolume

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTotalLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getTotalLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: shaftLength

      double (*value field*)      Length of spine shaft.


   .. py:attribute:: shaftDiameter

      double (*value field*)      Diameter of spine shaft.


   .. py:attribute:: headLength

      double (*value field*)      Length of spine head.


   .. py:attribute:: headDiameter

      double (*value field*)      Diameter of spine head, and also the diameter of the PSD.


   .. py:attribute:: psdArea

      double (*value field*)      Area of the Post synaptic density, PSD. This is the same as the cross-section area of spine head, perpendicular to shaft. Assumes that the head is a cylinder and that its length does not change.
      This is useful to scale # of surface molecules on the PSD.


   .. py:attribute:: headVolume

      double (*value field*)      Volume of spine head, treating it as a cylinder. When this is scaled by the user, both the diameter and the length of the spine head scale by the cube root of the ratio to the previous volume. The diameter of the PSD is pegged to the diameter fo the spine head.
      This is useful to scale total # of molecules in the head.


   .. py:attribute:: totalLength

      double (*value field*)      Length of entire spine. Scales both the length of the shaft and of the spine head, without changing any of the diameters.
