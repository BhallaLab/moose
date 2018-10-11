/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../builtins/Interpol2D.h"
#include "HHGate2D.h"

static const double SINGULARITY = 1.0e-6;

const Cinfo* HHGate2D::initCinfo()
{
	///////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////
		static ReadOnlyLookupValueFinfo< HHGate2D, vector< double >, double >
			A( "A",
			"lookupA: Look up the A gate value from two doubles, passed"
			"in as a vector. Uses linear interpolation in the 2D table"
			 "The range of the lookup doubles is predefined based on "
			 "knowledge of voltage or conc ranges, and the granularity "
			 "is specified by the xmin, xmax, and dx field, and their "
			 "y-axis counterparts.",
			&HHGate2D::lookupA );
		static ReadOnlyLookupValueFinfo< HHGate2D, vector< double >, double >
			B( "B",
			"lookupB: Look up B gate value from two doubles in a vector.",
			&HHGate2D::lookupB );

		static ElementValueFinfo< HHGate2D, vector< vector< double > > > tableA(
			"tableA",
			"Table of A entries",
                        &HHGate2D::setTableA,
			&HHGate2D::getTableA
		);

		static ElementValueFinfo< HHGate2D, vector< vector< double > > > tableB(
			"tableB",
			"Table of B entries",
			&HHGate2D::setTableB,
			&HHGate2D::getTableB);

		static ElementValueFinfo< HHGate2D, double > xminA( "xminA",
			"Minimum range for lookup",
			&HHGate2D::setXminA,
			&HHGate2D::getXminA
		);

		static ElementValueFinfo< HHGate2D, double > xmaxA( "xmaxA",
			"Minimum range for lookup",
			&HHGate2D::setXmaxA,
			&HHGate2D::getXmaxA
		);

		static ElementValueFinfo< HHGate2D, unsigned int > xdivsA( "xdivsA",
			"Divisions for lookup. Zero means to use linear interpolation",
			&HHGate2D::setXdivsA,
			&HHGate2D::getXdivsA);

		static ElementValueFinfo< HHGate2D, double > yminA( "yminA",
			"Minimum range for lookup",
			&HHGate2D::setYminA,
			&HHGate2D::getYminA);

		static ElementValueFinfo< HHGate2D, double > ymaxA( "ymaxA",
			"Minimum range for lookup",
			&HHGate2D::setYmaxA,
			&HHGate2D::getYmaxA);

		static ElementValueFinfo< HHGate2D, unsigned int > ydivsA( "ydivsA",
			"Divisions for lookup. Zero means to use linear interpolation",
			&HHGate2D::setYdivsA,
			&HHGate2D::getYdivsA);

		static ElementValueFinfo< HHGate2D, double > xminB( "xminB",
			"Minimum range for lookup",
			&HHGate2D::setXminB,
			&HHGate2D::getXminB
		);

		static ElementValueFinfo< HHGate2D, double > xmaxB( "xmaxB",
			"Minimum range for lookup",
			&HHGate2D::setXmaxB,
			&HHGate2D::getXmaxB
		);

		static ElementValueFinfo< HHGate2D, unsigned int > xdivsB( "xdivsB",
			"Divisions for lookup. Zero means to use linear interpolation",
			&HHGate2D::setXdivsB,
			&HHGate2D::getXdivsB
		);

		static ElementValueFinfo< HHGate2D, double > yminB( "yminB",
			"Minimum range for lookup",
			&HHGate2D::setYminB,
			&HHGate2D::getYminB);

		static ElementValueFinfo< HHGate2D, double > ymaxB( "ymaxB",
			"Minimum range for lookup",
			&HHGate2D::setYmaxB,
			&HHGate2D::getYmaxB);

		static ElementValueFinfo< HHGate2D, unsigned int > ydivsB( "ydivsB",
			"Divisions for lookup. Zero means to use linear interpolation",
			&HHGate2D::setYdivsB,
                        &HHGate2D::getYdivsB);

        ///////////////////////////////////////////////////////
	// DestFinfos
	///////////////////////////////////////////////////////
	static Finfo* HHGate2DFinfos[] =
	{
		&A,			// ReadOnlyLookupValue
		&B,			// ReadOnlyLookupValue
		&tableA,	// ElementValue
		&tableB,	// ElementValue
                &xminA,
                &xmaxA,
                &xdivsA,
                &yminA,
                &ymaxA,
                &ydivsA,
                &xminB,
                &xmaxB,
                &xdivsB,
                &yminB,
                &ymaxB,
                &ydivsB
        };

	static string doc[] =
	{
		"Name", "HHGate2D",
		"Author", "Niraj Dudani, 2009, NCBS. Updated by Subhasis Ray, 2014, NCBS.",
		"Description", "HHGate2D: Gate for Hodkgin-Huxley type channels, equivalent to the "
		"m and h terms on the Na squid channel and the n term on K. "
		"This takes the voltage and state variable from the channel, "
		"computes the new value of the state variable and a scaling, "
		"depending on gate power, for the conductance. These two "
		"terms are sent right back in a message to the channel.",
	};

        static Dinfo< HHGate2D > dinfo;
	static Cinfo HHGate2DCinfo(
		"HHGate2D",
		Neutral::initCinfo(),
		HHGate2DFinfos, sizeof(HHGate2DFinfos)/sizeof(Finfo *),
                &dinfo,
                doc,
                sizeof(doc) / sizeof(string)
	);

	return &HHGate2DCinfo;
}

