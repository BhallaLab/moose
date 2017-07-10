/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <string>
#include <algorithm>

using namespace std;


#include "header.h"
#include "global.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToAllMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "Shell.h"
#include "Dinfo.h"
#include "Wildcard.h"

// Want to separate out this search path into the Makefile options
#include "../scheduling/Clock.h"

const unsigned int Shell::OkStatus = ~0;
const unsigned int Shell::ErrorStatus = ~1;

bool Shell::isBlockedOnParser_ = 0;
bool Shell::keepLooping_ = 1;
unsigned int Shell::numCores_;
unsigned int Shell::numNodes_;
unsigned int Shell::myNode_;
ProcInfo Shell::p_;
unsigned int Shell::numAcks_ = 0;
vector< unsigned int > Shell::acked_( 1, 0 );
bool Shell::doReinit_( 0 );
bool Shell::isParserIdle_( 0 );
double Shell::runtime_( 0.0 );

const Cinfo* Shell::initCinfo()
{

////////////////////////////////////////////////////////////////
// Value Finfos
////////////////////////////////////////////////////////////////
    static ReadOnlyValueFinfo< Shell, bool > isRunning(
        "isRunning",
        "Flag: Checks if simulation is in progress",
        &Shell::isRunning );

    static ValueFinfo< Shell, ObjId > cwe(
        "cwe",
        "Current working Element",
        &Shell::setCwe,
        &Shell::getCwe );

////////////////////////////////////////////////////////////////
// Dest Finfos: Functions handled by Shell
////////////////////////////////////////////////////////////////
    static DestFinfo handleUseClock( "useClock",
                                     "Deals with assignment of path to a given clock."
                                     " Arguments: path, field, tick number. ",
                                     new EpFunc4< Shell, string, string, unsigned int, unsigned int >(
                                         &Shell::handleUseClock )
                                   );
    static DestFinfo handleCreate( "create",
                                   "create( class, parent, newElm, name, numData, isGlobal )",
                                   new EpFunc6< Shell, string, ObjId, Id, string, NodeBalance, unsigned int >( &Shell::handleCreate ) );

    static DestFinfo handleDelete( "delete",
                                   "When applied to a regular object, this function operates "
                                   "on the Id (element) specified by the ObjId argument. "
                                   "The function deletes the entire object "
                                   "array on this Id, including all dataEntries on it,"
                                   "all its messages, and all its children. The DataIndex here "
                                   "is ignored, and all dataEntries are destroyed. \n"
                                   "When applied to a message: Destroys only that one specific "
                                   "message identified by the full ObjId. \n"
                                   "Args: ObjId\n",
                                   new EpFunc1< Shell, ObjId >( & Shell::destroy ) );

    static DestFinfo handleAddMsg( "addMsg",
                                   "Makes a msg. Arguments are:"
                                   " msgtype, src object, src field, dest object, dest field",
                                   new EpFunc6< Shell, string, ObjId, string, ObjId, string, unsigned int >
                                   ( & Shell::handleAddMsg ) );
    static DestFinfo handleQuit( "quit",
                                 "Stops simulation running and quits the simulator",
                                 new OpFunc0< Shell >( & Shell::handleQuit ) );
    static DestFinfo handleMove( "move",
                                 "handleMove( Id orig, Id newParent ): "
                                 "moves an Element to a new parent",
                                 new EpFunc2< Shell, Id, ObjId >( & Shell::handleMove ) );
    static DestFinfo handleCopy( "copy",
                                 "handleCopy( vector< Id > args, string newName, unsigned int nCopies, bool toGlobal, bool copyExtMsgs ): "
                                 " The vector< Id > has Id orig, Id newParent, Id newElm. "
                                 "This function copies an Element and all its children to a new parent."
                                 " May also expand out the original into nCopies copies."
                                 " Normally all messages within the copy tree are also copied. "
                                 " If the flag copyExtMsgs is true, then all msgs going out are also copied.",
                                 new EpFunc5< Shell, vector< ObjId >, string, unsigned int, bool, bool >(
                                     & Shell::handleCopy ) );

    static DestFinfo setclock( "setclock",
                               "Assigns clock ticks. Args: tick#, dt",
                               new OpFunc2< Shell, unsigned int, double >( & Shell::doSetClock ) );

    static Finfo* shellFinfos[] =
    {
        &setclock,
////////////////////////////////////////////////////////////////
//  Shared msg
////////////////////////////////////////////////////////////////
        // &master,
        // &worker,
        &handleCreate,
        &handleDelete,
        &handleCopy,
        &handleMove,
        &handleAddMsg,
        &handleQuit,
        &handleUseClock,
    };

    static Dinfo< Shell > d;
    static Cinfo shellCinfo (
        "Shell",
        Neutral::initCinfo(),
        shellFinfos,
        sizeof( shellFinfos ) / sizeof( Finfo* ),
        &d
        //new Dinfo< Shell >()
    );

#ifdef ENABLE_LOGGER
    float time = (float(clock() - t)/CLOCKS_PER_SEC);
    logger.initializationTime.push_back( time );
#endif

    return &shellCinfo;
}

