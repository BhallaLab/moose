/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "DiffPoolVec.h"
#include "FastMatrixElim.h"
#include "../mesh/VoxelJunction.h"
#include "DiffJunction.h"
#include "Dsolve.h"
#include "../mesh/Boundary.h"
#include "../mesh/MeshEntry.h"
#include "../mesh/ChemCompt.h"
#include "../mesh/MeshCompt.h"
#include "../shell/Wildcard.h"
#include "../kinetics/PoolBase.h"
#include "../kinetics/Pool.h"
#include "../kinetics/BufPool.h"
#include "../ksolve/ZombiePool.h"
#include "../ksolve/ZombieBufPool.h"

const Cinfo* Dsolve::initCinfo()
{
		///////////////////////////////////////////////////////
		// Field definitions
		///////////////////////////////////////////////////////

		static ValueFinfo< Dsolve, Id > stoich (
			"stoich",
			"Stoichiometry object for handling this reaction system.",
			&Dsolve::setStoich,
			&Dsolve::getStoich
		);

		static ElementValueFinfo< Dsolve, string > path (
			"path",
			"Path of reaction system. Must include all the pools that "
			"are to be handled by the Dsolve, can also include other "
			"random objects, which will be ignored.",
			&Dsolve::setPath,
			&Dsolve::getPath
		);

		static ReadOnlyValueFinfo< Dsolve, unsigned int > numVoxels(
			"numVoxels",
			"Number of voxels in the core reac-diff system, on the "
			"current diffusion solver. ",
			&Dsolve::getNumVoxels
		);
		static ReadOnlyValueFinfo< Dsolve, unsigned int > numAllVoxels(
			"numAllVoxels",
			"Number of voxels in the core reac-diff system, on the "
			"current diffusion solver. ",
			&Dsolve::getNumVoxels
		);
		static LookupValueFinfo<
				Dsolve, unsigned int, vector< double > > nVec(
			"nVec",
			"vector of # of molecules along diffusion length, "
			"looked up by pool index",
			&Dsolve::setNvec,
			&Dsolve::getNvec
		);

		static ValueFinfo< Dsolve, unsigned int > numPools(
			"numPools",
			"Number of molecular pools in the entire reac-diff system, "
			"including variable, function and buffered.",
			&Dsolve::setNumPools,
			&Dsolve::getNumPools
		);

		static ValueFinfo< Dsolve, Id > compartment (
			"compartment",
			"Reac-diff compartment in which this diffusion system is "
			"embedded.",
			&Dsolve::setCompartment,
			&Dsolve::getCompartment
		);

		static LookupValueFinfo< Dsolve, unsigned int, double > diffVol1 (
			"diffVol1",
			"Volume used to set diffusion scaling: firstVol[ voxel# ] "
			"Particularly relevant for diffusion between PSD and head.",
			&Dsolve::setDiffVol1,
			&Dsolve::getDiffVol1
		);

		static LookupValueFinfo< Dsolve, unsigned int, double > diffVol2 (
			"diffVol2",
			"Volume used to set diffusion scaling: secondVol[ voxel# ] "
			"Particularly relevant for diffusion between spine and dend.",
			&Dsolve::setDiffVol2,
			&Dsolve::getDiffVol2
		);

		static LookupValueFinfo< Dsolve, unsigned int, double > diffScale (
			"diffScale",
			"Geometry term to set diffusion scaling: diffScale[ voxel# ] "
			"Here the scaling term is given by cross-section area/length "
			"Relevant for diffusion between spine head and dend, or PSD.",
			&Dsolve::setDiffScale,
			&Dsolve::getDiffScale
		);


		///////////////////////////////////////////////////////
		// DestFinfo definitions
		///////////////////////////////////////////////////////

		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< Dsolve >( &Dsolve::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< Dsolve >( &Dsolve::reinit ) );

		static DestFinfo buildMeshJunctions( "buildMeshJunctions",
			"Builds junctions between mesh on current Dsolve, and another"
			" Dsolve. The meshes have to be compatible. ",
			new EpFunc1< Dsolve, Id >(
					&Dsolve::buildMeshJunctions ) );

		static DestFinfo buildNeuroMeshJunctions( "buildNeuroMeshJunctions",
			"Builds junctions between NeuroMesh, SpineMesh and PsdMesh",
			new EpFunc2< Dsolve, Id, Id >(
					&Dsolve::buildNeuroMeshJunctions ) );

		///////////////////////////////////////////////////////
		// Shared definitions
		///////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* dsolveFinfos[] =
	{
		&stoich,			// ElementValue
		&path,				// ElementValue
		&compartment,		// Value
		&numVoxels,			// ReadOnlyValue
		&numAllVoxels,			// ReadOnlyValue
		&nVec,				// LookupValue
		&numPools,			// Value
		&diffVol1,				// LookupValue
		&diffVol2,				// LookupValue
		&diffScale,				// LookupValue
		&buildMeshJunctions, 	// DestFinfo
		&buildNeuroMeshJunctions, 	// DestFinfo
		&proc,				// SharedFinfo
	};

	static Dinfo< Dsolve > dinfo;
	static  Cinfo dsolveCinfo(
		"Dsolve",
		Neutral::initCinfo(),
		dsolveFinfos,
		sizeof(dsolveFinfos)/sizeof(Finfo *),
		&dinfo
	);

	return &dsolveCinfo;
}

