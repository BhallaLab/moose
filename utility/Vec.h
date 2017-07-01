/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _VEC_H
#define _VEC_H

class Vec {
	public:
		Vec( double a0, double a1, double a2 );
		Vec()
			: a0_( 0.0 ), a1_( 0.0 ), a2_( 0.0 )
		{;}

		double length() const;

		double distance( const Vec& other ) const;

		double dotProduct( const Vec& other ) const;

		Vec crossProduct( const Vec& other ) const;

		/// Rescales vector so it has unit length.
		void unitLength();

		/// Generates vectors u and v to form a mutually orthogonal system.
		void orthogonalAxes( Vec& u, Vec& v ) const;

		double a0() const {
				return a0_;
		}
		double a1() const {
				return a1_;
		}
		double a2() const {
				return a2_;
		}

		/**
		 * Returns a point on the line from self to 'end', a fraction k
		 * from the 'self'.
		 * r = self + k.(end - self)
		 */
		Vec pointOnLine( const Vec& end, double k );

		bool operator==( const Vec& other ) const;

		Vec operator-( const Vec& other ) const;

		Vec operator+( const Vec& other ) const;

		Vec operator*( const double& other ) const;

	private:
		double a0_;
		double a1_;
	   	double a2_;
};

#endif // _VEC_H