static const Cinfo* shellCinfo = Shell::initCinfo();


Shell::Shell()
    :
    gettingVector_( 0 ),
    numGetVecReturns_( 0 ),
    cwe_( ObjId() )
{
    getBuf_.resize( 1, 0 );
}

Shell::~Shell()
{
    ;
}

void Shell::setShellElement( Element* shelle )
{
    shelle_ = shelle;
}


/**
 * This is the version used by the parser. Acts as a blocking,
 * serial-like interface to a potentially multithread, multinode call.
 * Returns the new Id index upon success, otherwise returns Id().
 * The data of the new Element is not necessarily allocated at this point,
 * that can be deferred till the global Instantiate or Reset calls.
 * Idea is that the model should be fully defined before load balancing.
 *
 */
Id Shell::doCreate( string type, ObjId parent, string name,
                    unsigned int numData,
                    NodePolicy nodePolicy,
                    unsigned int preferredNode )
{

    const Cinfo* c = Cinfo::find( type );
    if ( !isNameValid( name ) )
    {
        stringstream ss;
        ss << "Shell::doCreate: bad character in name'" << name <<
           "'. No Element created";
        warning( ss.str() );
        return Id();
    }

    if ( c )
    {
        if ( c->banCreation() )
        {
            stringstream ss;
            ss << "Shell::doCreate: Cannot create an object of class '" <<
               type << "' because it is an abstract base class or a FieldElement.\n";
            warning( ss.str() );
            return Id();
        }
        Element* pa = parent.element();
        if ( !pa )
        {
            stringstream ss;
            ss << "Shell::doCreate: Parent Element'" << parent << "' not found. No Element created";
            warning( ss.str() );
            return Id();
        }

        // TODO: This should be an error in future.
        // This logic of handling already existing path is now handled in
        // melements.cpp . Calling this section should become an error in
        // future.
        if ( Neutral::child( parent.eref(), name ) != Id() )
        {
            stringstream ss;
            ss << "Object with same path already present : " << parent.path()
                << "/" << name;
            moose::showWarn( ss.str() );
            return Id();
        }
        // Get the new Id ahead of time and pass to all nodes.
        Id ret = Id::nextId();
        NodeBalance nb( numData, nodePolicy, preferredNode );
        // Get the parent MsgIndex ahead of time and pass to all nodes.
        unsigned int parentMsgIndex = OneToAllMsg::numMsg();

        SetGet6< string, ObjId, Id, string, NodeBalance, unsigned int >::set(
            ObjId(), // Apply command to Shell
            "create",	// Function to call.
            type, 		// class of new object
            parent,		// Parent
            ret,		// id of new object
            name,		// name of new object
            nb,			// Node balance configuration
            parentMsgIndex	// Message index of child-parent msg.
        );

        // innerCreate( type, parent, ret, name, numData, isGlobal );

        return ret;
    }
    else
    {
        stringstream ss;
        ss << "Shell::doCreate: Class '" << type << "' not known. No Element created";
        warning( ss.str() );
    }

    return Id();
}

