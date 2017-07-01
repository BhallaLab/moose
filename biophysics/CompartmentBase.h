/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _COMPARTMENT_BASE_H
#define _COMPARTMENT_BASE_H

#ifdef  CYMOOSE

template<typename A>
class SrcFinfo1;


#endif     /* -----  CYMOOSE  ----- */

/**
 * The CompartmentBase class sets up the interface for all the
 * derived Compartment classes, used in
 * branched nerve calculations. Handles electronic structure and
 * also channels.
 */
namespace moose
{
class CompartmentBase
{
	public:
			CompartmentBase();
			virtual ~CompartmentBase();

			// Value Field access function definitions.
			void setVm( const Eref& e, double Vm );
			double getVm( const Eref& e ) const;
			void setEm( const Eref& e, double Em );
			double getEm( const Eref& e ) const;
			void setCm( const Eref& e, double Cm );
			double getCm( const Eref& e ) const;
			void setRm( const Eref& e, double Rm );
			double getRm( const Eref& e ) const;
			void setRa( const Eref& e, double Ra );
			double getRa( const Eref& e ) const;
			double getIm( const Eref& e ) const;
			void setInject( const Eref& e, double Inject );
			double getInject( const Eref& e ) const;
			void setInitVm( const Eref& e, double initVm );
			double getInitVm( const Eref& e ) const;
			void setDiameter( double diameter );
			double getDiameter() const;
			void setLength( double length );
			double getLength() const;
			void setX0( double value );
			double getX0() const;
			void setY0( double value );
			double getY0() const;
			void setZ0( double value );
			double getZ0() const;
			void setX( double value );
			double getX() const;
			void setY( double value );
			double getY() const;
			void setZ( double value );
			double getZ() const;

			// Dest function definitions.
			/**
			 * The process function does the object updating and sends out
			 * messages to channels, nernsts, and so on.
			 */
			void process( const Eref& e, ProcPtr p );

			/**
			 * The reinit function reinitializes all fields.
			 */
			void reinit( const Eref& e, ProcPtr p );

			/**
			 * The initProc function is for a second phase of 'process'
			 * operations. It sends the axial and raxial messages
			 * to other compartments. It has to be executed out of phase
			 * with the main process so that all compartments are
			 * equivalent and there is no calling order dependence in
			 * the results.
			 */
			void initProc( const Eref& e, ProcPtr p );

			/**
			 * Empty function to do another reinit step out of phase
			 * with the main one. Nothing needs doing there.
			 */
			void initReinit( const Eref& e, ProcPtr p );

			/**
			 * handleChannel handles information coming from the channel
			 * to the compartment
			 */
			void handleChannel( const Eref& e, double Gk, double Ek);

			/**
			 * handleRaxial handles incoming raxial message data.
			 */
			void handleRaxial( double Ra, double Vm);

			/**
			 * handleAxial handles incoming axial message data.
			 */
			void handleAxial( double Vm);

			/**
			 * Injects a constantly updated current into the compartment.
			 * Unlike the 'inject' field, this injected current is
			 * applicable only for a single timestep. So this is meant to
			 * be used as the destination of a message rather than as a
			 * one-time assignment.
			 */
			void injectMsg( const Eref& e, double current);

			/**
			 * Injects a constantly updated current into the
			 * compartment, with a probability prob. Note that it isn't
			 * the current amplitude that is random, it is the presence
			 * or absence of the current that is probabilistic.
			 */
			void randInject( const Eref& e, double prob, double current);

			/**
			 * Dummy function to act as recipient of 'cable' message,
			 * which is just for grouping compartments.
			 */
			void cable();

			/// Displaces compartment by specified distance vector.
			void displace( double dx, double dy, double dz );