static const Cinfo* dsolveCinfo = Dsolve::initCinfo();

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
Dsolve::Dsolve()
	:
		dt_( -1.0 ),
		numTotPools_( 0 ),
		numLocalPools_( 0 ),
		poolStartIndex_( 0 ),
		numVoxels_( 0 )
{;}

Dsolve::~Dsolve()
{;}

//////////////////////////////////////////////////////////////
// Field access functions
//////////////////////////////////////////////////////////////

void Dsolve::setNvec( unsigned int pool, vector< double > vec )
{
	if ( pool < pools_.size() ) {
		if ( vec.size() != pools_[pool].getNumVoxels() ) {
			cout << "Warning: Dsolve::setNvec: pool index out of range\n";
		} else {
			pools_[ pool ].setNvec( vec );
		}
	}
}

vector< double > Dsolve::getNvec( unsigned int pool ) const
{
	static vector< double > ret;
	if ( pool <  pools_.size() )
		return pools_[pool].getNvec();

	cout << "Warning: Dsolve::setNvec: pool index out of range\n";
	return ret;
}

static bool checkJn( const vector< DiffJunction >& jn, unsigned int voxel,
				const string& info )
{
	if ( jn.size() < 1 ) {
		cout << "Warning: Dsolve::" << info << ": junctions not defined.\n";
		return false;
	}
	if ( jn[0].vj.size() < voxel + 1 ) {
		cout << "Warning: Dsolve:: " << info << ": " << voxel <<
				"out of range.\n";
		return false;
	}
	return true;
}

void Dsolve::setDiffVol1( unsigned int voxel, double vol )
{
 	if ( checkJn( junctions_, voxel, "setDiffVol1" ) ) {
		VoxelJunction& vj = junctions_[0].vj[ voxel ];
		vj.firstVol = vol;
	}
}

double Dsolve::getDiffVol1( unsigned int voxel ) const
{
 	if ( checkJn( junctions_, voxel, "getDiffVol1" ) ) {
		const VoxelJunction& vj = junctions_[0].vj[ voxel ];
		return vj.firstVol;
	}
	return 0.0;
}

void Dsolve::setDiffVol2( unsigned int voxel, double vol )
{
 	if ( checkJn( junctions_, voxel, "setDiffVol2" ) ) {
		VoxelJunction& vj = junctions_[0].vj[ voxel ];
		vj.secondVol = vol;
	}
}

