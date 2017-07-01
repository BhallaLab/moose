/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"
#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#endif

#include "VoxelPoolsBase.h"
// #include "VoxelPools.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"

#include "RateTerm.h"
#include "FuncTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"
#include "../shell/Shell.h"
#include "../mesh/MeshEntry.h"
#include "../mesh/Boundary.h"
#include "../mesh/ChemCompt.h"

ZombiePoolInterface::ZombiePoolInterface()
		: stoich_(), compartment_(),
		isBuilt_( false )
{;}

//////////////////////////////////////////////////////////////////////////
// cross-compartment reaction stuff.
//////////////////////////////////////////////////////////////////////////
// void ZombiePoolInterface::xComptIn( const Eref& e, const ObjId& src,
// vector< double > values )
void ZombiePoolInterface::xComptIn( const Eref& e, Id srcZombiePoolInterface,
	vector< double > values )
{
	// Identify the xfer_ that maps to the srcZombiePoolInterface. Assume only a small
	// number of them, otherwise we should use a map.
	unsigned int comptIdx ;
	for ( comptIdx = 0 ; comptIdx < xfer_.size(); ++comptIdx ) {
		if ( xfer_[comptIdx].ksolve == srcZombiePoolInterface ) break;
	}
	assert( comptIdx != xfer_.size() );
	XferInfo& xf = xfer_[comptIdx];
	// assert( values.size() == xf.values.size() );
	xf.values = values;
//	xfer_[comptIdx].lastValues = values;
}

/////////////////////////////////////////////////////////////////////
// Functions for setup of cross-compartment transfer.
/////////////////////////////////////////////////////////////////////
/**
 * Figures out which voxels are involved in cross-compt reactions. Stores
 * in the appropriate xfer_ entry.
 */
void ZombiePoolInterface::assignXferVoxels( unsigned int xferCompt )
{
	assert( xferCompt < xfer_.size() );
	XferInfo& xf = xfer_[xferCompt];
	for ( unsigned int i = 0; i < getNumLocalVoxels(); ++i ) {
		if ( pools(i)->hasXfer( xferCompt ) )
			xf.xferVoxel.push_back( i );
	}
	xf.values.resize( xf.xferVoxel.size() * xf.xferPoolIdx.size(), 0 );
	xf.lastValues.resize( xf.xferVoxel.size() * xf.xferPoolIdx.size(), 0 );
	xf.subzero.resize( xf.xferVoxel.size() * xf.xferPoolIdx.size(), 0 );
}

/**
 * Figures out indexing of the array of transferred pool n's used to fill
 * out proxies on each timestep.
 */
void ZombiePoolInterface::assignXferIndex( unsigned int numProxyMols,
		unsigned int xferCompt,
		const vector< vector< unsigned int > >& voxy )
{
	unsigned int idx = 0;
	for ( unsigned int i = 0; i < voxy.size(); ++i ) {
		const vector< unsigned int >& rpv = voxy[i];
		if ( rpv.size()  > 0) { // There would be a transfer here
			for ( vector< unsigned int >::const_iterator
					j = rpv.begin(); j != rpv.end(); ++j ) {
				pools(*j)->addProxyTransferIndex( xferCompt, idx );
			}
			idx += numProxyMols;
		}
	}
}

/**
 * This function sets up the information about the pool transfer for
 * cross-compartment reactions. It consolidates the transfer into a
 * distinct vector for each direction of the transfer between each coupled
 * pair of ZombiePoolInterfaces.
 * This one call sets up the information about transfer on both sides
 * of the junction(s) between current ZombiePoolInterface and otherZombiePoolInterface.
 *
 * VoxelJunction refers to a specific junction between compartments.
 * It has the following relevant fields:
 * 	first, second: VoxelIndex for the first and second compartments.
 * 	firstVol, secondVol: VoxelVolume for the first and second compartments.
 */
void ZombiePoolInterface::setupXfer( Id myZombiePoolInterface, Id otherZombiePoolInterface,
	unsigned int numProxyMols, const vector< VoxelJunction >& vj )
{
	const ChemCompt *myCompt = reinterpret_cast< const ChemCompt* >(
			compartment_.eref().data() );
	ZombiePoolInterface* otherZombiePoolInterfacePtr =
			reinterpret_cast< ZombiePoolInterface* >(
					otherZombiePoolInterface.eref().data() );
	const ChemCompt *otherCompt = reinterpret_cast< const ChemCompt* >(
			otherZombiePoolInterfacePtr->compartment_.eref().data() );
	// Use this so we can figure out what the other side will send.
	vector< vector< unsigned int > > proxyVoxy( myCompt->getNumEntries() );
	vector< vector< unsigned int > > reverseProxyVoxy( otherCompt->getNumEntries() );
	assert( xfer_.size() > 0 && otherZombiePoolInterfacePtr->xfer_.size() > 0 );
	unsigned int myZombiePoolInterfaceIndex = xfer_.size() -1;
	unsigned int otherZombiePoolInterfaceIndex = otherZombiePoolInterfacePtr->xfer_.size() -1;
	for ( unsigned int i = 0; i < vj.size(); ++i ) {
		unsigned int j = vj[i].first;
		assert( j < getNumLocalVoxels() ); // Check voxel indices.
		proxyVoxy[j].push_back( vj[i].second );
		pools(j)->addProxyVoxy( myZombiePoolInterfaceIndex,
						otherZombiePoolInterfacePtr->compartment_, vj[i].second);
		unsigned int k = vj[i].second;
		assert( k < otherCompt->getNumEntries() );
		reverseProxyVoxy[k].push_back( vj[i].first );
		otherZombiePoolInterfacePtr->pools(k)->addProxyVoxy(
			otherZombiePoolInterfaceIndex, compartment_, vj[i].first );
	}

	// Build the indexing for the data values to transfer on each timestep
	assignXferIndex( numProxyMols, myZombiePoolInterfaceIndex, reverseProxyVoxy );
	otherZombiePoolInterfacePtr->assignXferIndex(
			numProxyMols, otherZombiePoolInterfaceIndex, proxyVoxy );
	// Figure out which voxels participate in data transfer.
	assignXferVoxels( myZombiePoolInterfaceIndex );
	otherZombiePoolInterfacePtr->assignXferVoxels( otherZombiePoolInterfaceIndex );
}


