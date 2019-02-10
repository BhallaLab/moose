/***
 *       Filename:  SocketStreamer.cpp
 *
 *    Description:  TCP and Unix Domain Socket to stream data.
 *
 *         Author:  Dilawar Singh <dilawar.s.rajput@gmail.com>
 *   Organization:  NCBS Bangalore
 *
 *        License:  See MOOSE licence.
 */

#include <algorithm>
#include <sstream>
#include <chrono>
#include <thread>
#include <unistd.h>

#include "../basecode/global.h"
#include "../basecode/header.h"
#include "../scheduling/Clock.h"
#include "../utility/utility.h"
#include "../shell/Shell.h"
#include "SocketStreamer.h"

const Cinfo* SocketStreamer::initCinfo()
{
    /*-----------------------------------------------------------------------------
     * Finfos
     *-----------------------------------------------------------------------------*/
    static ValueFinfo< SocketStreamer, size_t > port(
        "port"
        , "Set port number for streaming. Valid only of TCP socket."
        , &SocketStreamer::setPort
        , &SocketStreamer::getPort
    );

    static ValueFinfo< SocketStreamer, string > address(
        "address"
        , "Set adresss for socket e.g. http://localhost:31416 (host:port for TCP SOCKET) "
        ", or file:///tmp/MOOSE_SOCK for UNIX domain socket."
        , &SocketStreamer::setAddress
        , &SocketStreamer::getAddress
    );

    static ReadOnlyValueFinfo< SocketStreamer, size_t > numTables (
        "numTables"
        , "Number of Tables handled by SocketStreamer "
        , &SocketStreamer::getNumTables
    );

    /*-----------------------------------------------------------------------------
     *
     *-----------------------------------------------------------------------------*/
    static DestFinfo process(
        "process"
        , "Handle process call"
        , new ProcOpFunc<SocketStreamer>(&SocketStreamer::process)
    );

    static DestFinfo reinit(
        "reinit"
        , "Handles reinit call"
        , new ProcOpFunc<SocketStreamer> (&SocketStreamer::reinit)
    );

    static DestFinfo addTable(
        "addTable"
        , "Add a table to SocketStreamer"
        , new OpFunc1<SocketStreamer, Id>(&SocketStreamer::addTable)
    );

    static DestFinfo addTables(
        "addTables"
        , "Add many tables to SocketStreamer"
        , new OpFunc1<SocketStreamer, vector<Id> >(&SocketStreamer::addTables)
    );

    static DestFinfo removeTable(
        "removeTable"
        , "Remove a table from SocketStreamer"
        , new OpFunc1<SocketStreamer, Id>(&SocketStreamer::removeTable)
    );

    static DestFinfo removeTables(
        "removeTables"
        , "Remove tables -- if found -- from SocketStreamer"
        , new OpFunc1<SocketStreamer, vector<Id>>(&SocketStreamer::removeTables)
    );

    /*-----------------------------------------------------------------------------
     *  ShareMsg definitions.
     *-----------------------------------------------------------------------------*/
    static Finfo* procShared[] =
    {
        &process, &reinit, &addTable, &addTables, &removeTable, &removeTables
    };

    static SharedFinfo proc(
        "proc",
        "Shared message for process and reinit",
        procShared, sizeof( procShared ) / sizeof( const Finfo* )
    );

    static Finfo * socketStreamFinfo[] =
    {
        &port, &address, &proc, &numTables
    };

    static string doc[] =
    {
        "Name", "SocketStreamer",
        "Author", "Dilawar Singh (@dilawar, github), 2018",
        "Description", "SocketStreamer: Stream moose.Table data to a socket.\n"
    };

    static Dinfo< SocketStreamer > dinfo;

    static Cinfo tableStreamCinfo(
        "SocketStreamer",
        TableBase::initCinfo(),
        socketStreamFinfo,
        sizeof( socketStreamFinfo )/sizeof(Finfo *),
        &dinfo,
        doc,
        sizeof(doc) / sizeof(string)
    );

    return &tableStreamCinfo;
}

static const Cinfo* tableStreamCinfo = SocketStreamer::initCinfo();

