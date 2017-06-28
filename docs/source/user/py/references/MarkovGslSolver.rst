MarkovGslSolver
---------------

.. py:class:: MarkovGslSolver

   Solver for Markov Channel.

   .. py:attribute:: proc

      void (*shared message field*)      Shared message for process and reinit


   .. py:method:: getIsInitialized

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMethod

      (*destination message field*)      Assigns field value.


   .. py:method:: getMethod

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRelativeAccuracy

      (*destination message field*)      Assigns field value.


   .. py:method:: getRelativeAccuracy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAbsoluteAccuracy

      (*destination message field*)      Assigns field value.


   .. py:method:: getAbsoluteAccuracy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInternalDt

      (*destination message field*)      Assigns field value.


   .. py:method:: getInternalDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: init

      (*destination message field*)      Initialize solver parameters.


   .. py:method:: handleQ

      (*destination message field*)      Handles information regarding the instantaneous rate matrix from the MarkovRateTable class.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:attribute:: stateOut

      vector<double> (*source message field*)      Sends updated state to the MarkovChannel class.


   .. py:attribute:: isInitialized

      bool (*value field*)      True if the message has come in to set solver parameters.


   .. py:attribute:: method

      string (*value field*)      Numerical method to use.


   .. py:attribute:: relativeAccuracy

      double (*value field*)      Accuracy criterion


   .. py:attribute:: absoluteAccuracy

      double (*value field*)      Another accuracy criterion


   .. py:attribute:: internalDt

      double (*value field*)      internal timestep to use.
