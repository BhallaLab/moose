/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GRAUPNER_BRUNEL_2012_CA_PLASTICITY_SYN_HANDLER_H
#define _GRAUPNER_BRUNEL_2012_CA_PLASTICITY_SYN_HANDLER_H

#include "../basecode/global.h"
#include "../randnum/RNG.h"

#include <queue>

using namespace std;

// see pg 13 of Higgins et al | October 2014 | Volume 10 | Issue 10 | e1003834 | PLOS Comp Biol
// tP and tD are times spent above potentiation and depression thresholds
// Depending on tP and tD, I return A,B,C factors for weight update (see pg 13 ref above)
// A,B,C are used to compute the weight change for one or multiple synapses
// A is weight independent, B,D are weight dependent and C,E are accumulated noise
struct weightFactors
{
    weightFactors() : tP(0.0), tD(0.0), A(0.0), B(0.0), C(0.0), D(0.0), E(0.0) {};
    double tP; // time spent above potentiation threshold (thetaP) between two events
    double tD; // time spent between depression and potentiation thresholds between two events
    double t0; // time spent below depression threshold (thetaD) between two events
    double A;
    double B;
    double C;
    double D;
    double E;
};

/**
 * This handles simple synapses without plasticity. It uses a priority
 * queue to manage them. This gets inefficient for large numbers of
 * synapses but is pretty robust.
 */
class GraupnerBrunel2012CaPlasticitySynHandler: public SynHandlerBase
{
public:
    GraupnerBrunel2012CaPlasticitySynHandler();
    ~GraupnerBrunel2012CaPlasticitySynHandler();
    GraupnerBrunel2012CaPlasticitySynHandler& operator=(
            const GraupnerBrunel2012CaPlasticitySynHandler& other );

    ////////////////////////////////////////////////////////////////
    // Inherited virtual functions from SynHandlerBase
    ////////////////////////////////////////////////////////////////
    void vSetNumSynapses( unsigned int num );
    unsigned int vGetNumSynapses() const;
    Synapse* vGetSynapse( unsigned int i );
    void vProcess( const Eref& e, ProcPtr p );
    void vReinit( const Eref& e, ProcPtr p );
    /// Adds a new synapse, returns its index.
    unsigned int addSynapse();
    void dropSynapse( unsigned int droppedSynNumber );
    void addSpike( unsigned int index, double time, double weight );
    double getTopSpike( unsigned int index ) const;
    ////////////////////////////////////////////////////////////////
    void addPostSpike( const Eref& e, double time );

    void setCa( double v );
    double getCa() const;
    void setCaInit( double v );
    double getCaInit() const;
    void setTauCa( double v );
    double getTauCa() const;
    void setTauSyn( double v );
    double getTauSyn() const;
    void setNoiseSD( double v );
    double getNoiseSD() const;
    void setNoisy( bool v );
    bool getNoisy() const;
    void setBistable( bool v );
    bool getBistable() const;

    void setCaPre( double v );
    double getCaPre() const;
    void setCaPost( double v );
    double getCaPost() const;
    void setDelayD( double v );
    double getDelayD() const;

    void setThetaD( double v );
    double getThetaD() const;
    void setThetaP( double v );
    double getThetaP() const;
    void setGammaD( double v );
    double getGammaD() const;
    void setGammaP( double v );
    double getGammaP() const;

    void setWeightMax( double v );
    double getWeightMax() const;
    void setWeightMin( double v );
    double getWeightMin() const;
    void setWeightScale( double v );
    double getWeightScale() const;

    weightFactors updateCaWeightFactors( double currTime );
    void updateWeight( Synapse* synPtr, weightFactors *wFacPtr );

    void reinitSeed( );

    static const Cinfo* initCinfo();

private:

    vector< Synapse > synapses_;

    priority_queue< PreSynEvent, vector< PreSynEvent >, CompareSynEvent > events_;
    priority_queue< PreSynEvent, vector< PreSynEvent >, CompareSynEvent > delayDPreEvents_;
    priority_queue< PostSynEvent, vector< PostSynEvent >, ComparePostSynEvent > postEvents_;

    double Ca_;
    double CaInit_;
    double tauCa_;
    double tauSyn_;
    double CaPre_;
    double CaPost_;
    double delayD_;
    bool noisy_;
    double noiseSD_;
    bool bistable_;
    double thetaD_;
    double thetaP_;
    double gammaD_;
    double gammaP_;
    double weightMax_;
    double weightMin_;
    double weightScale_;
    double lastCaUpdateTime_;

    // NormalRng normalGenerator_;
    unsigned long seed_;
    moose::MOOSE_RANDOM_DEVICE rd_;
    moose::MOOSE_NORMAL_DISTRIBUTION<double> dist_;
    moose::MOOSE_RNG_DEFAULT_ENGINE rng_;

};

#endif // _GRAUPNER_BRUNEL_2012_CA_PLASTICITY_SYN_HANDLER_H