// Constructor
SocketStreamer::SocketStreamer() :
     currTime_(0.0)
    , numMaxClients_(1)
    , sockType_ ( UNIX_DOMAIN_SOCKET )
    , sockfd_(-1)
    , clientfd_(-1)
    , ip_( TCP_SOCKET_IP )
    , port_( TCP_SOCKET_PORT )
    , address_ ( "file://MOOSE" )
{
    clk_ = reinterpret_cast<Clock*>( Id(1).eref().data() );

    // Not all compilers allow initialization during the declaration of class
    // methods.
    columns_.push_back( "time" );               /* First column is time. */
    tables_.resize(0);
    tableIds_.resize(0);
    tableTick_.resize(0);
    tableDt_.resize(0);

}

SocketStreamer& SocketStreamer::operator=( const SocketStreamer& st )
{
    return *this;
}


// Deconstructor
SocketStreamer::~SocketStreamer()
{
    // Now cleanup the socket as well.
    all_done_ = true;
    if(sockfd_ > 0)
    {
        LOG(moose::debug, "Closing socket " << sockfd_ );
        shutdown(sockfd_, SHUT_RD);
        close(sockfd_);

        if( sockType_ == UNIX_DOMAIN_SOCKET )
            ::unlink( unixSocketFilePath_.c_str() );
    }

    if( processThread_.joinable() )
        processThread_.join();
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Stop a thread.
 * See: http://www.bo-yang.net/2017/11/19/cpp-kill-detached-thread
 *
 * @Param tname name of thread.
 */
/* ----------------------------------------------------------------------------*/
//void SocketStreamer::stopThread(const std::string& tname)
//{
//    ThreadMap::const_iterator it = tm_.find(tname);
//    if (it != tm_.end())
//    {
//        it->second.std::thread::~thread(); // thread not killed
//        tm_.erase(tname);
//        LOG(moose::debug, "Thread " << tname << " killed." );
//    }
//}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Connect to a client. If already connected to one, then do not make
 * any more connections.
 */
/* ----------------------------------------------------------------------------*/
void SocketStreamer::listenToClients(size_t numMaxClients)
{
    assert(0 < sockfd_ );
    assert( numMaxClients > 0 );
    numMaxClients_ = numMaxClients;
    if(-1 == listen(sockfd_, numMaxClients_))
        LOG(moose::error, "Failed listen() on socket " << sockfd_
                << ". Error was: " << strerror(errno) );

}

void SocketStreamer::initServer( void )
{
    setSocketType( );
    if( sockType_ == UNIX_DOMAIN_SOCKET )
        initUDSServer();
    else
        initTCPServer();

    LOG(moose::debug,  "Successfully created SocketStreamer server: " << sockfd_);

    //  Listen for incoming clients. This function does nothing if connection is
    //  already made.
    listenToClients(2);
}

void SocketStreamer::configureSocketServer( )
{
    // One can set socket option using setsockopt function. See manual page
    // for details. We are making it 'reusable'.
    int on = 1;

#ifdef SO_REUSEADDR
    if(0 > setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, (const char *)&on, sizeof(on)))
        LOG(moose::warning, "Warn: setsockopt() failed");
#endif

    if(0 > setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)))
        LOG(moose::warning, "Warn: setsockopt() failed");
}

void SocketStreamer::initUDSServer( void )
{
    // PF_UNIX means that sockets are local.
    sockfd_ = socket(PF_UNIX, SOCK_STREAM, 0);
    if( ! sockfd_)
    {
        isValid_ = false;
        perror( "Socket" );
    }

    if( sockfd_ > 0 )
    {
        unixSocketFilePath_ = address_.substr(7); bzero(&sockAddrUDS_, sizeof(sockAddrUDS_));
        sockAddrUDS_.sun_family = AF_UNIX;
        strncpy(sockAddrUDS_.sun_path, unixSocketFilePath_.c_str(), sizeof(sockAddrUDS_.sun_path)-1);
        configureSocketServer();

        // Bind. Make sure bind is not std::bind
        if(0 > ::bind(sockfd_, (struct sockaddr*) &sockAddrUDS_, sizeof(sockAddrUDS_)))
        {
            isValid_ = false;
            LOG(moose::warning, "Warn: Failed to create socket at " << unixSocketFilePath_
                << ". File descriptor: " << sockfd_
                << ". Erorr: " << strerror(errno)
               );
        }
    }

    if( (! isValid_) || (sockfd_ < 0) )
        ::unlink( unixSocketFilePath_.c_str() );
}