double Dsolve::getDiffVol2( unsigned int voxel ) const
{
 	if ( checkJn( junctions_, voxel, "getDiffVol2" ) ) {
		const VoxelJunction& vj = junctions_[0].vj[ voxel ];
		return vj.secondVol;
	}
	return 0.0;
}

void Dsolve::setDiffScale( unsigned int voxel, double adx )
{
 	if ( checkJn( junctions_, voxel, "setDiffScale" ) ) {
		VoxelJunction& vj = junctions_[0].vj[ voxel ];
		vj.diffScale = adx;
	}
}

double Dsolve::getDiffScale( unsigned int voxel ) const
{
 	if ( checkJn( junctions_, voxel, "getDiffScale" ) ) {
		const VoxelJunction& vj = junctions_[0].vj[ voxel ];
		return vj.diffScale;
	}
	return 0.0;
}

//////////////////////////////////////////////////////////////
// Process operations.
//////////////////////////////////////////////////////////////

static double integ( double myN, double rf, double rb, double dt )
{
	const double EPSILON = 1e-12;
	if ( myN > EPSILON && rf > EPSILON ) {
		double C = exp( -rf * dt / myN );
		myN *= C + ( rb / rf ) * ( 1.0 - C );
	} else {
		myN += ( rb - rf ) * dt;
	}
	if ( myN < 0.0 )
		return 0.0;
	return myN;
}

/**
 * Computes flux through a junction between diffusion solvers.
 * Most used at junctions on spines and PSDs, but can also be used
 * when a given diff solver is decomposed. At present the lookups
 * on the other diffusion solver assume that the data is on the local
 * node. Once this works well I can figure out how to do across nodes.
 */
void Dsolve::calcJunction( const DiffJunction& jn, double dt )
{
	const double EPSILON = 1e-15;
	Id oid( jn.otherDsolve );
	assert ( oid != Id() );
	assert ( oid.element()->cinfo()->isA( "Dsolve" ) );

	Dsolve* other = reinterpret_cast< Dsolve* >( oid.eref().data() );

	assert( jn.otherPools.size() == jn.myPools.size() );
	for ( unsigned int i = 0; i < jn.myPools.size(); ++i ) {
		DiffPoolVec& myDv = pools_[ jn.myPools[i] ];
		if ( myDv.getDiffConst() < EPSILON )
			continue;
		DiffPoolVec& otherDv = other->pools_[ jn.otherPools[i] ];
		if ( otherDv.getDiffConst() < EPSILON )
			continue;
		// This geom mean is used in case we have the odd situation of
		// different diffusion constants.
		double effectiveDiffConst =
			sqrt( myDv.getDiffConst() * otherDv.getDiffConst() );
		for ( vector< VoxelJunction >::const_iterator
			j = jn.vj.begin(); j != jn.vj.end(); ++j ) {
			double myN = myDv.getN( j->first );
			double otherN = otherDv.getN( j->second );
			// Here we do an exp Euler calculation
			// rf is rate from self to other.
			// double k = myDv.getDiffConst() * j->diffScale;
			double k = effectiveDiffConst * j->diffScale;
			double lastN = myN;
			myN = integ( myN,
				k * myN / j->firstVol,
				k * otherN / j->secondVol,
				dt
			);
			otherN += lastN - myN; // Simple mass conservation
			if ( otherN < 0.0 ) { // Avoid negatives
				myN += otherN;
				otherN = 0.0;
			}
			myDv.setN( j->first, myN );
			otherDv.setN( j->second, otherN );
		}
	}
}

void Dsolve::process( const Eref& e, ProcPtr p )
{
	for ( vector< DiffPoolVec >::iterator
					i = pools_.begin(); i != pools_.end(); ++i ) {
		i->advance( p->dt );
	}

	for ( vector< DiffJunction >::const_iterator
			i = junctions_.begin(); i != junctions_.end(); ++i ) {
		calcJunction( *i, p->dt );
	}
}

