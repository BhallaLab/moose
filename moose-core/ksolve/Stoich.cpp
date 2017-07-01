/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "PoolBase.h"
#include "ReacBase.h"
#include "EnzBase.h"
#include "CplxEnzBase.h"
#include "FuncTerm.h"
#include "RateTerm.h"
#include "FuncRateTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "VoxelPoolsBase.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "../builtins/Variable.h"
#include "../builtins/Function.h"
#include "ZombieFunction.h"
#include "Stoich.h"
#include "lookupVolumeFromMesh.h"
#include "../scheduling/Clock.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"

const Cinfo* Stoich::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ElementValueFinfo< Stoich, string > path(
        "path",
        "Wildcard path for reaction system handled by Stoich",
        &Stoich::setPath,
        &Stoich::getPath
    );

    static ValueFinfo< Stoich, Id > ksolve(
        "ksolve",
        "Id of Kinetic reaction solver class that works with this "
        "Stoich. "
        " Must be of class Ksolve, or Gsolve (at present) "
        " Must be assigned before the path is set.",
        &Stoich::setKsolve,
        &Stoich::getKsolve
    );

    static ValueFinfo< Stoich, Id > dsolve(
        "dsolve",
        "Id of Diffusion solver class that works with this Stoich."
        " Must be of class Dsolve "
        " If left unset then the system will be assumed to work in a"
        " non-diffusive, well-stirred cell. If it is going to be "
        " used it must be assigned before the path is set.",
        &Stoich::setDsolve,
        &Stoich::getDsolve
    );

    static ValueFinfo< Stoich, Id > compartment(
        "compartment",
        "Id of chemical compartment class that works with this Stoich."
        " Must be derived from class ChemCompt."
        " If left unset then the system will be assumed to work in a"
        " non-diffusive, well-stirred cell. If it is going to be "
        " used it must be assigned before the path is set.",
        &Stoich::setCompartment,
        &Stoich::getCompartment
    );

    static ReadOnlyValueFinfo< Stoich, unsigned int > numVarPools(
        "numVarPools",
        "Number of time-varying pools to be computed by the "
        "numerical engine",
        &Stoich::getNumVarPools
    );

    static ReadOnlyValueFinfo< Stoich, unsigned int > numBufPools(
        "numBufPools",
        "Number of buffered pools to be computed by the "
        "numerical engine. Includes pools controlled by functions.",
        &Stoich::getNumBufPools
    );

    static ReadOnlyValueFinfo< Stoich, unsigned int > numAllPools(
        "numAllPools",
        "Total number of pools handled by the numerical engine. "
        "This includes variable ones, buffered ones, and functions. "
        "It includes local pools as well as cross-solver proxy pools.",
        &Stoich::getNumAllPools
    );

    static ReadOnlyValueFinfo< Stoich, unsigned int > numProxyPools(
        "numProxyPools",
        "Number of pools here by proxy as substrates of a cross-"
        "compartment reaction.",
        &Stoich::getNumProxyPools
    );

    static ReadOnlyValueFinfo< Stoich, vector< unsigned int > >
    poolIdMap(
        "poolIdMap",
        "Map to look up the index of the pool from its Id."
        "poolIndex = poolIdMap[ Id::value() - poolOffset ] "
        "where the poolOffset is the smallest Id::value. "
        "poolOffset is passed back as the last entry of this vector."
        " Any Ids that are not pools return EMPTY=~0. ",
        &Stoich::getPoolIdMap
    );

    static ReadOnlyValueFinfo< Stoich, unsigned int > numRates(
        "numRates",
        "Total number of rate terms in the reaction system.",
        &Stoich::getNumRates
    );

    // Stuff here for getting Stoichiometry matrix to manipulate in
    // Python.
    static ReadOnlyValueFinfo< Stoich, vector< int > >
    matrixEntry(
        "matrixEntry",
        "The non-zero matrix entries in the sparse matrix. Their"
        "column indices are in a separate vector and the row"
        "informatino in a third",
        &Stoich::getMatrixEntry
    );
    // Stuff here for getting Stoichiometry matrix to manipulate in
    // Python.
    static ReadOnlyValueFinfo< Stoich, vector< unsigned int > >
    columnIndex(
        "columnIndex",
        "Column Index of each matrix entry",
        &Stoich::getColIndex
    );
    // Stuff here for getting Stoichiometry matrix to manipulate in
    // Python.
    static ReadOnlyValueFinfo< Stoich, vector< unsigned int > >
    rowStart(
        "rowStart",
        "Row start for each block of entries and column indices",
        &Stoich::getRowStart
    );

    // Proxy pool information
    static ReadOnlyLookupValueFinfo< Stoich, Id, vector< Id > >
    proxyPools(
        "proxyPools",
        "Return vector of proxy pools for X-compt reactions between "
        "current stoich, and the argument, which is a StoichId. "
        "The returned pools belong to the compartment handling the "
        "Stoich specified in the argument. "
        "If no pools are found, return an empty vector.",
        &Stoich::getProxyPools
    );

    static ReadOnlyValueFinfo< Stoich, int > status(
        "status",
        "Status of Stoich in the model building process. Values are: "
        "-1: Reaction path not yet assigned.\n "
        "0: Successfully built stoichiometry matrix.\n "
        "1: Warning: Missing a reactant in a Reac or Enz.\n "
        "2: Warning: Missing a substrate in an MMenz.\n "
        "3: Warning: Missing substrates as well as reactants.\n "
        "4: Warning: Compartment not defined.\n "
        "8: Warning: Neither Ksolve nor Dsolve defined.\n "
        "16: Warning: No objects found on path.\n "
        "",
        &Stoich::getStatus
    );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////
    static DestFinfo unzombify( "unzombify",
                                "Restore all zombies to their native state",
                                new OpFunc0< Stoich >( &Stoich::unZombifyModel )
                              );
    static DestFinfo buildXreacs( "buildXreacs",
                                  "Build cross-reaction terms between current stoich and "
                                  "argument. This function scans the voxels at which there are "
                                  "junctions between different compartments, and orchestrates "
                                  "set up of interfaces between the Ksolves that implement "
                                  "the X reacs at those junctions. ",
                                  new EpFunc1< Stoich, Id >( &Stoich::buildXreacs )
                                );

    static DestFinfo filterXreacs( "filterXreacs",
                                   "Filter cross-reaction terms on current stoich"
                                   "This function clears out absent rate terms that would "
                                   "otherwise try to compute cross reactions where the "
                                   "junctions are not present. ",
                                   new OpFunc0< Stoich >( &Stoich::filterXreacs )
                                 );
    static DestFinfo scaleBufsAndRates( "scaleBufsAndRates",
                                        "Args: voxel#, volRatio\n"
                                        "Handles requests for runtime volume changes in the specified "
                                        "voxel#, Used in adaptors changing spine vols.",
                                        new OpFunc2< Stoich, unsigned int, double >(
                                            &Stoich::scaleBufsAndRates )
                                      );

    //////////////////////////////////////////////////////////////
    // SrcFinfo Definitions
    //////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // SharedMsg Definitions
    //////////////////////////////////////////////////////////////

    static Finfo* stoichFinfos[] =
    {
        &path,				// ElementValue
        &ksolve,			// Value
        &dsolve,			// Value
        &compartment,		// Value
        &numVarPools,		// ReadOnlyValue
        &numBufPools,		// ReadOnlyValue
        &numAllPools,		// ReadOnlyValue
        &numProxyPools,		// ReadOnlyValue
        &poolIdMap,			// ReadOnlyValue
        &numRates,			// ReadOnlyValue
        &matrixEntry,		// ReadOnlyValue
        &columnIndex,		// ReadOnlyValue
        &rowStart,			// ReadOnlyValue
        &proxyPools,		// ReadOnlyLookupValue
        &status,			// ReadOnlyLookupValue
        &unzombify,			// DestFinfo
        &buildXreacs,		// DestFinfo
        &filterXreacs,		// DestFinfo
        &scaleBufsAndRates,	// DestFinfo
    };

    static Dinfo< Stoich > dinfo;
    static Cinfo stoichCinfo (
        "Stoich",
        Neutral::initCinfo(),
        stoichFinfos,
        sizeof( stoichFinfos ) / sizeof ( Finfo* ),
        &dinfo
    );

    return &stoichCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* stoichCinfo = Stoich::initCinfo();

//////////////////////////////////////////////////////////////

Stoich::Stoich()
    :
    useOneWay_( 0 ),
    path_( "" ),
    ksolve_(), // Must be reassigned to build stoich system.
    dsolve_(), // Must be assigned if diffusion is planned.
    compartment_(), // Must be assigned if diffusion is planned.
    kinterface_( 0 ),
    dinterface_( 0 ),
    rates_( 0 ), 	// No RateTerms yet.
    // uniqueVols_( 1, 1.0 ),
    numVoxels_( 1 ),
    status_( -1 )
{
    ;
}

