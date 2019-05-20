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
        , new OpFunc1<SocketStreamer, ObjId>(&SocketStreamer::addTable)
    );

    static DestFinfo addTables(
        "addTables"
        , "Add many tables to SocketStreamer"
        , new OpFunc1<SocketStreamer, vector<ObjId> >(&SocketStreamer::addTables)
    );

    static DestFinfo removeTable(
        "removeTable"
        , "Remove a table from SocketStreamer"
        , new OpFunc1<SocketStreamer, ObjId>(&SocketStreamer::removeTable)
    );

    static DestFinfo removeTables(
        "removeTables"
        , "Remove tables -- if found -- from SocketStreamer"
        , new OpFunc1<SocketStreamer, vector<ObjId>>(&SocketStreamer::removeTables)
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
    , sockfd_(-1)
    , clientfd_(-1)
    , sockInfo_( MooseSocketInfo( "file://MOOSE" ) )
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
        shutdown(sockfd_, SHUT_RDWR);
        close(sockfd_);

        if( sockInfo_.type == UNIX_DOMAIN_SOCKET )
            ::unlink( sockInfo_.filepath.c_str() );
    }

    if( processThread_.joinable())
        processThread_.join();

    // Close the client as well.
    if( clientfd_ > -1 )
    {
        shutdown(clientfd_, SHUT_RDWR);
        close(clientfd_);
    }
}

void SocketStreamer::addStringToDoubleVec(vector<double>&res, const string s)
{
    for(char c : s)
        res.push_back((double)c);
}

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
    if( sockInfo_.type == UNIX_DOMAIN_SOCKET )
        initUDSServer();
    else
        initTCPServer();

    LOG(moose::info,  "Successfully initialized streamer socket: " << sockfd_);

    //  Listen for incoming clients. This function does nothing if connection is
    //  already made.
    listenToClients(1);
}

void SocketStreamer::configureSocketServer( )
{
    // One can set socket option using setsockopt function. See manual page
    // for details. We are making it 'reusable'.
    int on = 1;

#ifdef SO_REUSEPORT
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
        bzero(&sockAddrUDS_, sizeof(sockAddrUDS_));
        sockAddrUDS_.sun_family = AF_UNIX;
        strncpy(sockAddrUDS_.sun_path, sockInfo_.filepath.c_str(), sizeof(sockAddrUDS_.sun_path)-1);
        configureSocketServer();

        // Bind. Make sure bind is not std::bind
        if(0 > ::bind(sockfd_, (struct sockaddr*) &sockAddrUDS_, sizeof(sockAddrUDS_)))
        {
            isValid_ = false;
            LOG(moose::warning, "Warn: Failed to create socket at " << sockInfo_.filepath
                << ". File descriptor: " << sockfd_
                << ". Erorr: " << strerror(errno)
               );
        }

        if(! moose::filepath_exists(sockInfo_.filepath))
        {
            LOG( moose::warning, "No file " << sockInfo_.filepath << " exists." );
            isValid_ = false;
        }
    }

    if( (! isValid_) || (sockfd_ < 0) )
    {
        LOG( moose::warning, "Failed to create socket : " << sockInfo_ );
        ::unlink( sockInfo_.filepath.c_str() );
    }
}

