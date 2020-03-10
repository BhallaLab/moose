/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ZOMBIE_COMPARTMENT_H
#define _ZOMBIE_COMPARTMENT_H

#include "../basecode/global.h"

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/print_function.hpp"
#include "../biophysics/CompartmentBase.h"
#include "../biophysics/Compartment.h"
#include "../randnum/RNG.h"
#include "HinesMatrix.h"
#include "HSolveStruct.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"

/**
 * Zombie object that lets HSolve do its calculations, while letting the user
 * interact with this object as if it were the original object.
 */
//class DataHandler;

class ZombieCompartment: public CompartmentBase
{
public:
    ZombieCompartment();
    virtual ~ZombieCompartment();

    /*
     * Value Field access function definitions.
     */

    // Fields handled by solver.
    void vSetVm( const Eref& e , double Vm );
    double vGetVm( const Eref& e  ) const;
    void vSetEm( const Eref& e , double Em );
    double vGetEm( const Eref& e  ) const;
    void vSetCm( const Eref& e , double Cm );
    double vGetCm( const Eref& e  ) const;
    void vSetRm( const Eref& e , double Rm );
    double vGetRm( const Eref& e  ) const;
    void vSetRa( const Eref& e , double Ra );
    double vGetRa( const Eref& e  ) const;
    double vGetIm( const Eref& e  ) const;
    void vSetInject( const Eref& e , double Inject );
    double vGetInject( const Eref& e  ) const;
    void vSetInitVm( const Eref& e , double initVm );
    double vGetInitVm( const Eref& e  ) const;

    // Dest function definitions.
    void dummy( const Eref& e, ProcPtr p );

    /**
     * The process function does the object updating and sends out
     * messages to channels, nernsts, and so on.
     */
    void vProcess( const Eref& e, ProcPtr p );

    /**
     * The reinit function reinitializes all fields.
     */
    void vReinit( const Eref& e, ProcPtr p );

    /**
     * The initProc function is for a second phase of 'process'
     * operations. It sends the axial and raxial messages
     * to other compartments. It has to be executed out of phase
     * with the main process so that all compartments are
     * equivalent and there is no calling order dependence in
     * the results.
     */
    void vInitProc( const Eref& e, ProcPtr p );

    /**
     * Empty function to do another reinit step out of phase
     * with the main one. Nothing needs doing there.
     */
    void vInitReinit( const Eref& e, ProcPtr p );

    /**
     * handleChannel handles information coming from the channel
     * to the compartment
     */
    void vHandleChannel( const Eref& e, double Gk, double Ek);

    /**
     * handleRaxial handles incoming raxial message data.
     */
    void vHandleRaxial( double Ra, double Vm);

    /**
     * handleAxial handles incoming axial message data.
     */
    void vHandleAxial( double Vm);

    /**
     * Injects a constantly updated current into the compartment.
     * Unlike the 'inject' field, this injected current is
     * applicable only for a single timestep. So this is meant to
     * be used as the destination of a message rather than as a
     * one-time assignment.
     */
    void vInjectMsg( const Eref& e , double current);


    /**
     * Injects a constantly updated current into the
     * compartment, with a probability prob. Note that it isn't
     * the current amplitude that is random, it is the presence
     * or absence of the current that is probabilistic.
     */

    void vRandInject( const Eref& e , double prob, double current);

	/// Assigns the solver to the zombie
	void vSetSolver( const Eref& e, Id hsolve );

    /**
     * Initializes the class info.
     */
    static const Cinfo* initCinfo();

    //////////////////////////////////////////////////////////////////
    // utility funcs
    //////////////////////////////////////////////////////////////////
    double mtrand( void );

private:
    HSolve* hsolve_;

    static const double EPSILON;

    void copyFields( moose::Compartment* c );

    moose::RNG rng;

};

#endif // _ZOMBIE_COMPARTMENT_H
