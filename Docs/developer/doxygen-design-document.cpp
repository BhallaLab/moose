/**
\page DesignDocument Design Document

\section DD_Goals Goals
- Cleanup.
- Handle multithreading and MPI from the ground up.

\section DD_WhyDoThis Why do this?
It is a huge amount of work to refactor a large existing code base. This is
needed here, and was in fact anticipated, for two reasons:
- We needed the experience of building a fairly complete, functioning system
	to know what the underlying API must do.
- The original parallel stuff was a hack.

This redesign does a lot of things differently from the earlier MOOSE messaging.
- Introduces a buffer-based data transfer mechanism, with a fill/empty
	cycle to replace the earlier function-call mechanism. The fill/empty
	cycle is needed for multithreading and also works better with multinode
	data traffic.
- All Elements are now assumed to be array Elements, so indexing is built into
	messaging from the ground up.
- Field access function specification is cleaner.
- Separation of function specification from messaging. This means that any
	message can be used as a communication line for any function call 
	between two Elements. This gives an enormous simplification to message
	design. However, it entails:
- Runtime type-checking of message data, hopefully in a very efficient way.
	As message setup is itself runtime, and arbitrary functions can sit
	on the message channels, it turns out to be very hard to
	do complete checking at compile or setup time.
- Wildcard info merged into messages.
- Three-tier message manipulation hierarchy, for better introspection and
	relating to higher-level setup calls. These are
	Msg: Lowest level, manages info between two Elements e1 and e2. 
		Deals with the index-level connectivity for the Element arrays.
	Conn: Mid level. Manages a set of Msgs that together make a messaging
		unit that takes a function call/data from source to a set of
		destination Elements and their array entries. Has introspection
		info. Is a MOOSE field.
	Map: High level. Manages a set of Conns that together handle a
		conceptual group. Equivalent to an anatomical projection from
		one set of neurons to another in the brain. Equivalent to what
		the 'createmap' function would generate. Has introspection.
		Is a MOOSE Element.

- Field access and function calls now go through the messaging interface. Not
	necessarily the fastest way to do it, but simplifies life in a 
	multinode/multithreaded system, and reduces the complexity of the
	overall interface.
*/