void Dsolve::reinit( const Eref& e, ProcPtr p )
{
	build( p->dt );
	for ( vector< DiffPoolVec >::iterator
					i = pools_.begin(); i != pools_.end(); ++i ) {
		i->reinit();
	}
}
//////////////////////////////////////////////////////////////
// Solver coordination and setup functions
//////////////////////////////////////////////////////////////

void Dsolve::setStoich( Id id )
{
	if ( !id.element()->cinfo()->isA( "Stoich" ) ) {
		cout << "Dsolve::setStoich::( " << id << " ): Error: provided Id is not a Stoich\n";
		return;
	}

	stoich_ = id;
	poolMap_ = Field< vector< unsigned int > >::get( stoich_, "poolIdMap" );
	poolMapStart_ = poolMap_.back();
	poolMap_.pop_back();

	path_ = Field< string >::get( stoich_, "path" );
	// cout << "Pool Info for stoich " << id.path() << endl;

	for ( unsigned int i = 0; i < poolMap_.size(); ++i ) {
		unsigned int poolIndex = poolMap_[i];
		if ( poolIndex != ~0U && poolIndex < pools_.size() ) {
			// assert( poolIndex < pools_.size() );
			Id pid( i + poolMapStart_ );
			assert( pid.element()->cinfo()->isA( "PoolBase" ) );
			PoolBase* pb =
					reinterpret_cast< PoolBase* >( pid.eref().data());
			double diffConst = pb->getDiffConst( pid.eref() );
			double motorConst = pb->getMotorConst( pid.eref() );
			pools_[ poolIndex ].setId( pid.value() );
			pools_[ poolIndex ].setDiffConst( diffConst );
			pools_[ poolIndex ].setMotorConst( motorConst );
			/*
			cout << i << " poolIndex=" <<  poolIndex <<
					", id=" << pid.value() <<
					", name=" << pid.element()->getName() << endl;
					*/
		}
	}
}

Id Dsolve::getStoich() const
{
	return stoich_;
}

/// Inherited, defining dummy function here.
void Dsolve::setDsolve( Id dsolve )
{;}

void Dsolve::setCompartment( Id id )
{
	const Cinfo* c = id.element()->cinfo();
	if ( c->isA( "NeuroMesh" ) || c->isA( "SpineMesh" ) ||
					c->isA( "PsdMesh" ) || c->isA( "CylMesh" ) ) {
		compartment_ = id;
		numVoxels_ = Field< unsigned int >::get( id, "numMesh" );
		/*
		const MeshCompt* m = reinterpret_cast< const MeshCompt* >(
						id.eref().data() );
		numVoxels_ = m->getStencil().nRows();
		*/
	} else {
		cout << "Warning: Dsolve::setCompartment:: compartment must be "
				"NeuroMesh or CylMesh, you tried :" << c->name() << endl;
	}
}

void Dsolve::makePoolMapFromElist( const vector< ObjId >& elist,
				vector< Id >& temp )
{
	unsigned int minId = 0;
	unsigned int maxId = 0;
	temp.resize( 0 );
	for ( vector< ObjId >::const_iterator
			i = elist.begin(); i != elist.end(); ++i ) {
		if ( i->element()->cinfo()->isA( "PoolBase" ) ) {
			temp.push_back( i->id );
			if ( minId == 0 )
				maxId = minId = i->id.value();
			else if ( i->id.value() < minId )
				minId = i->id.value();
			else if ( i->id.value() > maxId )
				maxId = i->id.value();
		}
	}

	if ( temp.size() == 0 ) {
		cout << "Dsolve::makePoolMapFromElist::( " << path_ <<
				" ): Error: path is has no pools\n";
		return;
	}

	stoich_ = Id();
	poolMapStart_ = minId;
	poolMap_.resize( 1 + maxId - minId );
	for ( unsigned int i = 0; i < temp.size(); ++i ) {
		unsigned int idValue = temp[i].value();
		assert( idValue >= minId );
		assert( idValue - minId < poolMap_.size() );
		poolMap_[ idValue - minId ] = i;
	}
}

