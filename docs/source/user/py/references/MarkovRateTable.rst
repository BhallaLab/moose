MarkovRateTable
---------------

.. py:class:: MarkovRateTable

   Rate Table for Markov channel calculations.

   .. py:attribute:: channel

      void (*shared message field*)      This message couples the rate table to the compartment. The rate table needs updates on voltage in order to compute the rate table.


   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process message from thescheduler. The first entry is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt andso on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: handleVm

      (*destination message field*)      Handles incoming message containing voltage information.


   .. py:method:: process

      (*destination message field*)      Handles process call


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call


   .. py:method:: init

      (*destination message field*)      Initialization of the class. Allocates memory for all the tables.


   .. py:method:: handleLigandConc

      (*destination message field*)      Handles incoming message containing ligand concentration.


   .. py:method:: set1d

      (*destination message field*)      Setting up of 1D lookup table for the (i,j)'th rate.


   .. py:method:: set2d

      (*destination message field*)      Setting up of 2D lookup table for the (i,j)'th rate.


   .. py:method:: setconst

      (*destination message field*)      Setting a constant value for the (i,j)'th rate. Internally, this is	stored as a 1-D rate with a lookup table containing 1 entry.


   .. py:method:: setVm

      (*destination message field*)      Assigns field value.


   .. py:method:: getVm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setLigandConc

      (*destination message field*)      Assigns field value.


   .. py:method:: getLigandConc

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getQ

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSize

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: instratesOut

      vector< vector<double> > (*source message field*)      Sends out instantaneous rate information of varying transition ratesat each time step.


   .. py:attribute:: Vm

      double (*value field*)      Membrane voltage.


   .. py:attribute:: ligandConc

      double (*value field*)      Ligand concentration.


   .. py:attribute:: Q

      vector< vector<double> > (*value field*)      Instantaneous rate matrix.


   .. py:attribute:: size

      unsigned int (*value field*)      Dimension of the families of lookup tables. Is always equal to the number of states in the model.
