/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Dinfo.h"
#include "ElementValueFinfo.h"
#include "LookupElementValueFinfo.h"
#include "Shell.h"

const Cinfo* Neutral::initCinfo()
{
	/////////////////////////////////////////////////////////////////
	// Element Value Finfos
	/////////////////////////////////////////////////////////////////

	static ElementValueFinfo< Neutral, string > name( 
		"name",
		"Name of object", 
		&Neutral::setName, 
		&Neutral::getName );

	// Should be renamed to myId
	static ReadOnlyElementValueFinfo< Neutral, ObjId > me( 
		"me",
		"ObjId for current object", 
			&Neutral::getObjId );

	static ReadOnlyElementValueFinfo< Neutral, ObjId > parent( 
		"parent",
		"Parent ObjId for current object", 
			&Neutral::getParent );

	static ReadOnlyElementValueFinfo< Neutral, vector< Id > > children( 
		"children",
		"vector of ObjIds listing all children of current object", 
			&Neutral::getChildren );

	static ReadOnlyElementValueFinfo< Neutral, string > path( 
		"path",
		"text path for object", 
			&Neutral::getPath );

	/*
			*/
	static ReadOnlyElementValueFinfo< Neutral, string > className( 
		"className",
		"Class Name of object", 
			&Neutral::getClass );

	static ElementValueFinfo< Neutral, unsigned int > numData( 
		"numData",
		"# of Data entries on Element."
		"Note that on a FieldElement this does NOT refer to field entries,"
		"but to the number of DataEntries on the parent of the "
		"FieldElement.",
			&Neutral::setNumData,
			&Neutral::getNumData );

	static ElementValueFinfo< Neutral, unsigned int > numField( 
		"numField",
		"For a FieldElement: number of entries of self."
		"For a regular Element: One.",
			&Neutral::setNumField,
			&Neutral::getNumField );

	static ReadOnlyElementValueFinfo< Neutral, unsigned int > id(
		"idValue",
		"Object id of self, converted to an unsigned int.",
			&Neutral::getId );

	static ReadOnlyElementValueFinfo< Neutral, unsigned int > index( 
		"index",
		"For a FieldElement: Object index of parent."
		"For a regular Element: Object index (dataId) of self.",
			&Neutral::getIndex );

	static ReadOnlyElementValueFinfo< Neutral, unsigned int > fieldIndex( 
		"fieldIndex",
		"For a FieldElement: field Index of self."
		"For a regular Element: zero.",
			&Neutral::getFieldIndex );

	static ElementValueFinfo< Neutral, int > 
		tick( 
		"tick",
		"Clock tick for this Element for periodic execution in the "
		"main simulation event loop. A default is normally assigned, "
	    "based on object class, but one can override to any value "
		"between 0 and 19. Assigning to -1 means that the object is "
		"disabled and will not be called during simulation execution "
		"The actual timestep (dt) belonging to a clock tick is defined "
		"by the Clock object.",
			&Neutral::setTick,
			&Neutral::getTick );

	static ReadOnlyElementValueFinfo< Neutral, double > 
			dt(
			"dt",
			"Timestep used for this Element. Zero if not scheduled.",
			&Neutral::getDt );


	static ReadOnlyElementValueFinfo< Neutral, vector< string > > 
			valueFields(
			"valueFields",
			"List of all value fields on Element."
			"These fields are accessed through the assignment operations "
			"in the Python interface."
			"These fields may also be accessed as functions through the "
			"set<FieldName> and get<FieldName> commands.",
			&Neutral::getValueFields
	);

	static ReadOnlyElementValueFinfo< Neutral, vector< string > > 
			sourceFields(
			"sourceFields",
			"List of all source fields on Element, that is fields that "
			"can act as message sources. ",
			&Neutral::getSourceFields
	);

	static ReadOnlyElementValueFinfo< Neutral, vector< string > > 
			destFields(
			"destFields",
			"List of all destination fields on Element, that is, fields"
			"that are accessed as Element functions.",
			&Neutral::getDestFields
	);

	static ReadOnlyElementValueFinfo< Neutral, vector< ObjId > > msgOut( 
		"msgOut",
		"Messages going out from this Element", 
			&Neutral::getOutgoingMsgs );

	static ReadOnlyElementValueFinfo< Neutral, vector< ObjId > > msgIn( 
		"msgIn",
		"Messages coming in to this Element", 
			&Neutral::getIncomingMsgs );

	static ReadOnlyLookupElementValueFinfo< Neutral, string, vector< Id > > neighbors( 
		"neighbors",
		"Ids of Elements connected this Element on specified field.", 
			&Neutral::getNeighbors );

	static ReadOnlyLookupElementValueFinfo< Neutral, string, vector< ObjId > > msgDests( 
		"msgDests",
		"ObjIds receiving messages from the specified SrcFinfo", 
			&Neutral::getMsgDests );

	static ReadOnlyLookupElementValueFinfo< Neutral, string, vector< string > > msgDestFunctions( 
		"msgDestFunctions",
		"Matching function names for each ObjId receiving a msg from "
		"the specified SrcFinfo", 
			&Neutral::getMsgDestFunctions );

	static ReadOnlyLookupElementValueFinfo< Neutral, string, bool >isA( 
		"isA",
		"Returns true if the current object is derived from the specified "
		"the specified class", 
			&Neutral::isA );

	/////////////////////////////////////////////////////////////////
	// Value Finfos
	/////////////////////////////////////////////////////////////////
	static ValueFinfo< Neutral, Neutral > thisFinfo (
		"this",
		"Access function for entire object",
		&Neutral::setThis,
		&Neutral::getThis
	);
	/////////////////////////////////////////////////////////////////
	// SrcFinfos
	/////////////////////////////////////////////////////////////////
	static SrcFinfo1< int > childOut( "childOut", 
		"Message to child Elements");

	/////////////////////////////////////////////////////////////////
	// DestFinfos
	/////////////////////////////////////////////////////////////////
	static DestFinfo parentMsg( "parentMsg", 
		"Message from Parent Element(s)", 
		new EpFunc1< Neutral, int >( &Neutral::destroy ) );

	static DestFinfo blockNodeBalance( "blockNodeBalance",
		"Request conversion of data into a blockDataHandler subclass,"
		"and to carry out node balancing of data."
		"The amount of data is not altered, and if the data is already a"
		"blockDataHandler subclass it retains its original dimensions"
		"If converting from a generalDataHandler to blockHandler it treats"
		"it as a linear array"
		"The function preserves the old data and shuffles info around"
		"between nodes to complete the balancing."
		"Arguments are: myNode, startNode, endNode",
		new EpFunc3< Neutral, unsigned int, unsigned int, unsigned int >(
			&Neutral::blockNodeBalance ) );

	static DestFinfo generalNodeBalance( "generalNodeBalance",
		"Request conversion of data into a generalDataHandler subclass,"
		"and to carry out node balancing of data as per specified args."
		"The amount of data is not altered. The generalDataHandler loses"
		"any dimensional information that may have been present in an "
		"earlier blockDataHandler incarnation."
		"Arguments are: myNode, nodeAssignment",
		new EpFunc2< Neutral, unsigned int, vector< unsigned int > >(
			&Neutral::generalNodeBalance ) );
			
	/////////////////////////////////////////////////////////////////
	// Setting up the Finfo list.
	/////////////////////////////////////////////////////////////////
	static Finfo* neutralFinfos[] = {
		&childOut,				// SrcFinfo
		&parentMsg,				// DestFinfo
		&thisFinfo,				// Value
		&name,					// Value
		&me,					// ReadOnlyValue
		&parent,				// ReadOnlyValue
		&children,				// ReadOnlyValue
		&path,					// ReadOnlyValue
		&className,				// ReadOnlyValue
		&numData,				// Value
		&numField,				// Value
		&id,					// ReadOnlyValue
		&index,					// ReadOnlyValue
		&fieldIndex,			// ReadOnlyValue
		&tick,					// Value
		&dt,					// ReadOnlyValue
		&valueFields,			// ReadOnlyValue
		&sourceFields,			// ReadOnlyValue
		&destFields,			// ReadOnlyValue
		&msgOut,				// ReadOnlyValue
		&msgIn,					// ReadOnlyValue
		&neighbors,			// ReadOnlyLookupValue
		&msgDests,				// ReadOnlyLookupValue
		&msgDestFunctions,		// ReadOnlyLookupValue
		&isA,					// ReadOnlyLookupValue
	};

	/////////////////////////////////////////////////////////////////
	// Setting up the Cinfo.
	/////////////////////////////////////////////////////////////////

	static string doc[] =
	{
		"Name", "Neutral",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "Neutral: Base class for all MOOSE classes. Provides"
		"access functions for housekeeping fields and operations, message"
		"traversal, and so on."
	};
	static Dinfo< Neutral > dinfo;
	static Cinfo neutralCinfo (
		"Neutral",
		0, // No base class.
		neutralFinfos,
		sizeof( neutralFinfos ) / sizeof( Finfo* ),
		&dinfo,
                doc,
                sizeof(doc)/sizeof(string)
	);

	return &neutralCinfo;
}