void Dsolve::setPath( const Eref& e, string path )
{
	vector< ObjId > elist;
	simpleWildcardFind( path, elist );
	if ( elist.size() == 0 ) {
		cout << "Dsolve::setPath::( " << path << " ): Error: path is empty\n";
		return;
	}
	vector< Id > temp;
	makePoolMapFromElist( elist, temp );

	setNumPools( temp.size() );
	for ( unsigned int i = 0; i < temp.size(); ++i ) {
	 	Id id = temp[i];
		double diffConst = Field< double >::get( id, "diffConst" );
		double motorConst = Field< double >::get( id, "motorConst" );
		const Cinfo* c = id.element()->cinfo();
		if ( c == Pool::initCinfo() ) {
			PoolBase::zombify( id.element(), ZombiePool::initCinfo(), Id(), e.id() );
		} else if ( c == BufPool::initCinfo() ) {
			PoolBase::zombify( id.element(), ZombieBufPool::initCinfo(), Id(), e.id() );
			// Any Functions will have to continue to manage the BufPools.
			// This needs them to be replicated, and for their messages
			// to be copied over. Not really set up here.
		} else {
			cout << "Error: Dsolve::setPath( " << path << " ): unknown pool class:" << c->name() << endl;
		}
		id.element()->resize( numVoxels_ );

		unsigned int j = temp[i].value() - poolMapStart_;
		assert( j < poolMap_.size() );
		pools_[ poolMap_[i] ].setId( id.value() );
		pools_[ poolMap_[j] ].setDiffConst( diffConst );
		pools_[ poolMap_[j] ].setMotorConst( motorConst );
	}
}

string Dsolve::getPath( const Eref& e ) const
{
	return path_;
}

/////////////////////////////////////////////////////////////
// Solver building
//////////////////////////////////////////////////////////////

/**
 * build: This function is called either by setStoich or setPath.
 * By this point the diffusion consts etc will be assigned to the
 * poolVecs.
 * This requires
 * - Stoich should be assigned, OR
 * - A 'path' should be assigned which has been traversed to find pools.
 * - compartment should be assigned so we know how many voxels.
 * - If Stoich, its 'path' should be set so we know numPools. It needs
 * to know the numVoxels from the compartment. At the time of
 * path setting the zombification is done, which takes the Id of
 * the solver.
 * - After this build can be done. Just reinit doesn't make sense since
 * the build does a lot of things which should not be repeated for
 * each reinit.
 */

void Dsolve::build( double dt )
{
	if ( doubleEq( dt, dt_ ) )
		return;
	if ( compartment_ == Id() ) {
		cout << "Dsolve::build: Warning: No compartment defined. \n"
				"Did you forget to assign 'stoich.dsolve = this' ?\n";
		return;
	}
	dt_ = dt;
	const MeshCompt* m = reinterpret_cast< const MeshCompt* >(
						compartment_.eref().data() );
	unsigned int numVoxels = m->getNumEntries();

	for ( unsigned int i = 0; i < numLocalPools_; ++i ) {
		bool debugFlag = false;
		vector< unsigned int > diagIndex;
		vector< double > diagVal;
		vector< Triplet< double > > fops;
		FastMatrixElim elim( numVoxels, numVoxels );
		if ( elim.buildForDiffusion(
			m->getParentVoxel(), m->getVoxelVolume(),
			m->getVoxelArea(), m->getVoxelLength(),
		    pools_[i].getDiffConst(), pools_[i].getMotorConst(), dt ) )
		{
			vector< unsigned int > parentVoxel = m->getParentVoxel();
			assert( elim.checkSymmetricShape() );
			vector< unsigned int > lookupOldRowsFromNew;
			elim.hinesReorder( parentVoxel, lookupOldRowsFromNew );
			assert( elim.checkSymmetricShape() );
			pools_[i].setNumVoxels( numVoxels_ );
			elim.buildForwardElim( diagIndex, fops );
			elim.buildBackwardSub( diagIndex, fops, diagVal );
			elim.opsReorder( lookupOldRowsFromNew, fops, diagVal );
			if (debugFlag )
				elim.print();
		}
		pools_[i].setOps( fops, diagVal );
	}
}