Stoich::~Stoich()
{
    unZombifyModel();
    // Note that we cannot do the unZombify here, because it is too
    // prone to problems with the ordering of the delete operations
    // relative to the zombies.
    for ( vector< RateTerm* >::iterator j = rates_.begin();
            j != rates_.end(); ++j )
        delete *j;

    for ( vector< FuncTerm* >::iterator j = funcs_.begin();
            j != funcs_.end(); ++j )
        delete *j;

    /*
     * Do NOT delete FuncTerms, they are just pointers stolen from
     * the non-zombified objects.
    for ( vector< FuncTerm* >::iterator i = funcs_.begin();
    	i != funcs_.end(); ++i )
    	delete *i;
    	*/
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void Stoich::setOneWay( bool v )
{
    useOneWay_ = v;
}

bool Stoich::getOneWay() const
{
    return useOneWay_;
}

void Stoich::setPath( const Eref& e, string v )
{
    if ( path_ != "" && path_ != v )
    {
        // unzombify( path_ );
        cout << "Stoich::setPath: need to clear old path.\n";
        status_ = -1;
        return;
    }
    if ( ksolve_ == Id() )
    {
        cout << "Stoich::setPath: need to first set ksolve.\n";
        status_ = -1;
        return;
    }
    vector< ObjId > elist;
    path_ = v;
    wildcardFind( path_, elist );
    setElist( e, elist );
}

void convWildcards( vector< Id >& ret, const vector< ObjId >& elist )
{
    ret.resize( elist.size() );
    for ( unsigned int i = 0; i < elist.size(); ++i )
        ret[i] = elist[i].id;
}

void filterWildcards( vector< Id >& ret, const vector< ObjId >& elist )
{
    ret.clear();
    ret.reserve( elist.size() );
    for ( vector< ObjId >::const_iterator
            i = elist.begin(); i != elist.end(); ++i )
    {
        if ( i->element()->cinfo()->isA( "PoolBase" ) ||
                i->element()->cinfo()->isA( "ReacBase" ) ||
                i->element()->cinfo()->isA( "EnzBase" ) ||
                i->element()->cinfo()->isA( "Function" ) )
            ret.push_back( i->id );
    }
}

void Stoich::setElist( const Eref& e, const vector< ObjId >& elist )
{
    if ( compartment_ == Id() )
    {
        cout << "Warning: Stoich::setElist/setPath: Compartment not set. Aborting.\n";
        status_ = 4;
        return;
    }
    if ( !( kinterface_ || dinterface_  ) )
    {
        cout << "Warning: Stoich::setElist/setPath: Neither solver has been set. Aborting.\n";
        status_ = 8;
        return;
    }
    status_ = 0;
    if ( kinterface_ )
        kinterface_->setCompartment( compartment_ );
    if ( dinterface_ )
        dinterface_->setCompartment( compartment_ );
    vector< Id > temp;
    filterWildcards( temp, elist );
    if( temp.size() == 0 )
    {
        cout << "Warning: Stoich::setElist/setPath: No kinetics objects found on path. Aborting.\n";
        status_ = 16;
        return;
    }
    locateOffSolverReacs( compartment_, temp );

    // allocateObjMap( temp );
    allocateModel( temp );
    if ( kinterface_ )
    {
        // kinterface_->setNumPools( n );
        kinterface_->setStoich( e.id() );
        Shell* shell =  reinterpret_cast< Shell* >( Id().eref().data() );
        shell->doAddMsg( "Single",
                         compartment_, "voxelVolOut", ksolve_, "voxelVol" );
    }
    if ( dinterface_ )
    {
        // dinterface_->setNumPools( n );
        dinterface_->setStoich( e.id() );
    }
    zombifyModel( e, temp );
    if ( kinterface_ )
    {
        kinterface_->setDsolve( dsolve_ );
        // kinterface_->setupCrossSolverReacs( offSolverPoolVec_ );
        kinterface_->setupCrossSolverReacVols( subComptVec_, prdComptVec_);
        kinterface_->updateRateTerms();
    }
}

void Stoich::setupCrossSolverReacVols() const
{
    kinterface_->setupCrossSolverReacVols( subComptVec_, prdComptVec_);
}

/*
// Utility function for getting inputs to a single Function object.
static vector< ObjId > inputsToFunc( Id func )
{
	static const Finfo* inputFinfo =
			Cinfo::find( "Variable" )->findFinfo("input");
	static const DestFinfo* df = dynamic_cast< const DestFinfo* >( inputFinfo );
	assert( df );
	static FuncId fid = df->funcId();
	assert( func.element()->cinfo()->isA( "Function" );
	unsigned int numInputs = Field< unsigned int >::get( func, "numVars" );
	Id varId( func.value() );
	vector< ObjId > caller;
	varId.element()->getInputMsgs( caller, fid );
	cout << "NUM Inputs = " << numInputs << ", numObjId = " << caller.size() << endl;
	return caller;
}

// routine to get all inputs (pool Ids) to Pool Functions
void Stoich::inputsToPoolFuncs(
		vector< pair< Id, vector< unsigned int> >& ret ) const
{
	ret.clear();
	vector< Id >::iterator i;
	vector< ObjId >::iterator j;
	for ( i = poolFuncVec_.begin(); i != poolFuncVec_.end(); ++i )
		vector< ObjId > ovec = inputsToFunc( *i );
		vector< unsigned int > poolIndex;
		poolIndex.reserve( ovec.size() );
		for ( j = ovec.begin(); j != ovec.end(); ++j ) {
			unsigned int poolIndex =
			poolIndex.push_back( convertIdToPoolIndex( j->id ) );
		}
		ret.push_back( pair( *i, poolIndex) );
	return ret;
}
*/

//////////////////////////////////////////////////////////////////////

string Stoich::getPath( const Eref& e ) const
{
    return path_;
}

void Stoich::setKsolve( Id ksolve )
{
    ksolve_ = Id();
    kinterface_ = 0;
    if ( ! (
                ksolve.element()->cinfo()->isA( "Ksolve" )  ||
                ksolve.element()->cinfo()->isA( "Gsolve" )
            )
       )
    {
        cout << "Error: Stoich::setKsolve: invalid class assigned,"
             " should be either Ksolve or Gsolve\n";
        return;
    }
    ksolve_ = ksolve;
    kinterface_ = reinterpret_cast< ZombiePoolInterface* >(
                      ksolve.eref().data() );

    if ( ksolve.element()->cinfo()->isA( "Gsolve" ) )
        setOneWay( true );
    else
        setOneWay( false );

}

Id Stoich::getKsolve() const
{
    return ksolve_;
}

void Stoich::setDsolve( Id dsolve )
{
    dsolve_ = Id();
    dinterface_ = 0;
    if ( ! (
                dsolve.element()->cinfo()->isA( "Dsolve" )
            )
       )
    {
        cout << "Error: Stoich::setDsolve: invalid class assigned,"
             " should be Dsolve\n";
        return;
    }
    dsolve_ = dsolve;
    dinterface_ = reinterpret_cast< ZombiePoolInterface* >(
                      dsolve.eref().data() );
}

Id Stoich::getDsolve() const
{
    return dsolve_;
}

void Stoich::setCompartment( Id compartment )
{
    if ( ! (
                compartment.element()->cinfo()->isA( "ChemCompt" )
            )
       )
    {
        cout << "Error: Stoich::setCompartment: invalid class assigned,"
             " should be ChemCompt or derived class\n";
        return;
    }
    compartment_ = compartment;
    vector< double > temp;
    vector< double > vols =
        Field< vector < double > >::get( compartment, "voxelVolume" );
    if ( vols.size() > 0 )
    {
        numVoxels_ = vols.size();
        sort( vols.begin(), vols.end() );
        double bigVol = vols.back();
        assert( bigVol > 0.0 );
        temp.push_back( vols[0] / bigVol );
        for ( vector< double >::iterator
                i = vols.begin(); i != vols.end(); ++i )
        {
            if ( !doubleEq( temp.back(), *i / bigVol ) )
                temp.push_back( *i / bigVol );
        }
    }
}

Id Stoich::getCompartment() const
{
    return compartment_;
}

unsigned int Stoich::getNumVarPools() const
{
    return varPoolVec_.size();
}

unsigned int Stoich::getNumBufPools() const
{
    return bufPoolVec_.size();
}

unsigned int Stoich::getNumAllPools() const
{
    return varPoolVec_.size() + offSolverPoolVec_.size() + bufPoolVec_.size();
}

unsigned int Stoich::getNumProxyPools() const
{
    return offSolverPoolVec_.size();
}

vector< unsigned int > Stoich::getPoolIdMap() const
{
    if ( poolLookup_.size() == 0 )
        return vector< unsigned int >( 1, 0 );

    unsigned int minId = 1000000;
    unsigned int maxId = 0;
    unsigned int maxIndex = 0;
    map< Id, unsigned int >::const_iterator i;
    for ( i = poolLookup_.begin(); i != poolLookup_.end(); ++i )
    {
        unsigned int j = i->first.value();
        if ( j < minId ) minId = j;
        if ( j > maxId ) maxId = j;
        if ( maxIndex < i->second ) maxIndex = i->second;
    }
    vector< unsigned int > ret( maxId - minId + 2, ~0U );
    for ( i = poolLookup_.begin(); i != poolLookup_.end(); ++i )
    {
        unsigned int j = i->first.value() - minId;
        ret[j] = i->second;
    }
    ret[ ret.size() -1 ] = minId;

    return ret;
}

unsigned int Stoich::getNumRates() const
{
    return rates_.size();
}

/// Number of rate terms for reactions purely on this compartment.
unsigned int Stoich::getNumCoreRates() const
{
    return
        reacVec_.size() * (1 + useOneWay_) +
        enzVec_.size() * (2 + useOneWay_ ) +
        mmEnzVec_.size() + incrementFuncVec_.size();
}


const RateTerm* Stoich::rates( unsigned int i ) const
{
    assert( i < rates_.size() );
    return rates_[i];
}

const vector< RateTerm* >& Stoich::getRateTerms() const
{
    return rates_;
}

unsigned int Stoich::getNumFuncs() const
{
    return funcs_.size();
}

const FuncTerm* Stoich::funcs( unsigned int i ) const
{
    assert( i < funcs_.size() );
    assert( funcs_[i]);
    return funcs_[i];
}

vector< int > Stoich::getMatrixEntry() const
{
    return N_.matrixEntry();
}

vector< unsigned int > Stoich::getColIndex() const
{
    return N_.colIndex();
}

vector< unsigned int > Stoich::getRowStart() const
{
    return N_.rowStart();
}

vector< Id > Stoich::getProxyPools( Id i ) const
{
    static vector< Id > dummy;
    if ( !i.element()->cinfo()->isA( "Stoich" ) )
    {
        cout << "Warning: Stoich::getProxyPools: argument " << i <<
             " is not a Stoich\n";
        return dummy;
    }
    Id compt = Field< Id >::get( i, "compartment" );
    map< Id, vector< Id > >::const_iterator j =
        offSolverPoolMap_.find( compt );
    if ( j != offSolverPoolMap_.end() )
        return j->second;
    return dummy;
}

int Stoich::getStatus() const
{
    return status_;
}

//////////////////////////////////////////////////////////////
// Model setup functions
//////////////////////////////////////////////////////////////

/**
 * Checks if specified reac is off solver. As side-effect it compiles
 * a vector of the pools that are off-solver, and the corresponding
 * compartments for those pools
 */
static bool isOffSolverReac( const Element* e, Id myCompt,
                             //	vector< Id >& offSolverPools,
                             vector< Id >& poolCompts,
                             map< Id, Id >& poolComptMap )
{
    assert( myCompt != Id() );
    assert( myCompt.element()->cinfo()->isA( "ChemCompt" ) );
    bool ret = false;
    vector< Id > neighbors;
    e->getNeighbors( neighbors, e->cinfo()->findFinfo( "subOut" ));
    vector< Id > n2;
    e->getNeighbors( n2, e->cinfo()->findFinfo( "prdOut" ));
    neighbors.insert( neighbors.end(), n2.begin(), n2.end() );
    for ( vector< Id >::const_iterator
            j = neighbors.begin(); j != neighbors.end(); ++j )
    {
        Id otherCompt = getCompt( *j );
        if ( myCompt != otherCompt )
        {
            // offSolverPools.push_back( *j );
            poolCompts.push_back( otherCompt );
            poolComptMap[ *j ] = otherCompt; // Avoids duplication of pools
            ret = true;
            if ( j->element()->cinfo()->isA( "BufPool" ) )
            {
                cout << "Warning: Avoid BufPool: " << j->path() <<
                     "\n as reactant in cross-compartment reactions\n";
            }
        }
    }
    return ret;
}

/**
 * Extracts and orders the compartments associated with a given reac.
 */
pair< Id, Id > extractCompts( const vector< Id >& compts )
{
    pair< Id, Id > ret;
    for ( vector< Id >::const_iterator i = compts.begin();
            i != compts.end(); ++i )
    {
        if ( ret.first == Id() )
        {
            ret.first = *i;
        }
        else if ( ret.first != *i )
        {
            if ( ret.second == Id() )
                ret.second = *i;
            else
            {
                cout << "Error: Stoich::extractCompts: more than 2 compartments\n";
                assert( 0 );
            }
        }
    }
    if ( ( ret.second != Id() ) && ret.second < ret.first )
    {
        Id temp = ret.first;
        ret.first = ret.second;
        ret.second = ret.first;
    }

    return ret;
}

void Stoich::locateOffSolverReacs( Id myCompt, vector< Id >& elist )
{
    offSolverPoolVec_.clear();
    offSolverReacVec_.clear();
    offSolverEnzVec_.clear();
    offSolverMMenzVec_.clear();
    offSolverReacCompts_.clear();
    offSolverEnzCompts_.clear();
    offSolverMMenzCompts_.clear();
    map< Id, Id > poolComptMap; // < pool, compt >

    vector< Id > temp;
    temp.reserve( elist.size() );
    for ( vector< Id >::const_iterator
            i = elist.begin(); i != elist.end(); ++i )
    {
        const Element* e = i->element();
        if ( e->cinfo()->isA( "ReacBase" ) || e->cinfo()->isA( "EnzBase" ) )
        {
            vector< Id > compts;
            if ( isOffSolverReac( e, myCompt, compts, poolComptMap	) )
            {
                if ( e->cinfo()->isA( "ReacBase" ) )
                {
                    offSolverReacVec_.push_back( *i );
                    offSolverReacCompts_.push_back( extractCompts(compts));
                }
                else if ( e->cinfo()->isA( "CplxEnzBase" ) )
                {
                    offSolverEnzVec_.push_back( *i );
                    offSolverEnzCompts_.push_back( extractCompts(compts));
                }
                else if ( e->cinfo()->isA( "EnzBase" ) )
                {
                    offSolverMMenzVec_.push_back( *i );
                    offSolverMMenzCompts_.push_back(extractCompts(compts));
                }
            }
            else
            {
                temp.push_back( *i );
            }
        }
        else
        {
            temp.push_back( *i );
        }
    }

    offSolverPoolMap_.clear();
    for ( map< Id, Id >::iterator
            i = poolComptMap.begin(); i != poolComptMap.end(); ++i )
    {
        // fill in the map for activeOffSolverPools.
        offSolverPoolMap_[i->second].push_back( i->first );
    }

    // Ensure we don't have repeats, and the pools are ordered by compt
    offSolverPoolVec_.clear();
    for ( map< Id, vector< Id > >::iterator
            i = offSolverPoolMap_.begin(); i != offSolverPoolMap_.end(); ++i )
    {
        if ( i->first != myCompt )
        {
            offSolverPoolVec_.insert( offSolverPoolVec_.end(),
                                      i->second.begin(), i->second.end() );
        }
    }

    elist = temp;
}

///////////////////////////////////////////////////////////////////
// Model allocation stuff here
///////////////////////////////////////////////////////////////////

/*
void Stoich::allocateObjMap( const vector< Id >& elist )
{
	vector< Id > temp( elist );
	temp.insert( temp.end(), offSolverPoolVec_.begin(),
					offSolverPoolVec_.end() );
	temp.insert( temp.end(), offSolverReacs_.begin(),
					offSolverReacs_.end() );
	if ( temp.size() == 0 )
		return;
	objMapStart_ = ~0;
	unsigned int maxId = 0;
	for ( vector< Id >::const_iterator
					i = temp.begin(); i != temp.end(); ++i ) {
		if ( objMapStart_ > i->value() )
			objMapStart_ = i->value();
		if ( maxId < i->value() )
			maxId = i->value();
	}
	objMap_.clear();
	objMap_.resize( 1 + maxId - objMapStart_, 0 );
	*/
/**
 * If this assertion fails it usually means that the elist passed to
 * the solver is not properly restricted to objects located on the
 * current compartment. As a result of this, traversal for finding
 * off-compartment pools generates repeats with the ones in the elist.
 * Note that pool compartment assignment is determined by following
 * the mesh message, and thus a tree-based elist construction for
 * compartments may be incompatible with the generation of the lists
 * of off-compartment pools. It is up to the upstream code to
 * ensure that this is done properly.
 *
 * This assertion also fails if the Ids concerned had a dimension
 * greater than 1.
 */
/*
	assert( objMap_.size() >= temp.size() );
}
*/


/// Identifies and allocates objects in the Stoich.
void Stoich::allocateModelObject( Id id )
{
    static const Cinfo* poolCinfo = Cinfo::find( "Pool" );
    static const Cinfo* bufPoolCinfo = Cinfo::find( "BufPool" );
    static const Cinfo* reacCinfo = Cinfo::find( "Reac" );
    static const Cinfo* enzCinfo = Cinfo::find( "Enz" );
    static const Cinfo* mmEnzCinfo = Cinfo::find( "MMenz" );
    static const Cinfo* functionCinfo = Cinfo::find( "Function" );
    static const Finfo* f1 = functionCinfo->findFinfo( "valueOut" );
    static const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( f1 );
    assert( sf );

    Element* ei = id.element();
    if ( ei->cinfo() == poolCinfo )
    {
        // objMap_[ id.value() - objMapStart_ ] = numVarPools_;
        varPoolVec_.push_back( id );
        // ++numVarPools_;
    }
    else if ( ei->cinfo() == bufPoolCinfo )
    {
        bufPoolVec_.push_back( id );
    }
    else if ( ei->cinfo() == mmEnzCinfo )
    {
        mmEnzVec_.push_back( ei->id() );
        // objMap_[ id.value() - objMapStart_ ] = numReac_;
        // ++numReac_;
    }
    else if ( ei->cinfo() == reacCinfo )
    {
        reacVec_.push_back( ei->id() );
        /*
        if ( useOneWay_ ) {
        	objMap_[ id.value() - objMapStart_ ] = numReac_;
        	numReac_ += 2;
        } else {
        	objMap_[ id.value() - objMapStart_ ] = numReac_;
        	++numReac_;
        }
        */
    }
    else if ( ei->cinfo() == enzCinfo )
    {
        enzVec_.push_back( ei->id() );
        /*
        if ( useOneWay_ ) {
        	objMap_[ id.value() - objMapStart_ ] = numReac_;
        	numReac_ += 3;
        } else {
        	objMap_[ id.value() - objMapStart_ ] = numReac_;
        	numReac_ += 2;
        }
        */
    }
    else if ( ei->cinfo() == functionCinfo )
    {
        vector< ObjId > tgt;
        vector< string > func;
        ei->getMsgTargetAndFunctions( 0, sf, tgt, func );
        if ( func.size() > 0 && func[0] == "increment" )
        {
            incrementFuncVec_.push_back( ei->id() );
            // objMap_[ id.value() - objMapStart_ ] = numReac_;
            // numReac_++;
        }
        else if ( func.size() > 0 && func[0] == "setNumKf" )
        {
            reacFuncVec_.push_back( ei->id() );
        }
        else     // Assume it controls the N of a pool.
        {
            poolFuncVec_.push_back( ei->id() );
            // objMap_[ id.value() - objMapStart_ ] = numFunctions_;
            // ++numFunctions_;
        }
    }
}

// Sorts, unique, erase any extras.
void myUnique( vector< Id >& v )
{
    sort( v.begin(), v.end() );
    vector< Id >::iterator last = unique( v.begin(), v.end() );
    v.erase( last, v.end() );
}

/// Using the computed array sizes, now allocate space for them.
void Stoich::resizeArrays()
{
    myUnique( varPoolVec_ );
    myUnique( bufPoolVec_ );
    myUnique( offSolverPoolVec_ );
    myUnique( reacVec_ );
    myUnique( offSolverReacVec_ );
    myUnique( enzVec_ );
    myUnique( offSolverEnzVec_ );
    myUnique( mmEnzVec_ );
    myUnique( offSolverMMenzVec_ );

    unsigned int totNumPools = varPoolVec_.size() + bufPoolVec_.size() +
                               + offSolverPoolVec_.size();

    species_.resize( totNumPools, 0 );

    unsigned int totNumRates =
        ( reacVec_.size() + offSolverReacVec_.size() ) * (1+useOneWay_) +
        ( enzVec_.size() + offSolverEnzVec_.size() ) * (2 + useOneWay_ ) +
        mmEnzVec_.size() + offSolverMMenzVec_.size() +
        incrementFuncVec_.size();
    rates_.resize( totNumRates, 0 );
    funcs_.resize( poolFuncVec_.size(), 0 );
    N_.setSize( totNumPools, totNumRates );
    if ( kinterface_ )
        kinterface_->setNumPools( totNumPools );
    if ( dinterface_ ) // Only set up var pools managed locally.
        dinterface_->setNumPools( varPoolVec_.size() );
}

/// Calculate sizes of all arrays, and allocate them.
void Stoich::allocateModel( const vector< Id >& elist )
{
    // numVarPools_ = 0;
    // numReac_ = 0;
    // numFunctions_ = 0;
    // vector< Id > bufPools;
    varPoolVec_.clear();
    bufPoolVec_.clear();
    // offSolverPoolVec is filled up by the locateOffSolverReacs function
    reacVec_.clear();
    enzVec_.clear();
    mmEnzVec_.clear();
    poolFuncVec_.clear();
    incrementFuncVec_.clear();
    reacFuncVec_.clear();

    for ( vector< Id >::const_iterator i = elist.begin();
            i != elist.end(); ++i )
        allocateModelObject( *i );
    resizeArrays();

    buildPoolLookup();
    buildRateTermLookup();
    buildFuncLookup();
}

///////////////////////////////////////////////////////////////////
// Build the lookup maps for conversion of Ids to internal indices.
///////////////////////////////////////////////////////////////////
void Stoich::buildPoolLookup()
{
    // The order of pools is: varPools, offSolverVarPools, bufPools.
    poolLookup_.clear();
    int poolNum = 0;
    vector< Id >::iterator i;
    for ( i = varPoolVec_.begin(); i != varPoolVec_.end(); ++i )
        poolLookup_[ *i ] = poolNum++;
    for ( i = offSolverPoolVec_.begin(); i != offSolverPoolVec_.end(); ++i)
        poolLookup_[ *i ] = poolNum++;
    for ( i = bufPoolVec_.begin(); i != bufPoolVec_.end(); ++i )
        poolLookup_[ *i ] = poolNum++;
}

void Stoich::buildRateTermLookup()
{
    // The order of pools is: varPools, offSolverVarPools, bufPools.
    rateTermLookup_.clear();
    int termNum = 0;
    vector< Id >::iterator i;
    for ( i = reacVec_.begin(); i != reacVec_.end(); ++i )
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 1 + useOneWay_;
    }
    for ( i = enzVec_.begin(); i != enzVec_.end(); ++i )
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 2 + useOneWay_;
    }
    for ( i = mmEnzVec_.begin(); i != mmEnzVec_.end(); ++i )
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 1;
    }
    for ( i=incrementFuncVec_.begin(); i != incrementFuncVec_.end(); ++i )
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 1;
    }
    for ( i = offSolverReacVec_.begin(); i != offSolverReacVec_.end(); ++i)
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 1 + useOneWay_;
    }
    for ( i = offSolverEnzVec_.begin(); i != offSolverEnzVec_.end(); ++i )
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 2 + useOneWay_;
    }
    for ( i=offSolverMMenzVec_.begin(); i != offSolverMMenzVec_.end(); ++i)
    {
        rateTermLookup_[ *i ] = termNum;
        termNum += 1;
    }
}