static const Cinfo* neutralCinfo = Neutral::initCinfo();


Neutral::Neutral()
	// : name_( "" )
{
	;
}

////////////////////////////////////////////////////////////////////////
// Access functions
////////////////////////////////////////////////////////////////////////


void Neutral::setThis( Neutral v )
{
	;
}

Neutral Neutral::getThis() const
{
	return *this;
}

void Neutral::setName( const Eref& e, string name )
{
	if ( e.id().value() <= 3 ) {
		cout << "Warning: Neutral::setName on '" << e.id().path() << 
			   "'. Cannot rename core objects\n";
		return;
	}
	if ( !Shell::isNameValid( name ) ) {
		cout << "Warning: Neutral::setName on '" << e.id().path() << 
			   "'. Illegal character in name.\n";
		return;
	}
	ObjId pa = parent( e );
	Id sibling = Neutral::child( pa.eref(), name );
	if ( sibling == Id() ) { // OK, no existing object with same name.
		e.element()->setName( name );
	} else {
		cout << "Warning: Neutral::setName: an object with the name '" <<
			name << "'\n already exists on the same parent. Not changed\n";
	}
}

string Neutral::getName( const Eref& e ) const
{
	return e.element()->getName();
}

ObjId Neutral::getObjId( const Eref& e ) const
{
	return e.objId();
}

