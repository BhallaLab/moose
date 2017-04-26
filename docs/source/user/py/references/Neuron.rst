Neuron
------

.. py:class:: Neuron

   Neuron - Manager for neurons. Handles high-level specification of distribution of spines, channels and passive properties. Also manages spine resizing through a Spine FieldElement.

   .. py:method:: setRM

      (*destination message field*)      Assigns field value.


   .. py:method:: getRM

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setRA

      (*destination message field*)      Assigns field value.


   .. py:method:: getRA

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCM

      (*destination message field*)      Assigns field value.


   .. py:method:: getCM

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setEm

      (*destination message field*)      Assigns field value.


   .. py:method:: getEm

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTheta

      (*destination message field*)      Assigns field value.


   .. py:method:: getTheta

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPhi

      (*destination message field*)      Assigns field value.


   .. py:method:: getPhi

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSourceFile

      (*destination message field*)      Assigns field value.


   .. py:method:: getSourceFile

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setCompartmentLengthInLambdas

      (*destination message field*)      Assigns field value.


   .. py:method:: getCompartmentLengthInLambdas

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumCompartments

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumSpines

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumBranches

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getPathDistanceFromSoma

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getGeometricalDistanceFromSoma

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getElectrotonicDistanceFromSoma

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCompartments

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setChannelDistribution

      (*destination message field*)      Assigns field value.


   .. py:method:: getChannelDistribution

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setPassiveDistribution

      (*destination message field*)      Assigns field value.


   .. py:method:: getPassiveDistribution

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSpineDistribution

      (*destination message field*)      Assigns field value.


   .. py:method:: getSpineDistribution

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCompartmentsFromExpression

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getValuesFromExpression

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSpinesFromExpression

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSpinesOnCompartment

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getParentCompartmentOfSpine

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: buildSegmentTree

      (*destination message field*)      Build the reference segment tree structure using the child compartments of the current Neuron. Fills in all the coords and length constant information into the segments, for later use when we build reduced compartment trees and channel distributions. Should only be called once, since subsequent use on a reduced model will lose the original full cell geometry.


   .. py:method:: setSpineAndPsdMesh

      (*destination message field*)      Assigns the spine and psd mesh to the Neuron. This is used to build up a mapping from Spine entries on the Neuron to chem spines and PSDs, so that volume change operations from the Spine can propagate to the chem systems.


   .. py:method:: setSpineAndPsdDsolve

      (*destination message field*)      Assigns the Dsolves used by spine and PSD to the Neuron. This is used to handle the rescaling of diffusion rates when spines are resized.


   .. py:method:: setNumSpine

      (*destination message field*)      Assigns number of field entries in field array.


   .. py:method:: getNumSpine

      (*destination message field*)      Requests number of field entries in field array.The requesting Element must provide a handler for the returned value.


   .. py:attribute:: RM

      double (*value field*)      Membrane resistivity, in ohm.m^2. Default value is 1.0.


   .. py:attribute:: RA

      double (*value field*)      Axial resistivity of cytoplasm, in ohm.m. Default value is 1.0.


   .. py:attribute:: CM

      double (*value field*)      Membrane Capacitance, in F/m^2. Default value is 0.01


   .. py:attribute:: Em

      double (*value field*)      Resting membrane potential of compartments, in Volts. Default value is -0.065.


   .. py:attribute:: theta

      double (*value field*)      Angle to rotate cell geometry, around long axis of neuron. Think Longitude. Units are radians. Default value is zero, which means no rotation.


   .. py:attribute:: phi

      double (*value field*)      Angle to rotate cell geometry, around elevation of neuron. Think Latitude. Units are radians. Default value is zero, which means no rotation.


   .. py:attribute:: sourceFile

      string (*value field*)      Name of source file from which to load a model. Accepts swc and dotp formats at present. Both these formats require that the appropriate channel definitions should have been loaded into /library.


   .. py:attribute:: compartmentLengthInLambdas

      double (*value field*)      Units: meters (SI).
      Electrotonic length to use for the largest compartment in the model. Used to define subdivision of branches into compartments. For example, if we set *compartmentLengthInLambdas*  to 0.1, and *lambda* (electrotonic length) is 250 microns, then it sets the compartment length to 25 microns. Thus a dendritic branch of 500 microns is subdivided into 20 commpartments. If the branch is shorter than *compartmentLengthInLambdas*, then it is not subdivided. If *compartmentLengthInLambdas* is set to 0 then the original compartmental structure of the model is preserved.  Note that this routine does NOT merge branches, even if *compartmentLengthInLambdas* is bigger than the branch. While all this subdivision is being done, the Neuron class preserves as detailed a geometry as it can, so it can rebuild the more detailed version if needed. Default value of *compartmentLengthInLambdas* is 0.


   .. py:attribute:: numCompartments

      unsigned int (*value field*)      Number of electrical compartments in model.


   .. py:attribute:: numSpines

      unsigned int (*value field*)      Number of dendritic spines in model.


   .. py:attribute:: numBranches

      unsigned int (*value field*)      Number of branches in dendrites.


   .. py:attribute:: pathDistanceFromSoma

      vector<double> (*value field*)      geometrical path distance of each segment from soma, measured by threading along the dendrite.


   .. py:attribute:: geometricalDistanceFromSoma

      vector<double> (*value field*)      geometrical distance of each segment from soma.


   .. py:attribute:: electrotonicDistanceFromSoma

      vector<double> (*value field*)      geometrical distance of each segment from soma, as measured along the dendrite.


   .. py:attribute:: compartments

      vector<ObjId> (*value field*)      Vector of ObjIds of electrical compartments. Order matches order of segments, and also matches the order of the electrotonic and geometricalDistanceFromSoma vectors.


   .. py:attribute:: channelDistribution

      vector<string> (*value field*)      Specification for distribution of channels, CaConcens and any other model components that are defined as prototypes and have to be placed on the electrical compartments.
      Arguments: proto path field expr [field expr]...
       Each entry is terminated with an empty string. The prototype is any object created in */library*, If a channel matching the prototype name already exists, then all subsequent operations are applied to the extant channel and a new one is not created. The paired arguments are as follows:
      The *field* argument specifies the name of the parameter that is to be assigned by the expression.
      The *expression* argument is a mathematical expression in the muparser framework, which permits most operations including trig and transcendental ones. Of course it also handles simple numerical values like 1.0, 1e-10 and so on. Available arguments for muParser are:
       p, g, L, len, dia, maxP, maxG, maxL
      	p: path distance from soma, measured along dendrite, in metres.
      	g: geometrical distance from soma, in metres.
      	L: electrotonic distance (# of lambdas) from soma, along dend. No units.
      	len: length of compartment, in metres.
      	dia: for diameter of compartment, in metres.
      	maxP: Maximum value of *p* for this neuron.
      	maxG: Maximum value of *g* for this neuron.
      	maxL: Maximum value of *L* for this neuron.
      The expression for the first field must evaluate to > 0 for the channel to be installed. For example, for channels, if Field == Gbar, and func( r, L, len, dia) < 0,
      then the channel is not installed. This feature is typically used with the sign() or Heaviside H() function to limit range: for example: H(1 - L) will only put channels closer than one length constant from the soma, and zero elsewhere.
      Available fields are:
      Channels: Gbar (install), Ek
      CaConcen: shellDia (install), shellFrac (install), tau, min
      Unless otherwise noted, all fields are scaled appropriately by the dimensions of their compartment. Thus the channel maximal conductance Gbar is automatically scaled by the area of the compartment, and the user does not need to insert this scaling into the calculations.
      All parameters are expressed in SI units. Conductance, for example, is Siemens/sq metre.

      Some example function forms might be for a channel Gbar:
       p < 10e-6 ? 400 : 0.0
      		equivalently,
       H(10e-6 - p) * 400
      		equivalently,
       ( sign(10e-6 - p) + 1) * 200
      Each of these forms instruct the function to set channel Gbar to 400 S/m^2 only within 10 microns path distance of soma

       L < 1.0 ? 100 * exp( -L ) : 0.0
       ->Set channel Gbar to an exponentially falling function of electrotonic distance from soma, provided L is under 1.0 lambdas.



   .. py:attribute:: passiveDistribution

      vector<string> (*value field*)      Specification for distribution of passive properties of cell.
      Arguments: . path field expr [field expr]...
      Note that the arguments list starts with a period.  Each entry is terminated with an empty string. The paired arguments are as follows:
      The *field* argument specifies the name of the parameter that is to be assigned by the expression.
      The *expression* argument is a mathematical expression in the muparser framework, which permits most operations including trig and transcendental ones. Of course it also handles simple numerical values like 1.0, 1e-10 and so on. Available arguments for muParser are:
       p, g, L, len, dia, maxP, maxG, maxL
      	p: path distance from soma, measured along dendrite, in metres.
      	g: geometrical distance from soma, in metres.
      	L: electrotonic distance (# of lambdas) from soma, along dend. No units.
      	len: length of compartment, in metres.
      	dia: for diameter of compartment, in metres.
      	maxP: Maximum value of *p* for this neuron.
      	maxG: Maximum value of *g* for this neuron.
      	maxL: Maximum value of *L* for this neuron.
      Available fields are:
      RM, RA, CM, Rm, Ra, Cm, Em, initVm
      The first three fields are scaled appropriately by the dimensions of their compartment. Thus the membrane resistivity RM (ohms.m^2) is automatically scaled by the area of the compartment, and the user does not need to insert this scaling into the calculations to compute Rm.Using the Rm field lets the user directly assign the membrane resistance (in ohms), presumably using len and dia.
      Similarly, RA (ohms.m) and CM (Farads/m^2) are specific units and the actual values for each compartment are assigned by scaling by length and diameter. Ra (ohms) and Cm (Farads) require explicit evaluation of the expression. All parameters are expressed in SI units. Conductance, for example, is Siemens/sq metre.
      Note that time these calculations do NOT currently include spines



   .. py:attribute:: spineDistribution

      vector<string> (*value field*)      Specification for distribution of spines on dendrite.
      Arguments: proto path spacing expr [field expr]...
       Each entry is terminated with an empty string. The *prototype* is any spine object created in */library*,
      The *path* is the wildcard path of compartments on which to place the spine.
      The *spacing* is the spacing of spines, in metres.
      The *expression* argument is a mathematical expression in the muparser framework, which permits most operations including trig and transcendental ones. Of course it also handles simple numerical values like 1.0, 1e-10 and so on. The paired arguments are as follows:
      The *field* argument specifies the name of the parameter that is to be assigned by the expression.
      The *expression* argument is a mathematical expression as above. Available arguments for muParser are::

        p, g, L, len, dia, maxP, maxG, maxL

      	p: path distance from soma, measured along dendrite, in metres.
      	g: geometrical distance from soma, in metres.
      	L: electrotonic distance (# of lambdas) from soma, along dend. No units.
      	len: length of compartment, in metres.
      	dia: for diameter of compartment, in metres.
      	maxP: Maximum value of *p* for this neuron.
      	maxG: Maximum value of *g* for this neuron.
      	maxL: Maximum value of *L* for this neuron.
      The expression for the *spacing* field must evaluate to > 0 for the spine to be installed. For example, if the expresssion is
      		H(1 - L)
      then the systemwill only put spines closer than one length constant from the soma, and zero elsewhere.
      Available spine parameters are:
      spacing, minSpacing, size, sizeDistrib angle, angleDistrib



   .. py:attribute:: compartmentsFromExpression

      string,vector<ObjId> (*lookup field*)      Vector of ObjIds of electrical compartments that match the 'path expression' pair in the argument string.


   .. py:attribute:: valuesFromExpression

      string,vector<double> (*lookup field*)      Vector of values computed for each electrical compartment that matches the 'path expression' pair in the argument string.This has 13 times the number of entries as # of compartments.For each compartment the entries are:
      val, p, g, L, len, dia, maxP, maxG, maxL, x, y, z, 0


   .. py:attribute:: spinesFromExpression

      string,vector<ObjId> (*lookup field*)      Vector of ObjIds of compartments comprising spines/heads that match the 'path expression' pair in the argument string.


   .. py:attribute:: spinesOnCompartment

      ObjId,vector<ObjId> (*lookup field*)      Vector of ObjIds of spines shafts/heads sitting on the specified electrical compartment. If each spine has a shaft and a head,and there are 10 spines on the compartment, there will be 20 entries in the returned vector, ordered shaft0, head0, shaft1, head1, ...


   .. py:attribute:: parentCompartmentOfSpine

      ObjId,ObjId (*lookup field*)      Returns parent compartment of specified spine compartment.Both the spine head or its shaft will return the same parent.
