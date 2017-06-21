/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "FuncOrder.h"
#include "HopFunc.h"
#include "../msg/OneToAllMsg.h"
#include "../shell/Shell.h"
#include "../scheduling/Clock.h"

Element::Element( Id id, const Cinfo* c, const string& name )
    :	name_( name ),
      id_( id ),
      cinfo_( c ),
      msgBinding_( c->numBindIndex() ),
      msgDigest_( c->numBindIndex() ),
      tick_( -1 ),
      isRewired_( false ),
      isDoomed_( false )
{
    id.bindIdToElement( this );
}


Element::~Element()
{
    // A flag that the Element is doomed, used to avoid lookups
    // when deleting Msgs.
    id_.zeroOut();
    markAsDoomed();
    for ( vector< vector< MsgFuncBinding > >::iterator
            i = msgBinding_.begin(); i != msgBinding_.end(); ++i )
    {
        for ( vector< MsgFuncBinding >::iterator
                j = i->begin(); j != i->end(); ++j )
        {
            // This call internally protects against double deletion.
            Msg::deleteMsg( j->mid );
        }
    }

    for ( vector< ObjId >::iterator i = m_.begin(); i != m_.end(); ++i )
        Msg::deleteMsg( *i );
}

/////////////////////////////////////////////////////////////////////////
// Element info functions
/////////////////////////////////////////////////////////////////////////

const string& Element::getName() const
{
    return name_;
}

void Element::setName( const string& val )
{
    name_ = val;
}

const Cinfo* Element::cinfo() const
{
    return cinfo_;
}

Id Element::id() const
{
    return id_;
}

/////////////////////////////////////////////////////////////////////////
// Msg Management
/////////////////////////////////////////////////////////////////////////
void Element::addMsg( ObjId m )
{
    while ( m_.size() > 0 )
    {
        if ( m_.back().bad() )
            m_.pop_back();
        else
            break;
    }
    m_.push_back( m );
    markRewired();
}

class matchMid
{
public:
    matchMid( ObjId mid )
        : mid_( mid )
    {
        ;
    }

    bool operator()( const MsgFuncBinding& m ) const
    {
        return m.mid == mid_;
    }
private:
    ObjId mid_;
};

/**
 * Called from ~Msg. This requires the usual scan through all msgs,
 * and could get inefficient.
 */
void Element::dropMsg( ObjId mid )
{
    if ( isDoomed() ) // This is a flag that the Element is doomed.
        return;
    // Here we have the spectacularly ugly C++ erase-remove idiot.
    m_.erase( remove( m_.begin(), m_.end(), mid ), m_.end() );

    for ( vector< vector< MsgFuncBinding > >::iterator i = msgBinding_.begin(); i != msgBinding_.end(); ++i )
    {
        matchMid match( mid );
        i->erase( remove_if( i->begin(), i->end(), match ), i->end() );
    }
    markRewired();
}

void Element::addMsgAndFunc( ObjId mid, FuncId fid, BindIndex bindIndex )
{
    if ( msgBinding_.size() < bindIndex + 1U )
        msgBinding_.resize( bindIndex + 1 );
    msgBinding_[ bindIndex ].push_back( MsgFuncBinding( mid, fid ) );
    markRewired();
}

void Element::clearBinding( BindIndex b )
{
    assert( b < msgBinding_.size() );
    vector< MsgFuncBinding > temp = msgBinding_[ b ];
    msgBinding_[ b ].resize( 0 );
    for( vector< MsgFuncBinding >::iterator i = temp.begin();
            i != temp.end(); ++i )
    {
        Msg::deleteMsg( i->mid );
    }
    markRewired();
}

/// Used upon ending of MOOSE session, to rapidly clear out messages
void Element::clearAllMsgs()
{
    markAsDoomed();
    m_.clear();
    msgBinding_.clear();
    msgDigest_.clear();
}