bool Shell::doDelete( ObjId oid )
{
    SetGet1< ObjId >::set( ObjId(), "delete", oid );
    /*
       Neutral n;
       n.destroy( i.eref(), 0 );
       */
    return true;
}

ObjId Shell::doAddMsg( const string& msgType,
                       ObjId src, const string& srcField,
                       ObjId dest, const string& destField )
{

    if ( !src.id.element() )
    {
        cout << myNode_ << ": Error: Shell::doAddMsg: src not found" << endl;
        return ObjId();
    }
    if ( !dest.id.element() )
    {
        cout << myNode_ << ": Error: Shell::doAddMsg: dest not found" << endl;
        return ObjId(0, BADINDEX );
    }
    const Finfo* f1 = src.id.element()->cinfo()->findFinfo( srcField );
    if ( !f1 )
    {
        cout << myNode_ << ": Shell::doAddMsg: Error: Failed to find field " << srcField <<
             " on src: " << src.id.element()->getName() << endl;
        return ObjId(0, BADINDEX );
    }
    const Finfo* f2 = dest.id.element()->cinfo()->findFinfo( destField );
    if ( !f2 )
    {
        cout << myNode_ << ": Shell::doAddMsg: Error: Failed to find field " << destField <<
             " on dest: " << dest.id.element()->getName() << endl;
        cout << "Available fields are : " << endl
             << moose::mapToString<string, Finfo*>(dest.id.element()->cinfo()->finfoMap());

        return ObjId( 0, BADINDEX );
    }
    if ( ! f1->checkTarget( f2 ) )
    {
        cout << myNode_ << ": Shell::doAddMsg: Error: Src/Dest Msg type mismatch: " << srcField << "/" << destField << endl;
        return ObjId( 0, BADINDEX );
    }

    const Msg* m = innerAddMsg( msgType, src, srcField, dest, destField, 0 );

    SetGet6< string, ObjId, string, ObjId, string, unsigned int >::set(
        ObjId(), // Apply command to Shell
        "addMsg",	// Function to call.
        msgType,
        src,
        srcField,
        dest,
        destField,
        m->mid().dataIndex
    );

    return m->mid();

    // const Msg* m = innerAddMsg( msgType, src, srcField, dest, destField );
    // return m->mid();
    // return Msg::lastMsg()->mid();
}

void Shell::doQuit()
{
    SetGet0::set( ObjId(), "quit" );
}

void Shell::doStart( double runtime, bool notify )
{
    Id clockId( 1 );
    SetGet2< double, bool >::set( clockId, "start", runtime, notify );

    /*-----------------------------------------------------------------------------
     *  Now that simulation is over, call cleanUp function of Streamer class
     *  objects. The purpose of this is to write whatever is left in tables to
     *  the output file.
     *-----------------------------------------------------------------------------*/
    vector< ObjId > streamers;
    wildcardFind( "/##[TYPE=Streamer]", streamers );
    LOG( moose::debug,  "total streamers " << streamers.size( ) );
    for( vector<ObjId>::const_iterator itr = streamers.begin()
            ; itr != streamers.end(); itr++ )
    {
        Streamer* pStreamer = reinterpret_cast<Streamer*>( itr->data( ) );
        pStreamer->cleanUp( );
    }
}

bool isDoingReinit()
{
    static Id clockId( 1 );
    assert( clockId.element() != 0 );

    return ( reinterpret_cast< const Clock* >(
                 clockId.eref().data() ) )->isDoingReinit();
}

void Shell::doReinit( )
{

    Id clockId( 1 );
    SetGet0::set( clockId, "reinit" );

}

void Shell::doStop( )
{
    Id clockId( 1 );
    SetGet0::set( clockId, "stop" );
}
////////////////////////////////////////////////////////////////////////

void Shell::doSetClock( unsigned int tickNum, double dt )
{
    LookupField< unsigned int, double >::set( ObjId( 1 ), "tickDt", tickNum, dt );
}

