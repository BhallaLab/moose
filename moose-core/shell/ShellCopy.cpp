/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "OneToAllMsg.h"
#include "Shell.h"
#include "../scheduling/Clock.h"

/// Returns the Id of the root of the copied tree upon success.
Id Shell::doCopy( Id orig, ObjId newParent, string newName,
	unsigned int n, bool toGlobal, bool copyExtMsg )
{
	if ( newName.length() > 0 && !isNameValid( newName ) ) {
		cout << "Error: Shell::doCopy: Illegal name for copy.\n";
		return Id();
	}

	if ( Neutral::isDescendant( newParent, orig ) ) {
		cout << "Error: Shell::doCopy: Cannot copy object to descendant in tree\n";
		return Id();
	}
	if ( n < 1 ) {
		cout << "Warning: Shell::doCopy( " << orig.path() << " to " <<
			newParent.path() << " ) : numCopies must be > 0, using 1 \n";
		return Id();
	}
	if ( Neutral::child( newParent.eref(), newName ) != Id() ) {
		cout << "Error: Shell::doCopy: Cannot copy object '" << newName <<
			   "' onto '" << newParent.path() <<
			   "' since object with same name already present.\n";
		return Id();
	}

	Eref sheller( shelle_, 0 );
	Id newElm = Id::nextId();
	vector< ObjId > args;
	args.push_back( orig );
	args.push_back( newParent );
	args.push_back( newElm );
	SetGet5< vector < ObjId >, string, unsigned int, bool, bool >::set(
			ObjId(), "copy",
			args, newName, n, toGlobal, copyExtMsg );
	/*
	if ( innerCopy( args, newName, n, toGlobal, copyExtMsg ) )
		return newElm;
	else
		return Id();
	*/
	return newElm;
}

/** Runs in parallel on all nodes.
 * Note that 'n' is the number of complete duplicates. If there were
 * 10 dataEntries in the original, there will now be 10 x n.
 */
Element* innerCopyElements( Id orig, ObjId newParent, Id newId,
	unsigned int n, bool toGlobal, map< Id, Id >& tree )
{
	// Element* e = new Element( newId, orig.element(), n, toGlobal );
	unsigned int newNumData = orig.element()->numData() * n;
	Element* e = orig.element()->copyElement(
				newParent, newId, newNumData, toGlobal );
	assert( e );
	Shell::adopt( newParent, newId, 0 );
	e->setTick( Clock::lookupDefaultTick( e->cinfo()->name() ) );

	// cout << Shell::myNode() << ": Copy: orig= " << orig << ", newParent = " << newParent << ", newId = " << newId << endl;
	tree[ orig ] = e->id();

	// cout << Shell::myNode() << ": ice, pa = " << newParent << ", pair= (" << orig << "," << e->id() << ")\n";
	vector< Id > kids;
	Neutral::children( orig.eref(), kids );

	for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i ) {
		// Needed in case parent is not on zero dataIndex.
		ObjId pa = Neutral::parent( *i );
		ObjId newParent( e->id(), pa.dataIndex );
		innerCopyElements( *i, newParent, Id::nextId(), n, toGlobal, tree );
	}
	return e;
}

void innerCopyMsgs( map< Id, Id >& tree, unsigned int n, bool copyExtMsgs )
{
	static const Finfo* cf = Neutral::initCinfo()->findFinfo( "childOut" );
	static const SrcFinfo1< int >* cf2 =
		dynamic_cast< const SrcFinfo1< int >* >( cf );
	assert( cf );
	assert( cf2 );

	/*
	cout << endl << Shell::myNode() << ": innerCopyMsg ";
	for ( map< Id, Id >::const_iterator i = tree.begin();
		i != tree.end(); ++i ) {
		cout << " (" << i->first << "," << i->second << ") ";
	}
	cout << endl;
	*/
	for ( map< Id, Id >::const_iterator i = tree.begin();
		i != tree.end(); ++i ) {
		Element *e = i->first.element();
		unsigned int j = 0;
		const vector< MsgFuncBinding >* b = e->getMsgAndFunc( j );
		while ( b ) {
			if ( j != cf2->getBindIndex() ) {
				for ( vector< MsgFuncBinding >::const_iterator k =
					b->begin();
					k != b->end(); ++k ) {
					ObjId mid = k->mid;
					const Msg* m = Msg::getMsg( mid );
					assert( m );
	/*
	cout << endl << Shell::myNode() << ": innerCopyMsg orig = (" <<
		e->id() << ", " << e->getName() << "), e1 = (" <<
		m->e1()->id() << ", " << m->e1()->getName() << "), e2 = (" <<
		m->e2()->id() << ", " << m->e2()->getName() << "), fid = " <<
		k->fid << ", mid = " << k->mid << endl;
		*/
					map< Id, Id >::const_iterator tgt;
					if ( m->e1() == e ) {
						tgt = tree.find( m->e2()->id() );
					} else if ( m->e2() == e ) {
						tgt = tree.find( m->e1()->id() );
					} else {
						assert( 0 );
					}
					if ( tgt != tree.end() )
						m->copy( e->id(), i->second, tgt->second,
							k->fid, j, n );
				}
			}
			b = e->getMsgAndFunc( ++j );
		}
	}
}


bool Shell::innerCopy( const vector< ObjId >& args, const string& newName,
	unsigned int n, bool toGlobal, bool copyExtMsgs )
{
	map< Id, Id > tree;
	// args are orig, newParent, newElm.
	assert( args.size() == 3 );
	Element* e = innerCopyElements( args[0], args[1], args[2],
					n, toGlobal, tree );
	if ( !e ) {
		return 0;
	}
	if ( newName != "" )
		e->setName( newName );
	//innerCopyData( orig, newParent );
	innerCopyMsgs( tree, n, copyExtMsgs );
	return 1;
}

void Shell::handleCopy( const Eref& er, vector< ObjId > args,
	string newName, unsigned int n, bool toGlobal, bool copyExtMsgs )
{
	if ( !innerCopy( args, newName, n, toGlobal, copyExtMsgs ) ) {
		cout << "Error on Shell::myNode()::Shell::handleCopy for " <<
				newName << ", " << n << endl;
	}
	/*
	static const Finfo* ackf =
		Shell::initCinfo()->findFinfo( "ack" );
	static const SrcFinfo2< unsigned int, unsigned int >*
		ack = dynamic_cast< const SrcFinfo2< unsigned int, unsigned int >* >( ackf );
	assert( ackf );
	assert( ack );

	if ( innerCopy( args, newName, n, toGlobal, copyExtMsgs ) )
		ack->send( Eref( shelle_, 0 ), Shell::myNode(), ErrorStatus );
	else
		ack->send( Eref( shelle_, 0 ), Shell::myNode(), OkStatus );
	*/
}
