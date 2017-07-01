/*
 * =====================================================================================
 *
 *       Filename:  cnpy.cpp
 *
 *    Description:  Write vector to numpy file.
 *
 *        Version:  1.0
 *        Created:  05/05/2016 11:58:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#include "cnpy.hpp"
#include <cstring>
#include "print_function.hpp"

using namespace std;

namespace cnpy2 {

// Check the endian-ness of machine at run-time. This is from library
// https://github.com/rogersce/cnpy
char BigEndianTest() {
    unsigned char x[] = {1,0};
    short y = *(short*) x;
    return y == 1 ? '<' : '>';
}

// And another function to convert given std datatype to numpy representation.
char map_type(const std::type_info& t)
{
    if(t == typeid(float) ) return 'f';
    if(t == typeid(double) ) return 'd';
    if(t == typeid(long double) ) return 'd';

    if(t == typeid(int) ) return 'i';
    if(t == typeid(char) ) return 'i';
    if(t == typeid(short) ) return 'i';
    if(t == typeid(long) ) return 'i';
    if(t == typeid(long long) ) return 'i';

    if(t == typeid(unsigned char) ) return 'u';
    if(t == typeid(unsigned short) ) return 'u';
    if(t == typeid(unsigned long) ) return 'u';
    if(t == typeid(unsigned long long) ) return 'u';
    if(t == typeid(unsigned int) ) return 'u';

    if(t == typeid(bool) ) return 'b';

    if(t == typeid(std::complex<float>) ) return 'c';
    if(t == typeid(std::complex<double>) ) return 'c';
    if(t == typeid(std::complex<long double>) ) return 'c';

    else return '?';
}

void split(vector<string>& strs, string& input, const string& pat)
{
    char* pch;
    pch = strtok( &input[0], pat.c_str() );
    while( pch != NULL )
    {
        strs.push_back( string(pch ) );
        pch = strtok( NULL, pat.c_str() );
    }
    delete pch;
}

/**
 * @brief Check if a numpy file is sane or not.
 *
 * Read first 8 bytes and compare with standard header.
 *
 * @param npy_file Path to file.
 *
 * @return  true if file is sane, else false.
 */
bool is_valid_numpy_file( FILE* fp )
{
    assert( fp );
    char buffer[__pre__size__];
    fread( buffer, sizeof(char), __pre__size__, fp );
    bool equal = true;
    // Check for equality
    for(size_t i = 0; i < __pre__size__; i++ )
        if( buffer[i] != __pre__[i] )
        {
            equal = false;
            break;
        }
    return equal;
}

/**
 * @brief Parser header from a numpy file. Store it in vector.
 *
 * @param header
 */
void parse_header( FILE* fp, string& header )
{
    // Read header, till we hit newline character.
    char ch;
    header.clear();
    while( ( ch = fgetc( fp )) != EOF )
    {
        if( '\n' == ch )
            break;
        header.push_back( ch );
    }
    assert( header.size() >= __pre__size__ );
}

/**
 * @brief Change shape in numpy header.
 *
 * @param
 * @param data_len
 * @param
 */
void change_shape_in_header( const string& filename
        , const size_t data_len, const size_t numcols
        )
{
    string header;

    // Always open file in r+b mode. a+b mode always append at the end.
    FILE* fp = fopen( filename.c_str(), "r+b" );
    if( ! fp )
    {
        moose::showWarn( "Failed to open " + filename );
        return;
    }

    parse_header( fp, header );

    size_t shapePos = header.find( "'shape':" );
    size_t lbrac = header.find( '(', shapePos );
    size_t rbrac = header.find( ')', lbrac );
    assert( lbrac > shapePos );
    assert( rbrac > lbrac );

    string prefixHeader = header.substr( 0, lbrac + 1 );
    string postfixHeader = header.substr( rbrac );

    string shapeStr = header.substr( lbrac + 1, rbrac - lbrac - 1);

    vector<string> tokens;
    split( tokens, shapeStr, "," );

    string newShape = "";
    for (size_t i = 0; i < tokens.size(); i++)
        newShape += moose::toString( atoi( tokens[i].c_str() ) + data_len/numcols ) + ",";

    string newHeader = prefixHeader + newShape + postfixHeader + "\n";
    if( newHeader.size() < header.size() )
    {
        cout << "Warn: Modified header can not be smaller than old header" << endl;
    }

    // Move to at the begining of file and write the new header.
    fseek(fp, 0, SEEK_SET);
    fwrite( newHeader.c_str(), sizeof(char), newHeader.size(), fp );
    fclose( fp );
}

}                                               /* Namespace cnpy2 ends. */
