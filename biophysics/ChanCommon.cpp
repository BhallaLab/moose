/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "ChanBase.h"
#include "ChanCommon.h"

///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
ChanCommon::ChanCommon()
    :
    Vm_( 0.0 ),
    Gbar_( 0.0 ), modulation_( 1.0 ),
    Ek_( 0.0 ),
    Gk_( 0.0 ), Ik_( 0.0 )
{
    ;
}

ChanCommon::~ChanCommon()
{
    ;
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void ChanCommon::vSetGbar( const Eref& e, double Gbar )
{
    Gbar_ = Gbar;
}

double ChanCommon::vGetGbar( const Eref& e ) const
{
    return Gbar_;
}

void ChanCommon::vSetModulation( const Eref& e, double modulation )
{
    modulation_ = modulation;
}

double ChanCommon::vGetModulation( const Eref& e ) const
{
    return modulation_;
}

double ChanCommon::getModulation() const
{
    return modulation_;
}

void ChanCommon::vSetEk( const Eref& e, double Ek )
{
    Ek_ = Ek;
}
double ChanCommon::vGetEk( const Eref& e ) const
{
    return Ek_;
}

void ChanCommon::vSetGk( const Eref& e, double Gk )
{
    Gk_ = Gk;
}
double ChanCommon::vGetGk( const Eref& e ) const
{
    return Gk_;
}

void ChanCommon::vSetIk( const Eref& e, double Ik )
{
    Ik_ = Ik;
}
double ChanCommon::vGetIk( const Eref& e ) const
{
    return Ik_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void ChanCommon::vHandleVm( double Vm )
{
    Vm_ = Vm;
}

///////////////////////////////////////////////////
// Looks like a dest function, but it is only called
// from the child class. Sends out various messages.
///////////////////////////////////////////////////

void ChanCommon::sendProcessMsgs(  const Eref& e, const ProcPtr info )
{
    ChanBase::channelOut()->send( e, Gk_, Ek_ );
    // This is used if the channel connects up to a conc pool and
    // handles influx of ions giving rise to a concentration change.
    ChanBase::IkOut()->send( e, Ik_ );
    // Needed by GHK-type objects
    ChanBase::permeability()->send( e, Gk_ );
}


void ChanCommon::sendReinitMsgs(  const Eref& e, const ProcPtr info )
{
    ChanBase::channelOut()->send( e, Gk_, Ek_ );
    ChanBase::IkOut()->send( e, Ik_ );
    // Needed by GHK-type objects
    ChanBase::permeability()->send( e, Gk_ );
}

void ChanCommon::updateIk()
{
    Ik_ = ( Ek_ - Vm_ ) * Gk_;
}

double ChanCommon::getVm() const
{
    return Vm_;
}

double ChanCommon::getGbar() const
{
    return Gbar_;
}
