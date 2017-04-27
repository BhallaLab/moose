VectorTable
-----------

.. py:class:: VectorTable

   This is a minimal 1D equivalent of the Interpol2D class. Provides simple functions for getting and setting up the table, along with a lookup function. This class is to be used while supplying lookup tables to the MarkovChannel class, in cases where the transition rate varies with either membrane voltage or ligand concentration.

   .. py:method:: setXdivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getXdivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmin

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setXmax

      (*destination message field*)      Assigns field value.


   .. py:method:: getXmax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getInvdx

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTable

      (*destination message field*)      Assigns field value.


   .. py:method:: getTable

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getLookupvalue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getLookupindex

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: xdivs

      unsigned int (*value field*)      Number of divisions.


   .. py:attribute:: xmin

      double (*value field*)      Minimum value in table.


   .. py:attribute:: xmax

      double (*value field*)      Maximum value in table.


   .. py:attribute:: invdx

      double (*value field*)      Maximum value in table.


   .. py:attribute:: table

      vector<double> (*value field*)      The lookup table.


   .. py:attribute:: lookupvalue

      double,double (*lookup field*)      Lookup function that performs interpolation to return a value.


   .. py:attribute:: lookupindex

      unsigned int,double (*lookup field*)      Lookup function that returns value by index.
