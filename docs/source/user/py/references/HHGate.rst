HHGate
------

.. py:class:: HHGate

   HHGate: Gate for Hodkgin-Huxley type channels, equivalent to the m and h terms on the Na squid channel and the n term on K. This takes the voltage and state variable from the channel, computes the new value of the state variable and a scaling, depending on gate power, for the conductance.

   .. py:method:: getA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAlpha

      (*destination message field*)      Assigns field value.


   .. py:method:: getAlpha

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setBeta

      (*destination message field*)      Assigns field value.


   .. py:method:: getBeta

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTau

      (*destination message field*)      Assigns field value.


   .. py:method:: getTau

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMInfinity

      (*destination message field*)      Assigns field value.


   .. py:method:: getMInfinity

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMin

      (*destination message field*)      Assigns field value.


   .. py:method:: getMin

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMax

      (*destination message field*)      Assigns field value.


   .. py:method:: getMax

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDivs

      (*destination message field*)      Assigns field value.


   .. py:method:: getDivs

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTableA

      (*destination message field*)      Assigns field value.


   .. py:method:: getTableA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTableB

      (*destination message field*)      Assigns field value.


   .. py:method:: getTableB

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setUseInterpolation

      (*destination message field*)      Assigns field value.


   .. py:method:: getUseInterpolation

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setAlphaParms

      (*destination message field*)      Assigns field value.


   .. py:method:: getAlphaParms

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setupAlpha

      (*destination message field*)      Set up both gates using 13 parameters, as follows:setupAlpha AA AB AC AD AF BA BB BC BD BF xdivs xmin xmaxHere AA-AF are Coefficients A to F of the alpha (forward) termHere BA-BF are Coefficients A to F of the beta (reverse) termHere xdivs is the number of entries in the table,xmin and xmax define the range for lookup.Outside this range the returned value will be the low [high]entry of the table.The equation describing each table is:y(x) = (A + B * x) / (C + exp((x + D) / F))The original HH equations can readily be cast into this form


   .. py:method:: setupTau

      (*destination message field*)      Identical to setupAlpha, except that the forms specified bythe 13 parameters are for the tau and m-infinity curves ratherthan the alpha and beta terms. So the parameters are:setupTau TA TB TC TD TF MA MB MC MD MF xdivs xmin xmaxAs before, the equation describing each curve is:y(x) = (A + B * x) / (C + exp((x + D) / F))


   .. py:method:: tweakAlpha

      (*destination message field*)      Dummy function for backward compatibility. It used to convertthe tables from alpha, beta values to alpha, alpha+betabecause the internal calculations used these forms. Notneeded now, deprecated.


   .. py:method:: tweakTau

      (*destination message field*)      Dummy function for backward compatibility. It used to convertthe tables from tau, minf values to alpha, alpha+betabecause the internal calculations used these forms. Notneeded now, deprecated.


   .. py:method:: setupGate

      (*destination message field*)      Sets up one gate at a time using the alpha/beta form.Has 9 parameters, as follows:setupGate A B C D F xdivs xmin xmax is\_betaThis sets up the gate using the equation:y(x) = (A + B * x) / (C + exp((x + D) / F))Deprecated.


   .. py:attribute:: alpha

      vector<double> (*value field*)      Parameters for voltage-dependent rates, alpha:Set up alpha term using 5 parameters, as follows:y(x) = (A + B * x) / (C + exp((x + D) / F))The original HH equations can readily be cast into this form


   .. py:attribute:: beta

      vector<double> (*value field*)      Parameters for voltage-dependent rates, beta:Set up beta term using 5 parameters, as follows:y(x) = (A + B * x) / (C + exp((x + D) / F))The original HH equations can readily be cast into this form


   .. py:attribute:: tau

      vector<double> (*value field*)      Parameters for voltage-dependent rates, tau:Set up tau curve using 5 parameters, as follows:y(x) = (A + B * x) / (C + exp((x + D) / F))


   .. py:attribute:: mInfinity

      vector<double> (*value field*)      Parameters for voltage-dependent rates, mInfinity:Set up mInfinity curve using 5 parameters, as follows:y(x) = (A + B * x) / (C + exp((x + D) / F))The original HH equations can readily be cast into this form


   .. py:attribute:: min

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: max

      double (*value field*)      Minimum range for lookup


   .. py:attribute:: divs

      unsigned int (*value field*)      Divisions for lookup. Zero means to use linear interpolation


   .. py:attribute:: tableA

      vector<double> (*value field*)      Table of A entries


   .. py:attribute:: tableB

      vector<double> (*value field*)      Table of alpha + beta entries


   .. py:attribute:: useInterpolation

      bool (*value field*)      Flag: use linear interpolation if true, else direct lookup


   .. py:attribute:: alphaParms

      vector<double> (*value field*)      Set up both gates using 13 parameters, as follows:setupAlpha AA AB AC AD AF BA BB BC BD BF xdivs xmin xmaxHere AA-AF are Coefficients A to F of the alpha (forward) termHere BA-BF are Coefficients A to F of the beta (reverse) termHere xdivs is the number of entries in the table,xmin and xmax define the range for lookup.Outside this range the returned value will be the low [high]entry of the table.The equation describing each table is:y(x) = (A + B * x) / (C + exp((x + D) / F))The original HH equations can readily be cast into this form


   .. py:attribute:: A

      double,double (*lookup field*)      lookupA: Look up the A gate value from a double. Usually doesso by direct scaling and offset to an integer lookup, usinga fine enough table granularity that there is little error.Alternatively uses linear interpolation.The range of the double is predefined based on knowledge ofvoltage or conc ranges, and the granularity is specified bythe xmin, xmax, and dV fields.


   .. py:attribute:: B

      double,double (*lookup field*)      lookupB: Look up the B gate value from a double.Note that this looks up the raw tables, which are transformedfrom the reference parameters.