			/// Scales electrical values along with setting length, dia
			void setGeomAndElec( const Eref& e,
							double length, double dia );
			/////////////////////////////////////////////////////////////
			// Here we define the virtual functions for each of the above
			// wrappers.
			/////////////////////////////////////////////////////////////
			// Value Field access function definitions.
			virtual void vSetVm( const Eref& e, double Vm ) = 0;
			virtual double vGetVm( const Eref& e ) const = 0;
			virtual void vSetEm( const Eref& e, double Em ) = 0;
			virtual double vGetEm( const Eref& e ) const = 0;
			virtual void vSetCm( const Eref& e, double Cm ) = 0;
			virtual double vGetCm( const Eref& e ) const = 0;
			virtual void vSetRm( const Eref& e, double Rm ) = 0;
			virtual double vGetRm( const Eref& e ) const = 0;
			virtual void vSetRa( const Eref& e, double Ra ) = 0;
			virtual double vGetRa( const Eref& e ) const = 0;
			virtual double vGetIm( const Eref& e ) const = 0;
			virtual void vSetInject( const Eref& e, double Inject ) = 0;
			virtual double vGetInject( const Eref& e ) const = 0;
			virtual void vSetInitVm( const Eref& e, double initVm ) = 0;
			virtual double vGetInitVm( const Eref& e ) const = 0;

			// Dest function definitions.
			/**
			 * The process function does the object updating and sends out
			 * messages to channels, nernsts, and so on.
			 */
			virtual void vProcess( const Eref& e, ProcPtr p ) = 0;

			/**
			 * The reinit function reinitializes all fields.
			 */
			virtual void vReinit( const Eref& e, ProcPtr p ) = 0;

			/**
			 * The initProc function is for a second phase of 'process'
			 * operations. It sends the axial and raxial messages
			 * to other compartments. It has to be executed out of phase
			 * with the main process so that all compartments are
			 * equivalent and there is no calling order dependence in
			 * the results.
			 */
			virtual void vInitProc( const Eref& e, ProcPtr p ) = 0;

			/**
			 * Empty function to do another reinit step out of phase
			 * with the main one. Nothing needs doing there.
			 */
			virtual void vInitReinit( const Eref& e, ProcPtr p ) = 0;

			/**
			 * handleChannel handles information coming from the channel
			 * to the compartment
			 */
			virtual void vHandleChannel( const Eref& e, double Gk, double Ek) = 0;

			/**
			 * handleRaxial handles incoming raxial message data.
			 */
			virtual void vHandleRaxial( double Ra, double Vm) = 0;

			/**
			 * handleAxial handles incoming axial message data.
			 */
			virtual void vHandleAxial( double Vm) = 0;

			/**
			 * Injects a constantly updated current into the compartment.
			 * Unlike the 'inject' field, this injected current is
			 * applicable only for a single timestep. So this is meant to
			 * be used as the destination of a message rather than as a
			 * one-time assignment.
			 */
			virtual void vInjectMsg( const Eref& e, double current) = 0;

			/**
			 * Injects a constantly updated current into the
			 * compartment, with a probability prob. Note that it isn't
			 * the current amplitude that is random, it is the presence
			 * or absence of the current that is probabilistic.
			 */
			virtual void vRandInject( const Eref& e, double prob, double current) = 0;

			/////////////////////////////////////////////////////////////
			// Required for solver setup
			/////////////////////////////////////////////////////////////

			virtual void vSetSolver( const Eref& e, Id hsolve );

			/////////////////////////////////////////////////////////////
			/**
			 * A utility function to check for assignment to fields that
			 * must be > 0
			 */
			bool rangeWarning( const string& field, double value );

			/**
			 * A utility function update length if any of the coords change
			 */
			void updateLength();

			/**
			 * Initializes the class info.
			 */
			static const Cinfo* initCinfo();

			/**
			 * This Finfo is used to send out Vm to channels, spikegens, etc
			 *
			 * It is exposed here so that HSolve can also use it to send out
			 * the Vm to the recipients.
			 */
			static SrcFinfo1< double >* VmOut();

			/**
			 * This function is used to swap Cinfos of any CompartmentBase
			 * derived class. Used for making ZombieCompartments.
			 */
			static void zombify( Element* orig, const Cinfo* zClass,
						Id hsolve );
	private:
			double diameter_;
			double length_;
			double x0_;
			double y0_;
			double z0_;
			double x_;
			double y_;
			double z_;
};
}

// Used by solver, readcell, etc.

#endif // _COMPARTMENT_BASE_H
