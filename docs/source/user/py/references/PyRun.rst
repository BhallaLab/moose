PyRun
-----

.. py:class:: PyRun

   Runs Python statements from inside MOOSE.

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: setRunString

      (*destination message field*)      Assigns field value.


   .. py:method:: getRunString

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInitString

      (*destination message field*)      Assigns field value.


   .. py:method:: getInitString

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setInputVar

      (*destination message field*)      Assigns field value.


   .. py:method:: getInputVar

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setOutputVar

      (*destination message field*)      Assigns field value.


   .. py:method:: getOutputVar

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: trigger

      (*destination message field*)      Executes the current runString whenever a message arrives. It stores the incoming value in local variable named `input\_`, which can be used in the `runString` (the underscore is added to avoid conflict with Python's builtin function `input`). If debug is True, it prints the input value.


   .. py:method:: run

      (*destination message field*)      Runs a specified string. Does not modify existing run or init strings.


   .. py:method:: process

      (*destination message field*)      Handles process call. Runs the current runString.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call. Runs the current initString.


   .. py:attribute:: output

      double (*source message field*)      Sends out the value of local variable called `output`. Thus, you can have Python statements which compute some value and assign it to the variable called `output` (which is defined at `reinit` call). This will be sent out to any target connected to the `output` field.


   .. py:attribute:: runString

      string (*value field*)      String to be executed at each time step.


   .. py:attribute:: initString

      string (*value field*)      String to be executed at initialization (reinit).


   .. py:attribute:: mode

      int (*value field*)      Flag to indicate whether runString should be executed for both trigger and process, or one of them


   .. py:attribute:: inputVar

      string (*value field*)      Name of local variable in which input balue is to be stored. Default is `input\_` (to avoid conflict with Python's builtin function `input`).


   .. py:attribute:: outputVar

      string (*value field*)      Name of local variable for storing output. Default is `output`
