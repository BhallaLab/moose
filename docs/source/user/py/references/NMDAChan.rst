NMDAChan
--------

.. py:class:: NMDAChan

   NMDAChan: Ligand-gated ion channel incorporating both the Mg block and a GHK calculation for Calcium component of the current carried by the channel. Assumes a steady reversal potential regardless of Ca gradients. Derived from SynChan.

   .. py:method:: setKMg\_A

      (*destination message field*)      Assigns field value.


   .. py:method:: getKMg\_A

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setKMg\_B

      (*destination message field*)      Assigns field value.


   .. py:method:: getKMg\_B

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCMg

      (*destination message field*)      Assigns field value.


   .. py:method:: getCMg

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTemperature

      (*destination message field*)      Assigns field value.


   .. py:method:: getTemperature

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setExtCa

      (*destination message field*)      Assigns field value.


   .. py:method:: getExtCa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setIntCa

      (*destination message field*)      Assigns field value.


   .. py:method:: getIntCa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setIntCaScale

      (*destination message field*)      Assigns field value.


   .. py:method:: getIntCaScale

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setIntCaOffset

      (*destination message field*)      Assigns field value.


   .. py:method:: getIntCaOffset

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCondFraction

      (*destination message field*)      Assigns field value.


   .. py:method:: getCondFraction

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getICa

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPermeability

      (*destination message field*)      Assigns field value.


   .. py:method:: getPermeability

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: assignIntCa

      (*destination message field*)      Assign the internal concentration of Ca. The final value is computed as:      intCa = assignIntCa * intCaScale + intCaOffset


   .. py:attribute:: ICaOut

      double (*source message field*)      Calcium current portion of the total current carried by the NMDAR


   .. py:attribute:: KMg\_A

      double (*value field*)      1/eta


   .. py:attribute:: KMg\_B

      double (*value field*)      1/gamma


   .. py:attribute:: CMg

      double (*value field*)      [Mg] in mM


   .. py:attribute:: temperature

      double (*value field*)      Temperature in degrees Kelvin.


   .. py:attribute:: extCa

      double (*value field*)      External concentration of Calcium in millimolar


   .. py:attribute:: intCa

      double (*value field*)      Internal concentration of Calcium in millimolar.This is the final value used by the internal calculations, and may also be updated by the assignIntCa message after offset and scaling.


   .. py:attribute:: intCaScale

      double (*value field*)      Scale factor for internal concentration of Calcium in mM, applied to values coming in through the assignIntCa message. Required because in many models the units of calcium may differ.


   .. py:attribute:: intCaOffset

      double (*value field*)      Offsetfor internal concentration of Calcium in mM, applied \_after\_ the scaling to mM is done. Applied to values coming in through the assignIntCa message. Required because in many models the units of calcium may differ.


   .. py:attribute:: condFraction

      double (*value field*)      Fraction of total channel conductance that is due to the passage of Ca ions. This is related to the ratio of permeabilities of different ions, and is typically in the range of 0.02. This small fraction is largely because the concentrations of Na and K ions are far larger than that of Ca. Thus, even though the channel is more permeable to Ca, the conductivity and hence current due to Ca is smaller.


   .. py:attribute:: ICa

      double (*value field*)      Current carried by Ca ions


   .. py:attribute:: permeability

      double (*value field*)      Permeability. Alias for Gbar. Note that the mapping is not really correct. Permeability is in units of m/s whereas Gbar is 1/ohm. Some nasty scaling is involved in the conversion, some of which itself involves concentration variables. Done internally.
