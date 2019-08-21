/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../basecode/LookupElementValueFinfo.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "ChemCompt.h"

SrcFinfo1< vector< double > >* ChemCompt::voxelVolOut()
{
    static SrcFinfo1< vector< double > > voxelVolOut( "voxelVolOut",
            "Sends updated voxel volume out to Ksolve, Gsolve, and Dsolve."
            "Used to request a recalculation of rates and of initial numbers."
                                                    );
    return &voxelVolOut;
}

const Cinfo* ChemCompt::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ElementValueFinfo< ChemCompt, double > volume(
        "volume",
        "Volume of entire chemical domain."
        "Assigning this only works if the chemical compartment has"
        "only a single voxel. Otherwise ignored."
        "This function goes through all objects below this on the"
        "tree, and rescales their molecule #s and rates as per the"
        "volume change. This keeps concentration the same, and also"
        "maintains rates as expressed in volume units.",
        &ChemCompt::setEntireVolume,
        &ChemCompt::getEntireVolume
    );

    static ReadOnlyValueFinfo< ChemCompt, vector< double > >
    voxelVolume(
        "voxelVolume",
        "Vector of volumes of each of the voxels.",
        &ChemCompt::getVoxelVolume
    );

    static ReadOnlyValueFinfo< ChemCompt, vector< double > >
    voxelMidpoint(
        "voxelMidpoint",
        "Vector of midpoint coordinates of each of the voxels. The "
        "size of this vector is 3N, where N is the number of voxels. "
        "The first N entries are for x, next N for y, last N are z. ",
        &ChemCompt::getVoxelMidpoint
    );

    static LookupElementValueFinfo<
    ChemCompt, unsigned int, double >
    oneVoxelVolume(
        "oneVoxelVolume",
        "Volume of specified voxel.",
        &ChemCompt::setOneVoxelVolume,
        &ChemCompt::getOneVoxelVolume
    );

    static ReadOnlyValueFinfo< ChemCompt, unsigned int > numDimensions(
        "numDimensions",
        "Number of spatial dimensions of this compartment. Usually 3 or 2",
        &ChemCompt::getDimensions
    );

    static ReadOnlyLookupValueFinfo< ChemCompt, unsigned int, vector< double > > stencilRate(
        "stencilRate",
        "vector of diffusion rates in the stencil for specified voxel."
        "The identity of the coupled voxels is given by the partner "
        "field 'stencilIndex'."
        "Returns an empty vector for non-voxelized compartments.",
        &ChemCompt::getStencilRate
    );

    static ReadOnlyLookupValueFinfo< ChemCompt, unsigned int, vector< unsigned int > > stencilIndex(
        "stencilIndex",
        "vector of voxels diffusively coupled to the specified voxel."
        "The diffusion rates into the coupled voxels is given by the "
        "partner field 'stencilRate'."
        "Returns an empty vector for non-voxelized compartments.",
        &ChemCompt::getStencilIndex
    );

    static ValueFinfo< ChemCompt, bool > isMembraneBound(
        "isMembraneBound",
        "Flag, set to True for meshes where each voxel is membrane "
        "bound. \n"
        "NeuroMesh and SpineMesh are false. \n"
        "CubeMesh, CylMesh, and EndoMesh can be either. If they are "
        "membrane bound they can still interact via channels and "
        "cross-compartment reactions. ",
        &ChemCompt::setIsMembraneBound,
        &ChemCompt::getIsMembraneBound
    );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////

    static DestFinfo buildDefaultMesh( "buildDefaultMesh",
                                       "Tells ChemCompt derived class to build a default mesh with the"
                                       "specified volume and number of meshEntries.",
                                       new EpFunc2< ChemCompt, double, unsigned int >(
                                           &ChemCompt::buildDefaultMesh )
                                     );

    static DestFinfo setVolumeNotRates( "setVolumeNotRates",
                                        "Changes volume but does not notify any child objects."
                                        "Only works if the ChemCompt has just one voxel."
                                        "This function will invalidate any concentration term in"
                                        "the model. If you don't know why you would want to do this,"
                                        "then you shouldn't use this function.",
                                        new OpFunc1< ChemCompt, double >(
                                            &ChemCompt::setVolumeNotRates )
                                      );

    static DestFinfo handleNodeInfo( "handleNodeInfo",
                                     "Tells ChemCompt how many nodes and threads per node it is "
                                     "allowed to use. Triggers a return meshSplitOut message.",
                                     new EpFunc2< ChemCompt, unsigned int, unsigned int >(
                                         &ChemCompt::handleNodeInfo )
                                   );

    static DestFinfo resetStencil( "resetStencil",
                                   "Resets the diffusion stencil to the core stencil that only "
                                   "includes the within-mesh diffusion. This is needed prior to "
                                   "building up the cross-mesh diffusion through junctions.",
                                   new OpFunc0< ChemCompt >(
                                       &ChemCompt::resetStencil )
                                 );


    //////////////////////////////////////////////////////////////
    // Field Elements
    //////////////////////////////////////////////////////////////

    static FieldElementFinfo< ChemCompt, MeshEntry > entryFinfo(
        "mesh",
        "Field Element for mesh entries",
        MeshEntry::initCinfo(),
        &ChemCompt::lookupEntry,
        &ChemCompt::setNumEntries,
        &ChemCompt::getNumEntries,
        false
    );

    static Finfo* chemMeshFinfos[] =
    {
        &volume,			// Value
        &voxelVolume,		// ReadOnlyLookupValue
        &voxelMidpoint,		// ReadOnlyLookupValue
        &oneVoxelVolume,	// ReadOnlyLookupValue
        &numDimensions,	// ReadOnlyValue
        &stencilRate,	// ReadOnlyLookupValue
        &stencilIndex,	// ReadOnlyLookupValue
        &isMembraneBound,	// Value
        voxelVolOut(),	// SrcFinfo
        &buildDefaultMesh,	// DestFinfo
        &setVolumeNotRates,		// DestFinfo
        &resetStencil,	// DestFinfo
        &entryFinfo,	// FieldElementFinfo
    };

    static string doc[] =
    {
        "Name", "ChemCompt",
        "Author", "Upi Bhalla",
        "Description", "Pure virtual base class for chemical compartments"

    };
    static Dinfo< short > dinfo;
    static Cinfo chemMeshCinfo (
        "ChemCompt",
        Neutral::initCinfo(),
        chemMeshFinfos,
        sizeof( chemMeshFinfos ) / sizeof ( Finfo* ),
        &dinfo,
        doc,
        sizeof(doc)/sizeof( string ),
        true // This is a base class, not be be created directly.
    );

    return &chemMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* chemMeshCinfo = ChemCompt::initCinfo();

