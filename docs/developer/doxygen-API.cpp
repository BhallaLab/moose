/**
\page AppProgInterface Applications Programming Interface, API. Async13 branch.

\section DataStructs Key Data structures
\subsection DataStructOverview	Overview
MOOSE represents all simulation concepts through objects. The API specifies
how to manipulate these objects. Specifically, it deals with their
creation, destruction, field access, computation, and exchange of data
through messages.

Objects in MOOSE are always wrapped in the Element container class.
Each Element holds an array of Objects, sized from zero to a very large
number limited only by machine memory.

The functions and fields of each class in MOOSE are defined in Finfos:
Field Info classes. These are visible to users as fields.

Data communication between Elements (that is, their constitutent Objects)
is managed by the Msg class: messages.

These three concepts: Elements, Finfos, and Msgs - are manipulated by
the API.

\subsection DataStructElementAccess	Id: Handles for Elements

Each Element is uniquely identified by an \b Id. Ids are consistent
across all nodes and threads of a multiprocessor simulation. Ids are
basically indices to a master array of all Elements. Ids are used by
the Python system too.

\subsection DataStructElementClasses	Element classes: Handling Objects within elements.
The \b Element is a virtual base class that manages objects.
It deals with creation, resizing, lookup and
destruction of the data. It handles load balancing. It manages fields. 
It manages messages.

\subsection DataStructObjectClasses	Object classes: Computational and data entities in MOOSE.
\b Objects in MOOSE do the actual work of computation and data structures. They
are insulated from the housekeeping jobs of creation, interfacing to scripts
and to messaging. To do this they present a very stereotyped interface to
the MOOSE Element wrapper. The following are the essential components of this
interface. These are discussed in more detail in the document 
"Building New MOOSE Classes."
\subsubsection ObjectsInMooseConstructor	Object Constructors
All MOOSE classes need a constructor \b Object() that correctly initializes 
all fields. This constructor does not take any arguments. It can be omitted
only if the default C++ constructor will guarantee initialization.
\subsubsection ObjectsInMooseAssignment		Object assignment operator
MOOSE needs to know how to copy objects. By default it does a bit-copy.
If this is not what you need, then you must explicitly specify an assignment
operator. For example, if you set up pointers and do not want your objects
to share the data in the pointers, you will want to specify an assignment 
operator to rebuild the contents of the pointers.
\subsubsection ObjectsInMooseFinfo	Object fields.
MOOSE needs to know what fields an object has. Fields can be of three main
kinds: value fields, message source fields, and message destination 
(aka function) fields. All these fields are managed by \b Finfo objects 
(Field infos), which are in turn organized by the Cinfo (Class Info) objects
as described below. In a nutshell, all fields are associated with a name,
access functions, and some documentation by creating Finfos for them, and
all the Finfos are stored in the Cinfo.
\subsubsection ObjectsInMooseCinfo	Object class information.
Every MOOSE class is managed by a \b Cinfo (Class Info) object. This is defined
in a static initializer function in every class. The Cinfo stores 
the class name and documentation, how to look up fields, how to 
handle data, and so on. 
\subsubsection ObjectsInMooseMsgs	Object message sending.
Any MOOSE object can call any function in any other object. This is managed
by the message source fields: \b SrcFinfos.  SrcFinfos defined as above all
present a \b send() function, which traverses all targets of the message and
calls the target function with the specified arguments. SrcFinfos are typed, so
precisely the correct number and type of arguments are always sent. Messages
can go across nodes, the user does not need to do anything special to
arrange this.

\subsection DataStructObjectAccess	ObjId: Identifiers for Objects within elements.

The \b ObjId specifies a specific object within the Element. All Elements
manage a linear array of identical objects, which can have any number of 
entries greater than zero, up to the limits of memory. The ObjId::dataIndex
field is the index into this array.
In addition, the ObjId has a field ObjId::fieldIndex that comes into use in a 
subset of objects. This is used when each object has to manage arrays of 
fields, which are made visible as FieldElements. For example, one could have 
an array of receptor channels, each of which manages an array of synapses. 
Thus to fully specify a synapse, one uses both the ObjId::dataIndex to
specify the parent receptor, and the ObjId::fieldIndex to specify the synapse
on that receptor.

\subsection DataStructObjId	ObjId: Fully specified handle for objects.

The ObjId is a composite of Id and DataId. It uniquely specifies any
entity in the simulation. It is consistent across nodes. 
In general, one would use the ObjId for most Object manipulation,
field access, and messaging API calls.
The ObjId can be initialized using a string path of an object. 
The string path of an object can be looked up from its ObjId.

\subsection DataStructTrees	Element hierarchies and path specifiers.
Elements are organized into a tree hierarchy, much like a Unix file
system. This is similar to the organization in GENESIS. Since every
Element has a name, it is possible to traverse the hierarchy by specifying
a path. For example, you might access a specific dendrite on cell 72 as 
follows: 

\verbatim
/network/cell[72]/dendrite[50]
\endverbatim

Note that this path specifier maps onto a single ObjId.
Every object can be indexed, and if no index is given then it assumed
that it refers to index zero. For example, the above path is identical
to: 

\verbatim
/network[0]/cell[72]/dendrite[50]
\endverbatim

Path specifiers can be arbitrarily nested. Additionally, one can have
single dimensional arrays at any level of nesting. Here is an example 
path with nested arrays:

\verbatim
/network/layerIV/cell[23]/dendrite[50]/synchan/synapse[1234]
\endverbatim

\subsection ObjIdAndPaths	ObjIds, paths, and dimensions.
Objects sit on the Elements, which follow a tree hierarchy. There are
two ways to find an object. 
First, the ObjId completely identifies an object no matter where it is in 
the object tree. 
Second, one can traverse the Element tree using indices to identify 
specific Objects. This too uniquely identifies each Object.
Every ObjId has a 'parent' ObjId, the exception being the root ObjId
which is its own parent.
Any ObjId can have its own 'child' objects in the tree.
The tree cannot loop back onto itself.
Objects are always stored as linear arrays. 

\verbatim
/foo[0]/bar
\endverbatim
is a different object from 
\verbatim
/foo[1]/bar
\endverbatim

Some useful API calls for dealing with the path:

ObjId::ObjId( const string& path ): Creates the ObjId pointing to an
	already created object on the specified path.

string ObjId::path(): Returns the path string for the specified ObjId.

\verbatim
ObjId f2( "/f1[2]/f2" );
assert( f2.path() == "/f1[2]/f2[0]" );
\endverbatim

There is a special meaning for the path for synapses. Recall that the 
ObjId for synapses (which are FieldElements of SynChans) has two
indices, the DataIndex and the FieldIndex. The DataIndex of the
synapse is identical to that of its parent SynChan.
This is illustrated as follows:

\verbatim
ObjId synchan( "/cell/synchan[20] );
assert( synchan.dataIndex == 20 );

ObjId synapse( "/cell/synchan[20]/synapse[5]" );
assert( synapse.dataIndex == 20 );
assert( synapse.fieldIndex == 5 );
\endverbatim

\subsection Wildcard_paths	Wildcard paths
Some commands take a \e wildcard path. This compactly specifies a large
number of ObjIds. Some example wildcards are

\verbatim
/network/##		// All possible children of network, followed recursively
/network/#		// All children of network, only one level.
/network/ce#	// All children of network whose name starts with 'ce'
/network/cell/dendrite[]	// All dendrites, regardless of index
/network/##[ISA=CaConc] 	// All descendants of network of class CaConc
/soma,/axon		// The elements soma and axon
\endverbatim


\section FieldAccess Setting and Getting Field values.
\subsection FieldAccessOverview Overview
There is a family of classes for setting and getting Field values.
These are the 
\li SetGet< A1, A2... >::set( ObjId id, const string& name, arg1, arg2... )
and
\li SetGet< A >::get( ObjId id, const string& name )
functions. Here A1, A2 are the templated classes of function arguments.
A is the return class from the \e get call.

Since Fields are synonymous with functions of MOOSE objects, 
the \e set family of commands is also used for calling object functions.
Note that the \e set functions do not have a return value.

The reason there has to be a family of classes is that all functions in
MOOSE are strongly typed. Thus there are SetGet classes for up to six
arguments.


\subsection FieldAccessExamples Examples of field access.
1. If you want to call a function foo( int A, double B ) on
ObjId oid, you would do:

\verbatim
                SetGet2< int, double >::set( oid, "foo", A, B );
\endverbatim

2. To call a function bar( int A, double B, string C ) on oid:
\verbatim
                SetGet3< int, double, string >::set( oid, "bar", A, B, C );
\endverbatim

3. To assign a field value  "short abc" on object oid:
\verbatim
                Field< short >::set( oid, "abc", 123 );
\endverbatim

4. To get a field value "double pqr" on object oid:
\verbatim
                double x = Field< short >::get( oid, "pqr" );
\endverbatim

5. To assign the double 'xcoord' field on all the objects on
element Id id, which has an array of the objects:
\verbatim
                vector< double > newXcoord;
                // Fill up the vector here.
                Field< double >::setVec( id, "xcoord", newXcoord );
\endverbatim
                Note that the dimensions of newXcoord should match those of
                the target element.

                You can also use a similar call if it is just a function on id:
\verbatim
                SetGet1< double >::setVec( id, "xcoord_func", newXcoord );
\endverbatim

6. To extract the double vector 'ycoord' field from all the objects on id:
\verbatim
                vector< double > oldYcoord; // Do not need to allocate.
                Field< double >::getVec( id, "ycoord", oldYcoord );
\endverbatim

7. To set/get LookupFields, that is fields which have an index to lookup:
\verbatim
                double x = LookupField< unsigned int, double >::get( objId, field, index );
                LookupField< unsigned int, double >::set( objId, field, index, value );
\endverbatim

\section APIcalls API system calls
\subsection FieldAccessOverview Overview
There is a special set of calls on the Shell object, which function as the
main MOOSE programmatic API. These calls are all prefixed with 'do'. Here is
the list of functions:

\li Id doCreate(  string type, Id parent, string name, vector< unsigned int > dimensions );
\li bool doDelete( Id id )
\li MsgId doAddMsg( const string& msgType, ObjId src, const string& srcField, ObjId dest, const string& destField);
\li void doQuit();
\li void doStart( double runtime );
\li void doReinit();
\li void doStop();
\li void doMove( Id orig, Id newParent );
\li Id doCopyId orig, Id newParent, string newName, unsigned int n, bool copyExtMsgs);
\li Id doFind( const string& path ) const
\li void doSetClock( unsigned int tickNum, double dt )
\li void doUseClock( string path, string field, unsigned int tick );
\li Id doLoadModel( const string& fname, const string& modelpath );



\section ClockScheduling Clocks, Ticks, and Scheduling
\subsection ClockOverview	Overview
Most of the computation in MOOSE occurs in a special function called 
\e process,
which is implemented in all object classes that advance their internal
state over time. The role of Clocks and Ticks is to set up the sequence of
calling \e process for different objects, which may have different intervals
for updating their internal state. The design of scheduling in moose is
similar to GENESIS.

As a simple example, suppose we had six objects, which had to advance their
internal state with the following intervals:
\li \b A: 5
\li \b B: 2
\li \b C: 2
\li \b D: 1
\li \b E: 3
\li \b F: 5

Suppose we had to run this for 10 seconds. The desired order of updates 
would be:

\verbatim
Time	Objects called
1	D
2	D,B,C
3	D,E
4	D,B,C
5	D,A,F
6	D,B,C,E
7	D
8	D,B,C
9	D,E
10	D,B,C,A,F
\endverbatim

\subsection ClockReinit	Reinit: Reinitializing state variables.
In addition to advancing the simulation, the Clocks and Ticks play a closely
related role in setting initial conditions. It is required that every object
that has a \e process call, must have a matching \e reinit function. When the
command \e doReinit is given from the shell, the simulation is reinitialized
to its boundary conditions. To do so, the \e reinit function is called in the 
same sequence that the \process would have been called at time 0 (zero).
For the example above, this sequence would be:\n
D,B,C,E,A,F

In other words, the ordering is first by dt for the object, and second by 
the sequence of the object in the list.

During reinit, the object is expected to restore all state variables to their
boundary condition. Objects typically also send out messages during reinit
to specify this boundary condition value to dependent objects. For example,
a compartment would be expected to send its initial \e Vm value out to a
graph object to indicate its starting value.

\subsection ClockSetup	Setting up scheduling
The API for setting up scheduling is as follows:\n
1. Create the objects to be scheduled.\n
2. Create Clock Ticks for each time interval using

\verbatim
	doSetClock( TickNumber, dt ).
\endverbatim

In many cases it is necessary to have a precise sequence of events
ocurring at the same time interval. In this case, set up two or more
Clock Ticks with the same dt but successive TickNumbers. They will
execute in the same order as their TickNumber. \n
Note that TickNumbers are unique. If you reuse a TickNumber, all that
will happen is that its previous value of dt will be overridden.

Note also that dt can be any positive decimal number, and does not 
have to be a multiple of any other dt.

3. Connect up the scheduled objects to their clock ticks:

\verbatim
	doUseClock( path, function, TickNumber )
\endverbatim

Here the \e path is a wildcard path that can specify any numer of objects.\n
The \e function is the name of the \e process message that is to be used. This
is provided because some objects may have multiple \e process messages.
The \e TickNumber identifies which tick to use.

Note that as soon as the \e doUseClock function is issued, both the 
\e process and \e reinit functions are managed by the scheduler as discussed
above.

\subsection ClockSchedExample	Example of scheduling.
As an example, here we set up the scheduling for the same 
set of objects A to F we have discussed above.\n
First we set up the clocks:

\verbatim
	doSetClock( 0, 1 );
	doSetClock( 1, 2 );
	doSetClock( 2, 3 );
	doSetClock( 3, 5 );
\endverbatim

Now we connect up the relevant objects to them.

\verbatim
	doUseClock( "D", "process", 0 );
	doUseClock( "B,C", "process", 1 );
	doUseClock( "E", "process", 2 );
	doUseClock( "A,F", "process", 3 );
\endverbatim

Next we initialize them:

\verbatim
	doReinit();
\endverbatim

During the \e doReinit call, the \e reinit function of the objects would be 
called in the following sequence:
\verbatim
	D, B, C, E, A, F
\endverbatim

Finally, we run the calculation for 10 seconds:

\verbatim
	doStart( 10 );
\endverbatim

*/