ObjId Neutral::getParent( const Eref& e ) const
{
	return parent( e );
}

/**
 * Gets Element children, not individual entries in the array.
 */
vector< Id > Neutral::getChildren( const Eref& e ) const
{
	vector< Id > ret;
	children( e, ret );
	return ret;
}

// Static function
void Neutral::children( const Eref& e, vector< Id >& ret )
{
		/*
	vector< Id > temp;
	getNeighbors( ret, neutralCinfo->findFinfo( "childOut" ) );
	for ( vector< Id >::iterator i = temp.begin(); i != temp.end(); ++i ) {
		if ( ret
	}
	*/



	static const Finfo* pf = neutralCinfo->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();
	static const Finfo* cf = neutralCinfo->findFinfo( "childOut" );
	static const SrcFinfo* cf2 = dynamic_cast< const SrcFinfo* >( cf );
	static const BindIndex bi = cf2->getBindIndex();
	
	const vector< MsgFuncBinding >* bvec = e.element()->getMsgAndFunc( bi );

	for ( vector< MsgFuncBinding >::const_iterator i = bvec->begin();
		i != bvec->end(); ++i ) {
		if ( i->fid == pafid ) {
			const Msg* m = Msg::getMsg( i->mid );
			assert( m );
			vector< vector< Eref > > kids;
			m->targets( kids );
			if ( e.dataIndex() == ALLDATA ) {
				for ( vector< vector< Eref > >::iterator
					i = kids.begin(); i != kids.end(); ++i ) {
					for ( vector< Eref >::iterator
						j = i->begin(); j != i->end(); ++j )
						ret.push_back( j->id() );
				}
			} else {
				const vector< Eref >& temp = kids[e.dataIndex()];
				for ( vector< Eref >::const_iterator
							i = temp.begin(); i != temp.end(); ++i )
					ret.push_back( i->id() );
			}
		}
	}
}


