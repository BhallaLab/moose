Func
----

.. py:class:: Func

   Func: general purpose function calculator using real numbers. It can
   parse mathematical expression defining a function and evaluate it
   and/or its derivative for specified variable values.
   The variables can be input from other moose objects. In case of
   arbitrary variable names, the source message must have the variable
   name as the first argument. For most common cases, input messages to
   set x, y, z and xy, xyz are made available without such
   requirement. This class handles only real numbers
   (C-double). Predefined constants are: pi=3.141592...,
   e=2.718281...

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: getValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDerivative

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMode

      (*destination message field*)      Assigns field value.


   .. py:method:: getMode

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setExpr

      (*destination message field*)      Assigns field value.


   .. py:method:: getExpr

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setVar

      (*destination message field*)      Assigns field value.


   .. py:method:: getVar

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getVars

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setX

      (*destination message field*)      Assigns field value.


   .. py:method:: getX

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setY

      (*destination message field*)      Assigns field value.


   .. py:method:: getY

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setZ

      (*destination message field*)      Assigns field value.


   .. py:method:: getZ

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: varIn

      (*destination message field*)      Handle value for specified variable coming from other objects


   .. py:method:: xIn

      (*destination message field*)      Handle value for variable named x. This is a shorthand. If the
      expression does not have any variable named x, this the first variable
      in the sequence `vars`.


   .. py:method:: yIn

      (*destination message field*)      Handle value for variable named y. This is a utility for two/three
       variable functions where the y value comes from a source separate
       from that of x. This is a shorthand. If the
      expression does not have any variable named y, this the second
      variable in the sequence `vars`.


   .. py:method:: zIn

      (*destination message field*)      Handle value for variable named z. This is a utility for three
       variable functions where the z value comes from a source separate
       from that of x or y. This is a shorthand. If the expression does not
       have any variable named y, this the second variable in the sequence `vars`.


   .. py:method:: xyIn

      (*destination message field*)      Handle value for variables x and y for two-variable function


   .. py:method:: xyzIn

      (*destination message field*)      Handle value for variables x, y and z for three-variable function


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: valueOut

      double (*source message field*)      Evaluated value of the function for the current variable values.


   .. py:attribute:: derivativeOut

      double (*source message field*)      Value of derivative of the function for the current variable values


   .. py:attribute:: value

      double (*value field*)      Result of the function evaluation with current variable values.


   .. py:attribute:: derivative

      double (*value field*)      Derivative of the function at given variable values.


   .. py:attribute:: mode

      unsigned int (*value field*)      Mode of operation:
       1: only the function value will be calculated
       2: only the derivative will be calculated
       3: both function value and derivative at current variable values will be calculated.


   .. py:attribute:: expr

      string (*value field*)      Mathematical expression defining the function. The underlying parser
      is muParser. In addition to the available functions and operators  from
      muParser, some more functions are added.

      Functions
      Name        args    explanation
      sin         1       sine function
      cos         1       cosine function
      tan         1       tangens function
      asin        1       arcus sine function
      acos        1       arcus cosine function
      atan        1       arcus tangens function
      sinh        1       hyperbolic sine function
      cosh        1       hyperbolic cosine
      tanh        1       hyperbolic tangens function
      asinh       1       hyperbolic arcus sine function
      acosh       1       hyperbolic arcus tangens function
      atanh       1       hyperbolic arcur tangens function
      log2        1       logarithm to the base 2
      log10       1       logarithm to the base 10
      log         1       logarithm to the base 10
      ln          1       logarithm to base e (2.71828...)
      exp         1       e raised to the power of x
      sqrt        1       square root of a value
      sign        1       sign function -1 if x<0; 1 if x>0
      rint        1       round to nearest integer
      abs         1       absolute value
      min         var.    min of all arguments
      max         var.    max of all arguments
      sum         var.    sum of all arguments
      avg         var.    mean value of all arguments
      rand        1       rand(seed), random float between 0 and 1,
                          if seed = -1, then a 'random' seed is created.
      rand2       3       rand(a, b, seed), random float between a and b,
                          if seed = -1, a 'random' seed is created using either
                          by random\_device or by reading system clock

      Operators
      Op  meaning         prioroty
      =   assignement     -1
      &&  logical and     1
      ||  logical or      2
      <=  less or equal   4
      >=  greater or equal        4
      !=  not equal       4
      ==  equal   4
      >   greater than    4
      <   less than       4
      +   addition        5
      -   subtraction     5
      *   multiplication  6
      /   division        6
      ^   raise x to the power of y       7

      ?:  if then else operator   C++ style syntax



   .. py:attribute:: vars

      vector<string> (*value field*)      Variable names in the expression


   .. py:attribute:: x

      double (*value field*)      Value for variable named x. This is a shorthand. If the
      expression does not have any variable named x, this the first variable
      in the sequence `vars`.


   .. py:attribute:: y

      double (*value field*)      Value for variable named y. This is a utility for two/three
       variable functions where the y value comes from a source separate
       from that of x. This is a shorthand. If the
      expression does not have any variable named y, this the second
      variable in the sequence `vars`.


   .. py:attribute:: z

      double (*value field*)      Value for variable named z. This is a utility for three
       variable functions where the z value comes from a source separate
       from that of x or z. This is a shorthand. If the expression does not
       have any variable named z, this the third variable in the sequence `vars`.


   .. py:attribute:: var

      string,double (*lookup field*)      Lookup table for variable values.
