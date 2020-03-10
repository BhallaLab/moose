/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToOneDataIndexMsg.h"
#include "OneToAllMsg.h"
#include "../basecode/SparseMatrix.h"
#include "SparseMsg.h"
#include "../shell/Shell.h" // For the myNode() and numNodes() definitions
#include "../basecode/MsgElement.h"

#include "../shell/Shell.h"

///////////////////////////////////////////////////////////////////////////

// Static field declaration.
Id Msg::msgManagerId_;
bool Msg::lastTrump_ = false;
const Msg* Msg::lastMsg_ = 0;

Msg::Msg( ObjId mid, Element* e1, Element* e2 )
    : mid_( mid), e1_( e1 ), e2_( e2 )
{
    e1->addMsg( mid_ );
    e2->addMsg( mid_ );
    lastMsg_ = this;
}

Msg::~Msg()
{
    if ( !lastTrump_ )
    {
        e1_->dropMsg( mid_ );
        e2_->dropMsg( mid_ );
    }

    /*
    if ( mid_ > 1 )
    	garbageMsg_.push_back( mid_ );
    	*/
}

// Static func
void Msg::deleteMsg( ObjId mid )
{
    const Msg* msg = getMsg( mid );
    delete( msg );
}

// Static func
const Msg* Msg::getMsg( ObjId m )
{
    return reinterpret_cast< const Msg* >( m.data() );
}

/**
 * Return the first element id
 */
Id Msg::getE1() const
{
    return e1_->id();
}

/**
 * Return the second element id
 */
Id Msg::getE2() const
{
    return e2_->id();
}

vector< string > Msg::getSrcFieldsOnE1() const
{
    vector< pair< BindIndex, FuncId > > ids;
    vector< string > ret;

    e1_->getFieldsOfOutgoingMsg( mid_, ids );

    for ( unsigned int i = 0; i < ids.size(); ++i )
    {
        string name = e1_->cinfo()->srcFinfoName( ids[i].first );
        if ( name == "" )
        {
            cout << "Error: Msg::getSrcFieldsOnE1: Failed to find field on msg " <<
                 e1_->getName() << "-->" << e2_->getName() << endl;
        }
        else
        {
            ret.push_back( name );
        }
    }
    return ret;
}

vector< string > Msg::getDestFieldsOnE2() const
{
    vector< pair< BindIndex, FuncId > > ids;
    vector< string > ret;

    e1_->getFieldsOfOutgoingMsg( mid_, ids );

    for ( unsigned int i = 0; i < ids.size(); ++i )
    {
        string name = e2_->cinfo()->destFinfoName( ids[i].second );
        if ( name == "" )
        {
            cout << "Error: Msg::getDestFieldsOnE2: Failed to find field on msg " <<
                 e1_->getName() << "-->" << e2_->getName() << endl;
        }
        else
        {
            ret.push_back( name );
        }
    }
    return ret;
}

vector< string > Msg::getSrcFieldsOnE2() const
{
    vector< pair< BindIndex, FuncId > > ids;
    vector< string > ret;

    e2_->getFieldsOfOutgoingMsg( mid_, ids );

    for ( unsigned int i = 0; i < ids.size(); ++i )
    {
        string name = e2_->cinfo()->srcFinfoName( ids[i].first );
        if ( name == "" )
        {
            cout << "Error: Msg::getSrcFieldsOnE2: Failed to find field on msg " <<
                 e1_->getName() << "-->" << e2_->getName() << endl;
        }
        else
        {
            ret.push_back( name );
        }
    }
    return ret;
}

vector< string > Msg::getDestFieldsOnE1() const
{
    vector< pair< BindIndex, FuncId > > ids;
    vector< string > ret;

    e2_->getFieldsOfOutgoingMsg( mid_, ids );

    for ( unsigned int i = 0; i < ids.size(); ++i )
    {
        string name = e1_->cinfo()->destFinfoName( ids[i].second );
        if ( name == "" )
        {
            cout << "Error: Msg::getDestFieldsOnE1: Failed to find field on msg " <<
                 e1_->getName() << "-->" << e2_->getName() << endl;
        }
        else
        {
            ret.push_back( name );
        }
    }
    return ret;
}

ObjId Msg::getAdjacent(ObjId obj) const
{
    return findOtherEnd(obj);
}

///////////////////////////////////////////////////////////////////////////
// Here we set up the Element related stuff for Msgs.
///////////////////////////////////////////////////////////////////////////