/**
 * Builds up the list of proxy pools on either side of the junction,
 * and assigns to the XferInfo data structures for use during runtime.
 */
unsigned int ZombiePoolInterface::assignProxyPools( const map< Id, vector< Id > >& xr,
				Id myZombiePoolInterface, Id otherZombiePoolInterface, Id otherComptId )
{
	map< Id, vector< Id > >::const_iterator i = xr.find( otherComptId );
	vector< Id > proxyMols;
	if ( i != xr.end() )
		proxyMols = i->second;
	ZombiePoolInterface* otherZombiePoolInterfacePtr =
			reinterpret_cast< ZombiePoolInterface* >(
					otherZombiePoolInterface.eref().data() );

	vector< Id > otherProxies = LookupField< Id, vector< Id > >::get(
			otherZombiePoolInterfacePtr->stoich_, "proxyPools", stoich_ );

	proxyMols.insert( proxyMols.end(),
					otherProxies.begin(), otherProxies.end() );
	// if ( proxyMols.size() == 0 )
		// return 0;
	sort( proxyMols.begin(), proxyMols.end() );
	xfer_.push_back( XferInfo( otherZombiePoolInterface ) );

	otherZombiePoolInterfacePtr->xfer_.push_back( XferInfo( myZombiePoolInterface ) );
	vector< unsigned int >& xfi = xfer_.back().xferPoolIdx;
	vector< unsigned int >& oxfi = otherZombiePoolInterfacePtr->xfer_.back().xferPoolIdx;
	xfi.resize( proxyMols.size() );
	oxfi.resize( proxyMols.size() );
	for ( unsigned int i = 0; i < xfi.size(); ++i ) {
		xfi[i] = getPoolIndex( proxyMols[i].eref() );
		oxfi[i] = otherZombiePoolInterfacePtr->getPoolIndex(
						proxyMols[i].eref() );
	}
	return proxyMols.size();
}


// This function cleans out the RateTerms of cross reactions that
// don't have anything to connect to.
// It should be called after all cross reacs have been assigned.
void ZombiePoolInterface::filterCrossRateTerms( const vector< Id >& xreacs,
			   	const vector< pair< Id, Id > >& xrt )
{
	for (unsigned int i = 0; i < getNumLocalVoxels(); ++i ) {
		pools(i)->filterCrossRateTerms( xreacs, xrt );
	}
}

/**
 * This function builds cross-solver reaction calculations. For the
 * specified pair of stoichs (this->stoich_, otherStoich) it identifies
 * interacting molecules, finds where the junctions are, sets up the
 * info to build the data transfer vector, and sets up the transfer
 * itself.
 */
void ZombiePoolInterface::setupCrossSolverReacs( const map< Id, vector< Id > >& xr,
	   Id otherStoich )
{
	const ChemCompt *myCompt = reinterpret_cast< const ChemCompt* >(
			compartment_.eref().data() );
	Id otherComptId = Field< Id >::get( otherStoich, "compartment" );
	Id myZombiePoolInterface = Field< Id >::get( stoich_, "ksolve" );
	if ( myZombiePoolInterface == Id() )
		return;
	Id otherZombiePoolInterface = Field< Id >::get( otherStoich, "ksolve" );
	if ( otherZombiePoolInterface == Id() )
		return;

	// Establish which molecules will be exchanged.
	unsigned int numPools = assignProxyPools( xr, myZombiePoolInterface, otherZombiePoolInterface,
					otherComptId );
	if ( numPools == 0 ) return;

	// Then, figure out which voxels do the exchange.
	// Note that vj has a list of pairs of voxels on either side of a
	// junction. If one voxel on self touches 5 voxels on other, then
	// there will be five entries in vj for this contact.
	// If one voxel on self touches two different compartments, then
	// a distinct vj vector must be built for those contacts.
	const ChemCompt *otherCompt = reinterpret_cast< const ChemCompt* >(
			otherComptId.eref().data() );
	vector< VoxelJunction > vj;
	myCompt->matchMeshEntries( otherCompt, vj );
	if ( vj.size() == 0 )
		return;

	// This function sets up the information about the pool transfer on
	// both sides.
	setupXfer( myZombiePoolInterface, otherZombiePoolInterface, numPools, vj );

	/// This sets up the volume scaling from cross reac terms
	// Deprecated. Handled by setupCrossSolverReacVols.
	// buildCrossReacVolScaling( otherZombiePoolInterface, vj );

	// Here we set up the messaging.
	Shell *shell = reinterpret_cast< Shell* >( Id().eref().data() );
	shell->doAddMsg( "Single", myZombiePoolInterface, "xCompt", otherZombiePoolInterface, "xCompt" );
}

