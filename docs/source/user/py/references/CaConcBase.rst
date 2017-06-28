CaConcBase
----------

.. py:class:: CaConcBase

   CaConcBase: Base class for Calcium concentration pool. Takes current from a channel and keeps track of calcium buildup and depletion by a single exponential process.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message to receive Process message from scheduler


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: setCa

      (*destination message field*)      Assigns field value.


   .. py:method:: getCa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCaBasal

      (*destination message field*)      Assigns field value.


   .. py:method:: getCaBasal

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCa\_base

      (*destination message field*)      Assigns field value.


   .. py:method:: getCa\_base

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTau

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setB

      (*destination message field*)      Assigns field value.


   .. py:method:: getB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setThick

      (*destination message field*)      Assigns field value.


   .. py:method:: getThick

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiameter

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiameter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCeiling

      (*destination message field*)      Assigns field value.


   .. py:method:: getCeiling

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setFloor

      (*destination message field*)      Assigns field value.


   .. py:method:: getFloor

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: current

      (*destination message field*)      Calcium Ion current, due to be converted to conc.


   .. py:method:: currentFraction

      (*destination message field*)      Fraction of total Ion current, that is carried by Ca2+.


   .. py:method:: increase

      (*destination message field*)      Any input current that increases the concentration.


   .. py:method:: decrease

      (*destination message field*)      Any input current that decreases the concentration.


   .. py:method:: basal

      (*destination message field*)      Synonym for assignment of basal conc.


   .. py:attribute:: concOut

      double (*source message field*)      Concentration of Ca in pool


   .. py:attribute:: Ca

      double (*value field*)      Calcium concentration.


   .. py:attribute:: CaBasal

      double (*value field*)      Basal Calcium concentration.


   .. py:attribute:: Ca\_base

      double (*value field*)      Basal Calcium concentration, synonym for CaBasal


   .. py:attribute:: tau

      double (*value field*)      Settling time for Ca concentration


   .. py:attribute:: B

      double (*value field*)      Volume scaling factor. Deprecated. This is a legacy field from GENESIS and exposes internal calculations. Please do not use.
      B = 1/(vol * F) so that it obeys:
      dC/dt = B*I\_Ca - C/tau


   .. py:attribute:: thick

      double (*value field*)      Thickness of Ca shell, assumed cylindrical. Legal range is between zero and the radius. If outside this range it is taken as the radius. Default zero, ie, the shell is the entire thickness of the cylinder


   .. py:attribute:: diameter

      double (*value field*)      Diameter of Ca shell, assumed cylindrical


   .. py:attribute:: length

      double (*value field*)      Length of Ca shell, assumed cylindrical


   .. py:attribute:: ceiling

      double (*value field*)      Ceiling value for Ca concentration. If Ca > ceiling, Ca = ceiling. If ceiling <= 0.0, there is no upper limit on Ca concentration value.


   .. py:attribute:: floor

      double (*value field*)      Floor value for Ca concentration. If Ca < floor, Ca = floor