void SocketStreamer::initTCPServer( void )
{
    // Create a blocking socket.
    LOG( moose::debug, "Creating TCP socket on port: "  << port_ );
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if( 0 > sockfd_ )
    {
        perror("socket");
        isValid_ = false;
        return;
    }

    configureSocketServer();
    bzero((char*) &sockAddrTCP_, sizeof(sockAddrTCP_));
    sockAddrTCP_.sin_family = AF_INET;
    sockAddrTCP_.sin_addr.s_addr = INADDR_ANY;
    sockAddrTCP_.sin_port = htons( port_ );

    // Bind. Make sure bind is not std::bind
    if(0 > ::bind(sockfd_, (struct sockaddr*) &sockAddrTCP_, sizeof(sockAddrTCP_)))
    {
        isValid_ = false;
        LOG(moose::warning, "Warn: Failed to create server at " << ip_ << ":" << port_
            << ". File descriptor: " << sockfd_
            << ". Erorr: " << strerror(errno)
           );
        return;
    }
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Stream data over socket.
 *
 * @Returns True of success, false otherwise. It is callee job to clean up data_
 *          on a successful return from this function.
 */
/* ----------------------------------------------------------------------------*/
bool SocketStreamer::streamData( )
{
    if( clientfd_ > 0)
    {
        buffer_ += dataToString();

        if( buffer_.size() < frameSize_ )
            buffer_ += string(frameSize_-buffer_.size(), ' ');

        string toSend = buffer_.substr(0, frameSize_);

        int sent = send(clientfd_, buffer_.substr(0, frameSize_).c_str(), frameSize_, MSG_MORE);
        buffer_ = buffer_.erase(0, sent);

        assert( sent == (int)frameSize_);
        LOG(moose::debug, "Sent " << sent << " bytes." );

        if(0 > sent)
        {
            LOG(moose::warning, "Failed to send. Error: " << strerror(errno)
                << ". client id: " << clientfd_ );
            return false;
        }

        // clear up the tables.
        for( auto t : tables_ )
            t->clearVec();
        return true;
    }
    else
        LOG(moose::warning, "No client found to stream data. ClientFD: " << clientfd_ );

    return false;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Convert table to string (use scientific notation).
 *
 * @Returns String in JSON like format.
 */
/* ----------------------------------------------------------------------------*/
string SocketStreamer::dataToString( )
{
    stringstream ss;
    // Enabling this would be require quite a lot of characters to be streamed.
    //ss.precision( 7 );
    //ss << std::scientific;
    vector<double> data;

    // Else stream the data.
    ss << "{";
    for( size_t i = 0; i < tables_.size(); i++)
    {
        ss << "\"" << columns_[i+1] << "\":[";
        ss << tables_[i]->toJSON(true);
        ss << "],";
    }

    // remove , at the end else it won't be a valid JSON.
    string res = ss.str();
    if( ',' == res.back())
        res.pop_back();
    res += "}\n";
    return res;
}

bool SocketStreamer::enoughDataToStream(size_t minsize)
{
    for( size_t i = 0; i < tables_.size(); i++)
        if(tables_[i]->getVec().size() >= minsize )
            return true;
    return false;
}

void SocketStreamer::connectAndStream( )
{
    currTime_ = clk_->getCurrentTime();

    // If server was invalid then there is no point.
    if( ! isValid_ )
    {
        LOG( moose::error, "Server could not set up." );
        return;
    }

    // Now lets get into a loop to stream data.
    while( (! all_done_) )
    {
        clientfd_ = ::accept(sockfd_, NULL, NULL);
        if( clientfd_ >= 0 )
            streamData();
    }
}

/**
 * @brief Reinit. We make sure it is non-blocking.
 *
 * @param e
 * @param p
 */
void SocketStreamer::reinit(const Eref& e, ProcPtr p)
{
    if( tables_.size() == 0 )
    {
        moose::showWarn( "No table found. Disabling SocketStreamer.\nDid you forget"
                " to call addTables() on SocketStreamer object. " + e.objId().path()
                );
        e.element()->setTick( -2 );             /* Disable process */
        return;
    }

    thisDt_ = clk_->getTickDt( e.element()->getTick() );

    // Push each table dt_ into vector of dt
    for( size_t i = 0; i < tables_.size(); i++)
    {
        Id tId = tableIds_[i];
        int tickNum = tId.element()->getTick();
        tableDt_.push_back( clk_->getTickDt( tickNum ) );
    }

    // This should only be called once.
    initServer();

    // Launch a thread in background which monitors the any client trying to
    // make connection to server.
    processThread_ = std::thread(&SocketStreamer::connectAndStream, this);
}

/**
 * @brief This function is called at its clock tick.
 *
 * @param e
 * @param p
 */
void SocketStreamer::process(const Eref& e, ProcPtr p)
{
    // It does nothing. See the connectAndStream function.
    ;
}

/**
 * @brief Add a table to streamer.
 *
 * @param table Id of table.
 */
void SocketStreamer::addTable( Id table )
{
    // If this table is not already in the vector, add it.
    for( size_t i = 0; i < tableIds_.size(); i++)
        if( table.path() == tableIds_[i].path() )
            return;                             /* Already added. */

    Table* t = reinterpret_cast<Table*>(table.eref().data());
    tableIds_.push_back( table );
    tables_.push_back( t );
    tableTick_.push_back( table.element()->getTick() );

    // NOTE: If user can make sure that names are unique in table, using name is
    // better than using the full path.
    if( t->getColumnName().size() > 0 )
        columns_.push_back( t->getColumnName( ) );
    else
        columns_.push_back( moose::moosePathToUserPath( table.path() ) );
}

/**
 * @brief Add multiple tables to SocketStreamer.
 *
 * @param tables
 */
void SocketStreamer::addTables( vector<Id> tables )
{
    if( tables.size() == 0 )
        return;
    for( vector<Id>::const_iterator it = tables.begin(); it != tables.end(); it++)
        addTable( *it );
}

void SocketStreamer::addTables( vector<ObjId> tables )
{
    if( tables.size() == 0 )
        return;
    for( auto t : tables )
        addTable( Id(t) );
}


/**
 * @brief Remove a table from SocketStreamer.
 *
 * @param table. Id of table.
 */
void SocketStreamer::removeTable( Id table )
{
    int matchIndex = -1;
    for (size_t i = 0; i < tableIds_.size(); i++)
    {
        if( table.path() == tableIds_[i].path() )
        {
            matchIndex = i;
            break;
        }
    }

    if( matchIndex > -1 )
    {
        tableIds_.erase( tableIds_.begin() + matchIndex );
        tables_.erase( tables_.begin() + matchIndex );
        columns_.erase( columns_.begin() + matchIndex );
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Determines socket type from the given address.
 */
/* ----------------------------------------------------------------------------*/
void SocketStreamer::setSocketType( )
{
    LOG( moose::debug,  "Socket address is " << address_ );
    if( "file://" == address_.substr(0, 7))
        sockType_ = UNIX_DOMAIN_SOCKET;
    else if( "http://" == address_.substr(0,7))
        sockType_ = TCP_SOCKET;
    else
        sockType_ = UNIX_DOMAIN_SOCKET;
    return;
}

/**
 * @brief Remove multiple tables -- if found -- from SocketStreamer.
 *
 * @param tables
 */
void SocketStreamer::removeTables( vector<Id> tables )
{
    for( vector<Id>::const_iterator it = tables.begin(); it != tables.end(); it++)
        removeTable( *it );
}

/**
 * @brief Get the number of tables handled by SocketStreamer.
 *
 * @return  Number of tables.
 */
size_t SocketStreamer::getNumTables( void ) const
{
    return tables_.size();
}


void SocketStreamer::setPort( const size_t port )
{
    port_ = port;
}

size_t SocketStreamer::getPort( void ) const
{
    assert( port_ > 1 );
    return port_;
}

void SocketStreamer::setAddress( const string addr )
{
    address_ = addr;
}

string SocketStreamer::getAddress( void ) const
{
    return address_;
}
