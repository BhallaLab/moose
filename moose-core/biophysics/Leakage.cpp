// Leakage.cpp ---
//
// Filename: Leakage.cpp
// Description:
// Author: subhasis ray
// Maintainer:
// Created: Mon Aug  3 02:32:29 2009 (+0530)
// Version:
// Last-Updated: Mon Aug 10 11:15:39 2009 (+0530)
//           By: subhasis ray
//     Update #: 71
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary: Reimplementation of leakage class of GENESIS
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

#include "../basecode/header.h"
#include "ChanBase.h"
#include "ChanCommon.h"
#include "Leakage.h"

const Cinfo* Leakage::initCinfo()
{
    static string doc[] =
    {
        "Name", "Leakage",
        "Author", "Subhasis Ray, 2009, Upi Bhalla 2014 NCBS",
        "Description", "Leakage: Passive leakage channel."
    };

    static Dinfo< Leakage > dinfo;

    static Cinfo LeakageCinfo(
        "Leakage",
        ChanBase::initCinfo(),
        0,
        0,
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string ));

    return &LeakageCinfo;
}

static const Cinfo* leakageCinfo = Leakage::initCinfo();


Leakage::Leakage()
{
    ;
}

Leakage::~Leakage()
{
    ;
}
//////// Function definitions

void Leakage::vProcess( const Eref & e, ProcPtr p )
{
    ChanCommon::vSetGk( e, this->vGetGbar( e ) * this->vGetModulation( e ));
    updateIk();
    sendProcessMsgs(e, p);
}

void Leakage::vReinit( const Eref & e, ProcPtr p )
{
    ChanCommon::vSetGk( e, this->vGetGbar( e ) * this->vGetModulation( e ));
    updateIk();
    sendReinitMsgs(e, p);
}

void Leakage::vSetGbar( const Eref& e, double gbar )
{
    ChanCommon::vSetGk( e, gbar * this->vGetModulation( e ) );
    ChanCommon::vSetGbar( e, gbar );
}

//
// Leakage.cpp ends here
