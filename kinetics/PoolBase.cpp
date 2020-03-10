/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "lookupVolumeFromMesh.h"
#include "PoolBase.h"

#define EPSILON 1e-15

const SpeciesId DefaultSpeciesId = 0;

const Cinfo* PoolBase::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ElementValueFinfo< PoolBase, double > n(
        "n",
        "Number of molecules in pool",
        &PoolBase::setN,
        &PoolBase::getN
    );

    static ElementValueFinfo< PoolBase, double > nInit(
        "nInit",
        "Initial value of number of molecules in pool",
        &PoolBase::setNinit,
        &PoolBase::getNinit
    );

    static ElementValueFinfo< PoolBase, double > diffConst(
        "diffConst",
        "Diffusion constant of molecule",
        &PoolBase::setDiffConst,
        &PoolBase::getDiffConst
    );

    static ElementValueFinfo< PoolBase, double > motorConst(
        "motorConst",
        "Motor transport rate molecule. + is away from soma, - is "
        "towards soma. Only relevant for ZombiePool subclasses.",
        &PoolBase::setMotorConst,
        &PoolBase::getMotorConst
    );

    static ElementValueFinfo< PoolBase, double > conc(
        "conc",
        "Concentration of molecules in this pool",
        &PoolBase::setConc,
        &PoolBase::getConc
    );

    static ElementValueFinfo< PoolBase, double > concInit(
        "concInit",
        "Initial value of molecular concentration in pool",
        &PoolBase::setConcInit,
        &PoolBase::getConcInit
    );

    static ElementValueFinfo< PoolBase, double > volume(
        "volume",
        "Volume of compartment. Units are SI. "
        "Utility field, the actual volume info is "
        "stored on a volume mesh entry in the parent compartment."
        "This mapping is implicit: the parent compartment must be "
        "somewhere up the element tree, and must have matching mesh "
        "entries. If the compartment isn't"
        "available the volume is just taken as 1",
        &PoolBase::setVolume,
        &PoolBase::getVolume
    );

    static ElementValueFinfo< PoolBase, unsigned int > speciesId(
        "speciesId",
        "Species identifier for this mol pool. Eventually link to ontology.",
        &PoolBase::setSpecies,
        &PoolBase::getSpecies
    );

    static ElementValueFinfo< PoolBase, bool > isBuffered(
        "isBuffered",
        "Flag: True if Pool is buffered. "
        "In the case of Pool and BufPool the field can be assigned, to "
        "change the type of the Pool object to BufPool, or vice versa. "
        "None of the messages are affected. "
        "This object class flip can only be done in the non-zombified "
        "form of the Pool/BufPool. In Zombies it is read-only.",
        &PoolBase::setIsBuffered,
        &PoolBase::getIsBuffered
    );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles process call",
                              new ProcOpFunc< PoolBase >( &PoolBase::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles reinit call",
                             new ProcOpFunc< PoolBase >( &PoolBase::reinit ) );

    static DestFinfo reacDest( "reacDest",
                               "Handles reaction input",
                               new OpFunc2< PoolBase, double, double >( &PoolBase::reac )
                             );

    static DestFinfo handleMolWt( "handleMolWt",
                                  "Separate finfo to assign molWt, and consequently diffusion const."
                                  "Should only be used in SharedMsg with species.",
                                  new EpFunc1< PoolBase, double >( &PoolBase::handleMolWt )
                                );
    //////////////////////////////////////////////////////////////
    // MsgDest Definitions: These three are used for non-reaction
    // calculations involving algebraically defined rate terms.
    //////////////////////////////////////////////////////////////
    static DestFinfo increment( "increment",
                                "Increments mol numbers by specified amount. Can be +ve or -ve",
                                new OpFunc1< PoolBase, double >( &PoolBase::increment )
                              );

    static DestFinfo decrement( "decrement",
                                "Decrements mol numbers by specified amount. Can be +ve or -ve",
                                new OpFunc1< PoolBase, double >( &PoolBase::decrement )
                              );

    static DestFinfo nIn( "nIn",
                          "Assigns the number of molecules in Pool to specified value",
                          new OpFunc1< PoolBase, double >( &PoolBase::nIn )
                        );

    //////////////////////////////////////////////////////////////
    // SrcFinfo Definitions
    //////////////////////////////////////////////////////////////

    static SrcFinfo1< double > nOut(
        "nOut",
        "Sends out # of molecules in pool on each timestep"
    );

    static SrcFinfo0 requestMolWt(
        "requestMolWt",
        "Requests Species object for mol wt"
    );

    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////
    static Finfo* reacShared[] =
    {
        &reacDest, &nOut
    };
    static SharedFinfo reac( "reac",
                             "Connects to reaction",
                             reacShared, sizeof( reacShared ) / sizeof( const Finfo* )
                           );
    static Finfo* procShared[] =
    {
        &process, &reinit
    };
    static SharedFinfo proc( "proc",
                             "Shared message for process and reinit",
                             procShared, sizeof( procShared ) / sizeof( const Finfo* )
                           );

    static Finfo* speciesShared[] =
    {
        &requestMolWt, &handleMolWt
    };

    static SharedFinfo species( "species",
                                "Shared message for connecting to species objects",
                                speciesShared, sizeof( speciesShared ) / sizeof ( const Finfo* )
                              );

    static Finfo* poolFinfos[] =
    {
        &n,			// Value
        &nInit,		// Value
        &diffConst,	// Value
        &motorConst,	// Value
        &conc,		// Value
        &concInit,	// Value
        &volume,	// Readonly Value
        &speciesId,	// Value
        &isBuffered,	// Value
        &increment,			// DestFinfo
        &decrement,			// DestFinfo
        &nIn,				// DestFinfo
        &reac,				// SharedFinfo
        &proc,				// SharedFinfo
        &species,			// SharedFinfo
    };

    static string doc[] =
    {
        "Name", "PoolBase",
        "Author", "Upi Bhalla",
        "Description", "Abstract base class for pools."
    };
    static ZeroSizeDinfo< int > dinfo;
    static Cinfo poolCinfo (
        "PoolBase",
        Neutral::initCinfo(),
        poolFinfos,
        sizeof( poolFinfos ) / sizeof ( Finfo* ),
        &dinfo,
        doc,
        sizeof( doc )/sizeof( string ),
        true // Ban creation as this is an abstract base class.
    );

    return &poolCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* poolCinfo = PoolBase::initCinfo();