/**
 * This fills the vols vector with the volume of the abutting
 * voxel on compt. If there are no abutting voxels on a given
 * voxel then that entry of the vols vector is filled with a zero.
 * There is exactly one vols entry for each voxel of the local compt.
 */
void ZombiePoolInterface::matchJunctionVols( vector< double >& vols, Id otherComptId )
		const
{
	vols.resize( getNumLocalVoxels() );
	for ( unsigned int i = 0; i < vols.size(); ++i )
		vols[i] = volume(i);
	if ( otherComptId == compartment_ ) {
		// This may legitimately happen if the substrate or product is
		// on the local compartment.
		// cout << "Warning: ZombiePoolInterface::matchJunctionVols: self compt.\n";
		return;
	}
	const ChemCompt *myCompt = reinterpret_cast< const ChemCompt* >(
			compartment_.eref().data() );
	const ChemCompt *otherCompt = reinterpret_cast< const ChemCompt* >(
			otherComptId.eref().data() );
	vector< VoxelJunction > vj;
	myCompt->matchMeshEntries( otherCompt, vj );
	if ( vj.size() == 0 )
		return;
	for ( vector< VoxelJunction >::const_iterator
			i = vj.begin(); i != vj.end(); ++i ) {
		assert( i->first < vols.size() );
		/*
		if ( !doubleEq( vols[ i->first ], 0.0 ) )
			cout << "Warning: ZombiePoolInterface::matchJuntionVols: repeated voxel\n";
			*/
		vols[ i->first ] = i->secondVol;
	}
}

/**
 * This function builds cross-solver reaction volume scaling.
 */
void ZombiePoolInterface::setupCrossSolverReacVols(
	const vector< vector< Id > >& subCompts,
	const vector< vector< Id > >& prdCompts )
{
	map< Id, vector< double > > comptVolMap;
	const Stoich* stoichPtr = reinterpret_cast< const Stoich* >(
					stoich_.eref().data() );
	unsigned int numOffSolverReacs =
			stoichPtr->getNumRates() - stoichPtr->getNumCoreRates();
	assert( subCompts.size() == numOffSolverReacs );
	assert( prdCompts.size() == numOffSolverReacs );
	for ( unsigned int i = 0 ; i < getNumLocalVoxels(); ++i )
		pools(i)->resetXreacScale( numOffSolverReacs );
	for( unsigned int i = 0; i < numOffSolverReacs; ++i ) {
		for ( unsigned int j = 0; j < subCompts[i].size(); ++j ) {
			map< Id, vector< double > >::iterator cvi;
			vector< double > vols;
			cvi = comptVolMap.find( subCompts[i][j] );
			if ( cvi == comptVolMap.end() ) {
				matchJunctionVols( vols, subCompts[i][j] );
				comptVolMap[subCompts[i][j]] = vols;
			} else {
				vols = cvi->second;
			}
			assert( vols.size() == getNumLocalVoxels() );
			for ( unsigned int k = 0; k < vols.size(); ++k )
				pools(k)->forwardReacVolumeFactor( i, vols[k] );
		}

		for ( unsigned int j = 0; j < prdCompts[i].size(); ++j ) {
			map< Id, vector< double > >::iterator cvi;
			vector< double > vols;
			cvi = comptVolMap.find( prdCompts[i][j] );
			if ( cvi == comptVolMap.end() ) {
				matchJunctionVols( vols, prdCompts[i][j] );
				comptVolMap[prdCompts[i][j]] = vols;
			} else {
				vols = cvi->second;
			}
			assert( vols.size() == getNumLocalVoxels() );
			for ( unsigned int k = 0; k < vols.size(); ++k )
				pools(k)->backwardReacVolumeFactor( i, vols[k] );
		}
	}
}

Id ZombiePoolInterface::getCompartment() const
{
	return compartment_;
}

void ZombiePoolInterface::setCompartment( Id compt )
{
	isBuilt_ = false; // We will have to now rebuild the whole thing.
	if ( compt.element()->cinfo()->isA( "ChemCompt" ) ) {
		compartment_ = compt;
		vector< double > vols =
			Field< vector < double > >::get( compt, "voxelVolume" );
		if ( vols.size() > 0 ) {
			setNumAllVoxels( vols.size() );
			for ( unsigned int i = 0; i < vols.size(); ++i ) {
				pools(i)->setVolume( vols[i] );
			}
		}
	}
}
