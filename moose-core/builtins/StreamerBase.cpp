/***
 *       Filename:  StreamerBase.cpp
 *
 *    Description:  Stream table data.
 *
 *        Version:  0.0.1
 *        Created:  2016-04-26

 *       Revision:  none
 *
 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *
 *        License:  GNU GPL2
 */


#include "global.h"
#include "header.h"
#include "StreamerBase.h"

#include "../scheduling/Clock.h"
#include "../utility/cnpy.hpp"

#include <algorithm>
#include <sstream>
#include <memory>

// Class function definitions
StreamerBase::StreamerBase()
{
}

StreamerBase& StreamerBase::operator=( const StreamerBase& st )
{
    this->outfilePath_ = st.outfilePath_;
    return *this;
}


StreamerBase::~StreamerBase()
{

}


string StreamerBase::getOutFilepath( void ) const
{
    return outfilePath_;
}

void StreamerBase::setOutFilepath( string filepath )
{
    outfilePath_ = filepath;
}


void StreamerBase::writeToOutFile( const string& filepath
        , const string& outputFormat
        , const string& openmode
        , const vector<double>& data
        , const vector<string>& columns
        )
{
    if( data.size() == 0 )
        return;

    if( "npy" == outputFormat )
        writeToNPYFile( filepath, openmode, data, columns );
    else if( "csv" == outputFormat or "dat" == outputFormat )
        writeToCSVFile( filepath, openmode, data, columns );
    else
    {
        LOG( moose::warning, "Unsupported format " << outputFormat
                << ". Use npy or csv. Falling back to default csv"
           );
        writeToCSVFile( filepath, openmode, data, columns );
    }
}


/*  Write to a csv file.  */
void StreamerBase::writeToCSVFile( const string& filepath, const string& openmode
        , const vector<double>& data, const vector<string>& columns )
{

    FILE* fp = fopen( filepath.c_str(), openmode.c_str() );
    if( NULL == fp )
    {
        LOG( moose::warning, "Failed to open " << filepath );
        return;
    }

    // If writing in "w" mode, write the header first.
    if( openmode == "w" )
    {
        string headerText = "";
        for( vector<string>::const_iterator it = columns.begin();
            it != columns.end(); it++ )
            headerText += ( *it + delimiter_ );
        headerText += eol;
        fprintf( fp, "%s", headerText.c_str() );
    }

    string text = "";
    for( size_t i = 0; i < data.size(); i+=columns.size() )
    {
        // Start of a new row.
        for( size_t ii = 0; ii < columns.size(); ii++ )
            text += moose::toString( data[i+ii] ) + delimiter_;

        // At the end of each row, we remove the delimiter_ and append newline_.
        *(text.end()-1) = eol;
    }
    fprintf( fp, "%s", text.c_str() );
    fclose( fp );
}

/*  write data to a numpy file */
void StreamerBase::writeToNPYFile( const string& filepath, const string& openmode
        , const vector<double>& data, const vector<string>& columns )
{
    cnpy2::save_numpy<double>( filepath, data, columns, openmode );
}