//////////////////////////////////////////////////////////////
PoolBase::PoolBase() : concInit_( 0.0 )
{;}

PoolBase::~PoolBase()
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void PoolBase::process( const Eref& e, ProcPtr p )
{
    vProcess( e, p );
}

void PoolBase::reinit( const Eref& e, ProcPtr p )
{
    vReinit( e, p );
}

void PoolBase::increment( double val )
{
    vIncrement(val);
}

void PoolBase::decrement( double val )
{
    vDecrement( val );
}

void PoolBase::nIn( double val)
{
    vnIn(val);
}

void PoolBase::reac( double A, double B )
{
    vReac( A, B );
}

void PoolBase::handleMolWt( const Eref& e, double v )
{
    vHandleMolWt( e, v );
}

//////////////////////////////////////////////////////////////
// virtual MsgDest Definitions
//////////////////////////////////////////////////////////////

void PoolBase::vProcess( const Eref& e, ProcPtr p )
{;}

void PoolBase::vReinit( const Eref& e, ProcPtr p )
{;}

void PoolBase::vReac( double A, double B )
{;}

void PoolBase::vHandleMolWt( const Eref& e, double v )
{;}

void PoolBase::vIncrement( double val )
{;}

void PoolBase::vDecrement( double val )
{;}

void PoolBase::vnIn( double val)
{;}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void PoolBase::setN( const Eref& e, double v )
{
    vSetN( e, v );
}

double PoolBase::getN( const Eref& e ) const
{
    return vGetN( e );
}

void PoolBase::setNinit( const Eref& e, double v )
{
    concInit_ =  v / ( NA * lookupVolumeFromMesh( e ) );
    vSetNinit( e, v );
}

double PoolBase::getNinit( const Eref& e ) const
{
    return vGetNinit( e );
}

