/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2015 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPINE_H
#define _SPINE_H

class Neuron;

/**
 * The Spine is a dendritic spine, typically with 2 compartments, the
 * shaft and the head.
 * It is a FieldElement, so it gets all its values from the parent
 * Neuron.
 * Later we can put in a maturation profile, so that a linear number
 * from 0 to 1 will define all the stages of spine growth from stub
 * to filopodium to mature spine.
 */
class Spine
{
	public:
		Spine();
		Spine( const Neuron* parent );
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		Id getShaft( const Eref& e ) const;
		Id getHead( const Eref& e ) const;

		void setShaftLength( const Eref& e, double len );
		double getShaftLength( const Eref& e ) const;
		void setShaftDiameter( const Eref& e, double dia );
		double getShaftDiameter( const Eref& e ) const;
		void setHeadLength( const Eref& e, double len );
		double getHeadLength( const Eref& e ) const;
		void setHeadDiameter( const Eref& e, double dia );
		double getHeadDiameter( const Eref& e ) const;
		void setHeadVolume( const Eref& e, double vol );
		double getHeadVolume( const Eref& e ) const;
		void setPsdArea( const Eref& e, double area );
		double getPsdArea( const Eref& e ) const;

		void setTotalLength( const Eref& e, double len );
		double getTotalLength( const Eref& e ) const;

		// rotate around dend, but still at right angles to it.
		void setAngle( const Eref& e, double theta );
		double getAngle( const Eref& e ) const;

		// Incline to dend, radians. Default is normal to dend and is 0.
		void setInclination( const Eref& e, double phi );
		double getInclination( const Eref& e ) const;

		void setMinimumSize( const Eref& e, double len );
		double getMinimumSize( const Eref& e ) const;
		void setMaximumSize( const Eref& e, double len );
		double getMaximumSize( const Eref& e ) const;

		// Assign to specific vector. Length of vector does size scaling.
		//void setVectorDirection( const Eref& e, vector< double > dir );
		//vector< double > getVectorDirection( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		// Lookup funcs for Boundary
		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		const Neuron* parent_;
		/**
		 * Used as a sanity check for assigning dimensions, to avoid
		 * unreasonable physiological values.
		 * Defaults to 20 nanometers, which is somewhat smaller than the
		 * 30 nm size estimated for synaptic vesicles.
		 */
		double minimumSize_;
		double maximumSize_;
};

#endif	// _SPINE_H
