/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Boundary.h"
#include "MeshEntry.h"
// #include "Stencil.h"
#include "ChemCompt.h"

const Cinfo* Boundary::initCinfo()
{
		static ValueFinfo< Boundary, double > reflectivity(
			"reflectivity",
			"What happens to a molecule hitting it: bounces, absorbed, diffused?",
			&Boundary::setReflectivity,
			&Boundary::getReflectivity
		);

		/*
		static DestFinfo geometry( "geometry",
			"Size message coming from geometry object for this Boundary"
			"The three arguments are volume, area, and perimeter. The"
			"receiving function decides which it cares about."
			"The originating geometry may return zero on some of the"
			"dimensions if they do not apply",
			new UpFunc3< ChemCompt, double, double, double >( &ChemCompt::extent ) );
			*/

		static DestFinfo adjacent( "adjacent",
			"Dummy message coming from adjacent compartment to current one"
			"Implies that compts are peers: do not surround each other",
			new OpFuncDummy() );

		static DestFinfo outside( "outside",
			"Dummy message coming from surrounding compartment to this one."
			"Implies that the originating compartment surrounds this one",
			new OpFuncDummy() );

		static SrcFinfo0 toAdjacent( "toAdjacent",
			"Dummy message going to adjacent compartment."
		);

		static SrcFinfo0 toInside( "toInside",
			"Dummy message going to surrounded compartment."
		);


	static Finfo* boundaryFinfos[] = {
		&reflectivity,	// Field
		// &geometry,		// DestFinfo
		&adjacent,		// DestFinfo
		&outside,		// DestFinfo
		&toAdjacent,	// SrcFinfo
		&toInside,		// SrcFinfo
	};

	static Dinfo< Boundary > dinfo;
	static Cinfo boundaryCinfo (
		"Boundary",
		// No base class, but eventually I guess it will be neutral.
		Neutral::initCinfo(),
		boundaryFinfos,
		sizeof( boundaryFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &boundaryCinfo;
}

static const Cinfo* boundaryCinfo = Boundary::initCinfo();

Boundary::Boundary()
	: reflectivity_( 1.0 )
{
	;
}

void Boundary::setReflectivity( const double v )
{
	reflectivity_ = v;
}

double Boundary::getReflectivity() const
{
	return reflectivity_;
}