/**
 * Should be called only from the Dsolve handling the NeuroMesh.
 */
// Would like to permit vectors of spines and psd compartments.
void Dsolve::buildNeuroMeshJunctions( const Eref& e, Id spineD, Id psdD )
{
	if ( !compartment_.element()->cinfo()->isA( "NeuroMesh" ) ) {
		cout << "Warning: Dsolve::buildNeuroMeshJunction: Compartment '" <<
				compartment_.path() << "' is not a NeuroMesh\n";
		return;
	}
	Id spineMesh = Field< Id >::get( spineD, "compartment" );
	if ( !spineMesh.element()->cinfo()->isA( "SpineMesh" ) ) {
		cout << "Warning: Dsolve::buildNeuroMeshJunction: Compartment '" <<
				spineMesh.path() << "' is not a SpineMesh\n";
		return;
	}
	Id psdMesh = Field< Id >::get( psdD, "compartment" );
	if ( !psdMesh.element()->cinfo()->isA( "PsdMesh" ) ) {
		cout << "Warning: Dsolve::buildNeuroMeshJunction: Compartment '" <<
				psdMesh.path() << "' is not a PsdMesh\n";
		return;
	}

	innerBuildMeshJunctions( spineD, e.id() );
	innerBuildMeshJunctions( psdD, spineD );
}

void Dsolve::buildMeshJunctions( const Eref& e, Id other )
{
	Id otherMesh;
	if ( other.element()->cinfo()->isA( "Dsolve" ) ) {
		otherMesh = Field< Id >::get( other, "compartment" );
		if ( compartment_.element()->cinfo()->isA( "ChemCompt" ) &&
			otherMesh.element()->cinfo()->isA( "ChemCompt" ) ) {
			innerBuildMeshJunctions( e.id(), other );
			return;
		}
	}
	cout << "Warning: Dsolve::buildMeshJunctions: one of '" <<
		compartment_.path() << ", " << otherMesh.path() <<
		"' is not a Mesh\n";
}

void printJunction( Id self, Id other, const DiffJunction& jn )
{
	cout << "Junction between " << self.path() << ", " << other.path() << endl;
	cout << "Pool indices: myPools, otherPools\n";
	for ( unsigned int i = 0; i < jn.myPools.size(); ++i )
		cout << i << "	" << jn.myPools[i] << "	" << jn.otherPools[i] << endl;
	cout << "Voxel junctions: first	second	firstVol	secondVol	diffScale\n";
	for ( unsigned int i = 0; i < jn.vj.size(); ++i ) {
		cout << i << "	" << jn.vj[i].first << "	" << jn.vj[i].second <<
		"	" << jn.vj[i].firstVol << "	" << jn.vj[i].secondVol <<
			   "	" << jn.vj[i].diffScale <<	endl;
	}
}