/// virtual func, this base version must be called by all derived classes
void Element::zombieSwap( const Cinfo* c )
{
    // cout << name_ << ", cname=" << c->name() << ", t0 = " << this->tick_ << ", t1 = " << Clock::lookupDefaultTick( c->name() ) << endl;
    if ( tick_ == -1 )   // Object is already disabled, let it be.
    {
        return;
    }
    bool zombieInName = ( c->name().substr(0, 6) == "Zombie" );
    if ( tick_ == -2 && !zombieInName )   // Object was a zombie and wants to come home.
    {
        int t = Clock::lookupDefaultTick( c->name() );
        setTick( t );
    }
    else if ( tick_ >= 0 )     // disable clock, with option to return
    {
        if ( zombieInName )
        {
            setTick( -2 );
        }
        else
        {
            int t = Clock::lookupDefaultTick( c->name() );
            setTick( t );
        }
    }
}

int Element::getTick() const
{
    return tick_;
}

void Element::innerSetTick( unsigned int tick )
{
    if ( tick < 32 )
        tick_ = tick;
}

void Element::dropAllMsgsFromSrc( Id src )
{
    static Id clockId( 1 );
    const Element* clock = clockId.element();
    vector< ObjId > msgs;
    for ( vector< ObjId >::const_iterator i = m_.begin();
            i != m_.end(); ++i )
    {
        const Msg* m = Msg::getMsg( *i );
        const Element* src;
        if ( m->e1() == this )
        {
            src = m->e2();
        }
        else
        {
            src = m->e1();
        }
        if ( src == clock )
        {
            msgs.push_back( *i );
        }
    }
    sort( msgs.begin(), msgs.end() );
    // C++ detritus.
    msgs.erase( unique( msgs.begin(), msgs.end() ), msgs.end() );
    for( vector< ObjId >::iterator
            i = msgs.begin(); i != msgs.end(); ++i )
        Msg::deleteMsg( *i );
}

static bool addClockMsg( unsigned int tick, Id tgt, const Finfo* f2 )
{
    Id clockId( 1 );
    stringstream ss;
    ss << "proc" << tick;

    const Finfo* f1 = clockId.element()->cinfo()->findFinfo( ss.str() );
    assert( f1 );
    assert( f2 );
    Msg* m = new OneToAllMsg( clockId.eref(), tgt.element(), 0 );
    if ( m )
    {
        if ( f1->addMsg( f2, m->mid(), clockId.element() ) )
        {
            return true;
        }
        delete m;
    }
    cout << "Error: Element::setTick: failed to connect " << tgt <<
         " to clock\n";
    return false;
}

void Element::setTick( int t )
{
    Id clockId( 1 );
    if ( t == tick_ )
        return;
    if ( tick_ >= 0 )   // Drop all messages coming here from clock.
    {
        dropAllMsgsFromSrc( clockId );
    }
    tick_ = t;
    if ( t < 0 || t > 31 )    // Only 32 ticks available.
    {
        // Don't need to add new ticks.
        return;
    }
    const Finfo* f2 = cinfo()->findFinfo( "init" );
    if ( f2 && dynamic_cast< const SharedFinfo* >( f2 ) )
    {
        // Must build init msg too. This comes on the previous tick.
        assert( t > 0 );
        addClockMsg( t-1, id(), f2 );
    }
    f2 = cinfo()->findFinfo( "proc" );
    if ( f2 )
    {
        addClockMsg( t, id(), f2 );
    }
    else
    {
        cout << "Element::setTick:Warning: Attempt to assign a tick to a '"
             << cinfo_->name() << "'.\nThis does not support process actions.\n";
        tick_ = -1;
    }
}
/////////////////////////////////////////////////////////////////////////
// Msg Information
/////////////////////////////////////////////////////////////////////////

const vector< MsgDigest >& Element::msgDigest( unsigned int index )
{
    if ( isRewired_ )
    {
        digestMessages();
        isRewired_ = false;
    }
    assert( index < msgDigest_.size() );
    return msgDigest_[ index ];
}

const vector< MsgFuncBinding >* Element::getMsgAndFunc( BindIndex b ) const
{
    if ( b < msgBinding_.size() )
        return &( msgBinding_[ b ] );
    return 0;
}

