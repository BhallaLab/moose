/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "../basecode/SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////

VoxelPoolsBase::VoxelPoolsBase() :
    stoichPtr_( 0 ),
    S_(1),
    Sinit_(1),
    volume_(1.0)
{
    ;
}

VoxelPoolsBase::~VoxelPoolsBase()
{}

//////////////////////////////////////////////////////////////
// Array ops
//////////////////////////////////////////////////////////////
/// Using the computed array sizes, now allocate space for them.
void VoxelPoolsBase::resizeArrays( unsigned int totNumPools )
{
    S_.resize( totNumPools, 0.0 );
    Sinit_.resize( totNumPools, 0.0);
}

void VoxelPoolsBase::reinit()
{
    S_ = Sinit_;
}

//////////////////////////////////////////////////////////////
// Access functions
//////////////////////////////////////////////////////////////
const double* VoxelPoolsBase::S() const
{
    return &S_[0];
}

vector< double >& VoxelPoolsBase::Svec()
{
    return S_;
}

double* VoxelPoolsBase::varS()
{
    return &S_[0];
}

const double* VoxelPoolsBase::Sinit() const
{
    return &Sinit_[0];
}

double* VoxelPoolsBase::varSinit()
{
    return &Sinit_[0];
}

unsigned int VoxelPoolsBase::size() const
{
    return Sinit_.size();
}

void VoxelPoolsBase::setVolume( double vol )
{
    volume_ = vol;
}

double VoxelPoolsBase::getVolume() const
{
    return volume_;
}

void VoxelPoolsBase::setVolumeAndDependencies( double vol )
{
    double ratio = vol / volume_;
    volume_ = vol;
    for ( vector< double >::iterator
            i = Sinit_.begin(); i != Sinit_.end(); ++i )
        *i *= ratio;

    for ( vector< double >::iterator i = S_.begin(); i != S_.end(); ++i )
        *i *= ratio;

    // I would like to update the xReacScaleSubstreates and Products here,
    // but I don't know the order of their reactions. So leave it to
    // a subsequent call via Ksolve or Stoich.
}

void VoxelPoolsBase::scaleVolsBufsRates(double ratio, const Stoich* stoichPtr)
{
    volume_ *= ratio; // Scale vol
    for ( vector< double >::iterator
            i = Sinit_.begin(); i != Sinit_.end(); ++i )
        *i *= ratio; // Scale Bufs
    // Here we also need to set the Ns for the buffered pools.
    unsigned int start = stoichPtr_->getNumVarPools();
    unsigned int end = start + stoichPtr_->getNumBufPools();
    assert( end == Sinit_.size() );
    for ( unsigned int i = start; i < end; ++i )
    {
        // Must not reassign pools that are controlled by functions.
        if ( !stoichPtr->isFuncTarget(i) )
            S_[i] = Sinit_[i];
    }

    // Scale rates. Start by clearing out old rates if any
    for ( unsigned int i = 0; i < rates_.size(); ++i )
        delete( rates_[i] );

    unsigned int numCoreRates = stoichPtr->getNumCoreRates();
    const vector< RateTerm* >& rates = stoichPtr->getRateTerms();
    rates_.resize( rates.size() );

    for ( unsigned int i = 0; i < numCoreRates; ++i )
        rates_[i] = rates[i]->copyWithVolScaling( getVolume(), 1, 1 );

    for ( unsigned int i = numCoreRates; i < rates.size(); ++i )
    {
        rates_[i] = rates[i]->copyWithVolScaling(  getVolume(),
                    getXreacScaleSubstrates(i - numCoreRates),
                    getXreacScaleProducts(i - numCoreRates ) );
    }
}

//////////////////////////////////////////////////////////////
// Zombie Pool Access functions
//////////////////////////////////////////////////////////////

void VoxelPoolsBase::setN( unsigned int i, double v )
{
    S_[i] = v;
    if ( S_[i] < 0.0 )
        S_[i] = 0.0;
}

double VoxelPoolsBase::getN( unsigned int i ) const
{
    return S_[i];
}

void VoxelPoolsBase::setNinit( unsigned int i, double v )
{
    Sinit_[i] = v;
    if ( Sinit_[i] < 0.0 )
        Sinit_[i] = 0.0;
}

double VoxelPoolsBase::getNinit( unsigned int i ) const
{
    return Sinit_[i];
}

void VoxelPoolsBase::setDiffConst( unsigned int i, double v )
{
    ; // Do nothing.
}

