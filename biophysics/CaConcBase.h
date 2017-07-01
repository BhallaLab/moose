/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CA_CONC_BASE_H
#define _CA_CONC_BASE_H

/**
 * This is the base class for CaConc. It handles calcium current influx,
 * conversion to calcium concentration, buffering, and pumps.
 * It obeys the equation:
 *
 * dC/dt = B*Ik - C/tau
 * where Ca = Ca_base + C.
 *
 * From the GENESIS notes:
 * In SI units, where concentration is moles/m^3
 * (milli-moles/liter) and current is in amperes, theory gives B
 * = 5.2e-6/(shell volume).  In practice, B is a parameter to be
 * fitted or estimated from experiment, as buffering, non-uniform
 * distribution of Ca, etc., will modify this value.  If thick =
 * 0, the readcell routine calculates B by dividing the "density"
 * parameter in the cell parameter file by the volume of the
 * compartment.  Otherwise, it scales as a true shell, with the
 * volume of a shell having thickness thick.  A negative value of
 * the "density" parameter may be used to indicate that it should
 * be taken as an absolute value of B, without scaling.
 *
 * This is a base class to provide the interface functions.
 */

class CaConcBase
{
	public:
		CaConcBase();
		///////////////////////////////////////////////////////////////
		// Message handling functions
		///////////////////////////////////////////////////////////////
		void reinit( const Eref&, ProcPtr info );
		void process( const Eref&, ProcPtr info );

		void current( const Eref& e, double I );
		void currentFraction( const Eref& e, double I, double fraction );
		void increase( const Eref& e, double I );
		void decrease( const Eref& e, double I );
		///////////////////////////////////////////////////////////////
		// Virtual Message handling functions
		///////////////////////////////////////////////////////////////
		virtual void vReinit( const Eref&, ProcPtr info ) = 0;
		virtual void vProcess( const Eref&, ProcPtr info ) = 0;

		virtual void vCurrent( const Eref& e, double I ) = 0;
		virtual void vCurrentFraction( const Eref& e, double I, double fraction ) = 0;
		virtual void vIncrease( const Eref& e, double I ) = 0;
		virtual void vDecrease( const Eref& e, double I ) = 0;
		///////////////////////////////////////////////////////////////
		// Field handling functions
		///////////////////////////////////////////////////////////////
		void setCa( const Eref& e, double val );
		double getCa( const Eref& e ) const;
		void setCaBasal( const Eref& e, double val );
		double getCaBasal( const Eref& e ) const;
		void setTau( const Eref& e, double val );
		double getTau( const Eref& e ) const;
		void setB( const Eref& e, double val );
		double getB( const Eref& e ) const;
        void setCeiling( const Eref& e, double val );
        double getCeiling( const Eref& e ) const;
        void setFloor( const Eref& e, double val );
        double getFloor( const Eref& e ) const;
        void setThickness( const Eref& e, double val );
        double getThickness( const Eref& e ) const;
        void setLength( const Eref& e, double val );
        double getLength( const Eref& e ) const;
        void setDiameter( const Eref& e, double val );
        double getDiameter( const Eref& e ) const;
		///////////////////////////////////////////////////////////////
		// Virtual Field handling functions
		///////////////////////////////////////////////////////////////
		virtual void vSetCa( const Eref& e, double val ) = 0;
		virtual double vGetCa( const Eref& e ) const = 0;
		virtual void vSetCaBasal( const Eref& e, double val ) = 0;
		virtual double vGetCaBasal( const Eref& e ) const = 0;
		virtual void vSetTau( const Eref& e, double val ) = 0;
		virtual double vGetTau( const Eref& e ) const = 0;
		virtual void vSetB( const Eref& e, double val ) = 0;
		virtual double vGetB( const Eref& e ) const = 0;
        virtual void vSetCeiling( const Eref& e, double val ) = 0;
        virtual double vGetCeiling( const Eref& e ) const = 0;
        virtual void vSetFloor( const Eref& e, double val ) = 0;
        virtual double vGetFloor( const Eref& e ) const = 0;

		///////////////////////////////////////////////////////////////
		// Utility function in case length, dia or thickness is updated
		void updateDimensions( const Eref& e );

		/// Used to set up the solver. Dummy for regular classes.
		virtual void vSetSolver( const Eref& e, Id hsolve );

		/**
		 * Swaps Cinfos in order to make Zombies.
		 */
		static void zombify( Element* orig, const Cinfo* zClass,
						Id hsolve );

		/*
		 * This Finfo is used to send out Ca concentration to channels.
		 *
		 * It is exposed here so that HSolve can also use it to send out
		 * Ca concentration to the recipients.
		 */
		static SrcFinfo1< double >* concOut();

		static const Cinfo* initCinfo();
	private:
		double thickness_;
		double diameter_;
		double length_;
};


#endif // _CA_CONC_BASE_H