string Neutral::getPath( const Eref& e ) const
{
	return path( e );
}

string Neutral::getClass( const Eref& e ) const
{
	return e.element()->cinfo()->name();
}

unsigned int Neutral::getNumData( const Eref& e ) const
{
	return e.element()->numData();
}

void Neutral::setNumData( const Eref& e, unsigned int num )
{
	e.element()->resize( num );
}

unsigned int Neutral::getNumField( const Eref& e ) const
{
	assert( e.isDataHere() );
	unsigned int rawIndex = e.element()->rawIndex( e.dataIndex() );
	return e.element()->numField( rawIndex );
}

void Neutral::setNumField( const Eref& e, unsigned int num )
{
	assert( e.isDataHere() );
	unsigned int rawIndex = e.element()->rawIndex( e.dataIndex() );
	e.element()->resizeField( rawIndex, num );
}

unsigned int Neutral::getId( const Eref& e ) const
{
	return e.id().value();
}

unsigned int Neutral::getIndex( const Eref& e ) const
{
	return e.dataIndex();
}

unsigned int Neutral::getFieldIndex( const Eref& e ) const
{
	return e.fieldIndex();
}

int Neutral::getTick( const Eref& e ) const
{
	return e.element()->getTick();
}

void Neutral::setTick( const Eref& e, int num )
{
	e.element()->setTick( num );
}

double Neutral::getDt( const Eref& e ) const
{
	int tick = e.element()->getTick();
	if ( tick < 0 ) 
		return 0.0;
	Id clockId( 1 );
	return LookupField< unsigned int, double >::get(
		clockId, "tickDt", tick );
}

vector< string > Neutral::getValueFields( const Eref& e ) const
{
	unsigned int num = e.element()->cinfo()->getNumValueFinfo();
	vector< string > ret( num );
	for ( unsigned int i = 0; i < num; ++i ) {
		const Finfo *f = e.element()->cinfo()->getValueFinfo( i );
		assert( f );
		ret[i] = f->name();
	}
	return ret;
}

vector< string > Neutral::getSourceFields( const Eref& e ) const
{
	unsigned int num = e.element()->cinfo()->getNumSrcFinfo();
	vector< string > ret( num );
	for ( unsigned int i = 0; i < num; ++i ) {
		const Finfo *f = e.element()->cinfo()->getSrcFinfo( i );
		assert( f );
		ret[i] = f->name();
	}
	return ret;
}

vector< string > Neutral::getDestFields( const Eref& e ) const
{
	unsigned int num = e.element()->cinfo()->getNumDestFinfo();
	vector< string > ret( num );
	for ( unsigned int i = 0; i < num; ++i ) {
		const Finfo *f = e.element()->cinfo()->getDestFinfo( i );
		assert( f );
		ret[i] = f->name();
	}
	return ret;
}

vector< ObjId > Neutral::getOutgoingMsgs( const Eref& e ) const
{
	vector< ObjId > ret;
	unsigned int numBindIndex = e.element()->cinfo()->numBindIndex();

	for ( unsigned int i = 0; i < numBindIndex; ++i ) {
		const vector< MsgFuncBinding >* v = 
			e.element()->getMsgAndFunc( i );
		if ( v ) {
			for ( vector< MsgFuncBinding >::const_iterator mb = v->begin();
				mb != v->end(); ++mb ) {
				ret.push_back( mb->mid );
			}
		}
	}
	return ret;
}

vector< ObjId > Neutral::getIncomingMsgs( const Eref& e ) const
{
	vector< ObjId > ret;
	const vector< ObjId >& msgIn = e.element()->msgIn();

	for (unsigned int i = 0; i < msgIn.size(); ++i ) {
		const Msg* m = Msg::getMsg( msgIn[i] );
			assert( m );
			if ( m->e2() == e.element() )
				ret.push_back( m->mid() );
	}
	return ret;
}

