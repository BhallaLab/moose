/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"
#include "../shell/Shell.h"
#include "Dinfo.h"

// Static declaration.
unsigned int Cinfo::numCoreOpFunc_ = 0;

Cinfo::Cinfo( const string& name,
				const Cinfo* baseCinfo,
				Finfo** finfoArray,
				unsigned int nFinfos,
				DinfoBase* d,
				const string* doc,
				unsigned int numDoc,
				bool banCreation
)
		: name_( name ), baseCinfo_( baseCinfo ), dinfo_( d ),
			numBindIndex_( 0 ), banCreation_( banCreation )
{
	if ( cinfoMap().find( name ) != cinfoMap().end() ) {
		cout << "Warning: Duplicate Cinfo name " << name << endl;
	}
	init( finfoArray, nFinfos );
	cinfoMap()[ name ] = this;
	doc_.clear();

#if  VERBOSITY > 2
	cerr << setw(VERBOSITY) << "Doing initCinfo for " << name << " with numDoc = " << numDoc << endl;
#endif     /* -----  not DEBUG  ----- */
	if ( doc && numDoc ) {
		for ( unsigned int i = 0; i < numDoc - 1; i += 2 ) {
			const string argName = doc[i];
			const string argVal = doc[i+1];
			// cout << "in initCinfo for " << name << ", doc[" << i << "] = " << doc[i] << ", " << doc[i+1] << endl;
			// doc_[ doc[i] ] = doc[i+i];
			doc_[ argName ] = argVal;
		}
	}
}

Cinfo::Cinfo()
		: name_( "dummy" ), baseCinfo_( 0 ), dinfo_( 0 ),
			numBindIndex_( 0 ), banCreation_( false )
{;}

Cinfo::Cinfo( const Cinfo& other )
		: name_( "dummy" ), baseCinfo_( 0 ), dinfo_( 0 ),
			numBindIndex_( 0 ), banCreation_( false )
{;}

/*
const Cinfo& Cinfo::operator=( const Cinfo& other )
{
	name_ = other.name_;
	baseCinfo_ = other.baseCinfo_;
	dinfo_ = 0;
	numBindIndex_ = other.numBindIndex_;
	doc_ = doc_;
	finfoMap_ = other
}
*/

Cinfo::~Cinfo()
{;} // The dinfos are statically allocated, we don't free them.

////////////////////////////////////////////////////////////////////
// Initialization funcs
////////////////////////////////////////////////////////////////////

/**
 * init: initializes the Cinfo. Must be called just once
 */
void Cinfo::init( Finfo** finfoArray, unsigned int nFinfos )
{
        if ( baseCinfo_ ) {
		// Copy over base Finfos.
		numBindIndex_ = baseCinfo_->numBindIndex_;
		finfoMap_ = baseCinfo_->finfoMap_;
		funcs_ = baseCinfo_->funcs_;
		postCreationFinfos_ = baseCinfo_->postCreationFinfos_;
	}
	for ( unsigned int i = 0; i < nFinfos; i++ ) {
		registerFinfo( finfoArray[i] );
	}
}

FuncId Cinfo::registerOpFunc( const OpFunc* f )
{
	FuncId ret = funcs_.size();
	funcs_.push_back( f );
	return ret;
}

void Cinfo::overrideFunc( FuncId fid, const OpFunc* f )
{
	assert ( funcs_.size() > fid );
	funcs_[ fid ] = f;
}

BindIndex Cinfo::registerBindIndex()
{
	return numBindIndex_++;
}

void Cinfo::registerFinfo( Finfo* f )
{
		finfoMap_[ f->name() ] = f;
		f->registerFinfo( this );
		if ( dynamic_cast< DestFinfo* >( f ) ) {
			destFinfos_.push_back( f );
		}
		else if ( dynamic_cast< SrcFinfo* >( f ) ) {
			srcFinfos_.push_back( f );
		}
		else if ( dynamic_cast< ValueFinfoBase* >( f ) ) {
			valueFinfos_.push_back( f );
		}
		else if ( dynamic_cast< LookupValueFinfoBase* >( f ) ) {
			lookupFinfos_.push_back( f );
		}
		else if ( dynamic_cast< SharedFinfo* >( f ) ) {
			sharedFinfos_.push_back( f );
		}
		else if ( dynamic_cast< FieldElementFinfoBase* >( f ) ) {
			fieldElementFinfos_.push_back( f );
		}
}

void Cinfo::registerPostCreationFinfo( const Finfo* f )
{
	postCreationFinfos_.push_back( f );
}

