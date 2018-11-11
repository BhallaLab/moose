// Interpol.cpp ---
//
// Filename: Interpol.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Wed Jun 25 15:25:24 2014 (+0530)
// Version:
// Last-Updated:
//           By:
//     Update #: 0
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change log:
//
//
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
//
//

//

#include "../basecode/header.h"
#include "../utility/numutil.h"
#include "TableBase.h"
#include "Interpol.h"


static SrcFinfo1< double >* lookupOut()
{
	static SrcFinfo1< double > lookupOut( "lookupOut",
		"respond to a request for a value lookup" );
	return &lookupOut;
}

const Cinfo * Interpol::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ValueFinfo< Interpol, double > xmin(
        "xmin",
        "Minimum value of x. x below this will result in y[0] being returned.",
        &Interpol::setXmin,
        &Interpol::getXmin
                                               );
    static ValueFinfo< Interpol, double > xmax(
        "xmax",
        "Maximum value of x. x above this will result in y[last] being"
        " returned.",
        &Interpol::setXmax,
        &Interpol::getXmax);

    static ReadOnlyValueFinfo< Interpol, double > y(
        "y",
        "Looked up value.",
        &Interpol::getY);
    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////

    static DestFinfo input( "input",
                            "Interpolates using the input as x value.",
                            new OpFunc1< Interpol, double >( &Interpol::handleInput ));

    static DestFinfo process( "process",
                              "Handles process call, updates internal time stamp.",
                              new ProcOpFunc< Interpol >( &Interpol::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call.",
                             new ProcOpFunc< Interpol >( &Interpol::reinit ) );
    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////
    static Finfo* procShared[] = {
        &process, &reinit
    };
    static SharedFinfo proc( "proc",
                             "Shared message for process and reinit",
                             procShared, sizeof( procShared ) / sizeof( const Finfo* )
                             );

    //////////////////////////////////////////////////////////////
    // Field Element for the vector data
    // Use a limit of 2^20 entries for the tables, about 1 million.
    //////////////////////////////////////////////////////////////

    static Finfo* interpolFinfos[] = {
        &xmin,		// Value
        &xmax,		// Value
        &y,
        // &xdivs,		// Value
        lookupOut(),
        &input,		// DestFinfo
        &proc,			// SharedFinfo
    };

    static string doc[] =
            {
                "Name", "Interpol",
                "Author", "Upinder Bhalla, Subhasis Ray, 2014, NCBS",
                "Description", "Interpol: Interpolation class. "
                "Handles lookup from a 1-dimensional array of real-numbered values."
                "Returns 'y' value based on given 'x' value. "
                "Can either use interpolation or roundoff to the nearest index.",
            };

    static Dinfo< Interpol > dinfo;
    static Cinfo interpolCinfo (
        "Interpol",
        TableBase::initCinfo(),
        interpolFinfos,
        sizeof( interpolFinfos ) / sizeof ( Finfo* ),
        &dinfo,
        doc,
        sizeof(doc) / sizeof(string));

    return &interpolCinfo;
}

static const Cinfo * interpolCinfo = Interpol::initCinfo();

Interpol::Interpol(): xmin_(0.0), xmax_(1.0)
{
}

Interpol::Interpol(double xmin, double xmax)
        : xmin_(xmin),
          xmax_(xmax)
{}

void Interpol::setXmin(double value)
{
    if (almostEqual(value, xmax_)){
        cerr << "Error: Interpol::setXmin: Xmin ~= Xmax : Assignment failed\n";
        return;
    }
    xmin_ = value;
}

double Interpol::getXmin() const
{
    return xmin_;
}

void Interpol::setXmax(double value)
{
    if (almostEqual(value, xmin_)){
        cerr << "Error: Interpol::setXmax: Xmin ~= Xmax : Assignment failed\n";
        return;
    }
    xmax_ = value;
}

double Interpol::getXmax() const
{
    return xmax_;
}

double Interpol::getY() const
{
    return y_;
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void Interpol::process( const Eref& e, ProcPtr p )
{
    y_ = interpolate(xmin_, xmax_, x_);
    lookupOut()->send( e, y_ );
}

void Interpol::reinit( const Eref& e, ProcPtr p )
{
    x_ = 0.0;
}

//////////////////////////////////////////////////////////////
// Used to handle direct messages into the interpol, or
// returned plot data from queried objects.
//////////////////////////////////////////////////////////////
void Interpol::handleInput( double v )
{
    x_ = v;
}

//
// Interpol.cpp ends here
