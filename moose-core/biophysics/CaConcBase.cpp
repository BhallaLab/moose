/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment.
 **           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

// #include <cfloat>
#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "CaConcBase.h"

#define VALENCE 2

///////////////////////////////////////////////////////
// MsgSrc definitions
///////////////////////////////////////////////////////
/*
 * This Finfo is used to send out Ca concentration to channels.
 *
 * It is exposed here so that HSolve can also use it to send out
 * Ca concentration to the recipients.
 */
// Static function.
SrcFinfo1< double >* CaConcBase::concOut()
{
    static SrcFinfo1< double > concOut( "concOut",
                                        "Concentration of Ca in pool" );
    return &concOut;
}

const Cinfo* CaConcBase::initCinfo()
{
    ///////////////////////////////////////////////////////
    // Shared message definitions
    ///////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call",
                              new ProcOpFunc< CaConcBase >( &CaConcBase::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call",
                             new ProcOpFunc< CaConcBase >( &CaConcBase::reinit ) );

    static Finfo* processShared[] =
    {
        &process, &reinit
    };

    static SharedFinfo proc( "proc",
                             "Shared message to receive Process message from scheduler",
                             processShared, sizeof( processShared ) / sizeof( Finfo* ) );

    ///////////////////////////////////////////////////////
    // Field definitions
    ///////////////////////////////////////////////////////
    static ElementValueFinfo< CaConcBase, double > Ca( "Ca",
            "Calcium concentration.",
            &CaConcBase::setCa,
            &CaConcBase::getCa
                                                     );
    static ElementValueFinfo< CaConcBase, double > CaBasal( "CaBasal",
            "Basal Calcium concentration.",
            &CaConcBase::setCaBasal,
            &CaConcBase::getCaBasal
                                                          );
    static ElementValueFinfo< CaConcBase, double > Ca_base( "Ca_base",
            "Basal Calcium concentration, synonym for CaBasal",
            &CaConcBase::setCaBasal,
            &CaConcBase::getCaBasal
                                                          );
    static ElementValueFinfo< CaConcBase, double > tau( "tau",
            "Settling time for Ca concentration",
            &CaConcBase::setTau,
            &CaConcBase::getTau
                                                      );
    static ElementValueFinfo< CaConcBase, double > B( "B",
            "Volume scaling factor. "
            "Deprecated. This is a legacy field from GENESIS and exposes "
            "internal calculations. Please do not use. \n"
            "B = 1/(vol * F* VALENCE) so that it obeys:\n"
            "dC/dt = B*I_Ca - C/tau",
            &CaConcBase::setB,
            &CaConcBase::getB
                                                    );
    static ElementValueFinfo< CaConcBase, double > thick( "thick",
            "Thickness of Ca shell, assumed cylindrical. Legal range is between 0 \n"
            "and the radius. If outside this range it is taken as the radius. \n"
            " Default zero, ie, the shell is the entire thickness of the cylinder \n",
            &CaConcBase::setThickness,
            &CaConcBase::getThickness
                                                        );
    static ElementValueFinfo< CaConcBase, double > length( "length",
            "Length of Ca shell, assumed cylindrical",
            &CaConcBase::setLength,
            &CaConcBase::getLength
                                                         );
    static ElementValueFinfo< CaConcBase, double > diameter( "diameter",
            "Diameter of Ca shell, assumed cylindrical",
            &CaConcBase::setDiameter,
            &CaConcBase::getDiameter
                                                           );
    static ElementValueFinfo< CaConcBase, double > ceiling( "ceiling",
            "Ceiling value for Ca concentration. If Ca > ceiling, Ca = ceiling. \n"
            "If ceiling <= 0.0, there is no upper limit on Ca concentration value.",
            &CaConcBase::setCeiling,
            &CaConcBase::getCeiling
                                                          );
    static ElementValueFinfo< CaConcBase, double > floor( "floor",
            "Floor value for Ca concentration. If Ca < floor, Ca = floor",
            &CaConcBase::setFloor,
            &CaConcBase::getFloor
                                                        );

    ///////////////////////////////////////////////////////
    // MsgDest definitions
    ///////////////////////////////////////////////////////

    static DestFinfo current( "current",
                              "Calcium Ion current, due to be converted to conc.",
                              new EpFunc1< CaConcBase, double >( &CaConcBase::current )
                            );

    static DestFinfo currentFraction( "currentFraction",
                                      "Fraction of total Ion current, that is carried by Ca2+.",
                                      new EpFunc2< CaConcBase, double, double >( &CaConcBase::currentFraction )
                                    );

    static DestFinfo increase( "increase",
                               "Any input current that increases the concentration.",
                               new EpFunc1< CaConcBase, double >( &CaConcBase::increase )
                             );

    static DestFinfo decrease( "decrease",
                               "Any input current that decreases the concentration.",
                               new EpFunc1< CaConcBase, double >( &CaConcBase::decrease )
                             );

    static DestFinfo basal( "basal",
                            "Synonym for assignment of basal conc.",
                            new EpFunc1< CaConcBase, double >( &CaConcBase::setCaBasal )
                          );

    static Finfo* CaConcBaseFinfos[] =
    {
        &proc,		// Shared
        concOut(),	// Src
        &Ca,		// Value
        &CaBasal,	// Value
        &Ca_base,	// Value
        &tau,		// Value
        &B,			// Value
        &thick,		// Value
        &diameter,	// Value
        &length,	// Value
        &ceiling,	// Value
        &floor,		// Value
        &current,	// Dest
        &currentFraction,	// Dest
        &increase,	// Dest
        &decrease,	// Dest
        &basal,		// Dest
    };

    // We want the Ca updates before channel updates, so along with compts.
    // static SchedInfo schedInfo[] = { { process, 0, 0 } };

    static string doc[] =
    {
        "Name", "CaConcBase",
        "Author", "Upinder S. Bhalla, 2014, NCBS",
        "Description",
        "CaConcBase: Base class for Calcium concentration pool. \n"
        "Takes current from a channel and keeps track of calcium buildup and \n"
        " depletion by a single exponential process. ",
    };

    static ZeroSizeDinfo< int > dinfo;

    static Cinfo CaConcBaseCinfo(
        "CaConcBase",
        Neutral::initCinfo(),
        CaConcBaseFinfos,
        sizeof( CaConcBaseFinfos )/sizeof(Finfo *),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &CaConcBaseCinfo;
}
///////////////////////////////////////////////////

static const Cinfo* caConcCinfo = CaConcBase::initCinfo();

CaConcBase::CaConcBase()
    :
    thickness_( 0.0 )
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void CaConcBase::setCa( const Eref& e, double Ca )
{
    vSetCa( e, Ca );
}
double CaConcBase::getCa( const Eref& e ) const
{
    return vGetCa( e );
}

void CaConcBase::setCaBasal( const Eref& e, double CaBasal )
{
    vSetCaBasal( e, CaBasal );
}
double CaConcBase::getCaBasal( const Eref& e ) const
{
    return vGetCaBasal( e );
}

void CaConcBase::setTau( const Eref& e, double tau )
{
    vSetTau( e, tau );
}
double CaConcBase::getTau( const Eref& e ) const
{
    return vGetTau( e );
}

void CaConcBase::setB( const Eref& e, double B )
{
    vSetB( e, B );
}
double CaConcBase::getB( const Eref& e ) const
{
    return vGetB( e );
}
void CaConcBase::setCeiling( const Eref& e, double ceiling )
{
    vSetCeiling( e, ceiling );
}
double CaConcBase::getCeiling( const Eref& e ) const
{
    return vGetCeiling( e );
}

void CaConcBase::setFloor( const Eref& e, double floor )
{
    vSetFloor( e, floor );
}
double CaConcBase::getFloor( const Eref& e ) const
{
    return vGetFloor( e );
}

void CaConcBase::updateDimensions( const Eref& e )
{
    double vol = PI * diameter_ * diameter_ * length_ * 0.25;
    if ( thickness_ > 0 && thickness_ < diameter_/2.0 )
    {
        double coreRadius = diameter_ / 2.0 - thickness_;
        vol -= PI * coreRadius * coreRadius * length_;
    }
    double B = 1.0 / ( VALENCE* FaradayConst * vol );
    vSetB( e, B );
}

void CaConcBase::setThickness( const Eref& e, double thickness )
{
    thickness_ = thickness;
    updateDimensions( e );
}

double CaConcBase::getThickness( const Eref& e ) const
{
    return thickness_;
}

void CaConcBase::setDiameter( const Eref& e, double diameter )
{
    diameter_ = diameter;
    updateDimensions( e );
}

double CaConcBase::getDiameter( const Eref& e ) const
{
    return diameter_;
}

void CaConcBase::setLength( const Eref& e, double length )
{
    length_ = length;
    updateDimensions( e );
}
double CaConcBase::getLength( const Eref& e ) const
{
    return length_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void CaConcBase::reinit( const Eref& e, ProcPtr p )
{
    vReinit( e, p );
}

void CaConcBase::process( const Eref& e, ProcPtr p )
{
    vProcess( e, p );
}

void CaConcBase::current( const Eref& e, double I )
{
    vCurrent( e, I );
}

void CaConcBase::currentFraction( const Eref& e, double I, double fraction )
{
    vCurrentFraction( e, I, fraction );
}

void CaConcBase::increase( const Eref& e, double I )
{
    vIncrease( e, I );
}

void CaConcBase::decrease( const Eref& e, double I )
{
    vDecrease( e, I );
}

/////////////////////////////////////////////////////
// Zombify
/////////////////////////////////////////////////////


// Dummy instantiation of vSetSolve, does nothing
void CaConcBase::vSetSolver( const Eref& e, Id hsolve )
{;}

// static func
void CaConcBase::zombify( Element* orig, const Cinfo* zClass, Id hsolve )
{
    if ( orig->cinfo() == zClass )
        return;
    unsigned int start = orig->localDataStart();
    unsigned int num = orig->numLocalData();
    if ( num == 0 )
        return;
    vector< double > data( num * 9 );

    unsigned int j = 0;
    for ( unsigned int i = 0; i < num; ++i )
    {
        Eref er( orig, i + start );
        const CaConcBase* cb = reinterpret_cast<const CaConcBase*>(er.data());
        data[j + 0] = cb->getCa( er );
        data[j + 1] = cb->getCaBasal( er );
        data[j + 2] = cb->getTau( er );
        data[j + 3] = cb->getB( er );
        data[j + 4] = cb->getCeiling( er );
        data[j + 5] = cb->getFloor( er );
        data[j + 6] = cb->getThickness( er );
        data[j + 7] = cb->getLength( er );
        data[j + 8] = cb->getDiameter( er );
        j += 9;
    }
    orig->zombieSwap( zClass );
    j = 0;
    for ( unsigned int i = 0; i < num; ++i )
    {
        Eref er( orig, i + start );
        CaConcBase* cb = reinterpret_cast< CaConcBase* >( er.data() );
        cb->vSetSolver( er, hsolve );
        cb->setCa( er, data[j + 0] );
        cb->setCaBasal( er, data[j + 1] );
        cb->setTau( er, data[j + 2] );
        cb->setB( er, data[j + 3] );
        cb->setCeiling( er, data[j + 4] );
        cb->setFloor( er, data[j + 5] );
        cb->setThickness( er, data[j + 6] );
        cb->setLength( er, data[j + 7] );
        cb->setDiameter( er, data[j + 8] );
        j += 9; //was 7?
    }
}