// Static utility func for building junctions
void Dsolve::innerBuildMeshJunctions( Id self, Id other )
{
	DiffJunction jn; // This is based on the Spine Dsolver.
	jn.otherDsolve = other.value();
	// Map pools between Dsolves
	Dsolve* mySolve = reinterpret_cast< Dsolve* >( self.eref().data() );
	map< string, unsigned int > myPools;
	for ( unsigned int i = 0; i < mySolve->pools_.size(); ++i ) {
			Id pool( mySolve->pools_[i].getId() );
			assert( pool != Id() );
			myPools[ pool.element()->getName() ] = i;
	}

	const Dsolve* otherSolve = reinterpret_cast< const Dsolve* >(
					other.eref().data() );
	for ( unsigned int i = 0; i < otherSolve->pools_.size(); ++i ) {
		Id otherPool( otherSolve->pools_[i].getId() );
		map< string, unsigned int >::iterator p =
			myPools.find( otherPool.element()->getName() );
		if ( p != myPools.end() ) {
			jn.otherPools.push_back( i );
			jn.myPools.push_back( p->second );
		}
	}

	// Map voxels between meshes.
	Id myMesh = Field< Id >::get( self, "compartment" );
	Id otherMesh = Field< Id >::get( other, "compartment" );

	const ChemCompt* myCompt = reinterpret_cast< const ChemCompt* >(
					myMesh.eref().data() );
	const ChemCompt* otherCompt = reinterpret_cast< const ChemCompt* >(
					otherMesh.eref().data() );
	myCompt->matchMeshEntries( otherCompt, jn.vj );
	vector< double > myVols = myCompt->getVoxelVolume();
	vector< double > otherVols = otherCompt->getVoxelVolume();
	for ( vector< VoxelJunction >::iterator
		i = jn.vj.begin(); i != jn.vj.end(); ++i ) {
		i->firstVol = myVols[i->first];
		i->secondVol = otherVols[i->second];
	}

	// printJunction( self, other, jn );
	mySolve->junctions_.push_back( jn );
}

/////////////////////////////////////////////////////////////
// Zombie Pool Access functions
//////////////////////////////////////////////////////////////
//
unsigned int Dsolve::getNumVarPools() const
{
	return 0;
}

unsigned int Dsolve::getNumVoxels() const
{
	return numVoxels_;
}

void Dsolve::setNumAllVoxels( unsigned int num )
{
	numVoxels_ = num;
	for ( unsigned int i = 0 ; i < numLocalPools_; ++i )
		pools_[i].setNumVoxels( numVoxels_ );
}

unsigned int Dsolve::convertIdToPoolIndex( const Eref& e ) const
{
	unsigned int i  = e.id().value() - poolMapStart_;
	if ( i < poolMap_.size() ) {
		return poolMap_[i];
	}
	cout << "Warning: Dsolve::convertIdToPoollndex: Id out of range, (" <<
		poolMapStart_ << ", " << e.id() << ", " <<
		poolMap_.size() + poolMapStart_ << "\n";
	return 0;
}

void Dsolve::setN( const Eref& e, double v )
{
	unsigned int pid = convertIdToPoolIndex( e );
	// Ignore silently, as this may be a valid pid for the ksolve to use.
	if ( pid >= pools_.size() )
		return;
	unsigned int vox = e.dataIndex();
	if ( vox < numVoxels_ ) {
		pools_[ pid ].setN( vox, v );
		return;
	}
	cout << "Warning: Dsolve::setN: Eref " << e << " out of range " <<
			pools_.size() << ", " << numVoxels_ << "\n";
}

double Dsolve::getN( const Eref& e ) const
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() ) return 0.0; // ignore silently
	unsigned int vox = e.dataIndex();
	if ( vox <  numVoxels_ ) {
		return pools_[ pid ].getN( vox );
	}
	cout << "Warning: Dsolve::setN: Eref " << e << " out of range " <<
			pools_.size() << ", " << numVoxels_ << "\n";
	return 0.0;
}

void Dsolve::setNinit( const Eref& e, double v )
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() )  // Ignore silently
		return;
	unsigned int vox = e.dataIndex();
	if ( vox < numVoxels_ ) {
		pools_[ pid ].setNinit( vox, v );
		return;
	}
	cout << "Warning: Dsolve::setNinit: Eref " << e << " out of range " <<
			pools_.size() << ", " << numVoxels_ << "\n";
}

