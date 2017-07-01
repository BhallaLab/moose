/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _Interpol2D_h
#define _Interpol2D_h

/**
 * 2 Dimensional table, with interpolation. The internal vector is accessed like
 * this: table_[ xIndex ][ yIndex ], with the x- and y-coordinates used as the
 * first and second indices respectively.
 */
class Interpol2D
{
	public:
		Interpol2D();
		Interpol2D(
			unsigned int xdivs, double xmin, double xmax,
			unsigned int ydivs, double ymin, double ymax );

		////////////////////////////////////////////////////////////
		// Here are the interface functions for the MOOSE class
		////////////////////////////////////////////////////////////
		void setXmin( double value );
		double getXmin() const;
		void setXmax( double value );
		double getXmax() const;
		void setXdivs( unsigned int value );
		unsigned int getXdivs() const;
		void setDx( double value );
		double getDx() const;

		void setYmin( double value );
		double getYmin() const;
		void setYmax( double value );
		double getYmax() const;
		void setYdivs( unsigned int value );
		unsigned int getYdivs() const;
		void setDy( double value );
		double getDy() const;

		void setSy( double value );
		double getSy() const;

        double getInterpolatedValue(vector<double> xy) const;

		////////////////////////////////////////////////////////////
		// Here are the Interpol2D Destination functions
		////////////////////////////////////////////////////////////
		/**
		 * Looks up table value based on indices v1 and v2, and sends value
		 * back on the 'lookupOut' message.
		 */
		void lookupReturn( const Eref& e, double v1, double v2 );
		void lookup( double v1, double v2 );

		void appendTableVector( vector< vector< double > > value );

		////////////////////////////////////////////////////////////
		// Here are the internal functions
		////////////////////////////////////////////////////////////
		double interpolate( double x, double y ) const;
		double indexWithoutCheck( double x, double y ) const;
		double innerLookup( double x, double y ) const;
		bool operator==( const Interpol2D& other ) const;
		bool operator<( const Interpol2D& other ) const;

		//Overloading the >> operator to allow use in OpFunc.
		friend istream& operator>>( istream&, Interpol2D& );

		double invDy() const {
			return invDy_;
		}

		void setTableValue( vector< unsigned int > index, double value );
		double getTableValue( vector< unsigned int > index ) const;

		void setTableVector( vector< vector< double > > value );
		vector< vector < double > > getTableVector() const;
		void appendTableVector(
			const vector< vector< double > >& value );

		/**
		 * Resizes 2-D vector. If either argument is zero, it remains
		 * unchanged
		 */
		void resize( unsigned int xsize, unsigned int ysize, double init = 0.0  );

		unsigned int xdivs() const;
		unsigned int ydivs() const;

		void print( const string& fname, bool doAppend ) const;
		void load( const string& fname, unsigned int skiplines );

		static const Cinfo* initCinfo();
		static const unsigned int MAX_DIVS;
	private:
		double xmin_;
		double xmax_;
		double invDx_;
		double ymin_;
		double ymax_;
		double invDy_;
		double sy_;
		vector< vector< double > > table_;
};


#endif // _Interpol2D_h
