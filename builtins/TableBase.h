/**********************************************************************
** This program is part of 'MOOSE',
** the Multi Object Oriented Simulation Environment.
**
**  Copyright (C) 2003-2016 Upinder S. Bhalla. and NCBS
**
** It is made available under the terms of the
** GNU Lesser General Public License version 3
**********************************************************************/

#ifndef _TABLE_BASE_H
#define _TABLE_BASE_H

/**
 * Base class for table operations. Provides basics for looking up table
 * and interpolation, but no process or messaging. Derived classes
 * deal with these.
 */
class TableBase
{
public:
    TableBase();

    /*-----------------------------------------------------------------------------
     *  Functions related to field assignment.
     *-----------------------------------------------------------------------------*/
    vector< double > getVec() const;
    const vector< double >& data( );

    void setVec( vector< double > val );

    double getOutputValue() const;
    void setOutputValue( double val );

    double getY( unsigned int index ) const;

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////
    void linearTransform( double scale, double offset );
    void xplot( string file, string plotname );
    void plainPlot( string file );
    void loadXplot( string fname, string plotname );

    void loadXplotRange(
            string fname, string plotname,
            unsigned int start, unsigned int end
            );

    void loadCSV( string fname, int startLine, int colNum, char separator );
    void compareXplot( string fname, string plotname, string op );
    void compareVec( vector< double > other, string op );
    void clearVec();

    //////////////////////////////////////////////////////////////////
    // Lookup funcs for table
    //////////////////////////////////////////////////////////////////
    double* lookupVec( unsigned int index );
    void setVecSize( unsigned int num );
    unsigned int getVecSize( ) const;
    double interpolate( double x, double xmin, double xmax ) const;

    static const Cinfo* initCinfo();

protected:
    vector< double >& vec();

private:
    double output_;
    vector< double > vec_;
};

#endif	// _TABLE_BASE_H