void SocketStreamer::initTCPServer( void )
{
    // Create a blocking socket.
    LOG( moose::debug, "Creating TCP socket on port: "  << sockInfo_.port );
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
    sockAddrTCP_.sin_port = htons( sockInfo_.port );

    // Bind. Make sure bind is not std::bind
    if(0 > ::bind(sockfd_, (struct sockaddr*) &sockAddrTCP_, sizeof(sockAddrTCP_)))
    {
        isValid_ = false;
        LOG(moose::warning, "Warn: Failed to create server at "
                << sockInfo_.host << ":" << sockInfo_.port
                << ". File descriptor: " << sockfd_
                << ". Erorr: " << strerror(errno)
           );
        return;
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Convert data to JSON.
 *
 * @Returns JSON representation.
 */
/* ----------------------------------------------------------------------------*/
void SocketStreamer::dataToStream(map<string, vector<double>>& data)
{
    for( size_t i = 0; i < tables_.size(); i++)
    {
        vector<double> vec;
        tables_[i]->collectData(vec, true, false);
        if( ! vec.empty() )
            data[columns_[i+1]] = vec;
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
int SocketStreamer::streamData( )
{
    map<string, vector<double>> data;
    dataToStream(data);

    if(data.empty())
    {
        LOG(moose::debug, "No data in tables.");
        return 0;
    }

    // Construct a void* array to send over the socket. Serialize the data.
    // e.g. H 10 / a / t a b l e / a V 4 0.1 0.2 0.3 0.2
    // H => Header start (chars)
    // V => Value starts (double)
    for(auto v: data)
    {
        // First header.
        vecToStream_.push_back((double)'H');
        vecToStream_.push_back(v.first.size());
        addStringToDoubleVec(vecToStream_, v.first);

        // Now data.
        vecToStream_.push_back((double)'V');
        vecToStream_.push_back(v.second.size());
        vecToStream_.insert(vecToStream_.end(), v.second.begin(), v.second.end());
    }

    size_t dtypeSize = sizeof(double);
    int sent = send(clientfd_, (void*) &vecToStream_[0], dtypeSize*vecToStream_.size(), MSG_MORE);
    LOG(moose::debug, "Sent " << sent << " bytes." );
    if( sent < 0 )
        return errno;

    vecToStream_.erase(vecToStream_.begin(), vecToStream_.begin()+(sent/dtypeSize));
    return 0;
}


bool SocketStreamer::enoughDataToStream(size_t minsize)
{
    for( size_t i = 0; i < tables_.size(); i++)
        if(tables_[i]->getVec().size() >= minsize)
            return true;
    return false;
}

void SocketStreamer::connect( )
{
    // If server was invalid then there is no point.
    if( ! isValid_ )
        return;

    struct sockaddr_storage clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    clientfd_ = ::accept(sockfd_,(struct sockaddr*) &clientAddr, &addrLen);
    assert( clientfd_ );

    char ipstr[INET6_ADDRSTRLEN];
    int port = -1;
    // deal with both IPv4 and IPv6:
    if (clientAddr.ss_family == AF_INET)
    {
        struct sockaddr_in *s = (struct sockaddr_in *)&clientAddr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    }
    else
    { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&clientAddr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }

    LOG(moose::info, "Connected to " << ipstr << ':' << port);
    return;
}

void SocketStreamer::stream( void )
{
    if(clientfd_ > 0)
    {
        if( EPIPE == streamData() )
        {
            LOG( moose::warning, "Broken pipe. Couldn't stream." );
            return;
        }
    }
    else
        LOG( moose::warning, "No client." );
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
    processThread_ = std::thread(&SocketStreamer::connect, this);

    // NOw introduce some delay.
    timeStamp_ = std::chrono::high_resolution_clock::now();
}

/**
 * @brief This function is called at its clock tick.
 *
 * @param e
 * @param p
 */
void SocketStreamer::process(const Eref& e, ProcPtr p)
{
    // processTickMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(
            // std::chrono::high_resolution_clock::now() - timeStamp_).count();
    // timeStamp_ = std::chrono::high_resolution_clock::now();
    stream();
}

/**
 * @brief Add a table to streamer.
 *
 * @param table Id of table.
 */
void SocketStreamer::addTable( ObjId table )
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
void SocketStreamer::addTables( vector<ObjId> tables )
{
    if( tables.size() == 0 )
        return;

    for(auto t : tables)
        addTable(t);
}

/**
 * @brief Remove a table from SocketStreamer.
 *
 * @param table. Id of table.
 */
void SocketStreamer::removeTable( ObjId table )
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

/**
 * @brief Remove multiple tables -- if found -- from SocketStreamer.
 *
 * @param tables
 */
void SocketStreamer::removeTables( vector<ObjId> tables )
{
    for(auto &t: tables) removeTable(t);
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
    sockInfo_.port = port;
}

size_t SocketStreamer::getPort( void ) const
{
    return sockInfo_.port;
}

void SocketStreamer::setAddress( const string addr )
{
    sockInfo_.setAddress(addr);
}

string SocketStreamer::getAddress( void ) const
{
    return sockInfo_.address;
}
