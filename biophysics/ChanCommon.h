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

#ifndef _ChanCommon_h
#define _ChanCommon_h

/**
 * The ChanCommon.g handles the data fields for all ion channel classes
 * in MOOSE, when they are using regular ee calculations rather than
 * being zombified by the solver.
 */

class ChanCommon: public virtual ChanBase
{
public:
    ChanCommon();
    ~ChanCommon();

    /////////////////////////////////////////////////////////////
    // Value field access function definitions
    /////////////////////////////////////////////////////////////

    void vSetGbar( const Eref& e, double Gbar );
    double vGetGbar( const Eref& e ) const;
    void vSetModulation( const Eref& e, double modulation );
    double vGetModulation( const Eref& e ) const;
    double getModulation() const;
    void vSetEk( const Eref& e, double Ek );
    double vGetEk( const Eref& e ) const;
    void vSetGk( const Eref& e, double Gk );
    double vGetGk( const Eref& e ) const;
    /// Ik is read-only for MOOSE, but we provide the set
    /// func for derived classes to update it.
    void vSetIk( const Eref& e, double Ic );
    double vGetIk( const Eref& e ) const;

    /////////////////////////////////////////////////////////////
    // Dest function definitions
    /////////////////////////////////////////////////////////////

    /**
     * Assign the local Vm_ to the incoming Vm from the compartment
     */
    void vHandleVm( double Vm );

    /////////////////////////////////////////////////////////////
    /**
     * This function sends out the messages expected of a channel,
     * after process. Used as a utility by various derived classes.
     */
    void sendProcessMsgs( const Eref& e, const ProcPtr info );
    void sendReinitMsgs( const Eref& e, const ProcPtr info );

    /////////////////////////////////////////////////////////////

    /**
     * Utility function for a common computation using local variables
     */
    void updateIk();

    /// Utility function to access Vm
    double getVm() const;

    /// Utility function to acces Gbar
    double getGbar() const;

    /// Specify the Class Info static variable for initialization.
    static const Cinfo* initCinfo();
protected:
    /// Vm_ is input variable from compartment, used for most rates
    double Vm_;

private:
    /// Channel maximal conductance
    double Gbar_;
    /// Channel modulation. Scales conductance.
    double modulation_;
    /// Reversal potential of channel
    double Ek_;

    /// Channel actual conductance depending on opening of gates.
    double Gk_;
    /// Channel current
    double Ik_;
};
#endif // _ChanCommon_h