void Stoich::buildFuncLookup()
{
    funcLookup_.clear();
    int funcNum = 0;
    vector< Id >::iterator i;
    for ( i = poolFuncVec_.begin(); i != poolFuncVec_.end(); ++i )
        funcLookup_[ *i ] = funcNum++;
}

///////////////////////////////////////////////////////////////////
// Stoich building stuff here for installing model components.
///////////////////////////////////////////////////////////////////

void Stoich::installAndUnschedFunc( Id func, Id pool, double volScale )
{
    static const Cinfo* varCinfo = Cinfo::find( "Variable" );
    static const Finfo* funcInputFinfo = varCinfo->findFinfo( "input" );
    static const DestFinfo* df = dynamic_cast< const DestFinfo* >( funcInputFinfo );
    assert( df );
    // Unsched Func
    func.element()->setTick( -2 ); // Disable with option to resurrect.

    // Install the FuncTerm
    FuncTerm* ft = new FuncTerm();

    Id ei( func.value() + 1 );

    unsigned int numSrc = Field< unsigned int >::get( func, "numVars" );
    vector< pair< Id, unsigned int> > srcPools;
#ifndef NDEBUG
    unsigned int n =
#endif
        ei.element()->getInputsWithTgtIndex( srcPools, df );
    assert( numSrc == n );
    vector< unsigned int > poolIndex( numSrc, 0 );
    for ( unsigned int i = 0; i < numSrc; ++i )
    {
        unsigned int j = srcPools[i].second;
        if ( j >= numSrc )
        {
            cout << "Warning: Stoich::installAndUnschedFunc: tgt index not allocated, " << j << ",	" << numSrc << endl;
            continue;
        }
        poolIndex[j] = convertIdToPoolIndex( srcPools[i].first );
    }
    ft->setReactantIndex( poolIndex );
    string expr = Field< string >::get( func, "expr" );
    ft->setExpr( expr );
    // Tie the output of the FuncTerm to the pool it controls.
    ft->setTarget( convertIdToPoolIndex( pool ) );
    ft->setVolScale( volScale );
    unsigned int funcIndex = convertIdToFuncIndex( func );
    assert( funcIndex != ~0U );
    funcs_[ funcIndex ] = ft;
}