vector< Id > Neutral::getNeighbors( const Eref& e, string field ) const
{
	vector< Id > ret;
	const Finfo* finfo = e.element()->cinfo()->findFinfo( field );
	if ( finfo )
		e.element()->getNeighbors( ret, finfo );
	else
		cout << "Warning: Neutral::getNeighbors: Id.Field '" << 
				e.id().path() << "." << field <<
				"' not found\n";
	return ret;
}

vector< ObjId > Neutral::getMsgDests( const Eref& e, string field ) const
{
	const Finfo* finfo = e.element()->cinfo()->findFinfo( field );
	const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( finfo );
	if ( sf ) {
		vector< ObjId > tgt;
		vector< string > func;
		e.element()->getMsgTargetAndFunctions( e.dataIndex(), sf,
			tgt, func );
		return tgt;
	} else {
		cout << "Warning: Neutral::getMsgDests: Id.Field '" << 
				e.id().path() << "." << field <<
				"' not found or not a SrcFinfo\n";
	}
	static vector< ObjId > ret( 0 );
	return ret;
}

vector< string > Neutral::getMsgDestFunctions( const Eref& e, string field ) const
{
	vector< string > ret( 0 );
	const Finfo* finfo = e.element()->cinfo()->findFinfo( field );
	const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( finfo );
	if ( sf ) {
		vector< ObjId > tgt;
		vector< string > func;
		e.element()->getMsgTargetAndFunctions( e.dataIndex(), sf,
			tgt, func );
		return func;
	} else {
		cout << "Warning: Neutral::getMsgDestFunctions: Id.Field '" << 
				e.id().path() << "." << field <<
				"' not found or not a SrcFinfo\n";
	}
	return ret;
}

bool Neutral::isA( const Eref& e, string className ) const
{
	return e.element()->cinfo()->isA( className );
}

//////////////////////////////////////////////////////////////////////////

unsigned int Neutral::buildTree( const Eref& e, vector< Id >& tree )
	const 
{
	unsigned int ret = 1;
	Eref er( e.element(), ALLDATA );
	vector< Id > kids = getChildren( er );
	sort( kids.begin(), kids.end() );
	kids.erase( unique( kids.begin(), kids.end() ), kids.end() );
	for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i )
		ret += buildTree( i->eref(), tree );
	tree.push_back( e.element()->id() );
	return ret;
}
//////////////////////////////////////////////////////////////////////////
// Dest Functions
//////////////////////////////////////////////////////////////////////////

//
// Stage 0: Check if it is a Msg. This is deleted by Msg::deleteMsg( ObjId )
// Stage 1: mark for deletion. This is done by setting cinfo = 0
// Stage 2: Clear out outside-going msgs
// Stage 3: delete self and attached msgs, 
void Neutral::destroy( const Eref& e, int stage )
{
	if ( e.element()->cinfo()->isA( "Msg" ) ) {
		Msg::deleteMsg( e.objId() );
		return;
	}
	vector< Id > tree;
	Eref er( e.element(), ALLDATA );
	unsigned int numDescendants = buildTree( er, tree );
	/*
	cout << "Neutral::destroy: id = " << e.id() << 
		", name = " << e.element()->getName() <<
		", numDescendants = " << numDescendants << endl;
		*/
	assert( numDescendants == tree.size() );
	Element::destroyElementTree( tree );
}

/**
 * Request conversion of data into a blockDataHandler subclass,
 * and to carry out node balancing of data as per args.
 */
void Neutral::blockNodeBalance( const Eref& e, 
	unsigned int, unsigned int, unsigned int )
{
}

/**
 * Request conversion of data into a generalDataHandler subclass,
 * and to carry out node balancing of data as per args.
 */
void Neutral::generalNodeBalance( const Eref& e, 
	unsigned int myNode, vector< unsigned int > nodeAssignment )
{
}


/////////////////////////////////////////////////////////////////////////
// Static utility functions.
/////////////////////////////////////////////////////////////////////////

