/**

\page HSolveDevOverview HSolve Overview

\section Introduction

This document gives an overview of the Hines' solver (HSolve) implementation
in MOOSE. At present it talks more about the interaction between HSolve and
the rest of MOOSE, and does not talk about HSolve's internals (that is, the
numerical implementation). Hence, it will be useful for someone who is
implementing a new numerical scheme in MOOSE.

When a neuronal model is read into MOOSE (from a NeuroML file, for example),
it is represented inside MOOSE by biophysical objects (of type Compartment,
HHChannel, etc.) linked up by messages. Representing the model in terms of
objects and messages is nice because it provides a natural interface that
the user and the rest of the system can use.

These objects have fields, using which the user can specify model
parameters, and monitor state variables during a simulation. The objects
also have some ODE solving code (usually Exponential Euler, or EE) which
allows them to advance their own state. The messages allow the objects to
talk to each other. In addition, the message connectivity depicts the
model's structure.

In absence of HSolve, these objects do the following things:
- They are woken up once every time-step to perform their calculations.
  (Usually a function called process()).
- Serve parameter and state variables via fields. For example, for plotting, a
  Compartment's Vm may be inquired once every few time-steps. Objects do this
  by providing a get() function for each field, and also a set() function to
  change the field values.
- Communicate with each other via messages. For example, a Compartment object
  will receive axial current from its neighbouring compartments, and also
  channel current from HHChannel objects.
- Communicate with "external" objects (e.g.: other neurons) via messages. For
  example, sending/receiving synaptic events, receiving current injection in a
  compartment, etc.

This method of doing calculations is good because it is simple to implement,
and also provides a fallback method. However, it is very slow for the following
reasons:
- The EE method itself is slow. Sometimes even for simple models with a 2-3
  compartments and channels, a timestep of 1e-6 seconds does not give
  accurate results. On the other hand, with HSolve, a timestep of 50e-6 is
  usually enough even for the biggest models.
- Objects exchange information using messages. With a model of ~100 compartments
  and 2 channels per compartment, one can expect ~1000 messages being exchanged
  per time-step. This can seriously slow down calculations.
- Objects may be spread out in memory, which will lead to a lot of cache misses.
  A single cache miss leads to a penalty of ~200-400 processor cycles.

The Hines' solver, in addition to being a higher-order integration method, also
increases speed by doing all the calculations in one place, and storing all the
data in arrays. This eliminates messaging overheads, and improves data locality.

At the same time, one will like to retain the original objects-and-messages
representation of the model, so that the user can easily inspect and
manipulate it. In MOOSE, this is accomplished by replacing the original
objects with "zombie" objects, whenever a solver like the HSolve is created.
The clients of the original objects remain unaware of this switch, and to
them, the zombie objects look just like the originals. The zombie objects
have the same fields as the original objects, and the message connectivity
is also retained. The illusion is made complete by letting the zombie
objects forward any field queries and incoming messages to the HSolve. More
detail on zombie objects is in the "Zombies" section below.

\section ConceptualOverview Conceptual Overview

MOOSE allows you to keep your main numerical code very loosely coupled with
the rest of the MOOSE system. HSolve makes good use of this, and keeps the
numerical code as independent of MOOSE-specific concepts/classes as
possible. The points of interaction between HSolve and the rest of MOOSE are
neatly contained in a few classes/files.

Note: At present, a single HSolve object handles calculations for a single
neuron. Soon, HSolve will also handle calculations for arrays of identical
neurons.

Here is an overview of how things proceed chronologically in a simulation:

-# The user loads in a model, from, say a NeuroML file. The model is represented
   inside MOOSE as a bunch of objects, connected by messages. The objects are
   of type Compartment, HHChannel, etc. The connections between these
   objects capture the structure of the model. Each of the objects have fields
   (e.g.: "Vm" for a Compartment, "Gk" for an HHChannel). The user can use
   these fields to read/modify the parameters and state of the model.
-# The objects are capable of doing their own calculations at simulation time,
   using the Exponential Euler method. Usually, the user "schedules" all the
   objects constituting the model. This means hooking up the objects to a clock,
   which will invoke the objects at regular intervals to do their calculations.
   However, since we want HSolve to the calculations instead of the original
   objects, this scheduling step is not necessary.
-# The user connects this single-neuron model with other, external things. For
   example, a Table object may be connected to a Compartment object for the
   purpose of monitoring its Vm, later during the simulation. Other examples
   are:
   - a Table providing time-varying current-injection to a compartment.
   - synaptic connections between compartments belonging to different
     neurons.
-# The user creates an HSolve object.
-# The user "schedules" the HSolve object so that it can do its calculations.
-# The user sets the "dt" field of the HSolve object.
-# The user points the HSolve object to the model. This is done by setting
   the HSolve's "target" field to the location of model inside MOOSE.
   
   (Note: MOOSE, arranges objects in a tree, just like directories and files 
   are arranged in a tree by filesystems. Hence, the location of a model is 
   simply the "path" to an object which contains all of the model's objects).
   
   Setting the "target" field causes HSolve to do the following:
   -# Traverse the model, and build internal data structures based on the
      model's structure, parameters and state.
   -# "Deschedule" all the original objects, so that they are not longer
      invoked by the clock to do their calculations.
   -# Create "zombie" objects. More on this in the "Zombies" section below.
-# The user runs the simulation. As mentioned above, only the HSolve is invoked
   every time-step to do its calculations. Further, the rest of the system
   continues to interact with the individual zombified biophysical objects, not
   knowing that HSolve is doing all the thinking in the background.

Note that at present, the user is responsible for carrying out all the above
steps. In the future, a "solver manager" will be implemented which will take
over most of the above responsibilities from the user. The user will mainly
need to specify the choice of solver: EE, HSolve, or any other, if present.

\section Zombies

When an HSolve object is created, it takes over all the above functions from
the original objects. At the same time, each of the original objects is
replaced by a corresponding "zombie" object. For example, a Compartment
object is replaced with a ZombieCompartment object. The user (or the rest
of the system) continues to interact with the zombie objects, unaware of the
switch. The role of the zombies is to act as fully-functional stand-ins,
while letting the HSolve do all the thinking.  Hence, a Table object can
continue requesting for Vm from the compartment it was connected to, not
knowing that the compartment has now been replaced by a zombie. Simliarly,
another Table object can continue feeding current inject values to a
compartment, not knowing that they are being fed into HSolve. All of this is
accomplished in the following way:

- The original objects are disconnected from the scheduling system, so that
  they are no longer woken up for performing their calculations. Instead, the
  HSolve object is invoked once every time-step.
- When a field query is made to a zombie object, it calls set/get functions on
  the HSolve, rather than on itself.
- Similarly, when an incoming message arrives, a function on the HSolve is
  called to handle it.
- During a simulation, the HSolve sends out messages on behalf of the original
  objects, to any outside objects that are connect to objects belonging to the
  handled neuronal model.

For further details about zombies, see the \ref ProgrammersGuide.

\section code C++ code: classes and files

Now we look at the different C++ classes that make up HSolve, and at the 
role they play in the processes described above.

At setup time, most of the information flow is in the MOOSE --> HSolve
direction. Here, the HSolveUtils class is of particular interest.

At simulation time, most of the information flow is in the HSolve --> MOOSE
direction. Here, the HSolve class and the Zombie classes capture most of the
interactions.

The numerical implementation is contained in the 3 classes HSolveActive, 
HSolvePassive, and HinesMatrix.

Further details below:

-# HSolveUtils: This is a little library of convenience functions built on top
   of more basic MOOSE API calls. This library is meant for someone
   implementing a numerical scheme, and wishing to read in the model. A
   typical call looks like: "For a given compartment, give me all its
   neighbouring compartments", or, "For a given compartment, give me all the
   HHChannels that it has".
-# HSolve: The user and the rest of MOOSE interact with this class, and the
   Zombie classes. HSolve does the following:
   -# Inherits numerical code and data structures from the HSolveActive
      class.
   -# It provides an interface for looking up and modifying the parameters
      and state of the model. This is implemented as a host of set/get
      functions, written in HSolveInterface.cpp.
   -# Elevates its own status from regular C++ class to a MOOSE class. It does
      so by registering itself as a class with the MOOSE system. Here it also
      tells MOOSE that it has fields called "target" and "dt" (as mentioned
      earlier). It also specifies that it has a field called 'process', which
      allows it to be connected to a clock from the MOOSE scheduling system.
      All of this is done in HSolve::initCinfo(). 
   -# When the "target" field is set, it sets up its internal data structures
      using code inherited from HSolveActive. At this point, it also
      converts all the original objects into zombies.
   .
-# HSolveActive: At setup time, when the "target" field of HSolve is set,
   it triggers the HSolveActive::setup() function. This function is encoded in
   HSolveActiveSetup.cpp. It traverses the model using the HSolveUtils API,
   interrogates the model's structure, parameter and state, and sets up all the
   internal data-structures accordingly. At simulation time, HSolveActive
   does the full-fledged calculations for a neuronal model with ion channels,
   calcium, synapses, etc.The entry point for these calculations is
   HSolveActive::step().
-# HSolvePassive: This class does the compartmental calculations for passive
   neurons. Derives from HinesMatrix.
-# HinesMatrix: This class stores the HinesMatrix.
-# Zombie*: These are the zombie classes.

*/
