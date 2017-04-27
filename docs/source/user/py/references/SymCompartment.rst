SymCompartment
--------------

.. py:class:: SymCompartment

   SymCompartment object, for branching neuron models. In symmetric
   compartments the axial resistance is equally divided on two sides of
   the node. The equivalent circuit of the passive compartment becomes:
   (NOTE: you must use a fixed-width font like Courier for correct rendition of the diagrams below)::

            Ra/2    B    Ra/2
          A-/\/\/\\_\_\_\_\_/\/\/\-- C
                    |
                \_\_\_\_|\_\_\_\_
               |         |
               |         \
               |         / Rm
              ---- Cm    \
              ----       /
               |         |
               |       \_\_\_\_\_
               |        ---  Em
               |\_\_\_\_\_\_\_\_\_|
                   |
                 \_\_|\_\_
                 /////


   In case of branching, the B-C part of the parent's axial resistance
   forms a Y with the A-B part of the children::

                                  B'
                                  |
                                  /
                                  \
                                  /
                                  \
                                  /
                                  |A'
                   B              |
     A-----/\/\/\-----/\/\/\------|C
                                  |
                                  |A"
                                  /
                                  \
                                  /
                                  \
                                  /
                                  |
                                  B"
   As per basic circuit analysis techniques, the C node is replaced using
   star-mesh transform. This requires all sibling compartments at a
   branch point to be connected via 'sibling' messages by the user (or
   by the cell reader in case of prototypes). For the same reason, the
   child compartment must be connected to the parent by
   distal-proximal message pair. The calculation of the
   coefficient for computing equivalent resistances in the mesh is done
   at reinit.

   .. py:attribute:: proximal

      void (*shared message field*)      This is a shared message between symmetric compartments.
      It goes from the proximal end of the current compartment to
      distal end of the compartment closer to the soma.



   .. py:attribute:: distal

      void (*shared message field*)      This is a shared message between symmetric compartments.
      It goes from the distal end of the current compartment to the
      proximal end of one further from the soma.
      The Ra values collected from children and
      sibling nodes are used for computing the equivalent resistance
      between each pair of nodes using star-mesh transformation.
      Mathematically this is the same as the proximal message, but
      the distinction is important for traversal and clarity.



   .. py:attribute:: sibling

      void (*shared message field*)      This is a shared message between symmetric compartments.
      Conceptually, this goes from the proximal end of the current
      compartment to the proximal end of a sibling compartment
      on a branch in a dendrite. However,
      this works out to the same as a 'distal' message in terms of
      equivalent circuit.  The Ra values collected from siblings
      and parent node are used for
      computing the equivalent resistance between each pair of
      nodes using star-mesh transformation.



   .. py:attribute:: sphere

      void (*shared message field*)      This is a shared message between a spherical compartment
      (typically soma) and a number of evenly spaced cylindrical
      compartments, typically primary dendrites.
      The sphere contributes the usual Ra/2 to the resistance
      between itself and children. The child compartments
      do not connect across to each other
      through sibling messages. Instead they just connect to the soma
      through the 'proximalOnly' message



   .. py:attribute:: cylinder

      void (*shared message field*)      This is a shared message between a cylindrical compartment
      (typically a dendrite) and a number of evenly spaced child
      compartments, typically dendritic spines, protruding from the
      curved surface of the cylinder. We assume that the resistance
      from the cylinder curved surface to its axis is negligible.
      The child compartments do not need to connect across to each
      other through sibling messages. Instead they just connect to the
      parent dendrite through the 'proximalOnly' message



   .. py:attribute:: proximalOnly

      void (*shared message field*)      This is a shared message between a dendrite and a parent
      compartment whose offspring are spatially separated from each
      other. For example, evenly spaced dendrites emerging from a soma
      or spines emerging from a common parent dendrite. In these cases
      the sibling dendrites do not need to connect to each other
      through 'sibling' messages. Instead they just connect to the
      parent compartment (soma or dendrite) through this message



   .. py:method:: raxialSym

      (*destination message field*)      Expects Ra and Vm from other compartment.


   .. py:method:: sumRaxial

      (*destination message field*)      Expects Ra from other compartment.


   .. py:method:: raxialSym

      (*destination message field*)      Expects Ra and Vm from other compartment.


   .. py:method:: sumRaxial

      (*destination message field*)      Expects Ra from other compartment.


   .. py:method:: raxialSym

      (*destination message field*)      Expects Ra and Vm from other compartment.


   .. py:method:: sumRaxial

      (*destination message field*)      Expects Ra from other compartment.


   .. py:method:: raxialSphere

      (*destination message field*)      Expects Ra and Vm from other compartment. This is a special case when other compartments are evenly distributed on a spherical compartment.


   .. py:method:: raxialCylinder

      (*destination message field*)      Expects Ra and Vm from other compartment. This is a special case when other compartments are evenly distributed on the curved surface of the cylindrical compartment, so we assume that the cylinder does not add any further resistance.


   .. py:method:: raxialSphere

      (*destination message field*)      Expects Ra and Vm from other compartment. This is a special case when other compartments are evenly distributed on a spherical compartment.


   .. py:attribute:: proximalOut

      double,double (*source message field*)      Sends out Ra and Vm on each timestep, on the proximal end of a compartment. That is, this end should be  pointed toward the soma. Mathematically the same as raxialOut but provides a logical orientation of the dendrite. One can traverse proximalOut messages to get to the soma.


   .. py:attribute:: sumRaxialOut

      double (*source message field*)      Sends out Ra


   .. py:attribute:: distalOut

      double,double (*source message field*)      Sends out Ra and Vm on each timestep, on the distal end of a compartment. This end should be pointed away from the soma. Mathematically the same as proximalOut, but gives an orientation to the dendrite and helps traversal.


   .. py:attribute:: sumRaxialOut

      double (*source message field*)      Sends out Ra


   .. py:attribute:: distalOut

      double,double (*source message field*)      Sends out Ra and Vm on each timestep, on the distal end of a compartment. This end should be pointed away from the soma. Mathematically the same as proximalOut, but gives an orientation to the dendrite and helps traversal.


   .. py:attribute:: sumRaxialOut

      double (*source message field*)      Sends out Ra


   .. py:attribute:: distalOut

      double,double (*source message field*)      Sends out Ra and Vm on each timestep, on the distal end of a compartment. This end should be pointed away from the soma. Mathematically the same as proximalOut, but gives an orientation to the dendrite and helps traversal.


   .. py:attribute:: cylinderOut

      double,double (*source message field*)       Sends out Ra and Vm to compartments (typically spines) on the curved surface of a cylinder. Ra is set to nearly zero, since we assume that the resistance from axis to surface is negligible.


   .. py:attribute:: proximalOut

      double,double (*source message field*)      Sends out Ra and Vm on each timestep, on the proximal end of a compartment. That is, this end should be  pointed toward the soma. Mathematically the same as raxialOut but provides a logical orientation of the dendrite. One can traverse proximalOut messages to get to the soma.
