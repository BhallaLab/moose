// Leakage.h ---

//
// Filename: Leakage.h
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Mon Aug  3 02:22:58 2009 (+0530)
// Version:
// Last-Updated: Mon Aug  3 03:07:21 2009 (+0530)
//           By: subhasis ray
//     Update #: 18
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary: Reimplementation of leakage class in GENESIS
//
//
//
//

// Change log:
//
// 2014-05-23: Subha - ported to async13 branch
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

#ifndef   	LEAKAGE_H_
#define   	LEAKAGE_H_

class Leakage: public ChanCommon
{
public:
    Leakage();
    ~Leakage();
    void vProcess( const Eref & e, ProcPtr p );
    void vReinit( const Eref & e, ProcPtr p );
    void vSetGbar( const Eref & e, double gbar );

    static const Cinfo * initCinfo();
};

#endif 	    /* !LEAKAGE_H_ */



//
// Leakage.h ends here