static const Cinfo* hhGate2DCinfo = HHGate2D::initCinfo();
///////////////////////////////////////////////////
HHGate2D::HHGate2D()
	: originalChanId_(0),
          originalGateId_(0)
{;}

HHGate2D::HHGate2D( Id originalChanId, Id originalGateId )
	:
		originalChanId_( originalChanId ),
		originalGateId_( originalGateId )
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
double HHGate2D::lookupA( vector< double > v ) const
{
	if ( v.size() < 2 ) {
		cerr << "Error: HHGate2D::getAValue: 2 real numbers needed to lookup 2D table.\n";
		return 0.0;
	}

	if ( v.size() > 2 ) {
		cerr << "Error: HHGate2D::getAValue: Only 2 real numbers needed to lookup 2D table. "
			"Using only first 2.\n";
	}

	return A_.innerLookup( v[ 0 ], v[ 1 ] );
}

double HHGate2D::lookupB( vector< double > v ) const
{
	if ( v.size() < 2 ) {
		cerr << "Error: HHGate2D::getAValue: 2 real numbers needed to lookup 2D table.\n";
		return 0.0;
	}

	if ( v.size() > 2 ) {
		cerr << "Error: HHGate2D::getAValue: Only 2 real numbers needed to lookup 2D table. "
			"Using only first 2.\n";
	}

	return B_.innerLookup( v[ 0 ], v[ 1 ] );
}

void HHGate2D::lookupBoth( double v, double c, double* A, double* B ) const
{
	*A = A_.innerLookup( v, c );
	*B = B_.innerLookup( v, c );
}


///////////////////////////////////////////////////
// Access functions for Interpols
///////////////////////////////////////////////////

vector< vector< double > > HHGate2D::getTableA( const Eref& e ) const
{
    return A_.getTableVector();
}

void HHGate2D::setTableA(const Eref& e, vector< vector< double > > value )
{
    A_.setTableVector(value);
}

vector< vector< double > > HHGate2D::getTableB(const Eref& e) const
{
    return B_.getTableVector();
}

void HHGate2D::setTableB(const Eref& e, vector< vector< double > > value )
{
    B_.setTableVector(value);
}

///////////////////////////////////////////////////
// Functions to check if this is original or copy
///////////////////////////////////////////////////
bool HHGate2D::isOriginalChannel( Id id ) const
{
	return ( id == originalChanId_ );
}

