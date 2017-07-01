// Interpol.h ---
//
// Filename: Interpol.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Wed Jun 25 15:13:52 2014 (+0530)
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

// Code:

#ifndef _INTERPOL_H
#define _INTERPOL_H

/**
 * 1 Dimensional table, with interpolation. The internal vector is
 * accessed like this: table_[ xIndex ] with the x-coordinate used as
 * the index.
 */
class Interpol: public TableBase
{
  public:
    Interpol();
    Interpol(double xmin, double xmax);

    ////////////////////////////////////////////////////////////
    // Here are the interface functions for the MOOSE class
    ////////////////////////////////////////////////////////////
    void setXmin( double value );
    double getXmin() const;
    void setXmax( double value );
    double getXmax() const;
    double getY() const;
    ////////////////////////////////////////////////////////////
    // Here are the Interpol Destination functions
    ////////////////////////////////////////////////////////////
    void handleInput(double x);
    void process( const Eref& e, ProcPtr p );
    void reinit( const Eref& e, ProcPtr p );


    static const Cinfo* initCinfo();
    static const unsigned int MAX_DIVS;
  protected:
    double x_;
    double xmin_;
    double xmax_;
    double y_;
};


#endif // _INTERPOL_H


//
// Interpol.h ends here