bool Element::hasMsgs( BindIndex b ) const
{
    return ( b < msgBinding_.size() && msgBinding_[b].size() > 0 );
}


void Element::showMsg() const
{
    cout << "Outgoing: \n";
    for ( map< string, Finfo* >::const_iterator i =
                cinfo_->finfoMap().begin();
            i != cinfo_->finfoMap().end(); ++i )
    {
        const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( i->second );
        if ( sf && msgBinding_.size() > sf->getBindIndex() )
        {
            const vector< MsgFuncBinding >& mb = msgBinding_[ sf->getBindIndex()];
            unsigned int numTgt = mb.size();
            if ( numTgt > 0 )
            {
                for ( unsigned int j = 0; j < numTgt; ++j )
                {
                    cout << sf->name() << " bindId=" << sf->getBindIndex() << ": ";
                    cout << j << ": MessageId=" << mb[j].mid <<
                         ", FuncId=" << mb[j].fid <<
                         ", " << Msg::getMsg( mb[j].mid )->e1()->getName() <<
                         " -> " <<
                         Msg::getMsg( mb[j].mid )->e2()->getName() << endl;
                }
            }
        }
    }
    cout << "Dest and Src: \n";
    for ( unsigned int i = 0; i < m_.size(); ++i )
    {
        const Msg* m = Msg::getMsg( m_[i] );
        cout << i << ": MessageId= " << m_[i] <<
             ", e1= " << m->e1()->name_ <<
             ", e2= " << m->e2()->name_ << endl;
    }
}


ObjId Element::findCaller( FuncId fid ) const
{
    for ( vector< ObjId >::const_iterator i = m_.begin();
            i != m_.end(); ++i )
    {
        const Msg* m = Msg::getMsg( *i );
        const Element* src;
        if ( m->e1() == this )
        {
            src = m->e2();
        }
        else
        {
            src = m->e1();
        }
        unsigned int ret = src->findBinding( MsgFuncBinding( *i, fid ) );
        if ( ret != ~0U )
        {
            return *i;
        }
    }
    return ObjId( 0, BADINDEX );
}

unsigned int Element::findBinding( MsgFuncBinding b ) const
{
    for ( unsigned int i = 0; i < msgBinding_.size(); ++i )
    {
        const vector< MsgFuncBinding >& mb = msgBinding_[i];
        vector< MsgFuncBinding>::const_iterator bi =
            find( mb.begin(), mb.end(), b );
        if ( bi != mb.end() )
            return i;
    }
    return ~0;
}

const vector< ObjId >& Element::msgIn() const
{
    return m_;
}

vector< FuncOrder>  putFuncsInOrder(
    const Element* elm, const vector< MsgFuncBinding >& vec )
{
    vector< FuncOrder > fo( vec.size() );
    for ( unsigned int j = 0; j < vec.size(); ++j )
    {
        const MsgFuncBinding& mfb = vec[j];
        const Msg* msg = Msg::getMsg( mfb.mid );
        if ( msg->e1() == elm )
        {
            fo[j].set( msg->e2()->cinfo()->getOpFunc( mfb.fid ), j );
        }
        else
        {
            fo[j].set( msg->e1()->cinfo()->getOpFunc( mfb.fid ), j );
        }
    }
    sort( fo.begin(), fo.end() );
    return fo;
}

/**
 * Picks which messages we retain in the MsgDigest. Does so at two levels:
 * - Builds an array[][] of bools to indicate messages sourced here that
 *   go off-node. These will have special HopFuncs added.
 * - Retains entries in erefs that are sourced anywhere, but targetted here.
 *   Need to be able to handle off-node messages landing here, as well as
 *   the entirely local messages.
 */

