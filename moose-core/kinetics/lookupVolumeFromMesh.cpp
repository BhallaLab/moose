/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "lookupVolumeFromMesh.h"

// Utility function: return the compartment in which the specified
// object is located.
// Simply traverses the tree toward the root till it finds a
// compartment. Pools use a special msg, but this works for reacs too.
ObjId getCompt( Id id )
{
    ObjId pa = Neutral::parent( id.eref() ).id;
    if ( pa == ObjId() )
        return pa;
    else if ( pa.element()->cinfo()->isA( "ChemCompt" ) )
        return pa;
    return getCompt( pa );
}


/// Utility function to find the size of a pool. We assume one-to-one
/// match between pool indices and mesh indices: that is what they are for.
double lookupVolumeFromMesh( const Eref& e )
{
    ObjId compt = getCompt( e.id() );
    if ( compt == ObjId() )
        return 1.0;
    return LookupField< unsigned int, double >::
           get( compt, "oneVoxelVolume", e.dataIndex() );
}

/**
 * Figures out all the volumes of the substrates or products on the
 * specified reaction 'reac'. The SrcFinfo is for the sub or prd msg.
 * Returns the index of the smallest vol. Passes back a vector of volumes.
 * The meshIndex is zero. Reasoning is as follows: both in the case of
 * well-stirred (single mesh entry) models, and in the case of spatial
 * models with consistent mesh sizes and alignments, the mesh entry
 * volumes are in the same ratio.
 * Cases with more complex arrangements may also use the current vols as
 * a starting point, but will need to add index-specific scaling factors
 * to their reaction system.
 */

unsigned int getReactantVols( const Eref& reac, const SrcFinfo* pools,
                              vector< double >& vols )
{
    static const unsigned int meshIndex = 0;

    const vector< MsgFuncBinding >* mfb =
        reac.element()->getMsgAndFunc( pools->getBindIndex() );
    unsigned int smallIndex = 0;

    vols.resize( 0 );
    if ( mfb )
    {
        for ( unsigned int i = 0; i < mfb->size(); ++i )
        {
            double v = 1;
            Element* pool = Msg::getMsg( (*mfb)[i].mid )->e2();
            if ( pool == reac.element() )
                pool = Msg::getMsg( (*mfb)[i].mid )->e1();
            assert( pool != reac.element() );
            Eref pooler( pool, meshIndex );
            if ( pool->cinfo()->isA( "PoolBase" ) )
            {
                v = lookupVolumeFromMesh( pooler );
            }
            else
            {
                cout << "Error: getReactantVols: pool is of unknown type\n";
                assert( 0 );
            }
            vols.push_back( v );
            if ( v < vols[0] )
                smallIndex = i;
        }
    }
    return smallIndex;
}

/**
 * Returns conversion factor to convert rates from concentration to
 * mol# units.
 * Handles arbitrary combinations of volumes.
 * Assumes that the reference volume for computing rates is the
 * smallest volume.
 * 26 Feb 2013: This is now changed to use the volume of the first entry.
 * Should only be used for substrates. For products need to find the
 * first substrate, separately, and use that to scale down the conv factor.
 * Assumes all calculations are in SI: cubic metres and millimolar.
 * 27 Feb 2013: This is changed to use the volume of a voxel of the
 * the home compartment of the reac.
 * Be warned: this can cause unexpected problems if the home compartment
 * isn't according to convention. For example, if there is a single
 * substrate and the home compartment is elsewhere, you will get very odd
 * Kf:kf values.
 * 9 Oct 2013: This is now changed to use the volume of the first
 * substrate. Note that if the conversion is for products, then the
 * routine has to look up the substrate list to get the first substrate.
 * Reason is that the home compartment was often wrong in ReadKkit.
 * Unfortunately this may yet cause problems with SBML. I don't know what
 * conventions they use for cross-compartment reactions.
 */

double convertConcToNumRateUsingMesh( const Eref& e, const SrcFinfo* pools,
                                      bool doPartialConversion )
{
    vector< double > vols;
    // unsigned int smallest = getReactantVols( e, pools, vols );
    double conv = 1.0;
    getReactantVols( e, pools, vols );
    if ( vols.size() == 0 )   // Should this be an assertion?
    {
        // cout << "Warning: convertConcToNumRateUsingMesh: zero reactants on " << e.id().path() << endl;
        return 1.0;
    }
    for ( unsigned int i = 0; i < vols.size(); ++i )
    {
        conv *= vols[i] * NA;
    }
    if ( !doPartialConversion )
    {
        if ( pools->name() == "subOut" )
        {
            conv /= vols[0] * NA;
        }
        else
        {
            const Finfo* f = e.element()->cinfo()->findFinfo( "subOut" );
            assert( f );
            const SrcFinfo* toSub = dynamic_cast< const SrcFinfo* >( f );
            assert( toSub );
            vector< double > subVols;
            getReactantVols( e, toSub, subVols );
            if ( subVols.size() == 0 ) // no substrates!
                return 1.0;
            conv /= subVols[0] * NA;
        }
        /*
        Id compt = getCompt( e.id() );
        if ( compt != Id() ) {
        	Id mesh( compt.value() + 1 );
        	double meshVol = Field< double >::get( mesh, "volume" );
        	conv /= meshVol * NA;
        }
        */
    }
    return conv;
}


/**
 * Generates conversion factor for rates from concentration to mol# units.
 * This variant already knows the volume, but has to figure out # of
 * reactants.
 */
double convertConcToNumRateUsingVol( const Eref& e, const SrcFinfo* pools,
                                     double volume, double scale, bool doPartialConversion )
{
    const vector< MsgFuncBinding >* mfb =
        e.element()->getMsgAndFunc( pools->getBindIndex() );
    double conversion = 1.0;
    if ( mfb && mfb->size() > 0 )
    {
        if ( doPartialConversion || mfb->size() > 1 )
        {
            conversion = scale * NA * volume;
            double power = doPartialConversion + mfb->size() - 1;
            if ( power > 1.0 )
            {
                conversion = pow( conversion, power );
            }
        }
        if ( conversion <= 0 )
            conversion = 1.0;
    }

    return conversion;
}

/**
 * Generates conversion factor for rates from concentration to mol# units.
 * This variant is used when the reactants are in different compartments
 * or mesh entries, and may therefore have different volumes.
 * We already know the reactants and their affiliations.
 */
double convertConcToNumRateInTwoCompts( double v1, unsigned int n1,
                                        double v2, unsigned int n2, double scale )
{
    double conversion = 1.0;

    for ( unsigned int i = 1; i < n1; ++i )
        conversion *= scale * NA * v1;
    for ( unsigned int i = 0; i < n2; ++i )
        conversion *= scale * NA * v2;

    if ( conversion <= 0 )
        conversion = 1.0;

    return conversion;
}