void Stoich::installAndUnschedFuncRate( Id func, Id pool )
{
    static const Cinfo* varCinfo = Cinfo::find( "Variable" );
    static const Finfo* funcInputFinfo = varCinfo->findFinfo( "input" );
    static const DestFinfo* df = dynamic_cast< const DestFinfo* >( funcInputFinfo );
    assert( df );
    // Unsched Func
    func.element()->setTick( -2 ); // Disable with option to resurrect.

    // Note that we set aside this index during allocateModelObject
    unsigned int rateIndex = convertIdToReacIndex( func );
    unsigned int tempIndex = convertIdToPoolIndex( pool );
    assert( rateIndex != ~0U );
    assert( tempIndex != ~0U );
    // Install the FuncReac
    FuncRate* fr = new FuncRate( 1.0, tempIndex );
    rates_[rateIndex] = fr;
    int stoichEntry = N_.get( tempIndex, rateIndex );
    N_.set( tempIndex, rateIndex, stoichEntry + 1 );

    Id ei( func.value() + 1 );

    unsigned int numSrc = Field< unsigned int >::get( func, "numVars" );
    vector< pair< Id, unsigned int > > srcPools;
#ifndef NDEBUG
    unsigned int n =
#endif
        ei.element()->getInputsWithTgtIndex( srcPools, df );
    assert( numSrc == n );
    vector< unsigned int > poolIndex( numSrc, 0 );
    for ( unsigned int i = 0; i < numSrc; ++i )
    {
        unsigned int j = srcPools[i].second;
        if ( j >= numSrc )
        {
            cout << "Warning: Stoich::installAndUnschedFuncRate: tgt index not allocated, " << j << ",	" << numSrc << endl;
            continue;
        }
        poolIndex[j] = convertIdToPoolIndex( srcPools[i].first );
    }
    fr->setFuncArgIndex( poolIndex );
    string expr = Field< string >::get( func, "expr" );
    fr->setExpr( expr );
}

void Stoich::installAndUnschedFuncReac( Id func, Id reac )
{
    static const Cinfo* varCinfo = Cinfo::find( "Variable" );
    // static const Finfo* funcSrcFinfo = varCinfo->findFinfo( "input" );
    static const Finfo* funcSrcFinfo = varCinfo->findFinfo( "input" );
    assert( funcSrcFinfo );
    // Unsched Func
    func.element()->setTick( -2 ); // Disable with option to resurrect.

    unsigned int rateIndex = convertIdToReacIndex( reac );
    assert( rateIndex != ~0U );
    // Install the FuncReac
    double k = rates_[rateIndex]->getR1();
    vector< unsigned int > reactants;
    unsigned int numForward = rates_[rateIndex]->getReactants( reactants );
    // The reactants vector has both substrates and products.
    reactants.resize( numForward );
    FuncReac* fr = new FuncReac( k, reactants );
    delete rates_[rateIndex];
    rates_[rateIndex] = fr;

    Id ei( func.value() + 1 );

    unsigned int numSrc = Field< unsigned int >::get( func, "numVars" );
    vector< Id > srcPools;
#ifndef NDEBUG
    unsigned int n =
#endif
        ei.element()->getNeighbors( srcPools, funcSrcFinfo);
    assert( numSrc == n );
    vector< unsigned int > poolIndex( numSrc, 0 );
    for ( unsigned int i = 0; i < numSrc; ++i )
        poolIndex[i] = convertIdToPoolIndex( srcPools[i] );
    fr->setFuncArgIndex( poolIndex );
    string expr = Field< string >::get( func, "expr" );
    fr->setExpr( expr );
}

