/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "Synapse.h"
#include "SynHandlerBase.h"

static const double RANGE = 1.0e-15;

SrcFinfo1< double >* SynHandlerBase::activationOut()
{
    static SrcFinfo1< double > activationOut(
        "activationOut",
        "Sends out level of activation on all synapses converging to "
        "this SynHandler"
    );
    return &activationOut;
}

/**
 * These are the base set of fields for any object managing synapses.
 * Note that these are duplicated in SynChanBase: if you change anything
 * here it must also be reflected there.
 */
const Cinfo* SynHandlerBase::initCinfo()
{
    static ValueFinfo< SynHandlerBase, unsigned int > numSynapses(
        "numSynapses",
        "Number of synapses on SynHandler. Duplicate field for num_synapse",
        &SynHandlerBase::setNumSynapses,
        &SynHandlerBase::getNumSynapses
    );
    //////////////////////////////////////////////////////////////////////
    static DestFinfo process( "process",
                              "Handles 'process' call. Checks if any spike events are due for"
                              "handling at this timestep, and does learning rule stuff if needed",
                              new ProcOpFunc< SynHandlerBase >(& SynHandlerBase::process ) );
    static DestFinfo reinit( "reinit",
                             "Handles 'reinit' call. Initializes all the synapses.",
                             new ProcOpFunc< SynHandlerBase >(& SynHandlerBase::reinit ) );

    static Finfo* processShared[] =
    {
        &process, &reinit
    };
    static SharedFinfo proc( "proc",
                             "Shared Finfo to receive Process messages from the clock.",
                             processShared, sizeof( processShared ) / sizeof( Finfo* )
                           );

    //////////////////////////////////////////////////////////////////////
    static Finfo* synHandlerFinfos[] =
    {
        &numSynapses,		// Value
        activationOut(),	// SrcFinfo
        &proc, 				// SharedFinfo
    };

    static string doc[] =
    {
        "Name", "SynHandlerBase",
        "Author", "Upi Bhalla",
        "Description",
        "Base class for handling synapse arrays converging onto a given "
        "channel or integrate-and-fire neuron. This class provides the "
        "interface for channels/intFires to connect to a range of synapse "
        "types, including simple synapses, synapses with different "
        "plasticity rules, and variants yet to be implemented. "
    };
    static ZeroSizeDinfo< int > dinfo;
    static Cinfo synHandlerCinfo (
        "SynHandlerBase",
        Neutral::initCinfo(),
        synHandlerFinfos,
        sizeof( synHandlerFinfos ) / sizeof ( Finfo* ),
        &dinfo,
        doc,
        sizeof( doc ) / sizeof( string )
    );

    return &synHandlerCinfo;
}

static const Cinfo* synHandlerCinfo = SynHandlerBase::initCinfo();

////////////////////////////////////////////////////////////////////////

SynHandlerBase::SynHandlerBase()
{
    ;
}

SynHandlerBase::~SynHandlerBase()
{
    ;
}

void SynHandlerBase::setNumSynapses( unsigned int num )
{
    vSetNumSynapses( num );
}

unsigned int SynHandlerBase::getNumSynapses() const
{
    return vGetNumSynapses();
}

Synapse* SynHandlerBase::getSynapse( unsigned int i )
{
    return vGetSynapse( i );
}

void SynHandlerBase::process( const Eref& e, ProcPtr p )
{
    vProcess( e, p );
}

void SynHandlerBase::reinit( const Eref& e, ProcPtr p )
{
    vReinit( e, p );
}

bool SynHandlerBase::rangeWarning( const string& field, double value )
{
    if ( value < RANGE )
    {
        cout << "Warning: Ignored attempt to set " << field <<
             " of SynHandler " <<
             // c->target().e->name() <<
             " to less than " << RANGE << endl;
        return 1;
    }
    return 0;
}