double Dsolve::getNinit( const Eref& e ) const
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() ) return 0.0; // ignore silently
	unsigned int vox = e.dataIndex();
	if ( vox < numVoxels_ ) {
		return pools_[ pid ].getNinit( vox );
	}
	cout << "Warning: Dsolve::setNinit: Eref " << e << " out of range " <<
			pools_.size() << ", " << numVoxels_ << "\n";
	return 0.0;
}

void Dsolve::setDiffConst( const Eref& e, double v )
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() )   // Ignore silently, out of range.
		return;
	pools_[ convertIdToPoolIndex( e ) ].setDiffConst( v );
}

double Dsolve::getDiffConst( const Eref& e ) const
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() )   // Ignore silently, out of range.
		return 0.0;
	return pools_[ convertIdToPoolIndex( e ) ].getDiffConst();
}

void Dsolve::setMotorConst( const Eref& e, double v )
{
	unsigned int pid = convertIdToPoolIndex( e );
	if ( pid >= pools_.size() )   // Ignore silently, out of range.
		return;
	pools_[ convertIdToPoolIndex( e ) ].setMotorConst( v );
}

void Dsolve::setNumPools( unsigned int numPoolSpecies )
{
	// Decompose numPoolSpecies here, assigning some to each node.
	numTotPools_ = numPoolSpecies;
	numLocalPools_ = numPoolSpecies;
	poolStartIndex_ = 0;

	pools_.resize( numLocalPools_ );
	for ( unsigned int i = 0 ; i < numLocalPools_; ++i ) {
		pools_[i].setNumVoxels( numVoxels_ );
		// pools_[i].setId( reversePoolMap_[i] );
		// pools_[i].setParent( me );
	}
}

unsigned int Dsolve::getNumPools() const
{
	return numTotPools_;
}

// July 2014: This is half-baked wrt the startPool.
void Dsolve::getBlock( vector< double >& values ) const
{
	unsigned int startVoxel = values[0];
	unsigned int numVoxels = values[1];
	unsigned int startPool = values[2];
	unsigned int numPools = values[3];

	assert( startVoxel + numVoxels <= numVoxels_ );
	assert( startPool >= poolStartIndex_ );
	assert( numPools + startPool <= numLocalPools_ );
	values.resize( 4 );

	for ( unsigned int i = 0; i < numPools; ++i ) {
		unsigned int j = i + startPool;
		if ( j >= poolStartIndex_ && j < poolStartIndex_ + numLocalPools_ ){
			vector< double >::const_iterator q =
				pools_[ j - poolStartIndex_ ].getNvec().begin();

			values.insert( values.end(),
				q + startVoxel, q + startVoxel + numVoxels );
		}
	}
}

void Dsolve::setBlock( const vector< double >& values )
{
	unsigned int startVoxel = values[0];
	unsigned int numVoxels = values[1];
	unsigned int startPool = values[2];
	unsigned int numPools = values[3];

	assert( startVoxel + numVoxels <= numVoxels_ );
	assert( startPool >= poolStartIndex_ );
	assert( numPools + startPool <= numLocalPools_ );
	assert( values.size() == 4 + numVoxels * numPools );

	for ( unsigned int i = 0; i < numPools; ++i ) {
		unsigned int j = i + startPool;
		if ( j >= poolStartIndex_ && j < poolStartIndex_ + numLocalPools_ ){
			vector< double >::const_iterator
				q = values.begin() + 4 + i * numVoxels;
			pools_[ j - poolStartIndex_ ].setNvec( startVoxel, numVoxels, q );
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Inherited virtual

void Dsolve::updateRateTerms( unsigned int index )
{
	;
}

unsigned int Dsolve::getPoolIndex( const Eref& e ) const
{
	return convertIdToPoolIndex( e );
}

unsigned int Dsolve::getNumLocalVoxels() const
{
	return numVoxels_;
}

VoxelPoolsBase* Dsolve::pools( unsigned int i )
{
	return 0;
}

double Dsolve::volume( unsigned int i ) const
{
	return 1.0;
}