void Stoich::convertRatesToStochasticForm()
{
    for ( unsigned int i = 0; i < rates_.size(); ++i )
    {
        vector< unsigned int > molIndex;
        if ( rates_[i]->getReactants( molIndex ) > 1 )
        {
            if ( molIndex.size() == 2 && molIndex[0] == molIndex[1] )
            {
                RateTerm* oldRate = rates_[i];
                rates_[ i ] = new StochSecondOrderSingleSubstrate(
                    oldRate->getR1(), molIndex[ 0 ]
                );
                delete oldRate;
            }
            else if ( molIndex.size() > 2 )
            {
                RateTerm* oldRate = rates_[ i ];
                rates_[i] = new StochNOrder( oldRate->getR1(), molIndex );
                delete oldRate;
            }
        }
    }
}

const KinSparseMatrix& Stoich::getStoichiometryMatrix() const
{
    return N_;
}

void Stoich::buildXreacs( const Eref& e, Id otherStoich )
{
	if ( status_ == 0 )
    	kinterface_->setupCrossSolverReacs( offSolverPoolMap_,otherStoich);
}

void Stoich::filterXreacs()
{
	if ( status_ == 0 ) {
    	kinterface_->filterCrossRateTerms( offSolverReacVec_, offSolverReacCompts_ );
    	kinterface_->filterCrossRateTerms( offSolverEnzVec_, offSolverEnzCompts_ );
    	kinterface_->filterCrossRateTerms( offSolverMMenzVec_, offSolverMMenzCompts_ );
	}
}

/*
void Stoich::comptsOnCrossReacTerms( vector< pair< Id, Id > >& xr ) const
{
	xr.clear();
	assert( offSolverReacVec_.size() == offSolverReacCompts_.size() );
	assert( offSolverEnzVec_.size() == offSolverEnzCompts_.size() );
	assert( offSolverMMenzVec_.size() == offSolverMMenzCompts_.size() );
	unsigned int numOffSolverRates = getNumRates() - getNumCoreRates();
	if ( numOffSolverRates == 0 )
		return;
	unsigned int k = getNumCoreRates();
	unsigned int j = 0;

	for ( unsigned int i = 0; i < offSolverReacs_.size() ; ++i ) {
		if ( i+1 < offSolverReacs_.size() )
			j = convertIdToReacIndex( offSolverReacs_[i+1] );
		else
			j = rates_.size();
		while ( k < j ) {
			xr.push_back( offSolverReacCompts_[i] );
			k++;
		}
	}
	assert( xr.size() == numXrates );
}
*/

//////////////////////////////////////////////////////////////
// Model zombification functions
//////////////////////////////////////////////////////////////

/// Returns Function, if any, acting as src of specified msg into pa.
static Id findFuncMsgSrc( Id pa, const string& msg )
{
    const Finfo *finfo = pa.element()->cinfo()->findFinfo( msg );
    if ( !finfo ) return Id();
    vector< Id > ret;
    if ( pa.element()->getNeighbors( ret, finfo ) > 0 )
    {
        if ( ret[0].element()->cinfo()->isA( "Function" ) )
            return ret[0];
    }
    return Id(); // failure


    /*
    cout << "fundFuncMsgSrc:: convert to DestFinfo\n";
    const DestFinfo *df = dynamic_cast< const DestFinfo* >( finfo );
    if ( !df ) return Id();
    FuncId fid = df->getFid();

    cout << "fundFuncMsgSrc:: findCaller\n";
    ObjId caller = pa.element()->findCaller( fid );
    if ( caller.bad() )
    	return Id();
    cout << "Found Func for " << pa.path() << " on " << msg << endl;
    cout << "Func was " << caller.path() << endl;
    return caller.id;
    */

    /*
    	vector< Id > kids = Neutral::getChildren( pa.eref() );
    	for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i )
    		if ( i->element()->cinfo()->isA( "Function" ) )
    			return( *i );
    	return Id();
    */

}

Id Stoich::zombifyPoolFuncWithScaling( Id pool )
{
    static const Cinfo* zfCinfo = Cinfo::find( "ZombieFunction" );
    Id funcId = findFuncMsgSrc( pool, "setN" );
    if ( funcId != Id() )
    {
        Element* fe = funcId.element();
        installAndUnschedFunc( funcId, pool, 1.0 );
        ZombieFunction::zombify( fe, zfCinfo,ksolve_,dsolve_);
    }
    else
    {
        funcId = findFuncMsgSrc( pool, "setConc" );
        if ( funcId != Id() )
        {
            // cout << "Warning: Stoich::zombifyModel: Prefer to use setN rather than setConc:" << pool.path() << endl;
            Element* fe = funcId.element();
            double vol = Field< double >::get( pool, "volume" );
            installAndUnschedFunc( funcId, pool, vol * NA );
            ZombieFunction::zombify( fe, zfCinfo,ksolve_,dsolve_);
        }
    }
    return funcId;
}

// e is the stoich Eref, elist is list of all Ids to zombify.
void Stoich::zombifyModel( const Eref& e, const vector< Id >& elist )
{
    static const Cinfo* poolCinfo = Cinfo::find( "Pool" );
    static const Cinfo* bufPoolCinfo = Cinfo::find( "BufPool" );
    static const Cinfo* reacCinfo = Cinfo::find( "Reac" );
    static const Cinfo* enzCinfo = Cinfo::find( "Enz" );
    static const Cinfo* mmEnzCinfo = Cinfo::find( "MMenz" );
    static const Cinfo* zombiePoolCinfo = Cinfo::find( "ZombiePool" );
    static const Cinfo* zombieBufPoolCinfo = Cinfo::find( "ZombieBufPool");
    static const Cinfo* zombieReacCinfo = Cinfo::find( "ZombieReac");
    static const Cinfo* zombieMMenzCinfo = Cinfo::find( "ZombieMMenz");
    static const Cinfo* zombieEnzCinfo = Cinfo::find( "ZombieEnz");
    static const Cinfo* zfCinfo = Cinfo::find( "ZombieFunction" );
    // static const Finfo* funcSrcFinfo = Cinfo::find( "Function")->findFinfo( "valueOut" );
    // vector< Id > meshEntries;
    vector< Id > temp = elist;

    temp.insert( temp.end(), offSolverReacVec_.begin(), offSolverReacVec_.end() );
    temp.insert( temp.end(), offSolverEnzVec_.begin(), offSolverEnzVec_.end() );
    temp.insert( temp.end(), offSolverMMenzVec_.begin(), offSolverMMenzVec_.end() );

    for ( vector< Id >::const_iterator i = temp.begin(); i != temp.end(); ++i )
    {
        Element* ei = i->element();
        if ( ei->cinfo() == poolCinfo )
        {
            // We need to check the increment message before we zombify the
            // pool, because ZombiePool doesn't have this message.
            Id funcId = findFuncMsgSrc( *i, "increment" );
            double concInit =
                Field< double >::get( ObjId( ei->id(), 0 ), "concInit" );
            // Look for func setting rate of change of pool
            // Id funcId = Neutral::child( i->eref(), "func" );
            if ( funcId != Id() )
            {
                // cout << "Found Msg src for increment at " << funcId.path() << endl;
                Element* fe = funcId.element();
                installAndUnschedFuncRate( funcId, (*i) );
                ZombieFunction::zombify( fe, zfCinfo,ksolve_,dsolve_);
            }
            else
            {
                funcId = zombifyPoolFuncWithScaling( *i );
            }
            PoolBase::zombify( ei, zombiePoolCinfo, ksolve_, dsolve_ );
            ei->resize( numVoxels_ );
            for ( unsigned int j = 0; j < numVoxels_; ++j )
            {
                ObjId oi( ei->id(), j );
                Field< double >::set( oi, "concInit", concInit );
            }
        }
        else if ( ei->cinfo() == bufPoolCinfo )
        {
            double concInit =
                Field< double >::get( ObjId( ei->id(), 0 ), "concInit" );
            // Look for func setting conc of pool
            // Id funcId = Neutral::child( i->eref(), "func" );
            Id funcId = zombifyPoolFuncWithScaling( *i );
            if ( funcId == Id() )
            {
                funcId = findFuncMsgSrc( *i, "increment" );
                if ( funcId != Id() )
                {
                    cout << "Warning: Stoich::zombifyModel: Probably you don't want to send increment to a BufPool:" << i->path() << endl;
                    Element* fe = funcId.element();
                    installAndUnschedFuncRate( funcId, (*i) );
                    ZombieFunction::zombify( fe, zfCinfo,ksolve_,dsolve_);
                }
            }
            PoolBase::zombify( ei, zombieBufPoolCinfo, ksolve_, dsolve_ );
            ei->resize( numVoxels_ );
            for ( unsigned int j = 0; j < numVoxels_; ++j )
            {
                ObjId oi( ei->id(), j );
                Field< double >::set( oi, "concInit", concInit );
            }
        }
        else if ( ei->cinfo() == reacCinfo )
        {
            ReacBase::zombify( ei, zombieReacCinfo, e.id() );
            // Id funcId = Neutral::child( i->eref(), "func" );
            Id funcId = findFuncMsgSrc( *i, "setNumKf" );
            if ( funcId != Id() )
            {
                Element* fe = funcId.element();
                installAndUnschedFuncReac( funcId, (*i) );
                ZombieFunction::zombify( fe, zfCinfo,ksolve_,dsolve_);
                /*
                } else {
                cout << "Warning: Stoich::zombifyModel: Failed to connect Func to Reac :" << i->path() << endl;
                return;
                */
            }
        }
        else if ( ei->cinfo() == mmEnzCinfo )
        {
            EnzBase::zombify( ei, zombieMMenzCinfo, e.id() );
        }
        else if ( ei->cinfo() == enzCinfo )
        {
            CplxEnzBase::zombify( ei, zombieEnzCinfo, e.id() );
        }
    }
}