double VoxelPoolsBase::getDiffConst( unsigned int i ) const
{
    return 0;
}

//////////////////////////////////////////////////////////////
// Handle cross compartment reactions
//////////////////////////////////////////////////////////////
void VoxelPoolsBase::xferIn(
    const vector< unsigned int >& poolIndex,
    const vector< double >& values,
    const vector< double >& lastValues,
    unsigned int voxelIndex	)
{
    unsigned int offset = voxelIndex * poolIndex.size();
    vector< double >::const_iterator i = values.begin() + offset;
    vector< double >::const_iterator j = lastValues.begin() + offset;
    for ( vector< unsigned int >::const_iterator
            k = poolIndex.begin(); k != poolIndex.end(); ++k )
    {
        S_[*k] += *i++ - *j++;
    }
}

void VoxelPoolsBase::xferInOnlyProxies(
    const vector< unsigned int >& poolIndex,
    const vector< double >& values,
    unsigned int numProxyPools,
    unsigned int voxelIndex	)
{
    unsigned int offset = voxelIndex * poolIndex.size();
    vector< double >::const_iterator i = values.begin() + offset;
    unsigned int proxyEndIndex = stoichPtr_->getNumVarPools() +
                                 stoichPtr_->getNumProxyPools();
    for ( vector< unsigned int >::const_iterator
            k = poolIndex.begin(); k != poolIndex.end(); ++k )
    {
        // if ( *k >= S_.size() - numProxyPools )
        if ( *k >= stoichPtr_->getNumVarPools() && *k < proxyEndIndex )
        {
            // cout << S_[*k] << ", " << Sinit_[*k] << ", " << *i <<  endl;
            Sinit_[*k] = *i;
            S_[*k] = *i;
        }
        i++;
    }
}

void VoxelPoolsBase::xferOut(
    unsigned int voxelIndex,
    vector< double >& values,
    const vector< unsigned int >& poolIndex)
{
    unsigned int offset = voxelIndex * poolIndex.size();
    vector< double >::iterator i = values.begin() + offset;
    for ( vector< unsigned int >::const_iterator
            k = poolIndex.begin(); k != poolIndex.end(); ++k )
    {
        *i++ = S_[*k];
    }
}

void VoxelPoolsBase::addProxyVoxy(
    unsigned int comptIndex, Id otherComptId, unsigned int voxel )
{
    if ( comptIndex >= proxyPoolVoxels_.size() )
    {
        proxyPoolVoxels_.resize( comptIndex + 1 );
    }

    proxyPoolVoxels_[comptIndex].push_back( voxel );
    proxyComptMap_[otherComptId] = comptIndex;
}

void VoxelPoolsBase::addProxyTransferIndex(
    unsigned int comptIndex, unsigned int transferIndex )
{
    if ( comptIndex >= proxyTransferIndex_.size() )
        proxyTransferIndex_.resize( comptIndex + 1 );
    proxyTransferIndex_[comptIndex].push_back( transferIndex );
}

bool VoxelPoolsBase::hasXfer( unsigned int comptIndex ) const
{
    if ( comptIndex >= proxyPoolVoxels_.size() )
        return false;
    return (proxyPoolVoxels_[ comptIndex ].size() > 0);
}

