/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment.
 **           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/
#ifndef _CONV_H
#define _CONV_H

#include <cstring>

/**
 * This set of templates defines converters. The conversions are from
 * string to object, object to string, and
 * binary buffer to object, object to binary buffer.
 * Many classes convert through a single template. Strings and other things
 * with additional data need special converters.
 * The key point is that the serialized form (buffer or output string)
 * must have a complete independent copy of the data. So if a pointer or
 * reference type is converted, then there must be a complete copy made.
 *
 * Conv is used only during while the memory for the argument is stable.
 * You can't create a Conv object from a variable, and free the variable
 * before using the Conv.
 */

static const unsigned int UnreasonablyLargeArray = 1000000;
template< class T > class Conv
{
    public:
        /**
         * Size, in units of sizeof(double). So a double would be 1,
         * something with 1 chars would be 1 as well, but something
         * with 9 chars would be 2.
         */
        static unsigned int size( const T& val )
        {
            return 1 + ( sizeof( T ) - 1 ) / sizeof( double );
        }


        static const T& buf2val( double** buf ) {
            const T* ret = reinterpret_cast< const T* >( *buf );
            *buf += size( *ret );
            return *ret;
        }

        /**
         * Converts data contents into double* buf. Buf must be allocated
         * ahead of time. Returns size of value.
         * Needs to be specialized for variable size and pointer-containing
         * types T.
         */
        static void val2buf( const T& val, double** buf ) {
            *reinterpret_cast< T* >( *buf ) = val;
            *buf += size( val );
        }

        /**
         * Default conversion from string just puts the string
         * into the char* pointer.
         */
        static void str2val( T& val, const string& s ) {
            istringstream is( s );
            is >> val;
        }

        /**
         * Default conversion into string just puts the char* representation
         * into the string. Arguably a bad way to do it.
         */
        static void val2str( string& s, const T& val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
            // ostringstream os( s );
            // os << val;
        }

        static string rttiType() {
            if ( typeid( T ) == typeid( char ))
                return "char";
            if ( typeid( T ) == typeid( int ) )
                return "int";
            if ( typeid( T ) == typeid( short ) )
                return "short";
            if ( typeid( T ) == typeid( long ) )
                return "long";
            if ( typeid( T ) == typeid( unsigned int ) )
                return "unsigned int";
            if ( typeid( T ) == typeid( unsigned long ) )
                return "unsigned long";
            if ( typeid( T ) == typeid( float ) )
                return "float";
            if ( typeid( T ) == typeid( double ) )
                return "double";
            if ( typeid( T ) == typeid( Id ) )
                return "Id";
            if ( typeid( T ) == typeid( ObjId ) )
                return "ObjId";
            return typeid( T ).name(); // this is not portable but may be more useful than "bad"
        }

    private:
};

/**
 * This stores the data as the equivalent of a char* string, terminated with
 * the usual \0, but allocates it as a double[].
 */
template<> class Conv< string >
{
    public:
        /**
         * This is the size used in the serialized form, as a double*
         * Note that we do some ugly stuff to get alignment on 8-byte
         * boundaries.
         * We need to have strlen + 1 as a minimum.
         */
        static unsigned int size( const string& val )
        {
            // extra char for termination of the string.
            return 1 + val.length() / sizeof( double );
        }

        // The return cannot be a reference, because the function may
        // be called many times in succession.
        static const string buf2val( double** buf ) {
            static string ret;
            ret = reinterpret_cast< const char* >( *buf );
            *buf += size( ret );
            return ret;
        }

        /**
         * Converts data contents into double* buf. Buf must be allocated
         * ahead of time. Returns size of value.
         * Needs to be specialized for variable size and pointer-containing
         * types T.
         */
        static void val2buf( const string& val, double** buf ) {
            char* temp = reinterpret_cast< char* >( *buf );
            strcpy( temp, val.c_str() );
            *buf += size( val );
        }

        static void str2val( string& val, const string& s ) {
            val = s;
        }

        static void val2str( string& s, const string& val ) {
            s = val;
        }

        static string rttiType() {
            return "string";
        }
    private:
};