void Shell::doUseClock( string path, string field, unsigned int tick )
{
    unsigned int msgIndex = OneToAllMsg::numMsg();
    SetGet4< string, string, unsigned int, unsigned int >::set( ObjId(),
            "useClock", path, field, tick, msgIndex );
    // innerUseClock( path, field, tick);
}

void Shell::doMove( Id orig, ObjId newParent )
{
    if ( orig == Id() )
    {
        cout << "Error: Shell::doMove: Cannot move root Element\n";
        return;
    }

    if ( newParent.element() == 0 )
    {
        cout << "Error: Shell::doMove: Cannot move object to null parent \n";
        return;
    }
    if ( Neutral::isDescendant( newParent, orig ) )
    {
        cout << "Error: Shell::doMove: Cannot move object to descendant in tree\n";
        return;

    }
    const string& name = orig.element()->getName();
    if ( Neutral::child( newParent.eref(), name ) != Id() )
    {
        stringstream ss;
        ss << "Shell::doMove: Object with same name already present: '"
           << newParent.path() << "/" << name << "'. Move failed.";
        warning( ss.str() );
        return;
    }

    SetGet2< Id, ObjId >::set( ObjId(), "move", orig, newParent );
    // innerMove( orig, newParent );
}

bool extractIndex( const string& s, unsigned int& index )
{
    vector< unsigned int > open;
    vector< unsigned int > close;

    index = 0;
    if ( s.length() == 0 ) // a plain slash is OK
        return true;

    if ( s[0] == '[' ) // Cannot open with a brace
        return false;

    for ( unsigned int i = 0; i < s.length(); ++i )
    {
        if ( s[i] == '[' )
            open.push_back( i+1 );
        else if ( s[i] == ']' )
            close.push_back( i );
    }

    if ( open.size() != close.size() )
        return false;
    if ( open.size() == 0 )
        return true; // the index was set already to zero.
    int j = atoi( s.c_str() + open[0] );
    if ( j >= 0 )
    {
        index = j;
        return true;
    }
    return false;
}

/**
 * Static func to subdivide a string at the specified separator.
 */
bool Shell::chopString( const string& path, vector< string >& ret,
                        char separator )
{
    // /foo/bar/zod
    // foo/bar/zod
    // ./foo/bar/zod
    // ../foo/bar/zod
    // .
    // /
    // ..
    ret.resize( 0 );
    if ( path.length() == 0 )
        return 1; // Treat it as an absolute path

    bool isAbsolute = 0;
    string temp = path;
    if ( path[0] == separator )
    {
        isAbsolute = 1;
        if ( path.length() == 1 )
            return 1;
        temp = temp.substr( 1 );
    }

    string::size_type pos = temp.find_first_of( separator );
    ret.push_back( temp.substr( 0, pos ) );
    while ( pos != string::npos )
    {
        temp = temp.substr( pos + 1 );
        if ( temp.length() == 0 )
            break;
        pos = temp.find_first_of( separator );
        ret.push_back( temp.substr( 0, pos ) );
    }
    return isAbsolute;
}

/**
 * Static func to check if an object name is legal. True if legal.
 */
bool Shell::isNameValid( const string& name )
{
    return ( name.length() > 0 &&
             name.find_first_of( "[] #?\"/\\" ) == string::npos );
}

/**
 * static func.
 *
 * Example: /foo/bar[10]/zod[3] would return:
 * ret: {"foo", "bar", "zod" }
 * index: { 0, 10, 3 }
 */
bool Shell::chopPath( const string& path, vector< string >& ret,
                      vector< unsigned int >& index )
{
    bool isAbsolute = chopString( path, ret, '/' );
    if ( isAbsolute )
    {
        index.clear();
    }
    else
    {
        index.clear();
    }
    for ( unsigned int i = 0; i < ret.size(); ++i )
    {
        index.push_back( 0 );
        if ( ret[i] == "." )
            continue;
        if ( ret[i] == ".." )
        {
            continue;
        }
        if ( !extractIndex( ret[i], index[i] ) )
        {
            cout << "Error: Shell::chopPath: Failed to parse indices in path '" <<
                 path << "'\n";
            ret.resize( 0 );
            index.resize( 0 );
            return isAbsolute;
        }
        size_t pos = ret[i].find_first_of( '[' );
        if ( ret[i].find_first_of( '[' ) != string::npos )
            ret[i] = ret[i].substr( 0, pos );
    }

    return isAbsolute;
}