void Stoich::unZombifyPools()
{
    static const Cinfo* poolCinfo = Cinfo::find( "Pool" );
    static const Cinfo* bufPoolCinfo = Cinfo::find( "BufPool" );
    static const Cinfo* zombiePoolCinfo = Cinfo::find( "ZombiePool" );
    static const Cinfo* zombieBufPoolCinfo = Cinfo::find( "ZombieBufPool");
    unsigned int i;
    for ( i = 0; i < varPoolVec_.size(); ++i )
    {
        Element* e = varPoolVec_[i].element();
        if ( !e || e->isDoomed() )
            continue;
        if ( e != 0 &&  e->cinfo() == zombiePoolCinfo )
            PoolBase::zombify( e, poolCinfo, Id(), Id() );
    }

    for ( i = 0; i < bufPoolVec_.size(); ++i )
    {
        Element* e = bufPoolVec_[i].element();
        if ( !e || e->isDoomed() )
            continue;
        if ( e != 0 &&  e->cinfo() == zombieBufPoolCinfo )
            PoolBase::zombify( e, bufPoolCinfo, Id(), Id() );
    }
}

void Stoich::unZombifyModel()
{
    static const Cinfo* reacCinfo = Cinfo::find( "Reac" );
    static const Cinfo* enzCinfo = Cinfo::find( "Enz" );
    static const Cinfo* mmEnzCinfo = Cinfo::find( "MMenz" );
    static const Cinfo* functionCinfo = Cinfo::find( "Function");
    static const Cinfo* zombieReacCinfo = Cinfo::find( "ZombieReac");
    static const Cinfo* zombieMMenzCinfo = Cinfo::find( "ZombieMMenz");
    static const Cinfo* zombieEnzCinfo = Cinfo::find( "ZombieEnz");
    static const Cinfo* zombieFunctionCinfo = Cinfo::find( "ZombieFunction");

    unZombifyPools();

	vector< Id > temp = reacVec_; temp.insert( temp.end(),
					offSolverReacVec_.begin(), offSolverReacVec_.end() );
    for ( vector< Id >::iterator i = temp.begin(); i != temp.end(); ++i )
    {
        Element* e = i->element();
        if ( e != 0 &&  e->cinfo() == zombieReacCinfo )
            ReacBase::zombify( e, reacCinfo, Id() );
    }

	temp = mmEnzVec_; temp.insert( temp.end(),
					offSolverMMenzVec_.begin(), offSolverMMenzVec_.end() );
    for ( vector< Id >::iterator i = temp.begin(); i != temp.end(); ++i )
    {
        Element* e = i->element();
        if ( e != 0 &&  e->cinfo() == zombieMMenzCinfo )
            EnzBase::zombify( e, mmEnzCinfo, Id() );
    }

	temp = enzVec_; temp.insert( temp.end(),
					offSolverEnzVec_.begin(), offSolverEnzVec_.end() );
    for ( vector< Id >::iterator i = temp.begin(); i != temp.end(); ++i )
    {
        Element* e = i->element();
        if ( e != 0 &&  e->cinfo() == zombieEnzCinfo )
            CplxEnzBase::zombify( e, enzCinfo, Id() );
    }

	temp = poolFuncVec_; temp.insert( temp.end(),
		incrementFuncVec_.begin(), incrementFuncVec_.end() );
    for ( vector< Id >::iterator i = temp.begin(); i != temp.end(); ++i )
    {
        Element* e = i->element();
        if ( e != 0 && e->cinfo() == zombieFunctionCinfo )
        {
            ZombieFunction::zombify( e, functionCinfo, Id(), Id() );
            //cout << "ZombieFunction unzombify: " << e->getTick() << endl;
        }
        if ( e != 0 && e->getTick() == -2 )
        {
            int t = Clock::lookupDefaultTick( e->cinfo()->name() );
            e->setTick( t );
        }
    }
}

unsigned int Stoich::convertIdToPoolIndex( Id id ) const
{
    map< Id, unsigned int >::const_iterator i = poolLookup_.find( id );
    if ( i != poolLookup_.end() )
    {
        return i->second;
    }
    return ~0U;
}

unsigned int Stoich::convertIdToReacIndex( Id id ) const
{
    map< Id, unsigned int >::const_iterator i = rateTermLookup_.find( id );
    if ( i != rateTermLookup_.end() )
    {
        return i->second;
    }
    return ~0U;
}

unsigned int Stoich::convertIdToFuncIndex( Id id ) const
{
    map< Id, unsigned int >::const_iterator i = funcLookup_.find( id );
    if ( i != funcLookup_.end() )
    {
        return i->second;
    }
    return ~0U;
}

ZeroOrder* Stoich::makeHalfReaction(
    double rate, const vector< Id >& reactants )
{
    ZeroOrder* rateTerm = 0;
    if ( reactants.size() == 1 )
    {
        rateTerm =  new FirstOrder(
            rate, convertIdToPoolIndex( reactants[0] ) );
    }
    else if ( reactants.size() == 2 )
    {
        rateTerm = new SecondOrder( rate,
                                    convertIdToPoolIndex( reactants[0] ),
                                    convertIdToPoolIndex( reactants[1] ) );
    }
    else if ( reactants.size() > 2 )
    {
        vector< unsigned int > temp;
        for ( unsigned int i = 0; i < reactants.size(); ++i )
            temp.push_back( convertIdToPoolIndex( reactants[i] ) );
        rateTerm = new NOrder( rate, temp );
    }
    else
    {
        cout << "Warning: Stoich::makeHalfReaction: no reactants\n";
        status_ |= 1;
        rateTerm = new ZeroOrder(0.0); // Dummy RateTerm to avoid crash.
    }
    return rateTerm;
}

void Stoich::installReaction( Id reacId,
                              const vector< Id >& subs,
                              const vector< Id >& prds )
{
    static vector< Id > dummy;
    unsigned int rateIndex = innerInstallReaction( reacId, subs, prds );
    if ( rateIndex < getNumCoreRates() ) // Only handle off-compt reacs
        return;
    vector< Id > subCompt;
    vector< Id > prdCompt;
    for ( vector< Id >::const_iterator
            i = subs.begin(); i != subs.end(); ++i )
        subCompt.push_back( getCompt( *i ).id );
    for ( vector< Id >::const_iterator
            i = prds.begin(); i != prds.end(); ++i )
        prdCompt.push_back( getCompt( *i ).id );

    assert ( rateIndex - getNumCoreRates() == subComptVec_.size() );
    assert ( rateIndex - getNumCoreRates() == prdComptVec_.size() );
    if ( useOneWay_ )
    {
        subComptVec_.push_back( subCompt );
        subComptVec_.push_back( prdCompt );
        prdComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
    }
    else
    {
        subComptVec_.push_back( subCompt );
        prdComptVec_.push_back( prdCompt );
    }
}

/**
 * This takes the specified forward and reverse half-reacs belonging
 * to the specified Reac, and builds them into the Stoich.
 */
unsigned int Stoich::innerInstallReaction( Id reacId,
        const vector< Id >& subs,
        const vector< Id >& prds )
{
    ZeroOrder* forward = makeHalfReaction( 0, subs );
    ZeroOrder* reverse = makeHalfReaction( 0, prds );
    unsigned int rateIndex = convertIdToReacIndex( reacId );
    unsigned int revRateIndex = rateIndex;
    if ( useOneWay_ )
    {
        rates_[ rateIndex ] = forward;
        revRateIndex = rateIndex + 1;
        rates_[ revRateIndex ] = reverse;
    }
    else
    {
        rates_[ rateIndex ] =
            new BidirectionalReaction( forward, reverse );
    }

    vector< unsigned int > molIndex;
    vector< double > reacScaleSubstrates;
    vector< double > reacScaleProducts;

    if ( useOneWay_ )
    {
        unsigned int numReactants = forward->getReactants( molIndex );
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( molIndex[i], rateIndex );
            N_.set( molIndex[i], rateIndex, temp - 1 );
            temp = N_.get( molIndex[i], revRateIndex );
            N_.set( molIndex[i], revRateIndex, temp + 1 );
        }

        numReactants = reverse->getReactants( molIndex );
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( molIndex[i], rateIndex );
            N_.set( molIndex[i], rateIndex, temp + 1 );
            temp = N_.get( molIndex[i], revRateIndex );
            N_.set( molIndex[i], revRateIndex, temp - 1 );
        }
    }
    else
    {
        unsigned int numReactants = forward->getReactants( molIndex );
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( molIndex[i], rateIndex );
            N_.set( molIndex[i], rateIndex, temp - 1 );
        }

        numReactants = reverse->getReactants( molIndex );
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( molIndex[i], revRateIndex );
            N_.set( molIndex[i], rateIndex, temp + 1 );
        }
    }
    return rateIndex;
}

void installDummy( RateTerm** entry, Id enzId, const string& s )
{
    cout << "Warning: Stoich::installMMenz: No " << s << " for: "  <<
         enzId.path() << endl;
    *entry = new ZeroOrder( 0.0 );
}

/**
 * This takes the baseclass for an MMEnzyme and builds the
 * MMenz into the Stoich.
 */