// Filter out the messages going off-node. Eliminate from erefs and
// flip flags in targetNodes.
// Do not set any flags for messages originating from off-node, though.
// We're not interested in setting up targets that are the responsibility
// of other nodes.
// Retain Erefs sourced from off-node that go to current node, but eliminate
// those sourced and going off-node.
void filterOffNodeTargets(
    unsigned int start, // This specifies the range of dataIndex of parent
    unsigned int end,	// Element that are present on current node.
    bool isSrcGlobal,
    unsigned int myNode, // I pass this in to help with debugging.
    vector< vector < Eref > >& erefs,
    vector< vector< bool > >& targetNodes ) // targetNodes[srcDataId][node]
{
    // i is index of src dataid
    for ( unsigned int i = 0; i < erefs.size(); ++i )
    {
        vector< Eref > temp;
        vector< Eref >& vec = erefs[i];
        for ( unsigned int j = 0; j < vec.size(); ++j )   // list of tgts.
        {
            const Eref& er = vec[j];
            unsigned int node = er.getNode();
            if ( !isSrcGlobal && // Don't send any  msgs
                    // off-node, as it will be dealt with by the off-node src.
                    i >= start && i < end )   // Sourced from current node.
            {
                if ( node != myNode )				// Target is off-node
                    targetNodes[i][node] = true;
                if ( er.dataIndex() == ALLDATA || er.element()->isGlobal())
                {
                    for ( unsigned int k = 0; k < Shell::numNodes(); ++k )
                        if ( k != myNode )
                            targetNodes[i][k] = true;
                }
            }
            if ( node == myNode ) // Regardless of source, target is onNode.
                temp.push_back( er );
        }
        erefs[i] = temp; // Swap out the original set with the new one.
    }
}

void Element::putTargetsInDigest(
    unsigned int srcNum, const MsgFuncBinding& mfb,
    const FuncOrder& fo,
    vector< vector< bool > >& targetNodes )
// targetNodes[srcDataId][node]
{
    const Msg* msg = Msg::getMsg( mfb.mid );
    vector< vector < Eref > > erefs;
    if ( msg->e1() == this )
        msg->targets( erefs );
    else if ( msg->e2() == this )
        msg->sources( erefs );
    else
        assert( 0 );

    if ( Shell::numNodes() > 1 )
        filterOffNodeTargets(
            localDataStart(),
            localDataStart() + numLocalData(),
            isGlobal(), Shell::myNode(),
            erefs, targetNodes );

    for ( unsigned int j = 0; j < erefs.size(); ++j )
    {
        vector< MsgDigest >& md =
            msgDigest_[ msgBinding_.size() * j + srcNum ];
        // k->func(); erefs[ j ];
        if ( md.size() == 0 || md.back().func != fo.func() )
        {
            md.push_back( MsgDigest( fo.func(), erefs[j] ) );
            /*
            if ( md.back().targets.size() > 0 )
            	cout << "putTargetsInDigest: " << md.back().targets[0] <<
            		", eref = " << erefs[j][0] << endl;
            else
            	cout << "putTargetsInDigest: empty\n";
            */
        }
        else
        {
            md.back().targets.insert( md.back().targets.end(),
                                      erefs[ j ].begin(),
                                      erefs[ j ].end() );
        }
    }
}

// This puts the special HopFuncs into the MsgDigest. Furthermore, the
// Erefs to which they point are the originating Eref instance on the
// remote node. This Eref will then invoke its own send call to complete
// the message transfer.
void Element::putOffNodeTargetsInDigest(
    unsigned int srcNum, vector< vector< bool > >& targetNodes )
// targetNodes[srcDataId][node]
{
    if ( msgBinding_[ srcNum ].size() == 0 )
        return;
    const MsgFuncBinding& mfb = msgBinding_[ srcNum ][0];
    const Msg* msg = Msg::getMsg( mfb.mid );
    const OpFunc* func;
    if ( msg->e1() == this )
    {
        func = msg->e2()->cinfo()->getOpFunc( mfb.fid );
    }
    else
    {
        func = msg->e1()->cinfo()->getOpFunc( mfb.fid );
    }
    assert( func );
    // How do I eventually destroy these?
    const OpFunc* hop = func->makeHopFunc( srcNum );
    for ( unsigned int i = 0; i < numData(); ++i )
    {
        vector< Eref > tgts;
        for ( unsigned int j = 0; j < Shell::numNodes(); ++j )
        {
            if ( targetNodes[i][j] )
                tgts.push_back( Eref( this, i, j ) );
            // This is a hack. I encode the target node # in the FieldIndex
            // and the originating Eref in the remainder of the Eref.
            // The HopFunc has to extract both these things to push into
            // the correct SendBuffer.
        }
        if ( tgts.size() > 0 )
        {
            vector< MsgDigest >& md =
                msgDigest_[ msgBinding_.size() * i + srcNum ];
            md.push_back( MsgDigest( hop, tgts ) );
        }
    }
}