ObjId Shell::doFind( const string& path ) const
{
    if ( path == "/" || path == "/root" )
        return ObjId();

    ObjId curr;
    vector< string > names;
    vector< unsigned int > indices;
    bool isAbsolute = chopPath( path, names, indices );
    assert( names.size() == indices.size() );

    if ( !isAbsolute )
        curr = cwe_;

    for ( unsigned int i = 0; i < names.size(); ++i )
    {
        if ( names[i] == "." )
        {
        }
        else if ( names[i] == ".." )
        {
            curr = Neutral::parent( curr.eref() );
        }
        else
        {
            ObjId pa = curr;
            curr = Neutral::child( curr.eref(), names[i] );
            if ( curr == ObjId() ) // Neutral::child returned Id(), ie, bad.
                return ObjId( 0, BADINDEX );
            if ( curr.element()->hasFields() )
            {
                curr.dataIndex = pa.dataIndex;
                curr.fieldIndex = indices[i];
            }
            else
            {
                curr.dataIndex = indices[i];
                if ( curr.element()->numData() <= curr.dataIndex  )
                    return ObjId( 0, BADINDEX );
            }
        }
    }

    assert( curr.element() );
    if ( curr.element()->numData() <= curr.dataIndex )
        return ObjId( 0, BADINDEX );
    if ( curr.fieldIndex > 0 && !curr.element()->hasFields() )
        return ObjId( 0, BADINDEX );

    return curr;
}

////////////////////////////////////////////////////////////////
// DestFuncs
////////////////////////////////////////////////////////////////

string Shell::doVersion()
{
    return MOOSE_VERSION;
}

void Shell::setCwe( ObjId val )
{
    cwe_ = val;
}

ObjId Shell::getCwe() const
{
    return cwe_;
}

bool Shell::isRunning() const
{
    static Id clockId( 1 );
    assert( clockId.element() != 0 );

    return ( reinterpret_cast< const Clock* >( clockId.eref().data() ) )->isRunning();
}


/**
 * This function handles the message request to create an Element.
 * This request specifies the Id of the new Element and is handled on
 * all nodes.
 *
 * In due course we also have to set up the node decomposition of the
 * Element, but for now the num indicates the total # of array entries.
 * This gets a bit complicated if the Element is a multidim array.
 */
void Shell::handleCreate( const Eref& e,
                          string type, ObjId parent, Id newElm, string name,
                          NodeBalance nb, unsigned int parentMsgIndex )
{
    innerCreate( type, parent, newElm, name, nb, parentMsgIndex );
}



/**
 * Static utility function. Attaches child element to parent element.
 * Must only be called from functions executing in parallel on all nodes,
 * as it does a local message addition
 * MsgIndex is needed to be sure that the same msg identifies parent-child
 * connection on all nodes.
 */
bool Shell::adopt( ObjId parent, Id child, unsigned int msgIndex )
{
    static const Finfo* pf = Neutral::initCinfo()->findFinfo( "parentMsg" );
    // static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
    // static const FuncId pafid = pf2->getFid();
    static const Finfo* f1 = Neutral::initCinfo()->findFinfo( "childOut" );

    assert( !( child.element() == 0 ) );
    assert( !( child == Id() ) );
    assert( !( parent.element() == 0 ) );

    Msg* m = new OneToAllMsg( parent.eref(), child.element(), msgIndex );
    assert( m );

    // cout << myNode_ << ", Shell::adopt: mid = " << m->mid() << ", pa =" << parent << "." << parent()->getName() << ", kid=" << child << "." << child()->getName() << "\n";

    if ( !f1->addMsg( pf, m->mid(), parent.element() ) )
    {
        cout << "move: Error: unable to add parent->child msg from " <<
             parent.element()->getName() << " to " <<
             child.element()->getName() << "\n";
        return 0;
    }
    return 1;
}

