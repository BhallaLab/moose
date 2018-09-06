/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SYN_HANDLER_BASE_H
#define _SYN_HANDLER_BASE_H


class Synapse;
/**
 * This is a pure virtual base class for accessing and handling synapses.
 * It provides a uniform interface so that all classes that use synapses
 * can do so without duplication.
 * The derived classes have the responsibility of handling the
 * synaptic events as well as possible return events from the parent.
 */
class SynHandlerBase
{
public:
    SynHandlerBase();
    virtual ~SynHandlerBase();

    ////////////////////////////////////////////////////////////////
    // Field assignment stuff.
    ////////////////////////////////////////////////////////////////

    /**
     * Resizes the synapse storage
     */
    void setNumSynapses( unsigned int num );

    /**
     * Returns number of synapses defined.
     */
    unsigned int getNumSynapses() const;

    /**
     * Gets specified synapse
     */
    Synapse* getSynapse( unsigned int i );
    ////////////////////////////////////////////////////////////////

    void process( const Eref& e, ProcPtr p );
    void reinit( const Eref& e, ProcPtr p );

    /**
     * A utility function to check for assignment to fields that
     * must be > 0
     */
    bool rangeWarning( const string& field, double value );

    ////////////////////////////////////////////////////////////////
    /**
     * Adds a new synapse, returns its index. This is
     * triggered by addMsg of inputs to the child synapse. The
     * SynHandler has to ensure that we have enough synapses allocated
     * to handle the new message, and the return value is used so that
     * the new message knows the fieldIndex to send the message to.
     */
    virtual unsigned int addSynapse() = 0;

    /**
     * Drops an existing synapse, triggered by deleteMsg of an input
     * to the child synapse. This is a little messy because we can't
     * change the indices of the other synapses. The Handler has to
     * figure out how to handle the 'holes' in its array of synapses.
     */
    virtual void dropSynapse( unsigned int droppedSynNumber ) = 0;

    /**
     * Record arrival of a new spike event. The 'time' is time for
     * eventual arrival of the spike, and is typically well in the
     * future. The index specifies which synapse the spike came to.
     */
    virtual void addSpike(
        unsigned int index, double time, double weight ) = 0;
    virtual double getTopSpike( unsigned int index ) const = 0;
    ////////////////////////////////////////////////////////////////
    // Virtual func definitions for fields.
    ////////////////////////////////////////////////////////////////
    virtual void vSetNumSynapses( unsigned int num ) = 0;
    virtual unsigned int vGetNumSynapses() const = 0;
    virtual Synapse* vGetSynapse( unsigned int i ) = 0;
    virtual void vProcess( const Eref& e, ProcPtr p ) = 0;
    virtual void vReinit( const Eref& e, ProcPtr p ) = 0;
    ////////////////////////////////////////////////////////////////
    static SrcFinfo1< double >* activationOut();
    static const Cinfo* initCinfo();

private:

};

#endif // _SYN_HANDLER_BASE_H
