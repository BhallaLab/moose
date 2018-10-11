/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "SingleMsg.h"

// Initializing static variables
Id SingleMsg::managerId_;
vector< SingleMsg* > SingleMsg::msg_;

/////////////////////////////////////////////////////////////////////
// Here is the SingleMsg code
/////////////////////////////////////////////////////////////////////

SingleMsg::SingleMsg( const Eref& e1, const Eref& e2, unsigned int msgIndex)
    : Msg( ObjId( managerId_, (msgIndex != 0 ) ? msgIndex: msg_.size() ),
           e1.element(), e2.element() ),
      i1_( e1.dataIndex() ),
      i2_( e2.dataIndex() ),
      f2_( e2.fieldIndex() )
{
    if ( msgIndex == 0 )
    {
        msg_.push_back( this );
        return;
    }
    else if ( msg_.size() <= msgIndex )
    {
        msg_.resize( msgIndex + 1 );
    }
    msg_[ msgIndex ] = this;
}

SingleMsg::~SingleMsg()
{
    assert( mid_.dataIndex < msg_.size() );
    msg_[ mid_.dataIndex ] = 0; // ensure deleted ptr isn't reused.
}

Eref SingleMsg::firstTgt( const Eref& src ) const
{
    if ( src.element() == e1_ )
        return Eref( e2_, i2_, f2_ );
    else if ( src.element() == e2_ )
        return Eref( e1_, i1_ );
    return Eref( 0, 0 );
}

void SingleMsg::sources( vector< vector< Eref > >& v ) const
{
    v.clear();
    v.resize( e2_->numData() );
    v[i2_].resize( 1, Eref( e1_, i1_ ) );
}

void SingleMsg::targets( vector< vector< Eref > >& v ) const
{
    v.clear();
    v.resize( e1_->numData() );
    v[i1_].resize( 1, Eref( e2_, i2_, f2_ ) );
}



/*
bool SingleMsg::isMsgHere( const Qinfo& q ) const
{
	if ( q.isForward() )
		return ( i1_ == q.srcIndex() );
	else
		return ( i2_ == q.srcIndex() );
}
*/

DataId SingleMsg::i1() const
{
    return i1_;
}

DataId SingleMsg::i2() const
{
    return i2_;
}

Id SingleMsg::managerId() const
{
    return SingleMsg::managerId_;
}

ObjId SingleMsg::findOtherEnd( ObjId f ) const
{
    if ( f.element() == e1() )
    {
        if ( f.dataIndex == i1_ )
            return ObjId( e2()->id(), i2_ );
    }
    else if ( f.element() == e2() )
    {
        if ( f.dataIndex == i2_ )
            return ObjId( e1()->id(), i1_ );
    }
    return ObjId( 0, BADINDEX );
}

Msg* SingleMsg::copy( Id origSrc, Id newSrc, Id newTgt,
                      FuncId fid, unsigned int b, unsigned int n ) const
{
    const Element* orig = origSrc.element();
    if ( n <= 1 )
    {
        SingleMsg* ret = 0;
        if ( orig == e1() )
        {
            ret = new SingleMsg( Eref( newSrc.element(), i1_ ),
                                 Eref( newTgt.element(), i2_, f2_ ), 0 );
            ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
        }
        else if ( orig == e2() )
        {
            ret = new SingleMsg( Eref( newTgt.element(), i1_ ),
                                 Eref( newSrc.element(), i2_, f2_ ), 0 );
            ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
        }
        else
        {
            assert( 0 );
        }
        return ret;
    }
    else
    {
        // Here we need a SliceMsg which goes from one 2-d array to another.
        cout << "Error: SingleMsg::copy: SliceMsg not yet implemented\n";
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////
// Here we set up the MsgManager portion of the class.
///////////////////////////////////////////////////////////////////////

const Cinfo* SingleMsg::initCinfo()
{
    ///////////////////////////////////////////////////////////////////
    // Field definitions.
    ///////////////////////////////////////////////////////////////////
    static ValueFinfo< SingleMsg, DataId > index1(
        "i1",
        "Index of source object.",
        &SingleMsg::setI1,
        &SingleMsg::getI1
    );
    static ValueFinfo< SingleMsg, DataId > index2(
        "i2",
        "Index of dest object.",
        &SingleMsg::setI2,
        &SingleMsg::getI2
    );

    static Finfo* singleMsgFinfos[] =
    {
        &index1,		// value
        &index2,		// value
    };

    static Dinfo< short > dinfo;
    static Cinfo singleMsgCinfo (
        "SingleMsg",					// name
        Msg::initCinfo(),		// base class
        singleMsgFinfos,
        sizeof( singleMsgFinfos ) / sizeof( Finfo* ),	// num Fields
        &dinfo
    );

    return &singleMsgCinfo;
}

static const Cinfo* singleMsgCinfo = SingleMsg::initCinfo();


DataId SingleMsg::getI1() const
{
    return i1_;
}

void SingleMsg::setI1( DataId di )
{
    i1_ = di;
    e1()->markRewired();
    e2()->markRewired();
}

DataId SingleMsg::getI2() const
{
    return i2_;
}

void SingleMsg::setI2( DataId di )
{
    i2_ = di;
    e1()->markRewired();
    e2()->markRewired();
}

void SingleMsg::setTargetField( unsigned int f )
{
    f2_ = f;
    e1()->markRewired();
}

unsigned int SingleMsg::getTargetField() const
{
    return f2_;
}

/// Static function for Msg access
unsigned int SingleMsg::numMsg()
{
    return msg_.size();
}

/// Static function for Msg access
char* SingleMsg::lookupMsg( unsigned int index )
{
    assert( index < msg_.size() );
    return reinterpret_cast< char* >( msg_[index] );
}
