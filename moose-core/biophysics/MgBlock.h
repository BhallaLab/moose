#ifndef _Mg_block_h
#define _Mg_block_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
*********************************************************************
*/

//typedef double ( *PFDD )( double, double );

class MgBlock: public ChanCommon
{
	public:
		MgBlock();

		/////////////////////////////////////////////////////////////
		// Value field access function definitions
		/////////////////////////////////////////////////////////////

		void setKMg_A( double Gbar );
		double getKMg_A() const;
		void setKMg_B( double Ek );
		double getKMg_B() const;
		void setCMg( double CMg );
		double getCMg() const;
		void setZk( double Zk );
		double getZk() const;
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
		 */
// 		void channelFunc( double Vm );
		void origChannel( const Eref& e, double Gk, double Ek );

		static const Cinfo* initCinfo();
	private:
		/// charge
		double Zk_;
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
                double origGk_;
};


#endif // _Mg_block_h