bool VoxelPoolsBase::isVoxelJunctionPresent( Id i1, Id i2 ) const
{
    if ( i1 == Id () )
        return false;
    if ( proxyComptMap_.find( i1 ) == proxyComptMap_.end() )
        return false;
    if ( i2 == Id() ) // This is intentionally blank, only one jn.
        return true;
    // If there is an i2 but it isn't on the map, then not connected.
    if ( proxyComptMap_.find( i2 ) == proxyComptMap_.end() )
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////
// Cross reaction stuff.
////////////////////////////////////////////////////////////////////

void VoxelPoolsBase::resetXreacScale( unsigned int size )
{
    xReacScaleSubstrates_.assign( size, 1.0 );
    xReacScaleProducts_.assign( size, 1.0 );
}

void VoxelPoolsBase::forwardReacVolumeFactor( unsigned int i, double volume )
{
    assert( i < xReacScaleSubstrates_.size() );
    xReacScaleSubstrates_[i] *= volume / getVolume();
    // cout << "forwardReacVolumeFactor[" << i << "] = " << xReacScaleSubstrates_[i] <<endl;
}

void VoxelPoolsBase::backwardReacVolumeFactor( unsigned int i, double volume )
{
    assert( i < xReacScaleProducts_.size() );
    xReacScaleProducts_[i] *= volume / getVolume();
    // cout << "backwardReacVolumeFactor[" << i << "] = "<< xReacScaleProducts_[i] <<endl;
}

double VoxelPoolsBase::getXreacScaleSubstrates( unsigned int i ) const
{
    return xReacScaleSubstrates_[i];
}

double VoxelPoolsBase::getXreacScaleProducts( unsigned int i ) const
{
    return xReacScaleProducts_[i];
}

/**
 * Zeroes out rate terms that are involved in cross-reactions that
 * are not present on current voxel.
 */
void VoxelPoolsBase::filterCrossRateTerms(
    const vector< Id >& offSolverReacs,
    const vector< pair< Id, Id > >&  offSolverReacCompts  )
{
    assert (offSolverReacs.size() == offSolverReacCompts.size() );
    // unsigned int numCoreRates = stoichPtr_->getNumCoreRates();
    for ( unsigned int i = 0; i < offSolverReacCompts.size(); ++i )
    {
        const pair< Id, Id >& p = offSolverReacCompts[i];
        if ( !isVoxelJunctionPresent( p.first, p.second) )
        {
            Id reacId = offSolverReacs[i];
            const Cinfo* reacCinfo = reacId.element()->cinfo();
            unsigned int k = stoichPtr_->convertIdToReacIndex( offSolverReacs[i] );
            // Start by replacing the immediate cross reaction term.
            if ( rates_[k] )
                delete rates_[k];
            rates_[k] = new ExternReac;
            if ( stoichPtr_->getOneWay() )
            {
                k++; // Delete the next entry too, it is the reverse reacn.
                assert( k < rates_.size() );
                if ( reacCinfo->isA( "ReacBase" ) )
                {
                    if ( rates_[k] )
                        delete rates_[k];
                    rates_[k] = new ExternReac;
                }
                if ( reacCinfo->isA( "CplxEnzBase" ) )   // Delete next two.
                {
                    if ( rates_[k] )
                        delete rates_[k];
                    rates_[k] = new ExternReac;
                    k++;
                    assert( k < rates_.size() );
                    if ( rates_[k] )
                        delete rates_[k];
                    rates_[k] = new ExternReac;
                }
            }
            else
            {
                if ( reacCinfo->isA( "CplxEnzBase" ) )   // Delete next one.
                {
                    k++;
                    assert( k < rates_.size() );
                    if ( rates_[k] )
                        delete rates_[k];
                    rates_[k] = new ExternReac;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
void VoxelPoolsBase::print() const
{
    cout << "S_.size=" << S_.size() << ", volume = " << volume_ << endl;
    cout << "proxyPoolsVoxels.size()=" << proxyPoolVoxels_.size() <<
         ", proxyTransferIndex.size()=" << proxyTransferIndex_.size() <<
         endl;
    assert( proxyPoolVoxels_.size() == proxyTransferIndex_.size() );
    for ( unsigned int i = 0; i < proxyPoolVoxels_.size(); ++i )
    {
        cout << "ppv[" << i << "]=";
        const vector< unsigned int >& ppv = proxyPoolVoxels_[i];
        for ( unsigned int j = 0; j < ppv.size(); ++j )
        {
            cout << "	" << ppv[j];
        }
        cout << endl;
    }
    for ( unsigned int i = 0; i < proxyTransferIndex_.size(); ++i )
    {
        cout << "pti[" << i << "]=";
        const vector< unsigned int >& pti = proxyTransferIndex_[i];
        for ( unsigned int j = 0; j < pti.size(); ++j )
        {
            cout << "	" << pti[j];
        }
        cout << endl;
    }
    cout <<
         "xReacScaleSubstrates.size()=" << xReacScaleSubstrates_.size() <<
         ", xReacScaleProducts.size()=" << xReacScaleProducts_.size() <<
         endl;
    assert( xReacScaleSubstrates_.size() == xReacScaleProducts_.size() );
    for ( unsigned int i = 0; i < xReacScaleSubstrates_.size(); ++i )
    {
        cout << i << "	" << xReacScaleSubstrates_[i] << "	" <<
             xReacScaleProducts_[i] << endl;
    }
    cout << "##############    RATES    ######################\n";
    for ( unsigned int i = 0; i < rates_.size(); ++i )
    {
        cout << i << "	:	" << rates_[i]->getR1() << ",	" <<
             rates_[i]->getR2() << endl;
    }
}