void Cinfo::postCreationFunc( Id newId, Element* newElm ) const
{
	for ( vector< const Finfo* >::const_iterator i =
		postCreationFinfos_.begin();
		i != postCreationFinfos_.end(); ++i )
		(*i)->postCreationFunc( newId, newElm );
}

void buildFinfoElement( Id parent, vector< Finfo* >& f, const string& name )
{
	if ( f.size() > 0 ) {
		char* data = reinterpret_cast< char* >( &f[0] );
		Id id = Id::nextId();
		Element* e = new GlobalDataElement(
						id, Finfo::initCinfo(), name, f.size() );
		Finfo::initCinfo()->dinfo()->assignData( e->data( 0 ), f.size(), data, f.size());
		Shell::adopt( parent, id, 0 );
	}
}

// Static function called by init()
void Cinfo::makeCinfoElements( Id parent )
{
	static Dinfo< Cinfo > dummy;
	vector< unsigned int > dims( 1, 0 );

	vector< Id > cinfoElements;
	for ( map< string, Cinfo* >::iterator i = cinfoMap().begin();
		i != cinfoMap().end(); ++i ) {
		Id id = Id::nextId();
		char* data = reinterpret_cast< char* >( i->second );
		Element* e = new GlobalDataElement(
						id, Cinfo::initCinfo(), i->first );
		Cinfo::initCinfo()->dinfo()->assignData( e->data( 0 ), 1, data, 1 );
		// Cinfo* temp = reinterpret_cast< Cinfo* >( e->data( 0 ) );

		Shell::adopt( parent, id, 0 );
		cinfoElements.push_back( id );
		// cout << "Cinfo::makeCinfoElements: parent= " << parent << ", Id = " << id << ", name = " << i->first << endl;
	}
	vector< Id >::iterator j = cinfoElements.begin();
	for ( map< string, Cinfo* >::iterator i = cinfoMap().begin();
		i != cinfoMap().end(); ++i ) {
		buildFinfoElement( *j, i->second->srcFinfos_, "srcFinfo" );
		buildFinfoElement( *j, i->second->destFinfos_, "destFinfo" );
		buildFinfoElement( *j, i->second->valueFinfos_, "valueFinfo" );
		buildFinfoElement( *j, i->second->lookupFinfos_, "lookupFinfo" );
		buildFinfoElement( *j, i->second->sharedFinfos_, "sharedFinfo" );
		buildFinfoElement( *j, i->second->fieldElementFinfos_, "fieldElementFinfo" );
		j++;
	}
}

//////////////////////////////////////////////////////////////////////
// Look up operations.
//////////////////////////////////////////////////////////////////////

const Cinfo* Cinfo::find( const string& name )
{
	map<string, Cinfo*>::iterator i = cinfoMap().find(name);
	if ( i != cinfoMap().end() )
		return i->second;

#ifdef  RESULT_CHECK
        stringstream ss;
        ss << "+ " << name << " not found. Available names are " << endl;
        ss << mapToString<string, Cinfo*>( cinfoMap() );
        dump(ss.str(), "DEBUG");
#endif     /* -----  not RESULT_CHECK  ----- */

	return 0;
}

const Cinfo* Cinfo::baseCinfo() const
{
	return baseCinfo_;
}

/**
 * Looks up Finfo from name.
 */
const Finfo* Cinfo::findFinfo( const string& name ) const
{
	map< string, Finfo*>::const_iterator i = finfoMap_.find( name );
	if ( i != finfoMap_.end() )
		return i->second;
	return 0;
}

bool Cinfo::banCreation() const
{
	return banCreation_;
}

/**
 * looks up OpFunc by FuncId
 */
const OpFunc* Cinfo::getOpFunc( FuncId fid ) const {
	if ( fid < funcs_.size () )
		return funcs_[ fid ];
	return 0;
}

/*
FuncId Cinfo::getOpFuncId( const string& funcName ) const {
	map< string, FuncId >::const_iterator i = opFuncNames_.find( funcName );
	if ( i != opFuncNames_.end() ) {
		return i->second;
	}
	return 0;
}
*/

////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////

const std::string& Cinfo::name() const
{
	return name_;
}

unsigned int Cinfo::numBindIndex() const
{
	return numBindIndex_;
}

const map< string, Finfo* >& Cinfo::finfoMap() const
{
	return finfoMap_;
}

const DinfoBase* Cinfo::dinfo() const
{
	return dinfo_;
}