bool HHGate2D::isOriginalGate( Id id ) const
{
	return ( id == originalGateId_ );
}

Id HHGate2D::originalChannelId() const
{
	return originalChanId_;
}

double HHGate2D::getXminA(const Eref& e) const
{
    return A_.getXmin();
}

void HHGate2D::setXminA(const Eref& e, double value)
{
    A_.setXmin(value);
}

double HHGate2D::getXmaxA(const Eref& e) const
{
    return A_.getXmax();
}

void HHGate2D::setXmaxA(const Eref& e, double value)
{
    A_.setXmax(value);
}

unsigned int HHGate2D::getXdivsA(const Eref& e) const
{
    return A_.getXdivs();
}

void HHGate2D::setXdivsA(const Eref& e, unsigned int value)
{
    A_.setXdivs(value);
}

double HHGate2D::getYminA(const Eref& e) const
{
    return A_.getYmin();
}

void HHGate2D::setYminA(const Eref& e, double value)
{
    A_.setYmin(value);
}

double HHGate2D::getYmaxA(const Eref& e) const
{
    return A_.getYmax();
}

void HHGate2D::setYmaxA(const Eref& e, double value)
{
    A_.setYmax(value);
}

unsigned int HHGate2D::getYdivsA(const Eref& e) const
{
    return A_.getYdivs();
}

void HHGate2D::setYdivsA(const Eref& e, unsigned int value)
{
    A_.setYdivs(value);
}

double HHGate2D::getXminB(const Eref& e) const
{
    return B_.getXmin();
}

void HHGate2D::setXminB(const Eref& e, double value)
{
    B_.setXmin(value);
}

double HHGate2D::getXmaxB(const Eref& e) const
{
    return B_.getXmax();
}

void HHGate2D::setXmaxB(const Eref& e, double value)
{
    B_.setXmax(value);
}

unsigned int HHGate2D::getXdivsB(const Eref& e) const
{
    return B_.getXdivs();
}

void HHGate2D::setXdivsB(const Eref& e, unsigned int value)
{
    B_.setXdivs(value);
}

double HHGate2D::getYminB(const Eref& e) const
{
    return B_.getYmin();
}

void HHGate2D::setYminB(const Eref& e, double value)
{
    B_.setYmin(value);
}

double HHGate2D::getYmaxB(const Eref& e) const
{
    return B_.getYmax();
}

void HHGate2D::setYmaxB(const Eref& e, double value)
{
    B_.setYmax(value);
}

unsigned int HHGate2D::getYdivsB(const Eref& e) const
{
    return B_.getYdivs();
}

void HHGate2D::setYdivsB(const Eref& e, unsigned int value)
{
    B_.setYdivs(value);
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////
/*
void HHGate2D::gateFunc( const Eref& e, const Qinfo* q,
	double v1, double v2 )
{

	sendBack2< double, double >( c, gateSlot,
		h->A_.innerLookup( v1, v2 ) , h->B_.innerLookup( v1, v2 ) );
}
*/

/**
 * Request the gate explicitly to create Interpols, with the given ids. This is
 * used when the gate is a global object, and so the interpols need to be
 * globals too. Comes in use in TABCREATE in the parallel context.
void HHGate2D::createInterpols( const Conn* c, IdGenerator idGen )
{
	HHGate2D* h = static_cast< HHGate2D *>( c->data() );
	Eref e = c->target();

	const Cinfo* ic = initInterpol2DCinfo();

	// Here we must set the noDelFlag to 1 because these data
	// parts belong to the parent HHGate2D structure.
	Element* A = ic->create(
		idGen.next(), "A", static_cast< void* >( &h->A_ ), 1 );
	e.add( "childSrc", A, "child" );

	Element* B = ic->create(
		idGen.next(), "B", static_cast< void* >( &h->B_), 1 );
	e.add( "childSrc", B, "child" );
}
 */