void Stoich::installMMenz( Id enzId, const vector< Id >& enzMols,
                           const vector< Id >& subs, const vector< Id >& prds )
{
    MMEnzymeBase* meb;
    unsigned int enzSiteIndex = convertIdToReacIndex( enzId );
    RateTerm** entry = &rates_[enzSiteIndex];
    if ( enzMols.size() != 1 )
    {
        installDummy( entry, enzId, "enzmols" );
        status_ |= 2;
        return;
    }

    if ( prds.size() < 1 )
    {
        installDummy( entry, enzId, "products" );
        status_ |= 1;
        return;
    }
    unsigned int enzIndex = convertIdToPoolIndex( enzMols[0] );

    if ( subs.size() == 1 )
    {
        unsigned int subIndex = convertIdToPoolIndex( subs[0] );
        meb = new MMEnzyme1( 1, 1, enzIndex, subIndex );
    }
    else if ( subs.size() > 1 )
    {
        vector< unsigned int > v;
        for ( unsigned int i = 0; i < subs.size(); ++i )
            v.push_back( convertIdToPoolIndex( subs[i] ) );
        ZeroOrder* rateTerm = new NOrder( 1.0, v );
        meb = new MMEnzyme( 1, 1, enzIndex, rateTerm );
    }
    else
    {
        installDummy( entry, enzId, "substrates" );
        status_ |= 2;
        return;
    }
    installMMenz( meb, enzSiteIndex, subs, prds );
    if ( enzSiteIndex < getNumCoreRates() ) // Only handle off-compt reacs
        return;
    vector< Id > subCompt;
    vector< Id > dummy;
    for ( vector< Id >::const_iterator
            i = subs.begin(); i != subs.end(); ++i )
        subCompt.push_back( getCompt( *i ).id );
    subComptVec_.push_back( subCompt );
    prdComptVec_.push_back( dummy );
    assert ( enzSiteIndex - getNumCoreRates() == subComptVec_.size() );
    assert ( enzSiteIndex - getNumCoreRates() == prdComptVec_.size() );
}

/// This is the internal variant to install the MMenz.
void Stoich::installMMenz( MMEnzymeBase* meb, unsigned int rateIndex,
                           const vector< Id >& subs, const vector< Id >& prds )
{
    rates_[rateIndex] = meb;

    for ( unsigned int i = 0; i < subs.size(); ++i )
    {
        unsigned int poolIndex = convertIdToPoolIndex( subs[i] );
        int temp = N_.get( poolIndex, rateIndex );
        N_.set( poolIndex, rateIndex, temp - 1 );
    }
    for ( unsigned int i = 0; i < prds.size(); ++i )
    {
        unsigned int poolIndex = convertIdToPoolIndex( prds[i] );
        int temp = N_.get( poolIndex, rateIndex );
        N_.set( poolIndex, rateIndex, temp + 1 );
    }
}

// Handles dangling enzymes.
void Stoich::installDummyEnzyme( Id enzId, Id enzMolId )
{
    ZeroOrder* r1 = new ZeroOrder( 0.0 ); // Dummy
    ZeroOrder* r2 = new ZeroOrder( 0.0 ); // Dummy
    ZeroOrder* r3 = new ZeroOrder( 0.0 ); // Dummy
    vector< Id > dummy;
    unsigned int rateIndex = convertIdToReacIndex( enzId );
    if ( useOneWay_ )
    {
        rates_[ rateIndex ] = r1;
        rates_[ rateIndex + 1 ] = r2;
        rates_[ rateIndex + 2 ] = r3;
    }
    else
    {
        rates_[ rateIndex ] = new BidirectionalReaction( r1, r2 );
        rates_[ rateIndex + 1 ] = r3;
    }
    status_ = 1;
}

void Stoich::installEnzyme( Id enzId, Id enzMolId, Id cplxId,
                            const vector< Id >& subs, const vector< Id >& prds )
{
    vector< Id > temp( subs );
    temp.insert( temp.begin(), enzMolId );
    ZeroOrder* r1 = makeHalfReaction( 0, temp );
    temp.clear();
    temp.resize( 1, cplxId );
    ZeroOrder* r2 = makeHalfReaction( 0, temp );
    ZeroOrder* r3 = makeHalfReaction( 0, temp );

    installEnzyme( r1, r2, r3, enzId, enzMolId, prds );
    unsigned int rateIndex = convertIdToReacIndex( enzId );
    if ( rateIndex < getNumCoreRates() ) // Only handle off-compt reacs
        return;
    vector< Id > subCompt;
    vector< Id > dummy;
    for ( vector< Id >::const_iterator
            i = subs.begin(); i != subs.end(); ++i )
        subCompt.push_back( getCompt( *i ).id );

    if ( useOneWay_ )
    {
        // enz is split into 3 reactions. Only the first might be off-compt
        subComptVec_.push_back( subCompt );
        subComptVec_.push_back( dummy );
        subComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
        //assert ( 2 + rateIndex - getNumCoreRates() == subComptVec_.size());
        //assert ( 2 + rateIndex - getNumCoreRates() == prdComptVec_.size());
    }
    else
    {
        // enz is split into 2 reactions. Only the first might be off-compt
        subComptVec_.push_back( subCompt );
        subComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
        prdComptVec_.push_back( dummy );
        //assert ( 1+rateIndex - getNumCoreRates() == subComptVec_.size() );
        // assert ( 1+rateIndex - getNumCoreRates() == prdComptVec_.size() );
    }
}

void Stoich::installEnzyme( ZeroOrder* r1, ZeroOrder* r2, ZeroOrder* r3,
                            Id enzId, Id enzMolId, const vector< Id >& prds )
{
    unsigned int rateIndex = convertIdToReacIndex( enzId );

    if ( useOneWay_ )
    {
        rates_[ rateIndex ] = r1;
        rates_[ rateIndex + 1 ] = r2;
        rates_[ rateIndex + 2 ] = r3;
    }
    else
    {
        rates_[ rateIndex ] = new BidirectionalReaction( r1, r2 );
        rates_[ rateIndex + 1 ] = r3;
    }

    vector< unsigned int > poolIndex;
    unsigned int numReactants = r2->getReactants( poolIndex );
    assert( numReactants == 1 ); // Should be cplx as the only product
    unsigned int cplxPool = poolIndex[0];

    if ( useOneWay_ )
    {
        numReactants = r1->getReactants( poolIndex ); // Substrates
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( poolIndex[i], rateIndex ); // terms for r1
            N_.set( poolIndex[i], rateIndex, temp - 1 );
            temp = N_.get( poolIndex[i], rateIndex + 1 ); //terms for r2
            N_.set( poolIndex[i], rateIndex + 1, temp + 1 );
        }

        int temp = N_.get( cplxPool, rateIndex );	// term for r1
        N_.set( cplxPool, rateIndex, temp + 1 );
        temp = N_.get( cplxPool, rateIndex + 1 );	// term for r2
        N_.set( cplxPool, rateIndex + 1, temp -1 );
    }
    else     // Regular bidirectional reactions.
    {
        numReactants = r1->getReactants( poolIndex ); // Substrates
        for ( unsigned int i = 0; i < numReactants; ++i )
        {
            int temp = N_.get( poolIndex[i], rateIndex );
            N_.set( poolIndex[i], rateIndex, temp - 1 );
        }
        int temp = N_.get( cplxPool, rateIndex );
        N_.set( cplxPool, rateIndex, temp + 1 );
    }

    // Now assign reaction 3. The complex is the only substrate here.
    // Reac 3 is already unidirectional, so all we need to do to handle
    // one-way reactions is to get the index right.
    unsigned int reac3index = ( useOneWay_ ) ? rateIndex + 2 : rateIndex + 1;
    int temp = N_.get( cplxPool, reac3index );
    N_.set( cplxPool, reac3index, temp - 1 );

    // For the products, we go to the prd list directly.
    for ( unsigned int i = 0; i < prds.size(); ++i )
    {
        unsigned int j = convertIdToPoolIndex( prds[i] );
        int temp = N_.get( j, reac3index );
        N_.set( j, reac3index, temp + 1 );
    }
    // Enz is also a product here.
    unsigned int enzPool = convertIdToPoolIndex( enzMolId );
    temp = N_.get( enzPool, reac3index );
    N_.set( enzPool, reac3index, temp + 1 );
}

//////////////////////////////////////////////////////////////
// Field interface functions
//////////////////////////////////////////////////////////////

/**
 * Sets the forward rate v (given in millimoloar concentration units)
 * for the specified reaction throughout the compartment in which the
 * reaction lives. Internally the stoich uses #/voxel units so this
 * involves querying the volume subsystem about volumes for each
 * voxel, and scaling accordingly.
 * For now assume a uniform voxel volume and hence just convert on
 * 0 meshIndex.
 */
void Stoich::setReacKf( const Eref& e, double v ) const
{
    unsigned int i = convertIdToReacIndex( e.id() );
    if ( i != ~0U )
    {
        // rates_[ i ]->setR1( v / volScale );
        rates_[ i ]->setR1( v );
        kinterface_->updateRateTerms( i );
    }
}

/**
 * For now assume a single rate term.
 */
void Stoich::setReacKb( const Eref& e, double v ) const
{
    unsigned int i = convertIdToReacIndex( e.id() );
    if ( i == ~0U )
        return;

    if ( useOneWay_ )
    {
        rates_[ i + 1 ]->setR1( v );
        kinterface_->updateRateTerms( i + 1 );
    }
    else
    {
        rates_[ i ]->setR2( v );
        kinterface_->updateRateTerms( i );
    }
}

