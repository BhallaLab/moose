/*
 * =====================================================================================
 *
 *       Filename:  cnpy.h
 *
 *    Description:  Write a stl vector to numpy format 2.
 *
 *      This program is part of MOOSE simulator.
 *
 *        Version:  1.0
 *        Created:  05/04/2016 10:36:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 *
 * =====================================================================================
 */

#ifndef  cnpy_INC
#define  cnpy_INC

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <complex>
#include <typeinfo>

#ifdef  ENABLE_CPP11
#include <memory>
#include <array>
#else      /* -----  not ENABLE_CPP11  ----- */
#endif     /* -----  not ENABLE_CPP11  ----- */

#include <string>

#include <stdint.h>

#include "global.h"

#include "../utility/print_function.hpp"


using namespace std;

namespace cnpy2 {

// Check the endian-ness of machine at run-time. This is from library
// https://github.com/rogersce/cnpy
char BigEndianTest();

// And another function to convert given std datatype to numpy representation.
char map_type(const std::type_info& t);

void split(vector<string>& strs, string& input, const string& pat);

/**
 * @brief Check if a numpy file is sane or not.
 *
 * Read first 8 bytes and compare with standard header.
 *
 * @param npy_file Path to file.
 *
 * @return  true if file is sane, else false.
 */
bool is_valid_numpy_file( FILE* fp );

/**
 * @brief Parser header from a numpy file. Store it in vector.
 *
 * @param header
 */
void parse_header( FILE* fp, string& header );

/**
 * @brief Change shape in numpy header.
 *
 * @param
 * @param data_len
 * @param
 */
void change_shape_in_header( const string& filename
        , const size_t data_len, const size_t numcols
        );

static const unsigned int __pre__size__ = 8;
static char __pre__[__pre__size__] = {
    (char)0x93, 'N', 'U', 'M', 'P', 'Y'         /* Magic */
        , (char)0x01, (char) 0x00               /* format */
};

template< typename T>
void write_header(
        FILE* fp
        , const vector<string>& colnames
        , vector<unsigned int>shape ,  char version
        )
{
    // Heder are always at the begining of file.
    fseek( fp, 0, SEEK_SET );
    char endianChar = cnpy2::BigEndianTest();
    char formatChar = cnpy2::map_type( typeid(T) );

    string dict = ""; // This is the header to numpy file
    dict += "{'descr': [";
    for( vector<string>::const_iterator it = colnames.begin();
            it != colnames.end(); it++ )
        dict += "('" + *it + "' , '" + endianChar + formatChar + "'),";

    dict += "], 'fortran_order': False, 'shape': (";
    dict += moose::toString(shape[0]);
    for(size_t i = 1; i < shape.size(); i++)
    {
        dict += ",";
        dict += moose::toString(shape[i]);
    }
    if( shape.size() == 1) dict += ",";
    dict += "), }";

    // When appending to this file, we need to rewrite header. Size of header
    // might change since shape values might change. Minimum shape from (1,)
    // could become quite large (13132131321,) etc. For safety region, append a
    // chunk of whitespace characters so that overwriting header does not
    // overwrite the data. This trick would save us from copying data into
    // memory.
    dict += string(11, ' ');                    /* 32 bit number is fit */

    // pad with spaces so that preamble+headerlen+dict is modulo 16 bytes.
    // preamble is 8 bytes, header len is 4 bytes, total 12.
    // dict needs to end with \n
    unsigned int remainder = 16 - (12 + dict.size()) % 16;
    dict.insert(dict.end(),remainder,' ');
    *(dict.end()-1) = '\n';

    if( version == '2' )
        __pre__[6] = (char) 0x02;

    fwrite( __pre__, sizeof( char ), __pre__size__, fp );

    // Now write the size of dict. It is 2bytes long in version 1 and 4 bytes
    // long in version 2.
    if( version == '2' )
    {
        uint32_t s = dict.size();
        fwrite( (char*)&s, sizeof( uint32_t ), 1, fp );
    }
    else
    {
        int16_t s = dict.size();
        fwrite( (char*)&s, sizeof( uint16_t ), 1, fp );
    }
    fwrite( dict.c_str(), sizeof(char), dict.size(), fp );
}

// write to version 1 or version 2.
template<typename T>
void save_numpy(
        const string& outfile
        , const vector<double>& vec
        , vector<string> colnames
        , const string openmode
        , const char version = '1'
        )
{

    // In our application, we need to write a vector as matrix. We do not
    // support the stacking of matrices.
    vector<unsigned int> shape;

    if( colnames.size() == 0)
        return;

    shape.push_back( vec.size() / colnames.size());

    /* In mode "w", open the file and write a header as well. When file is open
     * in mode "a", we assume that file is alreay a valid numpy file.
     */
    if( openmode == "w" )
    {
        FILE* fp = fopen( outfile.c_str(), "wb" );
        if( NULL == fp )
        {
            moose::showWarn( "Could not open file " + outfile );
            return;
        }
        write_header<T>( fp, colnames, shape, version );
        fclose( fp );
    }
    else                                        /* Append mode. */
    {
        // Do a sanity check if file is really a numpy file.
        FILE* fp = fopen( outfile.c_str(), "r" );
        if( ! fp )
        {
            moose::showError( "Can't open " + outfile + " to validate" );
            return;
        }
        else if(! is_valid_numpy_file( fp ) )
        {
            moose::showWarn( outfile + " is not a valid numpy file"
                    + " I am not goind to write to it"
                    );
            return;
        }
        if( fp )
            fclose( fp );
        // And change the shape in header.
        change_shape_in_header( outfile, vec.size(), colnames.size() );
    }

    FILE* fp = fopen( outfile.c_str(), "ab" );
    if( NULL == fp )
    {
        moose::showWarn( "Could not open " + outfile + " to write " );
        return;
    }
    fwrite( &vec[0], sizeof(T), vec.size(), fp );
    fclose( fp );

}


}                                               /* Namespace cnpy2 ends. */
#endif   /* ----- #ifndef cnpy_INC  ----- */
