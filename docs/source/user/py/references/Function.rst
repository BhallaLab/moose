Function
--------

.. py:class:: Function

   General purpose function calculator using real numbers.
   It can parse mathematical expression defining a function and evaluate it and/or its derivative for specified variable values.You can assign expressions of the form::
   f(c0, c1, ..., cM, x0, x1, ..., xN, y0,..., yP )
    where `ci`'s are constants and `xi`'s and `yi`'s are variables.The constants must be defined before setting the expression and variables are connected via messages. The constants can have any name, but the variable names must be of the form x{i} or y{i}  where i is increasing integer starting from 0.
    The variables can be input from other moose objects. Such variables must be named `x{i}` in the expression and the source field is connected to Function.x[i]'s `input` destination field.
    In case the input variable is not available as a source field, but is a value field, then the value can be requested by connecting the `requestOut` message to the `get{Field}` destination on the target object. Such variables must be specified in the expression as y{i} and connecting the messages should happen in the same order as the y indices.
    This class handles only real numbers (C-double). Predefined constants are: pi=3.141592..., e=2.718281...

   .. py:attribute:: proc

      void (*shared message field*)      This is a shared message to receive Process messages from the scheduler objects.The first entry in the shared msg is a MsgDest for the Process operation. It has a single argument, ProcInfo, which holds lots of information about current time, thread, dt and so on. The second entry is a MsgDest for the Reinit operation. It also uses ProcInfo.


   .. py:method:: getValue

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getRate

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


   .. py:method:: setNumVars

      (*destination message field*)      Assigns field value.


   .. py:method:: getNumVars

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setNumX

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumX

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:method:: setC

      (*destination message field*)      Assigns field value.


   .. py:method:: getC

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setIndependent

      (*destination message field*)      Assigns field value.


   .. py:method:: getIndependent

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: process

      (*destination message field*)      Handles process call, updates internal time stamp.


   .. py:method:: reinit

      (*destination message field*)      Handles reinit call.


   .. py:attribute:: requestOut

      PSt6vectorIdSaIdEE (*source message field*)      Sends request for input variable from a field on target object


   .. py:attribute:: valueOut

      double (*source message field*)      Evaluated value of the function for the current variable values.


   .. py:attribute:: rateOut

      double (*source message field*)      Value of time-derivative of the function for the current variable values


   .. py:attribute:: derivativeOut

      double (*source message field*)      Value of derivative of the function for the current variable values


   .. py:attribute:: value

      double (*value field*)      Result of the function evaluation with current variable values.


   .. py:attribute:: rate

      double (*value field*)      Derivative of the function at given variable values. This is computed as the difference of the current and previous value of the function divided by the time step.


   .. py:attribute:: derivative

      double (*value field*)      Derivative of the function at given variable values. This is calulated using 5-point stencil  <http://en.wikipedia.org/wiki/Five-point\_stencil> at current value of independent variable. Note that unlike hand-calculated derivatives, numerical derivatives are not exact.


   .. py:attribute:: mode

      unsigned int (*value field*)      Mode of operation:
       1: only the function value will be sent out.
       2: only the derivative with respect to the independent variable will be sent out.
       3: only rate (time derivative) will be sent out.
       anything else: all three, value, derivative and rate will be sent out.



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
      ln  1       logarithm to base e (2.71828...)
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
      Op  meaning         priority
      =   assignment     -1
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
      %   floating point modulo         7

      ?:  if then else operator   C++ style syntax



   .. py:attribute:: numVars

      unsigned int (*value field*)      Number of variables used by Function.


   .. py:attribute:: independent

      string (*value field*)      Index of independent variable. Differentiation is done based on this. Defaults to the first assigned variable.


   .. py:attribute:: c

      string,double (*lookup field*)      Constants used in the function. These must be assigned before specifying the function expression.


.. py:class:: GammaRng

   Gamma distributed random number generator.

   .. py:method:: setAlpha

      (*destination message field*)      Assigns field value.


   .. py:method:: getAlpha

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTheta

      (*destination message field*)      Assigns field value.


   .. py:method:: getTheta

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: alpha

      double (*value field*)      Parameter alpha of the gamma distribution.


   .. py:attribute:: theta

      double (*value field*)      Parameter theta of the Gamma distribution.
