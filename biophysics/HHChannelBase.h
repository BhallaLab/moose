#ifndef _HHChannelBase_h
#define _HHChannelBase_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
*********************************************************************
*/

typedef double ( *PFDD )( double, double );

/**
 * The HHChannelBase is the base class for defining Hodgkin-Huxley type
 * channels, specifically dealing with derivatives used in the HSolver.
 * This is derived from the ChanBase rather than the ChanCommon, since
 * the Zombie classes used in the HSolver will not use the ChanCommon
 * fields.
 */

class HHChannelBase: public virtual ChanBase
{
	public:
		HHChannelBase();
		~HHChannelBase();

		/////////////////////////////////////////////////////////////
		// Value field access function definitions
		/////////////////////////////////////////////////////////////

		void setXpower( const Eref& e, double Xpower );
		double getXpower( const Eref& e) const;
		void setYpower( const Eref& e, double Ypower );
		double getYpower( const Eref& e) const;
		void setZpower( const Eref& e, double Zpower );
		double getZpower( const Eref& e) const;
		void setInstant( const Eref& e, int Instant );
		int getInstant( const Eref& e ) const;
		void setX( const Eref& e, double X );
		double getX( const Eref& e ) const;
		void setY( const Eref& e, double Y );
		double getY( const Eref& e ) const;
		void setZ( const Eref& e, double Z );
		double getZ( const Eref& e ) const;
		void setUseConcentration( const Eref& e, int value );
		int getUseConcentration( const Eref& e ) const;
		double vGetModulation( const Eref& e ) const;
		/////////////////////////////////////////////////////////////
		// Dest function definitions
		/////////////////////////////////////////////////////////////
		/**
		 * Assign the local conc_ to the incoming conc from the
		 * concentration calculations for the compartment. Typically
		 * the message source will be a CaConc object, but there
		 * are other options for computing the conc.
		 */
		void handleConc( const Eref& e, double conc );

		/////////////////////////////////////////////////////////////
		// Gate handling functions
		/////////////////////////////////////////////////////////////
		/**
		 * Access function used for the X gate. The index is ignored.
		 */
		HHGate* getXgate( unsigned int i );

		/**
		 * Access function used for the Y gate. The index is ignored.
		 */
		HHGate* getYgate( unsigned int i );

		/**
		 * Access function used for the Z gate. The index is ignored.
		 */
		HHGate* getZgate( unsigned int i );

		/**
		 * Dummy assignment function for the number of gates.
		 */
		void setNumGates( unsigned int num );

		/**
		 * Access function for the number of Xgates. Gives 1 if present,
		 * otherwise 0.
		 */
		unsigned int getNumXgates() const;
		/// Returns 1 if Y gate present, otherwise 0
		unsigned int getNumYgates() const;
		/// Returns 1 if Z gate present, otherwise 0
		unsigned int getNumZgates() const;

		/**
		 * Function for safely creating each gate, identified by strings
		 * as X, Y and Z. Will only work on a new channel, not on a
		 * copy. The idea is that the gates are always referred to the
		 * original 'library' channel, and their contents cannot be touched
		 * except by the original.
		 */
		void createGate( const Eref& e, string gateType );
		/////////////////////////////////////////////////////////////
		// Virtual Value field access function definitions
		/////////////////////////////////////////////////////////////
		virtual void vSetXpower( const Eref& e, double Xpower ) = 0;
		virtual void vSetYpower( const Eref& e, double Ypower ) = 0;
		virtual void vSetZpower( const Eref& e, double Zpower ) = 0;
		// getXpower etc functions are implemented here in the baseclass.
		virtual void vSetInstant( const Eref& e, int Instant ) = 0;
		virtual int vGetInstant( const Eref& e ) const = 0;
		virtual void vSetX( const Eref& e, double X ) = 0;
		virtual double vGetX( const Eref& e ) const = 0;
		virtual void vSetY( const Eref& e, double Y ) = 0;
		virtual double vGetY( const Eref& e ) const = 0;
		virtual void vSetZ( const Eref& e, double Z ) = 0;
		virtual double vGetZ( const Eref& e ) const = 0;
		virtual void vSetUseConcentration( const Eref& e, int value ) = 0;

		/////////////////////////////////////////////////////////////
		// Some more Virtual Value field functions from ChanBase,
		// to be defined in derived classes. Listed here for clarity.
		/////////////////////////////////////////////////////////////
		// void vSetGbar( double Gbar );
		// double vGetGbar() const;
		// void vSetEk( double Ek );
		// double vGetEk() const;
		// void vSetGk( double Gk );
		// double vGetGk() const;
		// void vSetIk( double Ic );
		// double vGetIk() const;
		// void vHandleVm( double Vm );

		/////////////////////////////////////////////////////////////
		// Virtual Dest function definitions
		/////////////////////////////////////////////////////////////
		// void vProcess( const Eref& e, ProcPtr p ); // Listed for clarity
		// void vReinit( const Eref& e, ProcPtr p ); // Listed for clarity

		virtual void vHandleConc( const Eref& e, double conc ) = 0;

		/////////////////////////////////////////////////////////////
		// Virtual Gate handling functions
		/////////////////////////////////////////////////////////////
		virtual HHGate* vGetXgate( unsigned int i ) const = 0;
		virtual HHGate* vGetYgate( unsigned int i ) const = 0;
		virtual HHGate* vGetZgate( unsigned int i ) const = 0;
		virtual void vCreateGate( const Eref& e, string gateType ) = 0;

		/////////////////////////////////////////////////////////////
		// Utility functions for taking integer powers.
		/////////////////////////////////////////////////////////////
		static double power1( double x, double p ) {
			return x;
		}
		static double power2( double x, double p ) {
			return x * x;
		}
		static double power3( double x, double p ) {
			return x * x * x;
		}
		static double power4( double x, double p ) {
			return power2( x * x, p );
		}
		static double powerN( double x, double p );

		static PFDD selectPower( double power);

		/////////////////////////////////////////////////////////////
		// Zombification functions.
		/////////////////////////////////////////////////////////////
		virtual void vSetSolver( const Eref& e, Id hsolve );
		static void zombify( Element* orig, const Cinfo* zClass, Id hsolve);

		/////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	protected:
		/// Exponent for X gate
		double Xpower_;
		/// Exponent for Y gate
		double Ypower_;
		/// Exponent for Z gate
		double Zpower_;
		/// Flag for use of conc for input to Z gate calculations.
		bool useConcentration_;

		/// Value used to scale channel conductance up or down
		double modulation_;
};


#endif // _HHChannelBase_h
