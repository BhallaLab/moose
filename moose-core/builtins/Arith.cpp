/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "../basecode/header.h"
#include "Arith.h"

static SrcFinfo1< double > *output() {
	static SrcFinfo1< double > output(
			"output",
			"Sends out the computed value"
			);
	return &output;
}

const Cinfo* Arith::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< Arith, string > function(
			"function",
			"Arithmetic function to perform on inputs.",
			&Arith::setFunction,
			&Arith::getFunction
		);
		static ValueFinfo< Arith, double > outputValue(
			"outputValue",
			"Value of output as computed last timestep.",
			&Arith::setOutput,
			&Arith::getOutput
		);
		static ReadOnlyValueFinfo< Arith, double > arg1Value(
			"arg1Value",
			"Value of arg1 as computed last timestep.",
			&Arith::getArg1
		);

		static LookupValueFinfo< Arith, unsigned int, double > anyValue(
			"anyValue",
			"Value of any of the internal fields, output, arg1, arg2, arg3,"
			"as specified by the index argument from 0 to 3.",
			&Arith::setIdentifiedArg,
			&Arith::getIdentifiedArg
		);
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo arg1( "arg1",
			"Handles argument 1. This just assigns it",
			new OpFunc1< Arith, double >( &Arith::arg1 ) );

		static DestFinfo arg2( "arg2",
			"Handles argument 2. This just assigns it",
			new OpFunc1< Arith, double >( &Arith::arg2 ) );

		static DestFinfo arg3( "arg3",
			"Handles argument 3. This sums in each input, and clears each clock tick.",
			new OpFunc1< Arith, double >( &Arith::arg3 ) );

		static DestFinfo arg1x2( "arg1x2",
			"Store the product of the two arguments in output_",
			new OpFunc2< Arith, double, double >( &Arith::arg1x2 ) );

		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< Arith >( &Arith::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< Arith >( &Arith::reinit ) );

		//////////////////////////////////////////////////////////////
		// SharedFinfo Definitions
		//////////////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* arithFinfos[] = {
		&function,	// Value
		&outputValue,	// Value
		&arg1Value,	// ReadOnly value
		&anyValue,	// LookupValue
		&arg1,		// DestFinfo
		&arg2,		// DestFinfo
		&arg3,		// DestFinfo
		&arg1x2,	// DestFinfo
		output(), 	// SrcFinfo
		&proc		// SharedFinfo
	};

	static Dinfo< Arith > dinfo;
	static Cinfo arithCinfo (
		"Arith",
		Neutral::initCinfo(),
		arithFinfos,
		sizeof( arithFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &arithCinfo;
}

static const Cinfo* arithCinfo = Arith::initCinfo();

Arith::Arith()
	: function_( "sum" ),
	output_( 0.0 ),
	arg1_( 0.0 ), arg2_( 0.0 ), arg3_( 0.0 )
{
	;
}

static bool doReport = 0;
void Arith::process( const Eref& e, ProcPtr p )
{
	output_ = arg1_ + arg2_ + arg3_; // Doing a hard-coded function.
	// cout << "process: " << e.element()->getName() << ", " << e.objId() << arg3_ << ", " << &arg3_ << endl;
	if ( doReport ) {
		cout <<
			e.element()->getName() << ", " << e.objId() << "		" <<
			arg3_ << "	" << &arg3_ << endl;
	}
	output()->send( e, output_ );
	arg3_ = 0.0;
}

void Arith::reinit( const Eref& e, ProcPtr p )
{
	// cout << "reinit: " << e.element()->getName() << ", " << e.objId() << arg3_ << endl;
	arg1_ = 0.0;
	arg2_ = 0.0;
	arg3_ = 0.0;
	output_ = 0.0;
}

void Arith::arg1( const double arg )
{
	arg1_ = arg;
}

void Arith::arg2( const double arg )
{
	arg2_ = arg;
}

void Arith::arg3( const double arg )
{
	if ( doReport )
		cout << arg << "	" << &arg3_ << endl;
	arg3_ += arg;
}

void Arith::arg1x2( double a1, double a2 )
{
	output_ = a1 * a2;
}


void Arith::setFunction( const string v )
{
	function_ = v;
}

string Arith::getFunction() const
{
	return function_;
}

void Arith::setOutput( double v )
{
	output_ = v;
}

double Arith::getOutput() const
{
	return output_;
}

double Arith::getArg1() const
{
	return arg1_;
}

double Arith::getIdentifiedArg( unsigned int i ) const
{
	if ( i == 0 )
		return output_;
	if ( i == 1 )
		return arg1_;
	if ( i == 2 )
		return arg2_;
	if ( i == 3 )
		return arg3_;
	return 0;
}

void Arith::setIdentifiedArg( unsigned int i, double val )
{
	if ( i == 0 )
		output_ = val;
	if ( i == 1 )
		arg1_ = val;
	if ( i == 2 )
		arg2_ = val;
	if ( i == 3 )
		arg3_ = val;
}