unsigned int findNumDigest( const vector< vector< MsgDigest > > & md,
                            unsigned int totFunc, unsigned int numData, unsigned int funcNum	)
{
    unsigned int ret = 0;
    for ( unsigned int i = 0; i < numData; ++i )
    {
        ret += md[ totFunc * i + funcNum ].size();
    }
    return ret;
}

void Element::digestMessages()
{
    bool report = 0; // for debugging
    msgDigest_.clear();
    msgDigest_.resize( msgBinding_.size() * numData() );
    vector< bool > temp( Shell::numNodes(), false );
    vector< vector< bool > > targetNodes( numData(), temp );
    // targetNodes[srcDataId][node]. The idea is that if any dataEntry has
    // a target off-node, it should flag the entry here so that it can
    // send the message request to the proxy on that node.
    for ( unsigned int i = 0; i < msgBinding_.size(); ++i )
    {
        // Go through and identify functions with the same ptr.
        vector< FuncOrder > fo = putFuncsInOrder( this, msgBinding_[i] );
        for ( vector< FuncOrder >::const_iterator
                k = fo.begin(); k != fo.end(); ++k )
        {
            const MsgFuncBinding& mfb = msgBinding_[i][ k->index() ];
            putTargetsInDigest( i, mfb, *k, targetNodes );
        }
        if ( Shell::numNodes() > 1 )
        {
            if ( report )
            {
                unsigned int numPre = findNumDigest( msgDigest_,
                                                     msgBinding_.size(), numData(), i );
                putOffNodeTargetsInDigest( i, targetNodes );
                unsigned int numPost = findNumDigest( msgDigest_,
                                                      msgBinding_.size(), numData(), i );
                cout << "\nfor Element " << name_;
                cout << ", Func: " << i << ", numFunc = " << fo.size() <<
                     ", numPre= " << numPre <<
                     ", numPost= " << numPost << endl;
                for ( unsigned int j = 0; j < numData(); ++j )
                {
                    cout << endl << j << "	";
                    for ( unsigned int node = 0; node < Shell::numNodes(); ++node)
                    {
                        cout << (int)targetNodes[j][node];
                    }
                }
                cout << endl;
            }
            else
            {
                putOffNodeTargetsInDigest( i, targetNodes );
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////
// Field Information
/////////////////////////////////////////////////////////////////////////
void Element::showFields() const
{
    vector< const SrcFinfo* > srcVec;
    vector< const DestFinfo* > destVec;
    vector< const SharedFinfo* > sharedVec;
    vector< const Finfo* > valueVec; // ValueFinfos are what is left.
    for ( map< string, Finfo* >::const_iterator i =
                cinfo_->finfoMap().begin();
            i != cinfo_->finfoMap().end(); ++i )
    {
        const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >( i->second);
        const DestFinfo* df = dynamic_cast< const DestFinfo* >( i->second);
        const SharedFinfo* shf = dynamic_cast< const SharedFinfo* >( i->second);
        if ( sf )
            srcVec.push_back( sf );
        else if ( df )
            destVec.push_back( df );
        else if ( shf )
            sharedVec.push_back( shf );
        else
            valueVec.push_back( i->second );
    }

    cout << "Showing SrcFinfos: \n";
    for ( unsigned int i = 0; i < srcVec.size(); ++i )
        cout << i << ": " << srcVec[i]->name() << "	Bind=" << srcVec[i]->getBindIndex() << endl;
    cout << "Showing " << destVec.size() << " DestFinfos: \n";
    /*
    for ( unsigned int i = 0; i < destVec.size(); ++i )
    	cout << i << ": " << destVec[i]->name() << "	FuncId=" << destVec[i]->getFid() << endl;
    	*/
    cout << "Showing SharedFinfos: \n";
    for ( unsigned int i = 0; i < sharedVec.size(); ++i )
    {
        cout << i << ": " << sharedVec[i]->name() << "	Src=[ ";
        for ( unsigned int j = 0; j < sharedVec[i]->src().size(); ++j )
            cout << " " << sharedVec[i]->src()[j]->name();
        cout << " ]	Dest=[ ";
        for ( unsigned int j = 0; j < sharedVec[i]->dest().size(); ++j )
            cout << " " << sharedVec[i]->dest()[j]->name();
        cout << " ]\n";
    }
    cout << "Listing ValueFinfos: \n";
    Eref er = this->id().eref();
    string val;
    for ( unsigned int i = 0; i < valueVec.size(); ++i )
    {
        valueVec[i]->strGet( er, valueVec[i]->name(), val );
        cout << i << ": " << valueVec[i]->name() << "	" <<
             val << endl;
    }
}

void Element::destroyElementTree( const vector< Id >& tree )
{
    for( vector< Id >::const_iterator i = tree.begin();
            i != tree.end(); i++ )
        i->element()->markAsDoomed(); // Indicate that Element is doomed
    bool killShell = false;

    // Do not destroy the shell till the very end.
    for( vector< Id >::const_iterator i = tree.begin();
            i != tree.end(); i++ )
    {
        if ( *i == Id() )
            killShell = true;
        else
            i->destroy();
    }
    if ( killShell )
        Id().destroy();
}

void Element::markAsDoomed()
{
    isDoomed_ = true;
}

bool Element::isDoomed() const
{
    return isDoomed_;
}

void Element::markRewired()
{
    isRewired_ = true;
}

void Element::printMsgDigest( unsigned int srcIndex, unsigned int dataId ) const
{
    unsigned int numSrcMsgs = msgBinding_.size();
    unsigned int start = 0;
    unsigned int end = numData();
    if ( dataId < numData() )
    {
        start = dataId;
        end = dataId + 1;
    }
    for (unsigned int i = start; i < end; ++i )
    {
        cout << i << ":	";
        const vector< MsgDigest> & md =
            msgDigest_[numSrcMsgs * i + srcIndex];
        for ( unsigned int j = 0; j < md.size(); ++j )
        {
            cout << j << ":	";
            for ( unsigned int k = 0; k < md[j].targets.size(); ++k )
            {
                cout << "	" <<
                     md[j].targets[k].dataIndex() << "," <<
                     md[j].targets[k].fieldIndex();
            }
        }
        cout << endl;
    }
}

void Element::replaceCinfo( const Cinfo* newCinfo )
{
    cinfo_ = newCinfo;
    // Stuff to be done for data is handled by derived classes in ZombeSwap.
}

//////////////////////////////////////////////////////////////////////////
// Message traversal
//////////////////////////////////////////////////////////////////////////
unsigned int Element::getOutputs( vector< Id >& ret, const SrcFinfo* finfo )
const
{
    assert( finfo ); // would like to check that finfo is on this.
    unsigned int oldSize = ret.size();

    const vector< MsgFuncBinding >* msgVec =
        getMsgAndFunc( finfo->getBindIndex() );
    if ( !msgVec )
        return 0;
    for ( unsigned int i = 0; i < msgVec->size(); ++i )
    {
        const Msg* m = Msg::getMsg( (*msgVec)[i].mid );
        assert( m );
        Id id = m->e2()->id();
        if ( m->e2() == this )
            id = m->e1()->id();
        ret.push_back( id );
    }

    return ret.size() - oldSize;
}

unsigned int Element::getMsgTargetAndFunctions( DataId srcDataId,
        const SrcFinfo* finfo ,
        vector< ObjId >& tgt,
        vector< string >& func
                                              ) const
{
    assert( finfo ); // would like to check that finfo is on this.
    assert( srcDataId < this->numData() );

    tgt.resize( 0 );
    func.resize( 0 );

    const vector< MsgFuncBinding >* msgVec =
        getMsgAndFunc( finfo->getBindIndex() );
    for ( unsigned int i = 0; i < msgVec->size(); ++i )
    {
        const Msg* m = Msg::getMsg( (*msgVec)[i].mid );
        assert( m );
        FuncId fid = (*msgVec)[i].fid;
        if ( m->e1() == this )   // regular direction message.
        {
            string name = m->e2()->cinfo()->destFinfoName( fid );
            vector< vector< Eref > > t;
            m->targets( t );
            assert( t.size() == this->numData() );
            vector< Eref >& row = t[srcDataId];
            for ( vector< Eref >::const_iterator
                    e = row.begin(); e != row.end(); ++e )
            {
                tgt.push_back( e->objId() );
                func.push_back( name );
            }
        }
        else if ( m->e2() == this )
        {
            string name = m->e1()->cinfo()->destFinfoName( fid );
            vector< vector< Eref > > t;
            m->sources( t );
            assert( t.size() == this->numData() );
            vector< Eref >& row = t[srcDataId];
            for ( vector< Eref >::const_iterator
                    e = row.begin(); e != row.end(); ++e )
            {
                tgt.push_back( e->objId() );
                func.push_back( name );
            }
        }
        else
        {
            assert( 0 );
        }
    }
    return tgt.size();
}

// Returns multiple Obj sources and their srcFinfo names.
unsigned int Element::getMsgSourceAndSender( FuncId fid,
        vector< ObjId >& srcObj,
        vector< string >& sender ) const
{
    for ( vector< ObjId >::const_iterator i = m_.begin();
            i != m_.end(); ++i )
    {
        const Msg* m = Msg::getMsg( *i );
        const Element* src;
        if ( m->e1() == this )
        {
            src = m->e2();
        }
        else
        {
            src = m->e1();
        }
        unsigned int ret = src->findBinding( MsgFuncBinding( *i, fid ) );
        if ( ret != ~0U )
        {
            // problem here, to get dataId
            srcObj.push_back( ObjId( src->id(), 0 ) );
            sender.push_back( src->cinfo()->srcFinfoName( ret ) );
        }
    }
    return srcObj.size();
}

vector< ObjId > Element::getMsgTargets( DataId srcDataId,
                                        const SrcFinfo* finfo  ) const
{
    assert( finfo ); // would like to check that finfo is on this.
    assert( srcDataId < this->numData() );

    vector< ObjId > ret;
    Eref er( const_cast< Element* >( this ), srcDataId );

    const vector< MsgDigest >&md = er.msgDigest( finfo->getBindIndex() );
    for ( vector< MsgDigest >::const_iterator
            i = md.begin(); i != md.end(); ++i )
    {
        for ( vector< Eref >::const_iterator
                j = i->targets.begin(); j != i->targets.end(); ++j )
        {
            if ( j->dataIndex() == ALLDATA )
            {
                for ( unsigned int k = 0;
                        k < j->element()->numData(); ++k )
                    ret.push_back( ObjId( j->id(), k ) );
            }
            else
            {
                ret.push_back( j->objId() );
            }
        }
    }
    return ret;
}

unsigned int Element::getInputs( vector< Id >& ret, const DestFinfo* finfo )
const
{
    assert( finfo ); // would like to check that finfo is on src.
    unsigned int oldSize = ret.size();

    FuncId fid = finfo->getFid();
    vector< ObjId > caller;
    getInputMsgs( caller, fid );
    for ( vector< ObjId >::iterator i = caller.begin();
            i != caller.end(); ++i  )
    {
        const Msg* m = Msg::getMsg( *i );
        assert( m );

        Id id = m->e1()->id();
        if ( m->e1() == this )
            id = m->e2()->id();
        ret.push_back( id );
    }
    return ret.size() - oldSize;
}

/**
 * Returns vectors of sources of messages to a field or data element.
 * To go with each entry, also return the field or data index of _target_.
 * Needed to track which inputs go to with field index of current element.
 * So if A-->x0, B-->x1, and C-->x2, it will return A, B, C, no matter
 * in which order the messages were created.
 * If there are multiple messages to x0, it will return the first.
 */
unsigned int Element::getInputsWithTgtIndex( vector< pair< Id, unsigned int> >& ret, const DestFinfo* finfo )
const
{
    assert( finfo ); // would like to check that finfo is on src.
    ret.clear();

    FuncId fid = finfo->getFid();
    vector< ObjId > caller;
    getInputMsgs( caller, fid );
    for ( vector< ObjId >::iterator i = caller.begin();
            i != caller.end(); ++i  )
    {
        const Msg* m = Msg::getMsg( *i );
        assert( m );

        if ( m->e1() == this )
        {
            Eref tgt = m->firstTgt( Eref( m->e2(), 0 ) );
            unsigned int idx = this->hasFields() ? tgt.fieldIndex(): tgt.dataIndex();
            ret.push_back( pair< Id, unsigned int >( m->e2()->id(), idx ) );
        }
        else if ( m->e2() == this )
        {
            Eref tgt = m->firstTgt( Eref( m->e1(), 0 ) );
            unsigned int idx = this->hasFields() ? tgt.fieldIndex(): tgt.dataIndex();
            ret.push_back( pair< Id, unsigned int >( m->e1()->id(), idx ) );
        }
    }
    return ret.size();
}

unsigned int Element::getNeighbors( vector< Id >& ret, const Finfo* finfo )
const
{
    ret.resize( 0 );
    if ( !finfo )
        return 0;

    const SrcFinfo* srcF = dynamic_cast< const SrcFinfo* >( finfo );
    const DestFinfo* destF = dynamic_cast< const DestFinfo* >( finfo );
    const SharedFinfo* sharedF = dynamic_cast< const SharedFinfo* >( finfo );
    assert( srcF || destF || sharedF );


    if ( srcF )
        return getOutputs( ret, srcF );
    else if ( destF )
        return getInputs( ret, destF );
    else if ( ! sharedF->src().empty() )
        return getOutputs( ret, sharedF->src().front() );
    else if ( ! sharedF->dest().empty() )
    {
        Finfo* subFinfo = sharedF->dest().front();
        const DestFinfo* subDestFinfo =
            dynamic_cast< const DestFinfo* >( subFinfo );
        assert( subDestFinfo );
        return getInputs( ret, subDestFinfo );
    }
    else
    {
        assert( 0 );
    }
    return 0;
}

// May return multiple Msgs.
unsigned int Element::getInputMsgs( vector< ObjId >& caller, FuncId fid)
const
{
    for ( vector< ObjId >::const_iterator i = m_.begin();
            i != m_.end(); ++i )
    {
        const Msg* m = Msg::getMsg( *i );
        const Element* src;
        if ( m->e1() == this )
        {
            src = m->e2();
        }
        else
        {
            src = m->e1();
        }
        unsigned int ret = src->findBinding( MsgFuncBinding( *i, fid ) );
        if ( ret != ~0U )
        {
            caller.push_back( *i );
        }
    }
    return caller.size();
}

unsigned int Element::getFieldsOfOutgoingMsg( ObjId mid,
        vector< pair< BindIndex, FuncId > >& ret ) const
{
    ret.resize( 0 );
    for ( unsigned int i = 0; i < msgBinding_.size(); ++i )
    {
        const vector< MsgFuncBinding >& mb = msgBinding_[i];
        for ( vector< MsgFuncBinding >::const_iterator j = mb.begin();
                j != mb.end(); ++j )
        {
            if ( j->mid == mid )
            {
                ret.push_back( pair< BindIndex, FuncId >( i, j->fid ));
            }
        }
    }
    return ret.size();
}