// This uses Km to set the StoichR1, which is actually in # units.
// It is OK to do for the Stoich because the volume is defined to be
// 1.66e-21, such that conc == #.
void Stoich::setMMenzKm( const Eref& e, double v ) const
{
    // Identify MMenz rate term
    unsigned int index = convertIdToReacIndex( e.id() );
    RateTerm* rt = rates_[ index ];
    //MMEnzymeBase* enz = dynamic_cast< MMEnzymeBase* >( rt );
    //assert( enz );
    // Identify MMenz Enzyme substrate. I would have preferred the parent,
    // but that gets messy.
    // unsigned int enzMolIndex = enz->getEnzIndex();

    // This function can be replicated to handle multiple different voxels.
    /*
    vector< double > vols;
    getReactantVols( e, subOut, vols );
    if ( vols.size() == 0 ) {
    	cerr << "Error: Stoich::setMMenzKm: no substrates for enzyme " <<
    		e << endl;
    	return;
    }
    */
    // Do scaling and assignment.
    rt->setR1( v );
    kinterface_->updateRateTerms( index );
}

double Stoich::getMMenzNumKm( const Eref& e ) const
{
    return getR1( e );
}

void Stoich::setMMenzKcat( const Eref& e, double v ) const
{
    unsigned int index = convertIdToReacIndex( e.id() );
    RateTerm* rt = rates_[ index ];
    // MMEnzymeBase* enz = dynamic_cast< MMEnzymeBase* >( rt );
    // assert( enz );

    rt->setR2( v );
    kinterface_->updateRateTerms( index );
}

double Stoich::getMMenzKcat( const Eref& e ) const
{
    return getR2( e );
}

/// Later handle all the volumes when this conversion is done.
void Stoich::setEnzK1( const Eref& e, double v ) const
{
    unsigned int index = convertIdToReacIndex( e.id() );

    rates_[ index  ]->setR1( v );
    kinterface_->updateRateTerms( index );
}

void Stoich::setEnzK2( const Eref& e, double v ) const
{
    unsigned int index = convertIdToReacIndex( e.id() );
    if ( useOneWay_ )
    {
        rates_[ index + 1 ]->setR1( v );
        kinterface_->updateRateTerms( index + 1 );
    }
    else
    {
        rates_[ index ]->setR2( v );
        kinterface_->updateRateTerms( index );
    }
}

void Stoich::setEnzK3( const Eref& e, double v ) const
{
    unsigned int index = convertIdToReacIndex( e.id() );
    if ( useOneWay_ )
    {
        rates_[ index + 2 ]->setR1( v );
        kinterface_->updateRateTerms( index + 2 );
    }
    else
    {
        rates_[ index + 1 ]->setR1( v );
        kinterface_->updateRateTerms( index + 1 );
    }
}

double Stoich::getEnzNumK1( const Eref& e ) const
{
    return getR1( e );
}

double Stoich::getEnzK2( const Eref& e ) const
{
    if ( useOneWay_ )
        return getR1offset1( e );
    else
        return getR2( e );
}

double Stoich::getEnzK3( const Eref& e ) const
{
    if ( useOneWay_ )
        return getR1offset2( e );
    else
        return getR1offset1( e );
}

/**
 * Looks up the matching rate for R1. Later we may have additional
 * scaling terms for the specified voxel.
 */
double Stoich::getR1( const Eref& e ) const
{
    return rates_[ convertIdToReacIndex( e.id() ) ]->getR1();
}
double Stoich::getR1offset1( const Eref& e ) const
{
    return rates_[ convertIdToReacIndex( e.id() ) + 1 ]->getR1();
}
double Stoich::getR1offset2( const Eref& e ) const
{
    return rates_[ convertIdToReacIndex( e.id() ) + 2 ]->getR1();
}

/**
 * Looks up the matching rate for R2. Later we may have additional
 * scaling terms for the specified voxel.
 */
double Stoich::getR2( const Eref& e ) const
{
    return rates_[ convertIdToReacIndex( e.id() ) ]->getR2();
}

void Stoich::setFunctionExpr( const Eref& e, string expr )
{
    unsigned int index = convertIdToReacIndex( e.id() );
    FuncRate* fr = 0;
    if ( index != ~0U )
        fr = dynamic_cast< FuncRate* >( rates_[index] );
    if ( fr )
    {
        fr->setExpr( expr );
    }
    else
    {
        index = convertIdToFuncIndex( e.id() );
        if ( index != ~0U )
        {
            FuncTerm *ft = dynamic_cast< FuncTerm* >( funcs_[index] );
            if ( ft )
            {
                ft->setExpr( expr );
                return;
            }
        }
        cout << "Warning: Stoich::setFunctionExpr( " << e.id().path() <<
             ", " << expr << " ): func not found";
    }
}
/////////////////////////////////////////////////////////////////////
SpeciesId Stoich::getSpecies( unsigned int poolIndex ) const
{
    return species_[ poolIndex ];
}

void Stoich::setSpecies( unsigned int poolIndex, SpeciesId s )
{
    species_[ poolIndex ] = s;
}

// for debugging.
void Stoich::print() const
{
    N_.print();
}

// for debugging
void Stoich::printRates() const
{
    for ( vector< Id >::const_iterator
            i = reacVec_.begin(); i != reacVec_.end(); ++i )
    {
        double Kf = Field< double >::get( *i, "Kf");
        double Kb = Field< double >::get( *i, "Kb");
        double kf = Field< double >::get( *i, "kf");
        double kb = Field< double >::get( *i, "kb");
        cout << "Id=" << *i << ", (Kf,Kb) = (" << Kf << ", " << Kb <<
             "), (kf, kb) = (" << kf << ", " << kb << ")\n";
    }
}

/////////////////////////////////////////////////////////////////////
const vector< Id >& Stoich::getOffSolverPools() const
{
    return offSolverPoolVec_;
}

vector< Id > Stoich::getOffSolverCompts() const
{
    vector< Id > ret;
    for ( map< Id, vector< Id > >::const_iterator
            i = offSolverPoolMap_.begin(); i != offSolverPoolMap_.end(); ++i )
        ret.push_back( i->first );

    return ret;
}

const vector< Id >& Stoich::offSolverPoolMap( Id compt ) const
{
    static vector< Id > blank( 0 );
    map< Id, vector < Id > >::const_iterator i =
        offSolverPoolMap_.find( compt );
    if ( i != offSolverPoolMap_.end() )
        return i->second;
    return blank;
}

/////////////////////////////////////////////////////////////////////
// Numeric funcs. These are in Stoich because the rate terms are here.
/////////////////////////////////////////////////////////////////////


// s is the array of pools, S_[meshIndex][0]
void Stoich::updateFuncs( double* s, double t ) const
{
    for ( vector< FuncTerm* >::const_iterator i = funcs_.begin();
            i != funcs_.end(); ++i )
    {
        if ( *i )
        {
            (*i)->evalPool( s, t );
            // s holds arguments and also result location
        }
    }
}

/**
 * updateJunctionRates:
 * Updates the rates for cross-compartment reactions. These are located
 * at the end of the rates_ vector, and are directly indexed by the
 * reacTerms.
void Stoich::updateJunctionRates( const double* s,
	const vector< unsigned int >& reacTerms, double* yprime )
{
	for ( vector< unsigned int >::const_iterator i = reacTerms.begin();
					i != reacTerms.end(); ++i )
	{
			assert( *i < rates_[0].size() );
			*yprime++ += (*rates_[0][*i])( s );
	}
}
 */

void Stoich::updateRatesAfterRemesh()
{
    for ( vector< Id >::iterator
            i = reacVec_.begin(); i != reacVec_.end(); ++i )
    {
        double Kf = Field< double >::get( *i, "Kf");
        double Kb = Field< double >::get( *i, "Kb");
        setReacKf( i->eref(), Kf );
        setReacKb( i->eref(), Kb );
    }
    vector< Id >::iterator i;
    for (i = offSolverReacVec_.begin(); i != offSolverReacVec_.end(); ++i)
    {
        assert ( i->element()->cinfo()->isA( "ReacBase" ) );
        double Kf = Field< double >::get( *i, "Kf");
        double Kb = Field< double >::get( *i, "Kb");
        setReacKf( i->eref(), Kf );
        setReacKb( i->eref(), Kb );
    }
    for (i = offSolverEnzVec_.begin(); i != offSolverEnzVec_.end(); ++i)
    {
        assert ( i->element()->cinfo()->isA( "CplxEnzBase" ) );
        double concK1 = Field< double >::get( *i, "concK1");
        double k3 = Field< double >::get( *i, "k3");
        double k2 = Field< double >::get( *i, "k2");
        setEnzK3( i->eref(), k3 );
        setEnzK2( i->eref(), k2 );
        setEnzK1( i->eref(), concK1 );
    }
    for (i=offSolverMMenzVec_.begin(); i != offSolverMMenzVec_.end(); ++i)
    {
        assert( i->element()->cinfo()->isA( "MMEnzBase" ) );
        double Km = Field< double >::get( *i, "Km");
        double kcat = Field< double >::get( *i, "kcat");
        setMMenzKm( i->eref(), Km );
        setMMenzKcat( i->eref(), kcat );
    }
}


/*
unsigned int Stoich::indexOfMatchingVolume( double vol ) const
{
	assert( rates_.size() == uniqueVols_.size() );
	assert( rates_.size() > 0 );

	if ( rates_.size() == 1 && uniqueVols_[0] < 0 ) {
		return 0;
	}
	double bigVol = uniqueVols_[0];
	for ( unsigned int i = 0; i < uniqueVols_.size(); ++i ) {
		if ( doubleEq( vol/bigVol, uniqueVols_[i]/bigVol ) )
			return i;
	}
	assert( 0 );
	return 0;
}
*/


/////////////////////////////////////////////////////////////////////////
// Functions for resizing of specified voxels
/////////////////////////////////////////////////////////////////////////

void Stoich::scaleBufsAndRates( unsigned int index, double volScale )
{
    if ( !kinterface_ || status_ != 0 )
        return;
    kinterface_->pools( index )->scaleVolsBufsRates( volScale, this );
}
