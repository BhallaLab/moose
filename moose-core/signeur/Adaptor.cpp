/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "Adaptor.h"

/**
 * This is the adaptor class. It is used in interfacing different kinds
 * of solver with each other, especially for electrical to chemical
 * signeur models.
 */
///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
static SrcFinfo1< double > *output()
{
	static SrcFinfo1< double > output( "output",
			"Sends the output value every timestep."
	);
	return &output;
}

/*
static SrcFinfo0 *requestInput()
{
	static SrcFinfo0 requestInput( "requestInput",
			"Sends out the request. Issued from the process call."
	);
	return &requestInput;
}
*/

static SrcFinfo1< vector< double >* >  *requestOut()
{
	static SrcFinfo1< vector< double >* > requestOut( "requestOut",
			"Sends out a request to a field with a double or array of doubles. "
			"Issued from the process call."
			"Works for any number of targets."
	);
	return &requestOut;
}

/*
static DestFinfo* handleInput() {
	static DestFinfo handleInput( "handleInput",
			"Handle the returned value, which is in a prepacked buffer.",
			new OpFunc1< Adaptor, PrepackedBuffer >( &Adaptor::handleBufInput )
	);
	return &handleInput;
}
*/

const Cinfo* Adaptor::initCinfo()
{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
	static ValueFinfo< Adaptor, double > inputOffset(
			"inputOffset",
			"Offset to apply to input message, before scaling",
			&Adaptor::setInputOffset,
			&Adaptor::getInputOffset
		);
	static ValueFinfo< Adaptor, double > outputOffset(
			"outputOffset",
			"Offset to apply at output, after scaling",
			&Adaptor::setOutputOffset,
			&Adaptor::getOutputOffset
		);
	static ValueFinfo< Adaptor, double > scale(
			"scale",
			"Scaling factor to apply to input",
			&Adaptor::setScale,
			&Adaptor::getScale
		);
	static ReadOnlyValueFinfo< Adaptor, double > outputValue(
			"outputValue",
			"This is the linearly transformed output.",
			&Adaptor::getOutput
		);

	///////////////////////////////////////////////////////
	// MsgDest definitions
	///////////////////////////////////////////////////////
	static DestFinfo input(
			"input",
			"Input message to the adaptor. If multiple inputs are "
			"received, the system averages the inputs.",
		   	new OpFunc1< Adaptor, double >( &Adaptor::input )
		);
	/*
		new DestFinfo( "setup",
			Ftype4< string, double, double, double >::global(),
			RFCAST( &Adaptor::setup ),
			"Sets up adaptor in placeholder mode."
			"This is done when the kinetic model is yet to be built, "
			"so the adaptor is given the src/target molecule name as "
			"a placeholder. Later the 'build' function will complete "
			"setting up the adaptor.\n"
			"Args: moleculeName, scale, inputOffset, outputOffset. "
			"Note that the direction of the adaptor operation is given "
			"by whether the channel/Ca is connected as input or output."
		),
		new DestFinfo( "build", Ftype0::global(),
			RFCAST( &Adaptor::build ),
			"Completes connection to previously specified molecule "
			"on kinetic model."
		),
		*/

	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
	static  DestFinfo process( "process",
				"Handles 'process' call",
			new ProcOpFunc< Adaptor>( &Adaptor::process )
	);
	static  DestFinfo reinit( "reinit",
				"Handles 'reinit' call",
			new ProcOpFunc< Adaptor>( &Adaptor::reinit )
	);

	static Finfo* processShared[] =
	{
			&process, &reinit
	};
	static SharedFinfo proc( "proc",
		"This is a shared message to receive Process message "
		"from the scheduler. ",
		processShared, sizeof( processShared ) / sizeof( Finfo* )
	);

	//////////////////////////////////////////////////////////////////////
	// Now set it all up.
	//////////////////////////////////////////////////////////////////////
	static Finfo* adaptorFinfos[] =
	{
		&inputOffset,				// Value
		&outputOffset,				// Value
		&scale,						// Value
		&outputValue,				// ReadOnlyValue
		&input,						// DestFinfo
		output(),					// SrcFinfo
		requestOut(),				// SrcFinfo
		&proc,						// SharedFinfo
	};

	static string doc[] =
	{
		"Name", "Adaptor",
		"Author", "Upinder S. Bhalla, 2008, NCBS",
		"Description",
		" This is the adaptor class. It is used in interfacing different kinds"
		" of solver with each other, especially for electrical to chemical"
		" signeur models."
		" The Adaptor class is the core of the API for interfacing between"
		" different solution engines. It is currently in use for interfacing"
		" between chemical and electrical simulations, but could be used for other"
		" cases such as mechanical models."
		""
		""
		" The API for interfacing between solution engines rests on "
		" the following capabilities of MOOSE."
		" 1. The object-oriented interface with classes mapped to biological"
		" and modeling concepts such as electrical and chemical compartments,"
		" ion channels and molecular pools."
		" 2. The invisible mapping of Solvers (Objects implementing numerical"
		" engines) to the object-oriented interface. Solvers work behind the "
		" scenes to update the objects."
		" 3. The messaging interface which allows any visible field to be "
		" accessed and updated from any other object. "
		" 4. The clock-based scheduler which drives operations of any subset of"
		" objects at any interval. For example, this permits the operations of"
		" field access and update to take place at quite different timescales "
		" from the numerical engines."
		" 5. The implementation of Adaptor classes. These perform a linear"
		" transformation::"
		""
		" 	(y = scale * (x + inputOffset) + outputOffset ) "
		""
		" where y is output and x is the input. The input is the average of"
		" any number of sources (through messages) and any number of timesteps."
		" The output goes to any number of targets, again through messages."
		""
		" It is worth adding that messages can transport arbitrary data structures,"
		" so it would also be possible to devise a complicated opaque message"
		" directly between solvers. The implementation of Adaptors working on"
		" visible fields does this much more transparently and gives the user "
		" facile control over the scaling transformatoin."
		""
		" These adaptors are used especially in the rdesigneur framework of MOOSE,"
		" which enables multiscale reaction-diffusion and electrical signaling"
		" models."
		" As an example of this API in operation, I consider two mappings: "
		" 1. Calcium mapped from electrical to chemical computations."
		" 2. phosphorylation state of a channel mapped to the channel conductance."
		""
		" 1. Calcium mapping."
		" Problem statement."
		" Calcium is computed in the electrical solver as one or more pools that"
		" are fed by calcium currents, and is removed by an exponential "
		" decay process. This calcium pool is non-diffusive in the current "
		" electrical solver. It has to be mapped to chemical calcium pools at a"
		" different spatial discretization, which do diffuse."
		" In terms of the list of capabilities described above, this is how the"
		" API works."
		" 	1. The electrical model is partitioned into a number of electrical"
		" 		compartments, some of which have the 'electrical' calcium pool"
		" 		as child object in a UNIX filesystem-like tree. Thus the "
		" 		'electrical' calcium is represented as an object with "
		" 		concentration, location and so on."
		" 	2. The Solver computes the time-course of evolution of the calcium"
		" 		concentration. Whenever any function queries the 'concentration'"
		" 		field of the calcium object, the Solver provides this value."
		"  3. Messaging couples the 'electrical' calcium pool concentration to"
		"  	the adaptor (see point 5). This can either be a 'push' operation,"
		"  	where the solver pushes out the calcium value at its internal"
		"  	update rate, or a 'pull' operation where the adaptor requests"
		"  	the calcium concentration."
		"  4. The clock-based scheduler keeps the electrical and chemical solvers"
		"  	ticking away, but it also can drive the operations of the adaptor."
		"  	Thus the rate of updates to and from the adaptor can be controlled."
		"  5. The adaptor averages its inputs. Say the electrical solver is"
		"  	going at a timestep of 50 usec, and the chemical solver at 5000 "
		"  	usec. The adaptor will take 100 samples of the electrical "
		"  	concentration, and average them to compute the 'input' to the"
		"  	linear scaling. Suppose that the electrical model has calcium units"
		"  	of micromolar, but has a zero baseline. The chemical model has"
		"  	units of millimolar and a baseline of 1e-4 millimolar. This gives:"
		"  	y = 0.001x + 1e-4"
		"  	At the end of this calculation, the adaptor will typically 'push'"
		"  	its output to the chemical solver. Here we have similar situation"
		"  	to item (1), where the chemical entities are calcium pools in"
		"  	space, each with their own calcium concentration."
		"  	The messaging (3) determines another aspect of the mapping here: "
		"  	the fan in or fan out. In this case, a single electrical "
		"  	compartment may house 10 chemical compartments. Then the output"
		"  	message from the adaptor goes to update the calcium pool "
		"  	concentration on the appropriate 10 objects representing calcium"
		"  	in each of the compartments."
		""
		" In much the same manner, the phosphorylation state can regulate"
		" channel properties."
		"  1. The chemical model contains spatially distributed chemical pools"
		"  	that represent the unphosphorylated state of the channel, which in"
		"  	this example is the conducting form. This concentration of this"
		"  	unphosphorylated state is affected by the various reaction-"
		"  	diffusion events handled by the chemical solver, below."
		"  2. The chemical solver updates the concentrations"
		"  	of the pool objects as per reaction-diffusion calculations."
		"  3. Messaging couples these concentration terms to the adaptor. In this"
		"  	case we have many chemical pool objects for every electrical"
		"  	compartment. There would be a single adaptor for each electrical"
		"  	compartment, and it would average all the input values for calcium"
		"  	concentration, one for each mesh point in the chemical calculation."
		"  	As before, the access to these fields could be through a 'push'"
		"  	or a 'pull' operation."
		"  4. The clock-based scheduler oeperates as above."
		"  5. The adaptor averages the spatially distributed inputs from calcium,"
		"  	and now does a different linear transform. In this case it converts"
		"  	chemical concentration into the channel conductance. As before,"
		"  	the 'electrical' channel is an object (point 1) with a field for "
		"  	conductance, and this term is mapped into the internal data "
		"  	structures of the solver (point 2) invisibly to the user."
		""
	};

	static Dinfo< Adaptor > dinfo;
	static Cinfo adaptorCinfo(
		"Adaptor",
		Neutral::initCinfo(),
		adaptorFinfos,
		sizeof( adaptorFinfos ) / sizeof( Finfo * ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &adaptorCinfo;
}

static const Cinfo* adaptorCinfo = Adaptor::initCinfo();

////////////////////////////////////////////////////////////////////
// Here we set up Adaptor class functions
////////////////////////////////////////////////////////////////////
Adaptor::Adaptor()
	:
		output_( 0.0 ),
		inputOffset_( 0.0 ),
		outputOffset_( 0.0 ),
		scale_( 1.0 ),
		molName_( "" ),
		sum_( 0.0 ),
		counter_( 0 ),
		numRequestOut_( 0 )
{
	;
}
////////////////////////////////////////////////////////////////////
// Here we set up Adaptor value fields
////////////////////////////////////////////////////////////////////

void Adaptor::setInputOffset( double value )
{
	inputOffset_ = value;
}
double Adaptor::getInputOffset() const
{
	return inputOffset_;
}

void Adaptor::setOutputOffset( double value )
{
	outputOffset_ = value;
}
double Adaptor::getOutputOffset() const
{
	return outputOffset_;
}

void Adaptor::setScale( double value )
{
	scale_ = value;
}
double Adaptor::getScale() const
{
	return scale_;
}

double Adaptor::getOutput() const
{
	return output_;
}


////////////////////////////////////////////////////////////////////
// Here we set up Adaptor Destination functions
////////////////////////////////////////////////////////////////////

void Adaptor::input( double v )
{
	sum_ += v;
	++counter_;
}

// separated out to help with unit tests.
void Adaptor::innerProcess()
{
	if ( counter_ == 0 ) {
		output_ = outputOffset_;
	} else {
		output_ = outputOffset_ +
			scale_ * ( ( sum_ / counter_ ) - inputOffset_ );
	}
	sum_ = 0.0;
	counter_ = 0;
}

void Adaptor::process( const Eref& e, ProcPtr p )
{
	// static FuncId fid = handleInput()->getFid();
	if ( numRequestOut_ > 0 ) {
		vector< double > ret;
		requestOut()->send( e, &ret );
		assert( ret.size() == numRequestOut_ );
		for ( unsigned int i = 0; i < numRequestOut_; ++i ) {
			sum_ += ret[i];
		}
		counter_ += numRequestOut_;
	}
	innerProcess();
	output()->send( e, output_ );
}

void Adaptor::reinit( const Eref& e, ProcPtr p )
{
	numRequestOut_ = e.element()->getMsgTargets( e.dataIndex(),
					requestOut() ).size();
	process( e, p );
}

