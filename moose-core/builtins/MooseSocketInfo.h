/*
 *    Description:  Utility class for handling socket.
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 */

#ifndef MOOSESOCKETINFO_H
#define MOOSESOCKETINFO_H

typedef enum t_socket_type_ {TCP_SOCKET, UNIX_DOMAIN_SOCKET} SocketType; // Type of socket.

class MooseSocketInfo
{
public:
    MooseSocketInfo( const string& addr = "" )
        : address(addr), valid(false)
    {
        if( addr.size() > 0 )
            init();
    }

    ~MooseSocketInfo() { ; }

    void setAddress( const string addr )
    {
        address = addr;
        init();
    }

    void init( void )
    {
        if( "file://" == address.substr(0, 7))
        {
            type = UNIX_DOMAIN_SOCKET;
            filepath = address.substr(7);
            valid = true;
        }
        else if( "http://" == address.substr(0, 7))
        {
            type = TCP_SOCKET;
            auto colPos = address.find_last_of(':');
            if( colPos == string::npos )
            {
                port = 0;
                host = address;
            }
            else
            {
                host = address.substr(0, colPos);
                port = std::stoi(address.substr(colPos+1));
            }
            valid = true;
        }
        else if( '/' == address[0] )
        {
            type = UNIX_DOMAIN_SOCKET;
            filepath = address;
            valid = true;
        }
    }

    // C++ is so stupid: https://stackoverflow.com/q/476272/1805129
    friend std::ostream& operator<<(ostream& os, const MooseSocketInfo& info)
    {
        os << "ADDRESS= " << info.address << " TYPE= " << info.type;
        if( info.type == UNIX_DOMAIN_SOCKET)
            os << ". FILEPATH " << info.filepath;
        else
            os << ". HOST:PORT=" << info.host << ":" << info.port;
        return os;
    }

public:
    SocketType type;
    string address;
    string filepath;
    string host;
    bool valid;
    size_t port;
};


#endif /* end of include guard: MOOSESOCKETINFO_H */