// static function
bool Neutral::isDescendant( Id me, Id ancestor )
{
	static const Finfo* pf = neutralCinfo->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();

	Eref e = me.eref();
	
	while ( e.element()->id() != Id() && e.element()->id() != ancestor ) {
		ObjId mid = e.element()->findCaller( pafid );
		assert( mid != ObjId() );
		ObjId fid = Msg::getMsg( mid )->findOtherEnd( e.objId() );
		e = fid.eref();
	}
	return ( e.element()->id() == ancestor );
}

// static function
Id Neutral::child( const Eref& e, const string& name ) 
{
	static const Finfo* pf = neutralCinfo->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();
	static const Finfo* cf = neutralCinfo->findFinfo( "childOut" );
	static const SrcFinfo* cf2 = dynamic_cast< const SrcFinfo* >( cf );
	static const BindIndex bi = cf2->getBindIndex();
	
	const vector< MsgFuncBinding >* bvec = e.element()->getMsgAndFunc( bi );

	vector< Id > ret;

	for ( vector< MsgFuncBinding >::const_iterator i = bvec->begin();
		i != bvec->end(); ++i ) {
		if ( i->fid == pafid ) {
			const Msg* m = Msg::getMsg( i->mid );
			assert( m );
			Element* e2 = m->e2();
			if ( e2->getName() == name ) {
				if ( e.dataIndex() == ALLDATA ) {// Child of any index is OK
					return e2->id();
				} else {
					ObjId parent = m->findOtherEnd( m->getE2() );
					// If child is a fieldElement, then all parent indices
					// are permitted. Otherwise insist parent dataIndex OK.
					if ( e2->hasFields() || parent == e.objId() )
						return e2->id();
				}
			}
		}
	}
	return Id();
}

// Static function.
ObjId Neutral::parent( const Eref& e )
{
	return Neutral::parent( e.objId() );
}

ObjId Neutral::parent( ObjId oid )
{
	static const Finfo* pf = neutralCinfo->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();

	if ( oid.id == Id() ) {
		cout << "Warning: Neutral::parent: tried to take parent of root\n";
		return Id();
	}

	ObjId mid = oid.element()->findCaller( pafid );
	assert( mid != ObjId() );

	ObjId pa = Msg::getMsg( mid )->findOtherEnd( oid );
	return pa;
}

// Static function 
string Neutral::path( const Eref& e )
{
	static const Finfo* pf = neutralCinfo->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();

	vector< ObjId > pathVec;
	ObjId curr = e.objId();
	stringstream ss;

	pathVec.push_back( curr );
	while ( curr.id != Id() ) {
		ObjId mid = curr.eref().element()->findCaller( pafid );
		if ( mid == ObjId() ) {
			cout << "Error: Neutral::path:Cannot follow msg of ObjId: " <<
				   	e.objId() << " for func: " << pafid << endl;
			break;
		}
		curr = Msg::getMsg( mid )->findOtherEnd( curr );
		pathVec.push_back( curr );
	}
	if ( pathVec.size() <= 1 )
		return "/";
	for ( unsigned int i = 1; i < pathVec.size(); ++i ) {
		ss << "/";
		ObjId& oid = pathVec[ pathVec.size() - i - 1 ];
		ss << oid.element()->getName();
		if ( !oid.element()->hasFields() )
			ss << "[" << oid.dataIndex << "]";
		/*
		if ( !oid.element()->hasFields() )
			ss << "[" << oid.dataIndex << "]";
		if ( oid.element()->numData() > 1 )
			ss << "[" << oid.dataIndex << "]";
			*/
	}
	// Append braces if Eref was for a fieldElement. This should
	// work even if it is off-node.
	if ( e.element()->hasFields() ) {
		ss << "[" << e.fieldIndex() << "]";
	}

	return ss.str();
}

// Neutral does not have any fields.
istream& operator >>( istream& s, Neutral& d )
{
	return s;
}

ostream& operator <<( ostream& s, const Neutral& d )
{
	return s;
}

bool Neutral::isGlobalField( const string& field )
{
	if ( field.length() < 8 )
		return 0;
	if ( field.substr( 0, 4 ) == "set_" ) {
		if ( field == "set_name" )
			return 1;
		if ( field == "set_group" )
			return 1;
		if ( field == "set_lastDimension" ) // This is the critical one!
			return 1;
	}
	return 0;
}

