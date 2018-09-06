/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _INT_FIRE_BASE_H
#define _INT_FIRE_BASE_H

namespace moose
{
/**
 * The IntFire class sets up an integrate-and-fire compartment.
 */
class IntFireBase: public Compartment
{
public:
    IntFireBase();
    virtual ~IntFireBase();

    // Value Field access function definitions.
    void setThresh( const Eref& e,  double val );
    double getThresh( const Eref& e  ) const;
    void setVReset( const Eref& e,  double val );
    double getVReset( const Eref& e  ) const;
    void setRefractoryPeriod( const Eref& e,  double val );
    double getRefractoryPeriod( const Eref& e  ) const;
    double getLastEventTime( const Eref& e  ) const;
    bool hasFired( const Eref& e ) const;

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
     * activation handles information coming from the SynHandler
     * to the intFire.
     */
    void activation( double val );

    /// Message src for outgoing spikes.
    static SrcFinfo1< double >* spikeOut();

    /**
     * Initializes the class info.
     */
    static const Cinfo* initCinfo();
protected:
    double threshold_;
    double vReset_;
    double activation_;
    double refractT_;
    double lastEvent_;
    bool fired_;
};
} // namespace

#endif // _INT_FIRE_BASE_H
