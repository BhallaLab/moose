/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_ACTIVE_H
#define _HSOLVE_ACTIVE_H

#include "../basecode/header.h"
#include <set>
#include <limits> // Max and min 'double' values needed for lookup table init.
#include "../biophysics/CaConcBase.h"
#include "../biophysics/HHGate.h"
#include "../biophysics/ChanBase.h"
#include "../biophysics/ChanCommon.h"
#include "../biophysics/HHChannelBase.h"
#include "../biophysics/HHChannel.h"
#include "../biophysics/SpikeGen.h"
#include "HSolveUtils.h"
#include "HSolveStruct.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"

class HSolveActive: public HSolvePassive
{
    typedef vector< CurrentStruct >::iterator currentVecIter;

public:
    HSolveActive();

    void setup( Id seed, double dt );
    void step( ProcPtr info );			///< Equivalent to process
    void reinit( ProcPtr info );

protected:
    /**
     * Solver parameters: exposed as fields in MOOSE
     */

    /**
     * caAdvance_: This flag determines how current flowing into a calcium pool
     * is computed. A value of 0 means that the membrane potential at the
     * beginning of the time-step is used for the calculation. This is how
     * GENESIS does its computations. A value of 1 means the membrane potential
     * at the middle of the time-step is used. This is the correct way of
     * integration, and is the default way.
     */
    int                       caAdvance_;

    /**
     * vMin_, vMax_, vDiv_,
     * caMin_, caMax_, caDiv_:
     *
     * These are the parameters for the lookup tables for rate constants.
     * 'min' and 'max' are the boundaries within which the function is defined.
     * 'div' is the number of divisions between min and max.
     */
    double                    vMin_;
    double                    vMax_;
    int                       vDiv_;
    double                    caMin_;
    double                    caMax_;
    int                       caDiv_;

    /**
     * Internal data structures. Will also be accessed in derived class HSolve.
     */
    vector< CurrentStruct >   current_;			///< Channel current
    vector< double >          state_;			///< Fraction of gates open
    //~ vector< int >             instant_;
    vector< ChannelStruct >   channel_;			///< Vector of channels. Link
    ///< to compartment: chan2compt
    vector< SpikeGenStruct >  spikegen_;
    vector< SynChanStruct >   synchan_;
    vector< CaConcStruct >    caConc_;			///< Ca pool info
    vector< double >          ca_;				///< Ca conc in each pool
    vector< double >          caActivation_;	///< Ca current entering each
    ///< calcium pool
    vector< double* >         caTarget_;		///< For each channel, which
    ///< calcium pool is being fed?
    ///< Points into caActivation.
    LookupTable               vTable_;
    LookupTable               caTable_;
    vector< bool >            gCaDepend_;		///< Does the conductance
    ///< depend on Ca conc?
    vector< unsigned int >    caCount_;			///< Number of calcium pools in
    ///< each compartment
    vector< int >             caDependIndex_;	///< Which pool does each Ca
    ///< depdt channel depend upon?
    vector< LookupColumn >    column_;			///< Which column in the table
    ///< to lookup for this species
    vector< LookupRow >       caRowCompt_;      /**< Lookup row buffer.
		*   For each compartment, the lookup rows for calcium dependent
		*   channels are loaded into this vector before being used. The vector
		*   is then reused for the next compartment. This vector therefore has
		*   a size equal to the maximum number of calcium pools across all
		*   compartments. This is done in HSolveActive::advanceChannels */

    vector< LookupRow* >      caRow_;			/**< Points into caRowCompt.
		*   For each channel, points to the appropriate pool's LookupRow in the
		*   caRowCompt vector. This value is then used by the channel. Also
		*   happens in HSolveActive::advanceChannels */

    vector< int >             channelCount_;	///< Number of channels in each
    ///< compartment
    vector< currentVecIter >  currentBoundary_;	///< Used to designate compt
    ///< boundaries in the current_
    ///< vector.
    vector< unsigned int >    chan2compt_;		///< Index of the compt to
    ///< which a given (index)
    ///< channel belongs.
    vector< unsigned int >    chan2state_;		///< Converts a chnnel index to
    ///< a state index
    vector< double >          externalCurrent_; ///< External currents from
    ///< channels that HSolve
    ///< cannot internalize.
    vector< double >          prevExtCurr_; ///< Last tick's externalCurrent
    vector< double >          externalCalcium_; /// calcium from difshells
    vector< Id >              caConcId_;		///< Used for localIndex-ing.
    vector< Id >              channelId_;		///< Used for localIndex-ing.
    vector< Id >              gateId_;			///< Used for localIndex-ing.
    //~ vector< vector< Id > >    externalChannelId_;
    vector< unsigned int >    outVm_;			/**< VmOut info.
		*   Tells you which compartments have external voltage-dependent
		*   channels (if any), so that you can send out Vm values only in those
		*   places */
    vector< unsigned int >    outCa_;			/**< concOut info.
		*   Tells you which compartments have external calcium-dependent
		*   channels so that you can send out Calcium concentrations in only
		*   those compartments. */
     vector< unsigned int >    outIk_;

private:
    /**
     * Setting up of data structures: Defined in HSolveActiveSetup.cpp
     */
    void readHHChannels();
    void readGates();
    void readCalcium();
    void readSynapses();
    void readExternalChannels();
    void createLookupTables();
    void manageOutgoingMessages();

    void cleanup();

    /**
     * Reinit code: Defined in HSolveActiveSetup.cpp
     */
    void reinitSpikeGens( ProcPtr info );
    void reinitCompartments();
    void reinitCalcium();
    void reinitChannels();

    /**
     * Integration: Defined in HSolveActive.cpp
     */
    void calculateChannelCurrents();
    void updateMatrix();
    void forwardEliminate();
    void backwardSubstitute();
    void advanceCalcium();
    void advanceChannels( double dt );
    void advanceSynChans( ProcPtr info );
    void sendSpikes( ProcPtr info );
    void sendValues( ProcPtr info );

    static const int INSTANT_X;
    static const int INSTANT_Y;
    static const int INSTANT_Z;
};

#endif // _HSOLVE_ACTIVE_H
