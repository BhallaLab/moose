/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPECIES_H
#define _SPECIES_H

/**
 * The species is a unique chemically distinct compound.
 * Examples are Ca2+, PKC, H2O
 * Each pool is associated with a chemical species.
 * Each molecule is associated with a species too.
 */
class Species
{
	public:
		Species();
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setMolWt( double v );
		double getMolWt() const;

		//////////////////////////////////////////////////////////////////
		// DestFinfo
		//////////////////////////////////////////////////////////////////

		void handleMolWtRequest( const Eref& e );

		//////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		double molWt_;
};

#endif	// _SPECIES_H
