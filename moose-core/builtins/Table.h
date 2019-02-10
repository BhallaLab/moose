/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _TABLE_H
#define _TABLE_H

#include <fstream>

/**
 * Receives and records inputs. Handles plot and spiking data in batch mode.
 */
class Table: public TableBase
{
public:
    Table();
    ~Table();

    Table& operator= ( const Table& tab );

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////

    void setThreshold ( double v );
    double getThreshold() const;

    void setFormat ( const string format );
    string getFormat( ) const;

    void setColumnName( const string colname );
    string getColumnName( ) const;

    void setUseStreamer ( bool status );
    bool getUseStreamer ( void ) const;

    void setUseSpikeMode ( bool status );
    bool getUseSpikeMode ( void ) const;

    void setOutfile ( string outfilepath );
    string getOutfile ( void ) const;

    // Access the dt_ of table.
    double getDt ( void ) const;

    // merge time value among values. e.g. t1, v1, t2, v2, etc.
    void mergeWithTime( vector<double>& data );

    string toJSON(bool withTime=true);

    void clearAllVecs();

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////

    void process ( const Eref& e, ProcPtr p );
    void reinit ( const Eref& e, ProcPtr p );

    void input ( double v );
    void spike ( double v );

    //////////////////////////////////////////////////////////////////
    // Lookup funcs for table
    //////////////////////////////////////////////////////////////////

    static const Cinfo* initCinfo();

private:

    double threshold_;
    double lastTime_;
    double input_;
    bool fired_;
    bool useSpikeMode_;

    vector<double> data_;
    vector<double> tvec_;                       /* time data */
    vector<string> columns_;                    /* Store the name of tables */

    string tablePath_;


    /**
     * @brief Column name of this table. Use it when writing data to a datafile.
     */
    string tableColumnName_;

    /**
     * @brief If stream is set to true, then stream to outfile_. Default value
     * of outfile_ is table path starting from `pwd`/_tables_ . On table, set
     * streamToFile to true.
     */
    bool useStreamer_;

    /**
     * @brief Table directory into which dump the stream data.
     */
    string rootdir_;

    // On Table, set outfile to change this variable. By default it sets to,
    // `pwd1/_tables_/table.path().
    string outfile_;

    /**
     * @brief Wheather or not outfile path is set by user
     */
    bool outfileIsSet_;

    /**
     * @brief format of data. Default to csv.
     */
    string format_;

    /**
     * @brief text_ to write.
     */
    string text_;

    /**
     * @brief dt of its clock. Needed for creating time co-ordinates,
     */
    double dt_;

    /**
     * @brief Output stream.
     */
    std::ofstream of_;

};

#endif	// _TABLE_H
