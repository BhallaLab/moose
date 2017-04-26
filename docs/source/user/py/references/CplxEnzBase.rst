CplxEnzBase
-----------

.. py:class:: CplxEnzBase

   :		Base class for mass-action enzymes in which there is an  explicit pool for the enzyme-substrate complex. It models the reaction: E + S <===> E.S ----> E + P

   .. py:attribute:: enz

      void (*shared message field*)      Connects to enzyme pool


   .. py:attribute:: cplx

      void (*shared message field*)      Connects to enz-sub complex pool


   .. py:method:: setK1

      (*destination message field*)      Assigns field value.


   .. py:method:: getK1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setK2

      (*destination message field*)      Assigns field value.


   .. py:method:: getK2

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setK3

      (*destination message field*)      Assigns field value.


   .. py:method:: getK3

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRatio

      (*destination message field*)      Assigns field value.


   .. py:method:: getRatio

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setConcK1

      (*destination message field*)      Assigns field value.


   .. py:method:: getConcK1

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: enzDest

      (*destination message field*)      Handles # of molecules of Enzyme


   .. py:method:: cplxDest

      (*destination message field*)      Handles # of molecules of enz-sub complex


   .. py:attribute:: enzOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: cplxOut

      double,double (*source message field*)      Sends out increment of molecules on product each timestep


   .. py:attribute:: k1

      double (*value field*)      Forward reaction from enz + sub to complex, in # units.This parameter is subordinate to the Km. This means thatwhen Km is changed, this changes. It also means that whenk2 or k3 (aka kcat) are changed, we assume that Km remainsfixed, and as a result k1 must change. It is only whenk1 is assigned directly that we assume that the user knowswhat they are doing, and we adjust Km accordingly.k1 is also subordinate to the 'ratio' field, since setting the ratio reassigns k2.Should you wish to assign the elementary rates k1, k2, k3,of an enzyme directly, always assign k1 last.


   .. py:attribute:: k2

      double (*value field*)      Reverse reaction from complex to enz + sub


   .. py:attribute:: k3

      double (*value field*)      Forward rate constant from complex to product + enz


   .. py:attribute:: ratio

      double (*value field*)      Ratio of k2/k3


   .. py:attribute:: concK1

      double (*value field*)      K1 expressed in concentration (1/millimolar.sec) unitsThis parameter is subordinate to the Km. This means thatwhen Km is changed, this changes. It also means that whenk2 or k3 (aka kcat) are changed, we assume that Km remainsfixed, and as a result concK1 must change. It is only whenconcK1 is assigned directly that we assume that the user knowswhat they are doing, and we adjust Km accordingly.concK1 is also subordinate to the 'ratio' field, sincesetting the ratio reassigns k2.Should you wish to assign the elementary rates concK1, k2, k3,of an enzyme directly, always assign concK1 last.
