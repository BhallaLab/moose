HHGate2D
--------

.. py:class:: HHGate2D

   HHGate2D: Gate for Hodkgin-Huxley type channels, equivalent to the m and h terms on the Na squid channel and the n term on K. This takes the voltage and state variable from the channel, computes the new value of the state variable and a scaling, depending on gate power, for the conductance. These two terms are sent right back in a message to the channel.

   .. py:method:: getA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTableA

      (*destination message field*)      Assigns field value.


   .. py:method:: getTableA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTableB

      (*destination message field*)      Assigns field value.


   .. py:method:: getTableB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXminA

      (*destination message field*)      Assigns field value.


   .. py:method:: getXminA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmaxA

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmaxA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXdivsA

      (*destination message field*)      Assigns field value.


   .. py:method:: getXdivsA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYminA

      (*destination message field*)      Assigns field value.


   .. py:method:: getYminA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmaxA

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmaxA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYdivsA

      (*destination message field*)      Assigns field value.


   .. py:method:: getYdivsA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXminB

      (*destination message field*)      Assigns field value.


   .. py:method:: getXminB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmaxB

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmaxB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXdivsB

      (*destination message field*)      Assigns field value.


   .. py:method:: getXdivsB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYminB

      (*destination message field*)      Assigns field value.


   .. py:method:: getYminB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYmaxB

      (*destination message field*)      Assigns field value.


   .. py:method:: getYmaxB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setYdivsB

      (*destination message field*)      Assigns field value.


   .. py:method:: getYdivsB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: tableA

      vector< vector<double> > (*value field*)      Table of A entries


   .. py:attribute:: tableB

      vector< vector<double> > (*value field*)      Table of B entries


   .. py:attribute:: xminA

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: xmaxA

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: xdivsA

      unsigned int (*value field*)      Divisions for lookup. Zero means to use linear interpolation


   .. py:attribute:: yminA

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: ymaxA

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: ydivsA

      unsigned int (*value field*)      Divisions for lookup. Zero means to use linear interpolation


   .. py:attribute:: xminB

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: xmaxB

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: xdivsB

      unsigned int (*value field*)      Divisions for lookup. Zero means to use linear interpolation


   .. py:attribute:: yminB

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: ymaxB

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: ydivsB

      unsigned int (*value field*)      Divisions for lookup. Zero means to use linear interpolation


   .. py:attribute:: A

      vector<double>,double (*lookup field*)      lookupA: Look up the A gate value from two doubles, passedin as a vector. Uses linear interpolation in the 2D tableThe range of the lookup doubles is predefined based on knowledge of voltage or conc ranges, and the granularity is specified by the xmin, xmax, and dx field, and their y-axis counterparts.


   .. py:attribute:: B

      vector<double>,double (*lookup field*)      lookupB: Look up B gate value from two doubles in a vector.