/**
 * The template specialization of Conv< unsigned int > sets up alignment on
 * word boundaries by storing the data as a double.
 */
template<> class Conv< double >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( double val )
        {
            return 1;
        }

        static const double buf2val( double** buf ) {
            double ret = **buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( double val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( double &val, const string& s ) {
            val = atof( s.c_str() );
        }

        static void val2str( string& s, double val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "double";
        }
    private:
};

/**
 * The template specialization of Conv< unsigned int > sets up alignment on
 * word boundaries by storing the data as a double.
 */
template<> class Conv< float >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( float val )
        {
            return 1;
        }

        static const float buf2val( double** buf ) {
            float ret = **buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( float val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( float& val, const string& s ) {
            val = atof( s.c_str() );
        }

        static void val2str( string& s, float val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "float";
        }
    private:
};

/**
 * The template specialization of Conv< unsigned int > sets up alignment on
 * word boundaries by storing the data as a double.
 */
template<> class Conv< unsigned int >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( int val )
        {
            return 1;
        }

        static const unsigned int buf2val( double** buf ) {
            unsigned int ret = (unsigned int)**buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( unsigned int val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( unsigned int& val, const string& s ) {
            val = atoi( s.c_str() );
        }

        static void val2str( string& s, unsigned int val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "unsigned int";
        }
    private:
};

/**
 * The template specialization of Conv< int > sets up alignment on
 * word boundaries by storing the data as a double.
 */
template<> class Conv< int >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( int val )
        {
            return 1;
        }

        static const int buf2val( double** buf ) {
            int ret = (int)**buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( int val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( int& val, const string& s ) {
            val = atoi( s.c_str() );
        }

        static void val2str( string& s, int val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "int";
        }
    private:
};

template<> class Conv< unsigned short >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( unsigned short val )
        {
            return 1;
        }

        static const unsigned short buf2val( double** buf ) {
            unsigned short ret = (unsigned int)**buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( unsigned short val, double** buf ) {
            **buf = (double)val;
            (*buf)++;
        }

        static void str2val( unsigned short& val, const string& s ) {
            val = atoi( s.c_str() );
        }

        static void val2str( string& s, unsigned short val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "unsigned short";
        }
    private:
};

template<> class Conv< short >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( short val )
        {
            return 1;
        }

        static const short buf2val( double** buf ) {
            short ret = (short)**buf;
            (*buf)++;
            return ret;
        }
        static void val2buf( short val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( short& val, const string& s ) {
            val = atoi( s.c_str() );
        }

        static void val2str( string& s, short val ) {
            stringstream ss;
            ss << val;
            s = ss.str();
        }

        static string rttiType() {
            return "short";
        }
    private:
};

template<> class Conv< bool >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( bool val )
        {
            return 1;
        }

        static const bool buf2val( double** buf ) {
            bool ret = (**buf > 0.5);
            (*buf)++;
            return ret;
        }
        static void val2buf( bool val, double** buf ) {
            **buf = val;
            (*buf)++;
        }

        static void str2val( bool& val, const string& s ) {
            if ( s == "0" || s == "false" || s == "False" )
                val = 0;
            else
                val = 1;
        }

        static void val2str( string& s, bool val ) {
            if ( val > 0.5 )
                s = "1";
            else
                s = "0";
        }

        static string rttiType() {
            return "bool";
        }
    private:
};

/**
 * The template specialization of Conv< Id > sets up alignment on
 * word boundaries by storing the Id as a double. It also deals with
 * the string conversion issues.
 */

template<> class Conv< Id >
{
    public:
        /**
         * This is the size used in the serialized form.
         */
        static unsigned int size( Id val )
        {
            return 1;
        }

        static const Id buf2val( double** buf ) {
            Id ret( (unsigned int)**buf );
            (*buf)++;
            return ret;
        }
        static void val2buf( Id id, double** buf ) {
            **buf = id.value();
            (*buf)++;
        }

        static void str2val( Id& val, const string& s ) {
            Id temp( s ); // converts the path
            val = temp;
        }

        static void val2str( string& s, Id val ) {
            s = val.path();
        }

        static string rttiType() {
            return "Id";
        }
    private:
};

/**
 * Still more specialized partial specialization.
 * This works with any field that has a uniform size.
 * Assume strings are the only exception.
 * The first double in the vector vec_ holds the # of data entries that
 * follow it. This excludes the space for the size entry itself.
 * This can be a ragged array: the number of entries in each vector need
 * not be the same.
 * The order of entries is:
 * 0		:  Dimension of left index, can be zero. As double.
 * 1:numDim	:  Dimensions of right index. Any and all can be zero. Double.
 * numDim+1 to total: Data, in condensed format.
 */
template< class T > class Conv< vector< vector< T > > >
{
    public:
        static unsigned int size( const vector< vector < T > > & val)
        {
            unsigned int ret = 1 + val.size();
            for ( unsigned int i = 0; i < val.size(); ++i ) {
                if ( val[i].size() > 0 ) {
                    ret += val[i].size() * Conv< T >::size( val[i][0] );
                } else  {
                    T temp = T();
                    ret += val[i].size() * Conv< T >::size( temp );
                }
            }
            return ret;
        }

        static const vector< vector< T > > buf2val( double** buf )
        {
            static vector< vector< T > > ret;
            ret.clear();
            unsigned int numEntries = **buf; // first entry is vec size
            ret.resize( numEntries );
            (*buf)++;
            for ( unsigned int i = 0; i < numEntries; ++i ) {
                unsigned int rowSize = **buf;
                (*buf)++;
                for ( unsigned int j = 0; j < rowSize; ++j )
                    ret[i].push_back( Conv< T >::buf2val( buf ) );
            }
            return ret;
        }

        static void val2buf( const vector< vector< T > >& val, double**buf )
        {
            double* temp = *buf;
            *temp++ = val.size();
            for( unsigned int i = 0; i < val.size(); ++i ) {
                *temp++ = val[i].size();
                for ( unsigned int j = 0; j < val[i].size(); ++j ) {
                    Conv< T >::val2buf( val[i][j], &temp );
                }
            }
            *buf = temp;
        }

        static void str2val( vector< vector< T > >& val, const string& s ) {
            cout << "Specialized Conv< vector< vector< T > > >::str2val not done\n";
        }

        static void val2str( string& s, const vector< vector< T > >& val ) {
            cout << "Specialized Conv< vector< vector< T > > >::val2str not done\n";
        }
        static string rttiType() {
            string ret = "vector< vector<" + Conv< T >::rttiType() + "> >";
            return ret;
        }
    private:
};

/**
 * Trying to do a partial specialization.
 * This works with anything that has a uniform size.
 * Assume strings are the only exception.
 * The first double in the vector vec_ holds the # of data entries that
 * follow it. This excludes the space for the size entry itself.
 */
template< class T > class Conv< vector< T > >
{
    public:
        /**
         * Size of returned array in doubles.
         */
        static unsigned int size( const vector< T >& val )
        {
            unsigned int ret = 1;
            for ( unsigned int i = 0; i < val.size(); ++i ) {
                ret += Conv< T >::size( val[i] );
            }
            return ret;
        }

        static const vector< T > buf2val( double** buf )
        {
            static vector< T > ret;
            ret.clear();
            unsigned int numEntries = (unsigned int)**buf; // first entry is vec size
            (*buf)++;
            for ( unsigned int i = 0; i < numEntries; ++i )
                ret.push_back( Conv< T >::buf2val( buf ) );
            return ret;
        }

        static void val2buf( const vector< T >& val, double**buf )
        {
            double* temp = *buf;
            *temp++ = val.size();
            for( unsigned int i = 0; i < val.size(); ++i ) {
                Conv< T >::val2buf( val[i], &temp );
            }
            *buf = temp;
        }

        static void str2val( vector< T >& val, const string& s ) {
            cout << "Specialized Conv< vector< T > >::str2val not done\n";
        }

        static void val2str( string& s, const vector< T >& val ) {
            cout << "Specialized Conv< vector< T > >::val2str not done\n";
        }
        static string rttiType() {
            string ret = "vector<" + Conv< T >::rttiType() + ">";
            return ret;
        }
    private:
};

#endif // _CONV_H
