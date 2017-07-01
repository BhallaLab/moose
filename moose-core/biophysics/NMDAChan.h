/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NMDACHAN_H
#define _NMDACHAN_H

/**
 * The NMDAChan incorporates calculations for Mg block, Nernst potential
 * and/or GHK estimates for the fraction of current carried by Ca.
 * These can also be done by messaging but it is messy and slow, so
 * I've lumped them. Also I've assumed that we're only dealing with the
 * GHK equation for Ca. This is not so good considering that other ions
 * also go through the channel.
 */
class NMDAChan: public SynChan
{
	public:
		NMDAChan();
		~NMDAChan();

		/////////////////////////////////////////////////////////////
		// Value field access function definitions
		/////////////////////////////////////////////////////////////

		// Mg-block fields
		void setKMg_A( double Gbar );
		double getKMg_A() const;
		void setKMg_B( double Ek );
		double getKMg_B() const;
		void setCMg( double CMg );
		double getCMg() const;

		// GHK fields
		void setTemperature( double temperature );
		double getTemperature() const;
		void setExtCa( double conc ); /// Set external conc
		double getExtCa() const;
		void setIntCa( double conc ); /// Set external conc
		double getIntCa() const;
		double getICa() const;
		void setPermeability( double permeability );
		double getPermeability() const;
		void setIntCaScale( double v );
		double getIntCaScale() const;
		void setIntCaOffset( double v );
		double getIntCaOffset() const;
		void setCondFraction( double v );
		double getCondFraction() const;

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );

		void assignIntCa( double v );
///////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		/// 1/eta
		double KMg_A_;
		/// 1/gamma
		double KMg_B_;
		/// [Mg] in mM
		double CMg_;
        /// Original Gk passed by NMDA channel, we keep this
        /// separate from Gk so that the computed Gk can be
        /// recorded without being overwritten by origChannel
        /// message.
		double temperature_;
		double Cout_; // External Ca concentration
		double Cin_; // Internal Ca concentration
		double intCaScale_; // Scale factor for assignIntCa msg.
		double intCaOffset_; // Offset for assignIntCa msg.
		double condFraction_; // Offset for assignIntCa msg.
		double ICa_; // Current carried by Ca ions.
		double const_; // Scaling factor used in GHK calculations, actually
					// not a const but depends on temperature
					// const_ = F*valency/RT
		static const double valency_; // Always +2.

///////////////////////////////////////////////////
// Utility function
///////////////////////////////////////////////////

};


#endif // _NMDACHAN_H