// Conc is given in millimolar. Volume is in m^3
void PoolBase::setConc( const Eref& e, double c )
{
    vSetConc( e, c );
}

// Returns conc in millimolar.
double PoolBase::getConc( const Eref& e ) const
{
    return vGetConc( e );
}

void PoolBase::setConcInit( const Eref& e, double c )
{
    concInit_ = c;
    vSetConcInit( e, c );
}

double PoolBase::vGetConcInit( const Eref& e ) const
{
    return concInit_;
}

double PoolBase::getConcInit( const Eref& e ) const
{
    // return concInit_;
    return vGetConcInit( e );
}

void PoolBase::setDiffConst( const Eref& e, double v )
{
    vSetDiffConst( e, v );
}

double PoolBase::getDiffConst(const Eref& e ) const
{
    return vGetDiffConst( e );
}

void PoolBase::setMotorConst( const Eref& e, double v )
{
    vSetMotorConst( e, v );
}

double PoolBase::getMotorConst(const Eref& e ) const
{
    return vGetMotorConst( e );
}

void PoolBase::setVolume( const Eref& e, double v )
{
    vSetVolume( e, v );
}

double PoolBase::getVolume( const Eref& e ) const
{
    return vGetVolume( e );
}

void PoolBase::setSpecies( const Eref& e, unsigned int v )
{
    vSetSpecies( e, v );
}

unsigned int PoolBase::getSpecies( const Eref& e ) const
{
    return vGetSpecies( e );
}

/**
 * setIsBuffered is active only for Pool and BufPool. Otherwise ignored.
 */
void PoolBase::setIsBuffered( const Eref& e, bool v )
{
    vSetIsBuffered( e, v );
}

bool PoolBase::getIsBuffered( const Eref& e ) const
{
    return vGetIsBuffered( e );
}

//////////////////////////////////////////////////////////////
// Virtual Field Definitions
//////////////////////////////////////////////////////////////

/// Dummy MotorConst field for most Pool subclasses.
void PoolBase::vSetMotorConst( const Eref& e, double v )
{;}

double PoolBase::vGetMotorConst(const Eref& e ) const
{
    return 0.0;
}

/// Dummy default function for most pool subclasses.
void PoolBase::vSetIsBuffered( const Eref& e, bool v )
{;}

//////////////////////////////////////////////////////////////
// Zombie conversion routine: Converts Pool subclasses. There
// will typically be a target specific follow-up function, for example,
// to assign a pointer to the stoichiometry class.
// There should also be a subsequent call to resched for the entire tree.
//////////////////////////////////////////////////////////////
// static func
void PoolBase::zombify(Element* orig, const Cinfo* zClass, Id ksolve, Id dsolve)
{
    if ( orig->cinfo() == zClass )
        return;
    unsigned int start = orig->localDataStart();
    unsigned int num = orig->numLocalData();
    if ( num == 0 )
        return;
    vector< unsigned int > species( num, 0 );
    vector< double > concInit( num, 0.0 );
    vector< double > diffConst( num, 0.0 );
    vector< double > motorConst( num, 0.0 );
    for ( unsigned int i = 0; i < num; ++i )
    {
        Eref er( orig, i + start );
        const PoolBase* pb = reinterpret_cast< const PoolBase* >( er.data() );
        species[ i ] = pb->getSpecies( er );
        concInit[ i ] = pb->getConcInit( er );
        diffConst[ i ] = pb->getDiffConst( er );
        motorConst[ i ] = pb->getMotorConst( er );
    }
    orig->zombieSwap( zClass );
    for ( unsigned int i = 0; i < num; ++i )
    {
        Eref er( orig, i + start );
        PoolBase* pb = reinterpret_cast< PoolBase* >( er.data() );
        pb->vSetSolver( ksolve, dsolve );
        pb->setSpecies( er, species[i] );
        pb->setConcInit( er, concInit[i] );
        pb->setDiffConst( er, diffConst[i] );
        pb->setMotorConst( er, motorConst[i] );
    }
}

// Virtual func: default does nothing.
void PoolBase::vSetSolver( Id ksolve, Id dsolve )
{
    ;
}
