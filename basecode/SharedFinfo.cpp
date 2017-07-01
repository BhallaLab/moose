/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"

/**
 * This set of classes define Message Sources. Their main job is to supply
 * a type-safe send operation, and to provide typechecking for it.
 */

SharedFinfo::SharedFinfo( const string& name, const string& doc,
	Finfo** entries, unsigned int numEntries )
	: Finfo( name, doc )
{
	for ( unsigned int i = 0; i < numEntries; ++i )
	{
		Finfo* foo = entries[i];
		SrcFinfo* s = dynamic_cast< SrcFinfo* >( foo );
		// SrcFinfo* s = dynamic_cast< SrcFinfo* >( entries[i] );
		if ( s != 0 )
			src_.push_back( s );
		else
			dest_.push_back( entries[i] );
	}
}

void SharedFinfo::registerFinfo( Cinfo* c )
{
	for( vector< SrcFinfo* >::iterator i =
		src_.begin(); i != src_.end(); ++i)
		c->registerFinfo( *i );
	for( vector< Finfo* >::iterator i =
		dest_.begin(); i != dest_.end(); ++i)
		c->registerFinfo( *i );
}

bool SharedFinfo::strSet(
	const Eref& tgt, const string& field, const string& arg ) const
{
	return 0;
}

bool SharedFinfo::strGet(
	const Eref& tgt, const string& field, string& returnValue ) const
{
	return 0;
}

/**
 * It is possible that we have DestFinfos in this SharedFinfo, that have
 * not been registered. So we need to scan through.
 * Note that the register operation overwrites values if they already
 * exist. Best not to have conflicts!.
 */
/*
void SharedFinfo::registerOpFuncs(
		map< string, FuncId >& fnames, vector< OpFunc* >& funcs )
{
	for ( unsigned int i = 0; i < dest_.size(); ++i )
		dest_[i]->registerOpFuncs( fnames, funcs );
}

BindIndex SharedFinfo::registerBindIndex( BindIndex current )
{
	return current;
}
*/

bool SharedFinfo::checkTarget( const Finfo* target ) const
{
	const SharedFinfo* tgt = dynamic_cast< const SharedFinfo* >( target );
	if ( tgt ) {
		if ( src_.size() != tgt->dest_.size() &&
			dest_.size() != tgt->src_.size() )
			return 0;
		for ( unsigned int i = 0; i < src_.size(); ++i ) {
			if ( !src_[i]->checkTarget( tgt->dest_[i] ) )
				return 0;
		}
		for ( unsigned int i = 0; i < tgt->src_.size(); ++i ) {
			if ( !tgt->src_[i]->checkTarget( dest_[i] ) )
				return 0;
		}

		return 1;
	}
	return 0;
}

bool SharedFinfo::addMsg( const Finfo* target, ObjId mid,
	Element* srcElm ) const
{
	if ( !checkTarget( target ) )
		return 0;
	const SharedFinfo* tgt = dynamic_cast< const SharedFinfo* >( target );

	// We have a problem if the src and dest elms (e1 and e2) are the
	// same, because this messes up the logic to assign the isForward flag.
	// This is an issue only if the target Finfo wants to send data back.
	// In other words, there are dest_ finfos on this SharedFinfo.
	// Report this problem.
	const Msg* m = Msg::getMsg( mid );
	assert( m->e1() == srcElm );
	Element* destElm = m->e2();
	if ( srcElm == destElm && srcElm->id() != Id() ) {
		if ( dest_.size() > 0 ) {
			cout << "Error: SharedFinfo::addMsg: MessageId " << mid <<
			endl <<
			"Source Element == DestElement == " << srcElm->getName() <<
			endl << "Recommend that you individually set up messages for" <<
			" the components of the SharedFinfo, to ensure that the " <<
			"direction of messaging is consistent.\n";
			return 0;
		}
	}


	for ( unsigned int i = 0; i < src_.size(); ++i ) {
		if ( !src_[i]->addMsg( tgt->dest_[i], mid, srcElm ) ) {
			// Should never happen. The checkTarget should preclude this.
			cerr << "Error:SharedFinfo::addMsg: Failed on MessageId " <<
				mid << ", unrecoverable\n";
			exit(0);
		}
	}


	for ( unsigned int i = 0; i < tgt->src_.size(); ++i ) {
		if ( !tgt->src_[i]->addMsg( dest_[i], mid, destElm ) ) {
			// Should never happen. The checkTarget should preclude this.
			cerr << "Error:SharedFinfo::addMsg: Failed on MessageId " <<
				mid << ", unrecoverable\n";
			exit( 0 );
		}
	}
	return 1;
}

const vector< SrcFinfo* >& SharedFinfo::src() const
{
	return src_;
}


const vector< Finfo* >& SharedFinfo::dest() const
{
	return dest_;
}

/////////////////////////////////////////////////////////////////////
// overridden default virtual funcs for internal set/get names
/////////////////////////////////////////////////////////////////////
vector< string > SharedFinfo::innerSrc() const
{
	vector< string > ret;
	for ( vector< SrcFinfo* >::const_iterator i = src_.begin();
		i != src_.end(); ++i )
		ret.push_back( (*i)->name() );
	return ret;
}

vector< string > SharedFinfo::innerDest() const
{
	vector< string > ret;
	for ( vector< Finfo* >::const_iterator i = dest_.begin();
		i != dest_.end(); ++i )
		ret.push_back( (*i)->name() );
	return ret;
}

string SharedFinfo::rttiType() const
{
	return "void";
}
