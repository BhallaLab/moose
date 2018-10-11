// GapJunction.cpp ---
//
// Filename: GapJunction.cpp
// Description: Implements Gap junction
// Author: Subhasis Ray
// Maintainer:
// Created: Tue Jul  2 11:40:13 2013 (+0530)
// Version:
// Last-Updated: Tue Jul  2 14:26:01 2013 (+0530)
//           By: subha
//     Update #: 77
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

#include "../basecode/header.h"
#include "GapJunction.h"

static SrcFinfo2< double, double >* channel1Out()
{
    static SrcFinfo2< double, double > channel1Out(
        "channel1Out",
        "Sends Gk and Vm from one compartment to the other");
    return &channel1Out;
}

static SrcFinfo2< double, double >* channel2Out()
{
    static SrcFinfo2< double, double > channel2Out(
        "channel2Out",
        "Sends Gk and Vm from one compartment to the other");
    return &channel2Out;
}

const Cinfo* GapJunction::initCinfo()
{

    static ValueFinfo< GapJunction, double > Gk(
        "Gk",
        "Conductance of the gap junction",
        &GapJunction::setGk,
        &GapJunction::getGk);

    ///////////////////////////////////////////////////////////////////
    // Shared messages
    ///////////////////////////////////////////////////////////////////
    static DestFinfo process(
        "process",
        "Handles 'process' call",
        new ProcOpFunc< GapJunction >( &GapJunction::process ) );

    static DestFinfo reinit(
        "reinit",
        "Handles 'reinit' call",
        new ProcOpFunc< GapJunction >( &GapJunction::reinit ) );

    static Finfo* processShared[] = {
        &process, &reinit
    };

    static SharedFinfo proc(
        "proc",
        "This is a shared message to receive Process messages "
        "from the scheduler objects. The Process should be called "
        "_second_ in each clock tick, after the Init message."
        "The first entry in the shared msg is a MsgDest "
        "for the Process operation. It has a single argument, "
        "ProcInfo, which holds lots of information about current "
        "time, thread, dt and so on. The second entry is a MsgDest "
        "for the Reinit operation. It also uses ProcInfo. ",
        processShared, sizeof( processShared ) / sizeof( Finfo* ));

    static DestFinfo Vm1(
        "Vm1",
        "Handles Vm message from compartment",
        new OpFunc1 < GapJunction, double >( &GapJunction::setVm1 ));

    static Finfo * channel1Shared[] = {
        channel1Out(), &Vm1,
    };

    static SharedFinfo channel1(
        "channel1",
        "This is a shared message to couple the conductance and Vm from\n"
        "terminal 2 to the compartment at terminal 1. The first entry is source\n"
        "sending out Gk and Vm2, the second entry is destination for Vm1.",
        channel1Shared, sizeof(channel1Shared)/sizeof(Finfo*));

    static DestFinfo Vm2(
        "Vm2",
        "Handles Vm message from another compartment",
        new OpFunc1< GapJunction, double >( &GapJunction::setVm2 ));

    static Finfo * channel2Shared[] = {
        channel2Out(), &Vm2,
    };

    static SharedFinfo channel2(
        "channel2",
        "This is a shared message to couple the conductance and Vm from\n"
        "terminal 1 to the compartment at terminal 2. The first entry is source\n"
        "sending out Gk and Vm1, the second entry is destination for Vm2.",
        channel2Shared, sizeof(channel2Shared)/sizeof(Finfo*));

    static Finfo * gapJunctionFinfos[] =
    {
        &channel1,
        &channel2,
        &Gk,
        &proc
    };

    static string doc[] = {
        "Name", "GapJunction",
        "Author", "Subhasis Ray, 2013",
        "Description", "Implementation of gap junction between two compartments. The shared\n"
        "fields, 'channel1' and 'channel2' can be connected to the 'channel'\n"
        "message of the compartments at either end of the gap junction. The\n"
        "compartments will send their Vm to the gap junction and receive the\n"
        "conductance 'Gk' of the gap junction and the Vm of the other\n"
        "compartment."
    };

	static Dinfo< GapJunction > dinfo;
    static Cinfo gapJunctionCinfo(
        "GapJunction",
        Neutral::initCinfo(),
        gapJunctionFinfos,
        sizeof(gapJunctionFinfos)/sizeof(Finfo*),
		&dinfo,
        doc,
        sizeof(doc) / sizeof(string));
    return &gapJunctionCinfo;
}

static const Cinfo * gapJunctionCinfo = GapJunction::initCinfo();

GapJunction::GapJunction():Vm1_(0.0), Vm2_(0.0), Gk_(1e-9)
{
    ;
}

GapJunction::~GapJunction()
{
    ;
}

void GapJunction::setGk( double g )
{
    Gk_ = g;
}

double GapJunction::getGk() const
{
    return Gk_;
}

void GapJunction::setVm1( double v )
{
    Vm1_ = v;
}

void GapJunction::setVm2( double v )
{
    Vm2_ = v;
}

void GapJunction::process( const Eref& e, ProcPtr p )
{
    channel1Out()->send(e, Gk_, Vm2_);
    channel2Out()->send(e, Gk_, Vm1_);
}

void GapJunction::reinit( const Eref&, ProcPtr p )
{
    Vm1_ = 0.0;
    Vm2_ = 0.0;
}



//
// GapJunction.cpp ends here