/// Adaptor for above function. Also static function.
bool Shell::adopt( Id parent, Id child, unsigned int msgIndex )
{
    return adopt( ObjId( parent ), child, msgIndex );
}

/**
 * This function actually creates the object. Runs on all nodes.
 * Assumes we've already done all the argument checking.
 */
void Shell::innerCreate( string type, ObjId parent, Id newElm, string name,
                         const NodeBalance& nb, unsigned int msgIndex )
// unsigned int numData, bool isGlobal
{
    const Cinfo* c = Cinfo::find( type );
    if ( c )
    {
        Element* ret = 0;
        switch ( nb.policy )
        {
        case MooseGlobal:
            ret = new GlobalDataElement( newElm, c, name, nb.numData );
            break;
        case MooseBlockBalance:
            ret = new LocalDataElement( newElm, c, name, nb.numData );
            break;
        case MooseSingleNode:
            cout << "Error: Shell::innerCreate: Yet to implement SingleNodeDataElement. Making BlockBalance.\n";
            ret = new LocalDataElement( newElm, c, name, nb.numData );
            // ret = new SingleNodeDataElement( newElm, c, name, numData, nb.preferredNode );
            break;
        };
        assert( ret );
        adopt( parent, newElm, msgIndex );
        ret->setTick( Clock::lookupDefaultTick( c->name() ) );
    }
    else
    {
        assert( 0 );
    }
}

void Shell::destroy( const Eref& e, ObjId oid)
{
    Neutral *n = reinterpret_cast< Neutral* >( e.data() );
    assert( n );
    // cout << myNode_ << ": Shell::destroy done for element id: " << eid << ", name = " << eid.element()->getName() << endl;
    n->destroy( oid.eref(), 0 );
    if ( cwe_.id == oid.id )
        cwe_ = ObjId();
}


/**
 * Wrapper function, that does the ack. Other functions also use the
 * inner function to build message trees, so we don't want it to emit
 * multiple acks.
 */
void Shell::handleAddMsg( const Eref& e,
                          string msgType, ObjId src, string srcField,
                          ObjId dest, string destField, unsigned int msgIndex )
{
    // Node 0 will have already called innerAddMsg to get the msgIndex
    if ( myNode() != 0 )
        innerAddMsg( msgType, src, srcField, dest, destField, msgIndex );
    /*
    if ( innerAddMsg( msgType, src, srcField, dest, destField ) )
    	ack()->send( Eref( shelle_, 0 ), Shell::myNode(), OkStatus );
    else
    	ack()->send( Eref( shelle_, 0), Shell::myNode(), ErrorStatus );
    */
}

/**
 * The actual function that adds messages. Does NOT send an ack.
 * The msgIndex specifies the index on which to place this message. If the
 * value is zero it does an automatic placement.
 * Returns zero on failure.
 */
