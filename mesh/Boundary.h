/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _BOUNDARY_H
#define _BOUNDARY_H

/**
 * manages geometries and optionally keeps track of adjacent compartments,
 * if any.
 */
class Boundary
{
	public:
		Boundary();

		void setReflectivity( const double v );
		double getReflectivity() const;

		static const Cinfo* initCinfo();
	private:
		/**
		 * Boundary condition. Reflective = 1. Completely diffusive = 0
		 * Unless it is completely reflective, there should be an adjacent
		 * compartment into which the molecules diffuse.
		 */
		double reflectivity_;
};

#endif	// _BOUNDARY_H
