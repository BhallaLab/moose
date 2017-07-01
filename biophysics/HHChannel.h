#ifndef _HHChannel_h
#define _HHChannel_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
*********************************************************************
*/

/**
 * The HHChannel class sets up a Hodkin-Huxley type ion channel.
 * The form used here is quite general and can handle up to 3
 * gates, named X, Y and Z. The Z gate can be a function of
 * concentration as well as voltage. The gates are normally computed
 * using the form
 *
 *            alpha(V)
 * closed <------------> open
 *          beta(V)
 *
 * where the rates for the transition are alpha and beta, and both
 * are functions of V.
 * The state variables for each gate (X_, Y_, and Z_) are
 * the fraction in the open state.
 *
 * Gates can also be computed instantaneously, giving the instantaneous
 * ratio of alpha to beta rather than solving the above conversion
 * process.
 * The actual functions alpha and beta are provided by an auxiliary
 * class, the HHGate. The idea is that all copies of a channel share the
 * same gate, thus saving a great deal of space. It also makes it
 * possible to cleanly change the parameters of all the channels of
 * a give class, all at once. Should one want to mutate a subset
 * of channels, they just need to set up separate gates.
 *
 * HHGates are implemented as a special category of FieldElement, so that
 * they can be accessed as readonly pointers available to the HHChannel.
 * The FieldElement containing the HHGate appears as a child Element of
 * the HHChannel. The HHChannel Element can be an array; the associated
 * HHGate is a singleton. So there has to be a local copy of the HHGate
 * on each node.
 */

class HHChannel: public HHChannelBase, public ChanCommon
{
#ifdef DO_UNIT_TESTS
	friend void testHHChannel();
	friend void testHHGateCreation();
#endif // DO_UNIT_TESTS
	public:
		HHChannel();
		~HHChannel();

		/////////////////////////////////////////////////////////////
		// Value field access function definitions
		/////////////////////////////////////////////////////////////

		void vSetXpower( const Eref& e, double Xpower );
		void vSetYpower( const Eref& e, double Ypower );
		void vSetZpower( const Eref& e, double Zpower );
		void vSetInstant( const Eref& e, int Instant );
		int vGetInstant( const Eref& e ) const;
		void vSetX( const Eref& e, double X );
		double vGetX( const Eref& e ) const;
		void vSetY( const Eref& e, double Y );
		double vGetY( const Eref& e ) const;
		void vSetZ( const Eref& e, double Z );
		double vGetZ( const Eref& e ) const;
		void vSetUseConcentration( const Eref& e, int value );
		void vSetModulation( const Eref& e, double modulation );
		double vGetModulation( const Eref& e ) const;

		void innerSetXpower( double Xpower );
		void innerSetYpower( double Ypower );
		void innerSetZpower( double Zpower );

		/////////////////////////////////////////////////////////////
		// Dest function definitions
		/////////////////////////////////////////////////////////////

		/**
		 * processFunc handles the update and calculations every
		 * clock tick. It first sends the request for evaluation of
		 * the gate variables to the respective gate objects and
		 * recieves their response immediately through a return
		 * message. This is done so that many channel instances can
		 * share the same gate lookup tables, but do so cleanly.
		 * Such messages should never go to a remote node.
		 * Then the function does its own little calculations to
		 * send back to the parent compartment through regular
		 * messages.
		 */
		void vProcess( const Eref& e, ProcPtr p );

		/**
		 * Reinitializes the values for the channel. This involves
		 * computing the steady-state value for the channel gates
		 * using the provided Vm from the parent compartment. It
		 * involves a similar cycle through the gates and then
		 * updates to the parent compartment as for the processFunc.
		 */
		void vReinit( const Eref& e, ProcPtr p );

		/**
		 * Assign the local Vm_ to the incoming Vm from the compartment
		void handleVm( double Vm );
		 */

		/**
		 * Assign the local conc_ to the incoming conc from the
		 * concentration calculations for the compartment. Typically
		 * the message source will be a CaConc object, but there
		 * are other options for computing the conc.
		 */
		void vHandleConc( const Eref& e, double conc );

		/////////////////////////////////////////////////////////////
		// Gate handling functions
		/////////////////////////////////////////////////////////////
		/**
		 * Access function used for the X gate. The index is ignored.
		 */
		HHGate* vGetXgate( unsigned int i ) const;

		/**
		 * Access function used for the Y gate. The index is ignored.
		 */
		HHGate* vGetYgate( unsigned int i ) const;

		/**
		 * Access function used for the Z gate. The index is ignored.
		 */
		HHGate* vGetZgate( unsigned int i ) const;

		/// Inner utility function for creating the gate.
		void innerCreateGate(
			 const string& gateName,
			HHGate** gatePtr, Id chanId, Id gateId );

		/// Returns true if channel is original, false if copy.
		bool checkOriginal( Id chanId ) const;

		void vCreateGate( const Eref& e, string gateType );
		/**
		 * Utility function for destroying gate. Works only on original
		 * HHChannel. Somewhat dangerous, should never be used after a
		 * copy has been made as the pointer of the gate will be in use
		 * elsewhere.
		 */
		void destroyGate( const Eref& e, string gateType );

		/**
		 * Inner utility for destroying the gate
		 */
		void innerDestroyGate( const string& gateName,
			HHGate** gatePtr, Id chanId );

		/**
		 * Utility for altering gate powers
		 */
		bool setGatePower( const Eref& e, double power,
			double* assignee, const string& gateType );

		/////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();

	private:
		/// Conc_ is input variable for Ca-dependent channels.
		double conc_;

		double ( *takeXpower_ )( double, double );
		double ( *takeYpower_ )( double, double );
		double ( *takeZpower_ )( double, double );

		/// bitmapped flag for X, Y, Z, to do equil calculation for gate
		int instant_;
		/// Channel actual conductance depending on opening of gates.
		/// State variable for X gate
		double X_;
		/// State variable for Y gate
		double Y_;
		/// State variable for Z gate
		double Z_;

        bool xInited_, yInited_, zInited_; // true when a state variable
        	// has been initialized
		double g_;	/// Internal variable used to calculate conductance

		double integrate( double state, double dt, double A, double B );

		/**
		 * HHGate data structure for the xGate. This is writable only
		 * on the HHChannel that originally created the HHGate, for others
		 * it must be treated as readonly.
		 */
		HHGate* xGate_;

		/// HHGate data structure for the yGate.
		HHGate* yGate_;

		/// HHGate data structure for the yGate.
		HHGate* zGate_;

		Id myId_;

		static const double EPSILON;
		static const int INSTANT_X;
		static const int INSTANT_Y;
		static const int INSTANT_Z;
};


#endif // _HHChannel_h