bool Cinfo::isA( const string& ancestor ) const
{
	if ( ancestor == "Neutral" ) return 1;
	const Cinfo* base = this;
	while( base && base != Neutral::initCinfo() ) {
		if ( ancestor == base->name_ )
			return 1;
		base = base->baseCinfo_;
	}
	return 0;
}

void Cinfo::reportFids() const
{
	for ( map< string, Finfo*>::const_iterator i = finfoMap_.begin();
		i != finfoMap_.end(); ++i ) {
		const DestFinfo* df = dynamic_cast< const DestFinfo* >(
			i->second );
		if ( df ) {
			cout << df->getFid() << "	" << df->name() << endl;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Private functions.
////////////////////////////////////////////////////////////////////////
map<string, Cinfo*>& Cinfo::cinfoMap()
{
	static map<std::string, Cinfo*> lookup_;
	return lookup_;
}



/*
map< OpFunc, FuncId >& Cinfo::funcMap()
{
	static map< OpFunc, FuncId > lookup_;
	return lookup_;
}
*/

////////////////////////////////////////////////////////////////////////
// MOOSE class functions.
////////////////////////////////////////////////////////////////////////

const Cinfo* Cinfo::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyValueFinfo< Cinfo, string > docs(
			"docs",
			"Documentation",
			&Cinfo::getDocs
		);

		static ReadOnlyValueFinfo< Cinfo, string > baseClass(
			"baseClass",
			"Name of base class",
			&Cinfo::getBaseClass
		);

    static string doc[] =
    {
        "Name", "Cinfo",
        "Author", "Upi Bhalla",
        "Description", "Class information object."
    };

	static Finfo* cinfoFinfos[] = {
		&docs,				// ReadOnlyValue
		&baseClass,		// ReadOnlyValue
	};

	static Dinfo< Cinfo > dinfo;
	static Cinfo cinfoCinfo (
		"Cinfo",
		Neutral::initCinfo(),
		cinfoFinfos,
		sizeof( cinfoFinfos ) / sizeof ( Finfo* ),
		&dinfo,
        doc,
        sizeof(doc)/sizeof(string)
	);

	return &cinfoCinfo;
}

static const Cinfo* cinfoCinfo = Cinfo::initCinfo();


///////////////////////////////////////////////////////////////////
// Field functions
///////////////////////////////////////////////////////////////////
string Cinfo::getDocs() const
{
    ostringstream doc;
    for (map <string, string>::const_iterator ii = doc_.begin(); ii != doc_.end(); ++ii){
        doc << '\n' << ii->first << ":\t\t" << ii->second << endl;
    }
	return doc.str();
}


static DestFinfo dummy(
		"dummy",
		"This Finfo is a dummy. If you are reading this you have used an invalid index",
0 );

string Cinfo::getBaseClass() const
{
	if ( baseCinfo_ )
		return baseCinfo_->name();
	else
		return "none";
}

////////////////////////////////////////////////////////////////////
// Below we have a set of functions for getting various categories of
// Finfos. These also return the base class finfos. The baseclass finfos
// come first, then the new finfos. This is a bit messy because it changes
// the indices of the new finfos, but I shouldn't be looking them up
// by index anyway.
////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getSrcFinfo( unsigned int i ) const
{
	if ( i >= getNumSrcFinfo() )
		return 0;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumSrcFinfo() )
			return srcFinfos_[ i - baseCinfo_->getNumSrcFinfo() ];
		else
			return baseCinfo_->getSrcFinfo( i );
			//return const_cast< Cinfo* >( baseCinfo_ )->getSrcFinfo( i );
	}

	return srcFinfos_[i];
}

unsigned int Cinfo::getNumSrcFinfo() const
{
	if ( baseCinfo_ )
		return srcFinfos_.size() + baseCinfo_->getNumSrcFinfo();
	else
		return srcFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getDestFinfo( unsigned int i ) const
{
	if ( i >= getNumDestFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumDestFinfo() )
			return destFinfos_[ i - baseCinfo_->getNumDestFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getDestFinfo( i );
	}

	return destFinfos_[i];
}

unsigned int Cinfo::getNumDestFinfo() const
{
	if ( baseCinfo_ )
		return destFinfos_.size() + baseCinfo_->getNumDestFinfo();
	else
		return destFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getValueFinfo( unsigned int i ) const
{
	if ( i >= getNumValueFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumValueFinfo() )
			return valueFinfos_[ i - baseCinfo_->getNumValueFinfo() ];
		else
			return const_cast< Cinfo* >(baseCinfo_)->getValueFinfo( i );
	}

	return valueFinfos_[i];
}

unsigned int Cinfo::getNumValueFinfo() const
{
	if ( baseCinfo_ )
		return valueFinfos_.size() + baseCinfo_->getNumValueFinfo();
	else
		return valueFinfos_.size();
}


////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getLookupFinfo( unsigned int i ) const
{
	if ( i >= getNumLookupFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumLookupFinfo() )
			return lookupFinfos_[ i - baseCinfo_->getNumLookupFinfo() ];
		else
			return const_cast< Cinfo* >(baseCinfo_)->getLookupFinfo( i );
	}

	return lookupFinfos_[i];
}

