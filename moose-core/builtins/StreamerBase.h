/***
 *       Filename:  StreamerBaseBase.h
 *
 *    Description:  Stream table data to a  stream.
 *
 *        Created:  2016-04-26
 *
 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *
 *        License:  GNU GPL2
 */

#ifndef  StreamerBase_INC
#define  StreamerBase_INC

#define STRINGSTREAM_DOUBLE_PRECISION       10

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include "TableBase.h"

using namespace std;

class TableBase;

class StreamerBase : public TableBase
{

public:
    StreamerBase();
    ~StreamerBase();

    StreamerBase& operator=( const StreamerBase& st );

    /* Functions to set and get Streamer fields */
    void setOutFilepath( string stream );
    string getOutFilepath() const;

    /** @brief Write to a output file in given format.
     *
     * @param filepath, path of
     * output file. If parent directories do not exist, they will be created. If
     * creation fails for some reason, data will be saved in current working
     * directory. The name of the file will be computed from the given directory
     * name by replacing '/' or '\' by '_'.
     *
     * @param format
     *
     *  npy : numpy binary format (version 1 and 2), version 1 is default.
     *  csv or dat: comma separated value (delimiter ' ' )
     *
     * @param  openmode (write or append)
     *
     * @param  data, vector of values
     *
     * @param ncols (number of columns). Incoming data will be formatted into a
     * matrix with ncols.
     */
    static void writeToOutFile(
            const string& filepath, const string& format
            , const string& openmode
            , const vector<double>& data
            , const vector<string>& columns
            );

    /**
     * @brief Write data to csv file. See the documentation of writeToOutfile
     * for details.
     */
    static void writeToCSVFile( const string& filepath, const string& openmode
            , const vector<double>& data, const vector<string>& columns
            );

    /**
     * @brief  Write to NPY format. See the documentation of
     * writeToOutfile for more details.
     */
    static void writeToNPYFile( const string& filepath, const string& openmode
            , const vector<double>& data
            , const vector<string>& columns
            );


private:

    string outfilePath_;

    static const char eol = '\n';
    static const char delimiter_ = ' ';

};

#endif   /* ----- #ifndef StreamerBase_INC  ----- */