ChemCompt::ChemCompt()
    :
    entry_( this ),
    isMembraneBound_( false )
{
    ;
}

ChemCompt::~ChemCompt()
{
    /*
    for ( unsigned int i = 0; i < stencil_.size(); ++i ) {
    if ( stencil_[i] )
    	delete stencil_[i];
    }
    */
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void ChemCompt::buildDefaultMesh( const Eref& e,
                                  double volume, unsigned int numEntries )
{
    this->innerBuildDefaultMesh( e, volume, numEntries );
}

void ChemCompt::handleNodeInfo( const Eref& e,
                                unsigned int numNodes, unsigned int numThreads )
{
    // Pass it down to derived classes along with the SrcFinfo
    innerHandleNodeInfo( e, numNodes, numThreads );
}

void ChemCompt::resetStencil()
{
    this->innerResetStencil();
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ChemCompt::setEntireVolume( const Eref& e, double volume )
{
    // If the reac system is not solved, then explicitly do scaling
    vector< ObjId > tgtVec =
        e.element()->getMsgTargets( e.dataIndex(), voxelVolOut() );
    if ( tgtVec.size() == 0 )
    {
        vector< double > childConcs;
        getChildConcs( e, childConcs );
        if ( vSetVolumeNotRates( volume ) )
        {
            setChildConcs( e, childConcs, 0 );
        }
    }
    else
    {
        vSetVolumeNotRates( volume );
        voxelVolOut()->send( e, this->vGetVoxelVolume() );
    }
}

double ChemCompt::getEntireVolume( const Eref& e ) const
{
    return vGetEntireVolume();
}

void ChemCompt::getChildConcs( const Eref& e, vector< double >& childConcs ) const
{
    vector< Id > kids;
    Neutral::children( e, kids );
    for ( vector < Id >::iterator i = kids.begin(); i != kids.end(); ++i )
    {
        if ( i->element()->cinfo()->isA( "PoolBase" ) )
        {
            childConcs.push_back( Field< double >::get( *i, "conc" ) );
            childConcs.push_back( Field< double >::get( *i, "concInit" ) );
        }
        else if ( i->element()->cinfo()->isA( "ReacBase" ) )
        {
            childConcs.push_back( Field< double >::get( *i, "Kf" ) );
            childConcs.push_back( Field< double >::get( *i, "Kb" ) );
        }
        else if ( i->element()->cinfo()->isA( "EnzBase" ) )
        {
            childConcs.push_back( Field< double >::get( *i, "Km" ) );
        }
        else if ( i->element()->cinfo()->isA( "ChemCompt" ) )
        {
            // Do NOT traverse into child ChemCompts, they look after their
            // own volumes.
            continue;
        }
        getChildConcs( i->eref(), childConcs );
    }
}

unsigned int ChemCompt::setChildConcs( const Eref& e,
        const vector< double >& conc, unsigned int start) const
{
    vector< Id > kids;
    Neutral::children( e, kids );
    for ( vector < Id >::iterator i = kids.begin(); i != kids.end(); ++i )
    {
        if ( i->element()->cinfo()->isA( "PoolBase" ) )
        {
            Field< double >::set( *i, "conc", conc[ start++ ] );
            Field< double >::set( *i, "concInit", conc[start++] );
        }
        else if ( i->element()->cinfo()->isA( "ReacBase" ) )
        {
            Field< double >::set( *i, "Kf", conc[ start++ ] );
            Field< double >::set( *i, "Kb", conc[ start++ ] );
        }
        else if ( i->element()->cinfo()->isA( "EnzBase" ) )
        {
            Field< double >::set( *i, "Km", conc[ start++ ] );
        }
        else if ( i->element()->cinfo()->isA( "ChemCompt" ) )
        {
            // Do NOT traverse into child ChemCompts, they look after their
            // own volumes.
            continue;
        }
        start = setChildConcs( i->eref(), conc, start );
    }
    return start;
}

vector< double > ChemCompt::getVoxelVolume() const
{
    return this->vGetVoxelVolume();
}

vector< double > ChemCompt::getVoxelMidpoint() const
{
    return this->vGetVoxelMidpoint();
}

double ChemCompt::getOneVoxelVolume( const Eref& e, unsigned int dataIndex ) const
{
    return this->getMeshEntryVolume( dataIndex );
}

void ChemCompt::setOneVoxelVolume( const Eref& e, unsigned int dataIndex, double volume )
{
    this->setMeshEntryVolume( dataIndex, volume );
}

// A virtual function, meant to be overridden.
void ChemCompt::setMeshEntryVolume( unsigned int dataIndex, double volume )
{
    cout << "Warning: ChemCompt::setMeshEntryVolume: Undefined except for PSD and spine mesh.\n";
}

unsigned int ChemCompt::getDimensions() const
{
    return this->innerGetDimensions();
}

vector< double > ChemCompt::getStencilRate( unsigned int row ) const
{
    return this->innerGetStencilRate( row );
}

vector< unsigned int > ChemCompt::getStencilIndex( unsigned int row ) const
{
    return this->getNeighbors( row );
}

bool ChemCompt::getIsMembraneBound() const
{
    return isMembraneBound_;
}

void ChemCompt::setIsMembraneBound( bool v )
{
    isMembraneBound_ = v;
}


//////////////////////////////////////////////////////////////
// Dest Definitions
//////////////////////////////////////////////////////////////

void ChemCompt::setVolumeNotRates( double volume )
{
    vSetVolumeNotRates( volume ); // Pass on to derived classes.
}

//////////////////////////////////////////////////////////////
// Element Field Definitions
//////////////////////////////////////////////////////////////

MeshEntry* ChemCompt::lookupEntry( unsigned int index )
{
    return &entry_;
}

void ChemCompt::setNumEntries( unsigned int num )
{
    this->innerSetNumEntries( num );
    // cout << "Warning: ChemCompt::setNumEntries: No effect. Use subclass-specific functions\nto build or resize mesh.\n";
}

unsigned int ChemCompt::getNumEntries() const
{
    return this->innerGetNumEntries();
}


//////////////////////////////////////////////////////////////
// Build the junction between this and another ChemCompt.
// This one function does the work for both meshes.
//////////////////////////////////////////////////////////////
void ChemCompt::buildJunction( ChemCompt* other, vector< VoxelJunction >& ret)
{
    matchMeshEntries( other, ret );
    extendStencil( other, ret );
}

void ChemCompt::flipRet( vector< VoxelJunction >& ret ) const
{
    vector< VoxelJunction >::iterator i;
    for ( i = ret.begin(); i != ret.end(); ++i )
    {
        unsigned int temp = i->first;
        i->first = i->second;
        i->second = temp;
        double vol = i->firstVol;
        i->firstVol = i->secondVol;
        i->secondVol = vol;
    }
}

////////////////////////////////////////////////////////////////////////
// Utility function

double ChemCompt::distance( double x, double y, double z )
{
    return sqrt(x*x + y*y + z*z);
}