const Msg* Shell::innerAddMsg( string msgType,
                               ObjId src, string srcField,
                               ObjId dest, string destField, unsigned int msgIndex )
{
    /*
    cout << myNode_ << ", Shell::handleAddMsg: " <<
    	msgType << ", " << mid <<
    	", src =" << src << "." << srcField <<
    	", dest =" << dest << "." << destField << "\n";
    	*/
    const Finfo* f1 = src.id.element()->cinfo()->findFinfo( srcField );
    if ( f1 == 0 ) return 0;
    const Finfo* f2 = dest.id.element()->cinfo()->findFinfo( destField );
    if ( f2 == 0 ) return 0;

    // Should have been done before msgs request went out.
    assert( f1->checkTarget( f2 ) );

    Msg *m = 0;
    if ( msgType == "diagonal" || msgType == "Diagonal" )
    {
        m = new DiagonalMsg( src.id.element(), dest.id.element(),
                             msgIndex );
    }
    else if ( msgType == "sparse" || msgType == "Sparse" )
    {
        m = new SparseMsg( src.id.element(), dest.id.element(), msgIndex );
    }
    else if ( msgType == "Single" || msgType == "single" )
    {
        m = new SingleMsg( src.eref(), dest.eref(), msgIndex );
    }
    else if ( msgType == "OneToAll" || msgType == "oneToAll" )
    {
        m = new OneToAllMsg( src.eref(), dest.id.element(), msgIndex );
    }
    else if ( msgType == "AllToOne" || msgType == "allToOne" )
    {
        m = new OneToAllMsg( dest.eref(), src.id.element(), msgIndex ); // Little hack.
    }
    else if ( msgType == "OneToOne" || msgType == "oneToOne" )
    {
        m = new OneToOneMsg( src.eref(), dest.eref(), msgIndex );
    }
    else
    {
        cout << myNode_ <<
             ": Error: Shell::handleAddMsg: msgType not known: "
             << msgType << endl;
        return m;
    }
    if ( m )
    {
        if ( f1->addMsg( f2, m->mid(), src.id.element() ) )
        {
            return m;
        }
        delete m;
        m = 0;
    }
    cout << myNode_ <<
         ": Error: Shell::handleAddMsg: Unable to make/connect Msg: "
         << msgType << " from " << src.id.element()->getName() <<
         " to " << dest.id.element()->getName() << endl;
    return m;
}

bool Shell::innerMove( Id orig, ObjId newParent )
{
    static const Finfo* pf = Neutral::initCinfo()->findFinfo( "parentMsg" );
    static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
    static const FuncId pafid = pf2->getFid();
    static const Finfo* f1 = Neutral::initCinfo()->findFinfo( "childOut" );

    assert( !( orig == Id() ) );
    assert( !( newParent.element() == 0 ) );

    ObjId mid = orig.element()->findCaller( pafid );
    Msg::deleteMsg( mid );

    Msg* m = new OneToAllMsg( newParent.eref(), orig.element(), 0 );
    assert( m );
    if ( !f1->addMsg( pf, m->mid(), newParent.element() ) )
    {
        cout << "move: Error: unable to add parent->child msg from " <<
             newParent.element()->getName() << " to " <<
             orig.element()->getName() << "\n";
        return 0;
    }
    return 1;
}

void Shell::handleMove( const Eref& e, Id orig, ObjId newParent )
{

    innerMove( orig, newParent );
    /*
    if ( innerMove( orig, newParent ) )
    	ack()->send( Eref( shelle_, 0 ), Shell::myNode(), OkStatus );
    else
    	ack()->send( Eref( shelle_, 0 ), Shell::myNode(), ErrorStatus );
    	*/
}

void insertSharedMsgs( const Finfo* f, const Element* e,
                       vector< ObjId >& msgs )
{
    const SharedFinfo* sf = dynamic_cast< const SharedFinfo *>( f );
    if ( sf )
    {
        for ( vector< Finfo* >::const_iterator j =
                    sf->dest().begin(); j != sf->dest().end(); ++j )
        {
            DestFinfo* df = dynamic_cast< DestFinfo* >( *j );
            assert( df );
            FuncId fid = df->getFid();
            // These are the messages to be zapped
            vector< ObjId > caller;
            if ( e->getInputMsgs( caller, fid ) > 0 )
            {
                msgs.insert( msgs.end(), caller.begin(), caller.end() );
            }
        }
    }
}