const Cinfo* Msg::initCinfo()
{
    ///////////////////////////////////////////////////////////////////
    // Field definitions.
    ///////////////////////////////////////////////////////////////////
    static ReadOnlyValueFinfo< Msg, Id > e1(
        "e1",
        "Id of source Element.",
        &Msg::getE1
    );
    static ReadOnlyValueFinfo< Msg, Id > e2(
        "e2",
        "Id of source Element.",
        &Msg::getE2
    );

    static ReadOnlyValueFinfo< Msg, vector< string > > srcFieldsOnE1(
        "srcFieldsOnE1",
        "Names of SrcFinfos for messages going from e1 to e2. There are"
        "matching entries in the destFieldsOnE2 vector",
        &Msg::getSrcFieldsOnE1
    );
    static ReadOnlyValueFinfo< Msg, vector< string > > destFieldsOnE2(
        "destFieldsOnE2",
        "Names of DestFinfos for messages going from e1 to e2. There are"
        "matching entries in the srcFieldsOnE1 vector",
        &Msg::getDestFieldsOnE2
    );
    static ReadOnlyValueFinfo< Msg, vector< string > > srcFieldsOnE2(
        "srcFieldsOnE2",
        "Names of SrcFinfos for messages going from e2 to e1. There are"
        "matching entries in the destFieldsOnE1 vector",
        &Msg::getSrcFieldsOnE2
    );
    static ReadOnlyValueFinfo< Msg, vector< string > > destFieldsOnE1(
        "destFieldsOnE1",
        "Names of destFinfos for messages going from e2 to e1. There are"
        "matching entries in the srcFieldsOnE2 vector",
        &Msg::getDestFieldsOnE1
    );

    static ReadOnlyLookupValueFinfo< Msg, ObjId, ObjId > adjacent(
        "adjacent",
        "The element adjacent to the specified element",
        &Msg::getAdjacent);

    static Finfo* msgFinfos[] =
    {
        &e1,		// readonly value
        &e2,		// readonly value
        &srcFieldsOnE1,	// readonly value
        &destFieldsOnE2,	// readonly value
        &srcFieldsOnE2,	// readonly value
        &destFieldsOnE1,	// readonly value
        &adjacent, // readonly lookup value
    };

    static Cinfo msgCinfo (
        "Msg",	// name
        Neutral::initCinfo(),				// base class
        msgFinfos,
        sizeof( msgFinfos ) / sizeof( Finfo* ),	// num Fields
        0
        // new Dinfo< Msg >()
    );

    return &msgCinfo;
}

static const Cinfo* msgCinfo = Msg::initCinfo();

// Static func. Returns the index to use for msgIndex.
unsigned int Msg::initMsgManagers()
{
    Dinfo< short > dummyDinfo;

    // This is to be the parent of all the msg managers.
    msgManagerId_ = Id::nextId();
    new GlobalDataElement(
        msgManagerId_, Neutral::initCinfo(), "Msgs", 1 );

    SingleMsg::managerId_ = Id::nextId();
    new MsgElement( SingleMsg::managerId_, SingleMsg::initCinfo(),
                    "singleMsg", &SingleMsg::numMsg, &SingleMsg::lookupMsg );

    OneToOneMsg::managerId_ = Id::nextId();
    new MsgElement( OneToOneMsg::managerId_, OneToOneMsg::initCinfo(),
                    "oneToOneMsg", &OneToOneMsg::numMsg, &OneToOneMsg::lookupMsg );

    OneToAllMsg::managerId_ = Id::nextId();
    new MsgElement( OneToAllMsg::managerId_, OneToAllMsg::initCinfo(),
                    "oneToAllMsg", &OneToAllMsg::numMsg, &OneToAllMsg::lookupMsg );

    DiagonalMsg::managerId_ = Id::nextId();
    new MsgElement( DiagonalMsg::managerId_, DiagonalMsg::initCinfo(),
                    "diagonalMsg", &DiagonalMsg::numMsg, &DiagonalMsg::lookupMsg );

    SparseMsg::managerId_ = Id::nextId();
    new MsgElement( SparseMsg::managerId_, SparseMsg::initCinfo(),
                    "sparseMsg", &SparseMsg::numMsg, &SparseMsg::lookupMsg );

    OneToOneDataIndexMsg::managerId_ = Id::nextId();
    new MsgElement( OneToOneDataIndexMsg::managerId_,
                    OneToOneDataIndexMsg::initCinfo(),
                    "oneToOneDataIndexMsg",
                    &OneToOneDataIndexMsg::numMsg,
                    &OneToOneDataIndexMsg::lookupMsg );

    // Do the 'adopt' only after all the message managers exist - we need
    // the OneToAll manager for the adoption messages themselves.
    assert( OneToAllMsg::numMsg() == 0 );
    unsigned int n = 1;
    Shell::adopt( Id(), msgManagerId_, n++ );
    Shell::adopt( msgManagerId_, SingleMsg::managerId_, n++ );
    Shell::adopt( msgManagerId_, OneToOneMsg::managerId_, n++ );
    Shell::adopt( msgManagerId_, OneToAllMsg::managerId_, n++ );
    Shell::adopt( msgManagerId_, DiagonalMsg::managerId_, n++ );
    Shell::adopt( msgManagerId_, SparseMsg::managerId_, n++ );

    return n;
}

void Msg::clearAllMsgs()
{
    lastTrump_ = true;
    for ( unsigned int i = 0; i < SingleMsg::numMsg(); ++i )
    {
        Msg* m = reinterpret_cast< Msg* >( SingleMsg::lookupMsg( i ) );
        if ( m ) delete m;
    }
    for ( unsigned int i = 0; i < OneToOneMsg::numMsg(); ++i )
    {
        Msg* m = reinterpret_cast< Msg* >( OneToOneMsg::lookupMsg( i ) );
        if ( m ) delete m;
    }
    for ( unsigned int i = 0; i < OneToAllMsg::numMsg(); ++i )
    {
        Msg* m = reinterpret_cast< Msg* >( OneToAllMsg::lookupMsg( i ) );
        if ( m ) delete m;
    }
    for ( unsigned int i = 0; i < DiagonalMsg::numMsg(); ++i )
    {
        Msg* m = reinterpret_cast< Msg* >( DiagonalMsg::lookupMsg( i ) );
        if ( m ) delete m;
    }
    for ( unsigned int i = 0; i < SparseMsg::numMsg(); ++i )
    {
        Msg* m = reinterpret_cast< Msg* >( SparseMsg::lookupMsg( i ) );
        if ( m ) delete m;
    }
}

/**
 * Static utility function, provided so that the shell function doing
 * message creation can retrieve the most recent message made.
 */
const Msg* Msg::lastMsg()
{
    return lastMsg_;
}

bool Msg::isLastTrump()
{
    return lastTrump_;
}
