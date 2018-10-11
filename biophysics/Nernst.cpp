/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "Nernst.h"

const double Nernst::R_OVER_F = 8.6171458e-5;
const double Nernst::ZERO_CELSIUS = 273.15;

	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
static SrcFinfo1< double > *Eout() {
	static SrcFinfo1< double > Eout( "Eout",
			"Computed reversal potential"
			);
	return &Eout;
}

const Cinfo* Nernst::initCinfo()
{
	static ReadOnlyValueFinfo< Nernst, double > E( "E",
		"Computed reversal potential",
			&Nernst::getE
	);
	static ValueFinfo< Nernst, double > temperature( "Temperature",
		"Temperature of cell",
		&Nernst::setTemperature,
		&Nernst::getTemperature
	);
	static ValueFinfo< Nernst, int > valence( "valence",
		"Valence of ion in Nernst calculation",
		&Nernst::setValence,
		&Nernst::getValence
	);
	static ValueFinfo< Nernst, double > Cin( "Cin",
		"Internal conc of ion",
		&Nernst::setCin,
		&Nernst::getCin
	);
	static ValueFinfo< Nernst, double > Cout( "Cout",
		"External conc of ion",
		&Nernst::setCout,
		&Nernst::getCout
	);
	static ValueFinfo< Nernst, double > scale( "scale",
		"Voltage scale factor",
		&Nernst::setScale,
		&Nernst::getScale
	);
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////


	///////////////////////////////////////////////////////
	// MsgDest definitions
	// These differ from field assignments because they trigger an
	// outgoing msg with the updated E.
	///////////////////////////////////////////////////////
	static DestFinfo ci( "ci",
		"Set internal conc of ion, and immediately send out the updated E",
		new EpFunc1< Nernst, double >( &Nernst::handleCin )
	);
	static DestFinfo co( "co",
		"Set external conc of ion, and immediately send out the updated E",
		new EpFunc1< Nernst, double >( &Nernst::handleCout )
	);

	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
	static Finfo* NernstFinfos[] =
	{
		Eout(),		// SrcFinfo
		&E,			// ReadOnlyValue
		&temperature,	// Value
		&valence,		// Value
		&Cin,		// Value
		&Cout,		// Value
		&scale,		// Value
		&ci,		// Dest
		&co,		// Dest
	};

	static string doc[] =
	{
		"Name", "Nernst",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "Nernst: Calculates Nernst potential for a given ion based on "
				"Cin and Cout, the inside and outside concentrations. "
				"Immediately sends out the potential to all targets.",
	};

	static Dinfo< Nernst > dinfo;
	static const Cinfo NernstCinfo(
		"Nernst",
		Neutral::initCinfo(),
		NernstFinfos,
		sizeof( NernstFinfos ) / sizeof(Finfo *),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &NernstCinfo;
}

///////////////////////////////////////////////////
static const Cinfo* nernstCinfo = Nernst::initCinfo();

Nernst::Nernst()
	:
		E_( 0.0 ),
		Temperature_( 295 ),
		valence_( 1 ),
		Cin_( 1.0 ),
		Cout_( 1.0 ),
		scale_( 1.0 ),
		factor_( scale_ * R_OVER_F * Temperature_ / valence_ )
{;}
///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

double Nernst::getE() const
{
	return E_;
}

void Nernst::setTemperature( double value ) {
	if ( value > 0.0 ) {
		Temperature_ = value;
		factor_ = scale_ * R_OVER_F * Temperature_ / valence_;
	}
	updateE();
}

double Nernst::getTemperature() const
{
	return Temperature_;
}

void Nernst::setValence( int value ) {
	if ( value != 0 ) {
		valence_ = value;
	}
	factor_ = scale_ * R_OVER_F * Temperature_ / valence_;
	updateE();
}

int Nernst::getValence() const
{
	return valence_;
}

void Nernst::setCin( double value )
{
	Cin_ = value;
	updateE();
}
double Nernst::getCin() const
{
	return Cin_;
}

void Nernst::setCout( double value ) {
	Cout_ = value;
	updateE();
}
double Nernst::getCout() const
{
	return Cout_;
}

void Nernst::setScale( double value ) {
	scale_ = value;
	factor_ = scale_ * R_OVER_F * Temperature_ / valence_;
	updateE();
}
double Nernst::getScale() const
{
	return scale_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void Nernst::updateE( )
{
	E_ = factor_ * log( Cout_ / Cin_ );
}

void Nernst::handleCin( const Eref& er, double conc )
{
	Cin_ = conc;
	updateE();
	Eout()->send( er, E_ );
}

void Nernst::handleCout( const Eref& er, double conc )
{
	Cout_ = conc;
	updateE();
	Eout()->send( er, E_ );
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS
void testNernst()
{
	Nernst ne;
	ne.setValence( 1 );
	ne.setCin( 0.01 );
	ne.setCout( 1.0 );
	double E = ne.getE();
	assert( doubleApprox( E, 0.0585 * 2.0 ) );

	cout << "." << flush;
}
#endif