// Static function
void Shell::dropClockMsgs(
    const vector< ObjId >& list, const string& field )
{
    vector< ObjId > msgs; // These are the messages to delete.
    for ( vector< ObjId >::const_iterator
            i = list.begin(); i != list.end(); ++i )
    {
        // Sanity check: shouldn't try to operate on already deleted objects
        if ( i->element() )
        {
            const Finfo* f = i->element()->cinfo()->findFinfo( field );
            const DestFinfo* df = dynamic_cast< const DestFinfo *>( f );
            if ( df )
            {
                FuncId fid = df->getFid();

                // These are the messages to be zapped
                vector< ObjId > caller;
                if ( i->element()->getInputMsgs( caller, fid ) > 0 )
                {
                    msgs.insert( msgs.end(), caller.begin(), caller.end() );
                }
            }
            else
            {
                insertSharedMsgs( f, i->element(), msgs );
            }
        }
    }
    // Do the unique/erase bit. My favourite example of C++ hideousity.
    sort( msgs.begin(), msgs.end() );
    msgs.erase( unique( msgs.begin(), msgs.end() ), msgs.end() );
    // Delete them.
    for( vector< ObjId >::iterator i = msgs.begin(); i != msgs.end(); ++i )
        Msg::deleteMsg( *i );
}

// Non-static function. The innerAddMsg needs the shell.
void Shell::addClockMsgs(
    const vector< ObjId >& list, const string& field, unsigned int tick,
    unsigned int msgIndex	)
{
    if ( !Id( 1 ).element() )
        return;
    ObjId clockId( 1 );
    dropClockMsgs( list, field ); // Forbid duplicate PROCESS actions.
    for ( vector< ObjId >::const_iterator i = list.begin();
            i != list.end(); ++i )
    {
        if ( i->element() )
        {
            stringstream ss;
            ss << "proc" << tick;
            const Msg* m = innerAddMsg( "OneToAll",
                                        clockId, ss.str(),
                                        *i, field, msgIndex++ );
            if ( m )
                i->element()->innerSetTick( tick );
        }
    }
}

bool Shell::innerUseClock( string path, string field, unsigned int tick,
                           unsigned int msgIndex )
{
    vector< ObjId > list;
    wildcardFind( path, list ); // By default scans only Elements.
    if ( list.size() == 0 )
    {
        // cout << "Warning: Shell::innerUseClock: no Elements found on path " << path << endl;
        return 0;
    }
    // string tickField = "proc";
    // Hack to get around a common error.
    if ( field.substr( 0, 4 ) == "proc" || field.substr( 0, 4 ) == "Proc" )
        field = "proc";
    if ( field.substr( 0, 4 ) == "init" || field.substr( 0, 4 ) == "Init" )
        field = "init";

    addClockMsgs( list, field, tick, msgIndex );
    for ( vector< ObjId >::iterator
            i = list.begin(); i != list.end(); ++i )
        i->id.element()->innerSetTick( tick );
    return 1;
}

void Shell::handleUseClock( const Eref& e,
                            string path, string field, unsigned int tick, unsigned int msgIndex )
{
    // cout << q->getProcInfo()->threadIndexInGroup << ": in Shell::handleUseClock with path " << path << endl << flush;
    innerUseClock( path, field, tick, msgIndex );
    /*
    if ( innerUseClock( path, field, tick ) )
    	ack()->send( Eref( shelle_, 0 ),
    		Shell::myNode(), OkStatus );
    else
    	ack()->send( Eref( shelle_, 0 ),
    		Shell::myNode(), ErrorStatus );
    		*/
}

/**
 * @brief This function is NOT called when simulation ends normally.
 */
void Shell::handleQuit()
{
    Shell::keepLooping_ = 0;
}

// Static function
bool Shell::keepLooping()
{
    return keepLooping_;
}

void Shell::warning( const string& text )
{
    moose::showWarn( text  );
}

void Shell::error( const string& text )
{
    cout << "Error: Shell:: " << text << endl;
}

/**
 * @brief Clean-up MOOSE before shutting down. This function is called whenever
 * keyboard interrupt terminates the simulation.
 */
void Shell::cleanSimulation()
{
    Eref sheller = Id().eref();
    Shell* s = reinterpret_cast< Shell* >( sheller.data() );
    vector< Id > kids;
    Neutral::children( sheller, kids );
    for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i )
    {
        if ( i->value() > 4 )                   /* These are created by users */
        {
            LOG( moose::debug
                    , "Shell::cleanSimulation: deleted cruft at " <<
                 i->value() << ": " << i->path());
            s->doDelete( *i );
        }
    }
    LOG( moose::info, "Cleaned up!");
}
