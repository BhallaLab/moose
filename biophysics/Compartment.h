/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _COMPARTMENT_H
#define _COMPARTMENT_H

/**
 * The Compartment class sets up an asymmetric compartment for
 * branched nerve calculations. Handles electronic structure and
 * also channels. This is not a particularly efficient way of doing
 * this, so we should use a solver for any substantial calculations.
 */
namespace moose
{
class Compartment: public CompartmentBase
{
public:
    Compartment();
    virtual ~Compartment();

    // Value Field access function definitions.
    virtual void vSetVm( const Eref& e, double Vm );
    virtual double vGetVm( const Eref& e ) const;
    virtual void vSetEm( const Eref& e, double Em );
    virtual double vGetEm( const Eref& e ) const;
    virtual void vSetCm( const Eref& e, double Cm );
    virtual double vGetCm( const Eref& e ) const;
    virtual void vSetRm( const Eref& e, double Rm );
    virtual double vGetRm( const Eref& e ) const;
    virtual void vSetRa( const Eref& e, double Ra );
    virtual double vGetRa( const Eref& e ) const;
    virtual double vGetIm( const Eref& e ) const;
    virtual void vSetInject( const Eref& e, double Inject );
    virtual double vGetInject( const Eref& e ) const;
    virtual void vSetInitVm( const Eref& e, double initVm );
    virtual double vGetInitVm( const Eref& e ) const;

    // Dest function definitions.
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
    void vInjectMsg( const Eref& e, double current);

    /**
     * Injects a constantly updated current into the
     * compartment, with a probability prob. Note that it isn't
     * the current amplitude that is random, it is the presence
     * or absence of the current that is probabilistic.
     */
    void vRandInject( const Eref& e, double prob, double current);

    /**
     * Dummy function to act as recipient of 'cable' message,
     * which is just for grouping compartments.
     */
    void cable();


    /**
     * Initializes the class info.
     */
    static const Cinfo* initCinfo();

protected:
    double Vm_;
    double initVm_;
    double Em_;
    double Cm_;
    double Rm_;
    double Ra_;
    double Im_;
    double lastIm_;
    double inject_;
    double A_;
    double B_;
    double sumInject_;

private:
    double invRm_;
    double dt_;
    static const double EPSILON;

};
}

// Used by solver, readcell, etc.

#endif // _COMPARTMENT_H