unsigned int Cinfo::getNumLookupFinfo() const
{
	if ( baseCinfo_ )
		return lookupFinfos_.size() + baseCinfo_->getNumLookupFinfo();
	else
		return lookupFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getSharedFinfo( unsigned int i )
{
	if ( i >= getNumSharedFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumSharedFinfo() )
			return sharedFinfos_[ i - baseCinfo_->getNumSharedFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getSharedFinfo( i );
	}

	return sharedFinfos_[i];
}

unsigned int Cinfo::getNumSharedFinfo() const
{
	if ( baseCinfo_ )
		return sharedFinfos_.size() + baseCinfo_->getNumSharedFinfo();
	else
		return sharedFinfos_.size();
}

////////////////////////////////////////////////////////////////////
Finfo* Cinfo::getFieldElementFinfo( unsigned int i ) const
{
	if ( i >= getNumFieldElementFinfo() )
		return &dummy;
	if ( baseCinfo_ ) {
		if ( i >= baseCinfo_->getNumFieldElementFinfo() )
			return fieldElementFinfos_[ i - baseCinfo_->getNumFieldElementFinfo() ];
		else
			return const_cast< Cinfo* >( baseCinfo_ )->getFieldElementFinfo( i );
	}

	return fieldElementFinfos_[i];
}

unsigned int Cinfo::getNumFieldElementFinfo() const
{
	if ( baseCinfo_ )
		return fieldElementFinfos_.size() + baseCinfo_->getNumFieldElementFinfo();
	else
		return fieldElementFinfos_.size();
}

////////////////////////////////////////////////////////////////////
void Cinfo::setNumFinfo( unsigned int val ) // Dummy function
{
	;
}

////////////////////////////////////////////////////////////////////
const string& Cinfo::srcFinfoName( BindIndex bid ) const
{
	static const string err = "";
	for ( vector< Finfo* >::const_iterator i = srcFinfos_.begin();
		i != srcFinfos_.end(); ++i ) {
		const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( *i );
		assert( sf );
		if ( sf->getBindIndex() == bid ) {
			return sf->name();
		}
	}
	if ( baseCinfo_ )
		return baseCinfo_->srcFinfoName( bid );
	cout << "Error: Cinfo::srcFinfoName( " << bid << " ): not found\n";
	return err;
}

const string& Cinfo::destFinfoName( FuncId fid ) const
{
	static const string err = "";
	for ( vector< Finfo* >::const_iterator i = destFinfos_.begin();
		i != destFinfos_.end(); ++i ) {
		const DestFinfo* df = dynamic_cast< const DestFinfo* >( *i );
		assert( df );
		if ( df->getFid() == fid ) {
			return df->name();
		}
	}
	if ( baseCinfo_ )
		return baseCinfo_->destFinfoName( fid );
	cout << "Error: Cinfo::destFinfoName( " << fid << " ): not found\n";
	return err;
}

////////////////////////////////////////////////////////////////////

// Static function. Should be called soonest after static initialization.
void Cinfo::rebuildOpIndex()
{
	numCoreOpFunc_ = OpFunc::rebuildOpIndex();
	unsigned int num = 0;
	for ( map< string, Cinfo* >::iterator
					i = cinfoMap().begin(); i != cinfoMap().end(); ++i )
	{
		vector< const OpFunc* >& vec = i->second->funcs_;
		for( vector< const OpFunc* >::iterator
				j = vec.begin(); j != vec.end(); ++j )
		{
			OpFunc* of = const_cast< OpFunc* >( *j );
			num += of->setIndex( num );
		}
	}
	// cout << "on node " << Shell::myNode() << ": oldNumOpFunc = " << numCoreOpFunc_ << ", new = " << num << endl;
	numCoreOpFunc_ = num;
}
