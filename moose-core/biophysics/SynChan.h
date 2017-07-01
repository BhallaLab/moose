/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SynChan_h
#define _SynChan_h

class SynChan: public ChanCommon
{
	public:
		SynChan();
		~SynChan();

		/////////////////////////////////////////////////////////////////
		// Value field access function definitions
		/////////////////////////////////////////////////////////////////

		void setTau1( double tau1 );
		double getTau1() const;

		void setTau2( double tau2 );
		double getTau2() const;

		void setNormalizeWeights( bool value );
		bool getNormalizeWeights() const;

		// override virtual func from ChanBase
		void vSetGbar( const Eref& e, double Gbar );

		/////////////////////////////////////////////////////////////////
		// Utility function for any time Gbar changes
		void normalizeGbar();

		/// Utility function used to do the alpha function calculations for
		/// Gk.
		/// Separated out for convenience so that derived classes can use.
		double calcGk();
		/////////////////////////////////////////////////////////////////
		// ElementFinfo access function definitions
		/////////////////////////////////////////////////////////////////
		/*
		unsigned int getNumSynapses() const;
		void setNumSynapses( unsigned int i );
		Synapse* getSynapse( unsigned int i );
		*/

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

		void vProcess( const Eref& e, ProcPtr p );
		void vReinit( const Eref& e, ProcPtr p );

		void activation( double val );
///////////////////////////////////////////////////
		/**
		 * Override base class function for spike handling
		 */
		/* void innerAddSpike( unsigned int synIndex, const double time ); */

		static const Cinfo* initCinfo();
	protected: // Used by NMDAChan

///////////////////////////////////////////////////
// Utility function
///////////////////////////////////////////////////
    // virtual unsigned int updateNumSynapse( Eref e );

		double tau1_;
		double tau2_;
		int normalizeWeights_;
		double xconst1_;
		double yconst1_;
		double xconst2_;
		double yconst2_;
		double norm_;
		double activation_;
		double X_;
		double Y_;
		double dt_; /// Tracks the timestep assigned at reinit.
};


#endif // _SynChan_h
