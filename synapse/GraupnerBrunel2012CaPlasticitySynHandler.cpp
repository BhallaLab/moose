/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "header.h"
#include "Synapse.h"
#include "SynEvent.h" // only using the SynEvent class from this
#include "SynHandlerBase.h"
#include "../randnum/Normal.h" // generate normal randum numbers for noisy weight update
#include "GraupnerBrunel2012CaPlasticitySynHandler.h"

const Cinfo* GraupnerBrunel2012CaPlasticitySynHandler::initCinfo()
{
	static string doc[] =
	{
		"Name", "GraupnerBrunel2012CaPlasticitySynHandler",
		"Author", "Aditya Gilra",
		"Description",
		"The GraupnerBrunel2012CaPlasticitySynHandler handles synapses"
        "with Ca-based plasticity as per Higgins et al. 2014 and Graupner and Brunel 2012."
        "Note 1:"
        "   Here, Ca ('chemical Ca') is updated only at each pre-spike, pre-spike+delayD and post-spike!"
        "   So it is inaccurate to use it for say Ca-dependent K channels in the electrical compartment,"
        "   for which you use are advised to use the CaPool i.e. 'electrical Ca'."
        "Note 2:"
        "   Ca here is post-synaptic 'chemical Ca' common for all synapses in this SynHandler,"
        "   so weights of all pre-synapses connected to this SynHandler get updated"
        "   at each pre-spike, pre-spike+delayD and post-spike!"
        "   So if all pre-synaptic weights start out the same, they remain the same!!"
        "   If you want to consider each pre-synapse independently,"
        "   have independent SynHandlers for each synapse."
        "   If these SynHandlers are in the same electrical compartment,"
        "   you're essentially assuming these are on different spines,"
        "   with their own 'chemical Ca' which won't match the"
        "   'electrical Ca' of the compartment (=dendrite)."
        "   If you put each SynHandler with a single synapse"
        "   in its own electrical compartment (=spine),"
        "   only then can you have an 'electrical Ca'"
        "   corresponding to the 'chemical Ca'."
		"Three priority queues are used to manage pre, post, and pre+delayD spikes."
	};

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > Ca(
        "Ca",
        "Ca is a post-synaptic decaying variable as a proxy for Ca concentration"
        "and receives an impulse whenever a pre- or post- spike occurs."
        "Caution: Ca is updated via an event-based rule, so it is only updated and valid"
        "when a pre- or post- spike has occured, or at time delayD after a pre-spike."
        "Do not use it to control a Ca dependent current, etc."
        "See notes in the class Description: all pre-synapses get updated via the same post-synaptic Ca.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setCa,
		&GraupnerBrunel2012CaPlasticitySynHandler::getCa
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > CaInit(
        "CaInit",
        "CaInit is the initial value for Ca",
		&GraupnerBrunel2012CaPlasticitySynHandler::setCaInit,
		&GraupnerBrunel2012CaPlasticitySynHandler::getCaInit
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > tauCa(
        "tauCa",
        "tauCa is the time constant for decay of Ca",
		&GraupnerBrunel2012CaPlasticitySynHandler::setTauCa,
		&GraupnerBrunel2012CaPlasticitySynHandler::getTauCa
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > tauSyn(
        "tauSyn",
        "tauSyn is the time constant for synaptic weight evolution equation",
		&GraupnerBrunel2012CaPlasticitySynHandler::setTauSyn,
		&GraupnerBrunel2012CaPlasticitySynHandler::getTauSyn
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > CaPre(
        "CaPre",
        "CaPre is added to Ca on every pre-spike",
		&GraupnerBrunel2012CaPlasticitySynHandler::setCaPre,
		&GraupnerBrunel2012CaPlasticitySynHandler::getCaPre
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > CaPost(
        "CaPost",
        "CaPost is added to Ca on every post-spike",
		&GraupnerBrunel2012CaPlasticitySynHandler::setCaPost,
		&GraupnerBrunel2012CaPlasticitySynHandler::getCaPost
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > delayD(
        "delayD",
        "Time delay D after pre-spike, when Ca is increased by Capre."
        " delayD represents NMDA rise time.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setDelayD,
		&GraupnerBrunel2012CaPlasticitySynHandler::getDelayD
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > gammaP(
        "gammaP",
        "gammaP is the potentiation factor for synaptic weight increase if Ca>thetaP",
		&GraupnerBrunel2012CaPlasticitySynHandler::setGammaP,
		&GraupnerBrunel2012CaPlasticitySynHandler::getGammaP
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > gammaD(
        "gammaD",
        "gammaD is the depression factor for synaptic weight decrease if Ca>thetaD",
		&GraupnerBrunel2012CaPlasticitySynHandler::setGammaD,
		&GraupnerBrunel2012CaPlasticitySynHandler::getGammaD
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > thetaP(
        "thetaP",
        "Potentiation threshold for Ca"
        "User must ensure thetaP>thetaD, else simulation results will be wrong.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setThetaP,
		&GraupnerBrunel2012CaPlasticitySynHandler::getThetaP
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > thetaD(
        "thetaD",
        "Depression threshold for Ca"
        "User must ensure thetaP>thetaD, else simulation results will be wrong.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setThetaD,
		&GraupnerBrunel2012CaPlasticitySynHandler::getThetaD
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, bool > bistable(
        "bistable",
        "If true, the synapse is bistable as in GraupnerBrunel2012 paper."
        "The effect of potential on the weight update is usually ignorable"
        " if Ca is above thetaP and thetaD most of the time.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setBistable,
		&GraupnerBrunel2012CaPlasticitySynHandler::getBistable
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, bool > noisy(
        "noisy",
        "If true, turn noise on as per noiseSD",
		&GraupnerBrunel2012CaPlasticitySynHandler::setNoisy,
		&GraupnerBrunel2012CaPlasticitySynHandler::getNoisy
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > noiseSD(
        "noiseSD",
        "Standard deviation of noise added to Ca",
		&GraupnerBrunel2012CaPlasticitySynHandler::setNoiseSD,
		&GraupnerBrunel2012CaPlasticitySynHandler::getNoiseSD
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > weightMax(
        "weightMax",
        "An upper bound on the weight",
		&GraupnerBrunel2012CaPlasticitySynHandler::setWeightMax,
		&GraupnerBrunel2012CaPlasticitySynHandler::getWeightMax
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > weightMin(
        "weightMin",
        "A lower bound on the weight",
		&GraupnerBrunel2012CaPlasticitySynHandler::setWeightMin,
		&GraupnerBrunel2012CaPlasticitySynHandler::getWeightMin
    );

    static ValueFinfo< GraupnerBrunel2012CaPlasticitySynHandler, double > weightScale(
        "weightScale",
        "Scale all pre-synaptic weights by weightScale before adding to activation (default 1.0)"
        "In the terminology of the paper Higgins et al 2012, weight is synaptic efficacy,"
        "while weightScale*weight is what finally is added to activation variable.",
		&GraupnerBrunel2012CaPlasticitySynHandler::setWeightScale,
		&GraupnerBrunel2012CaPlasticitySynHandler::getWeightScale
    );

    static DestFinfo addPostSpike( "addPostSpike",
        "Handles arriving spike messages from post-synaptic neuron, inserts into postEvent queue.",
        new EpFunc1< GraupnerBrunel2012CaPlasticitySynHandler, \
                double >( &GraupnerBrunel2012CaPlasticitySynHandler::addPostSpike ) );

	static FieldElementFinfo< SynHandlerBase, Synapse > synFinfo(
		"synapse",
		"Sets up field Elements for synapse",
		Synapse::initCinfo(),
		&SynHandlerBase::getSynapse,
		&SynHandlerBase::setNumSynapses,
		&SynHandlerBase::getNumSynapses
	);

	static Finfo* GraupnerBrunel2012CaPlasticitySynHandlerFinfos[] = {
		&synFinfo,			// FieldElement
		&addPostSpike,		// DestFinfo
		&Ca,    		    // Field
		&CaInit,    		// Field
		&tauCa, 		    // Field
		&tauSyn, 		    // Field
		&CaPre,        	    // Field
		&CaPost,	        // Field
		&delayD,	        // Field
		&thetaP,	        // Field
		&thetaD,	        // Field
		&gammaP,	        // Field
		&gammaD,	        // Field
        &weightMax,         // Field
        &weightMin,         // Field
        &weightScale,       // Field
        &noisy,             // Field
        &noiseSD,           // Field
        &bistable           // Field
	};

	static Dinfo< GraupnerBrunel2012CaPlasticitySynHandler > dinfo;
	static Cinfo synHandlerCinfo (
		"GraupnerBrunel2012CaPlasticitySynHandler",
		SynHandlerBase::initCinfo(),
		GraupnerBrunel2012CaPlasticitySynHandlerFinfos,
		sizeof( GraupnerBrunel2012CaPlasticitySynHandlerFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &synHandlerCinfo;
}

static const Cinfo* GraupnerBrunel2012CaPlasticitySynHandlerCinfo =\
    GraupnerBrunel2012CaPlasticitySynHandler::initCinfo();

GraupnerBrunel2012CaPlasticitySynHandler::GraupnerBrunel2012CaPlasticitySynHandler()
{
    Ca_ = 0.0;
    CaInit_ = 0.0;
    tauCa_ = 1.0;
    tauSyn_ = 1.0;
    CaPre_ = 0.0;
    CaPost_ = 0.0;
    thetaD_ = 0.0;
    thetaP_ = 0.0;
    gammaD_ = 0.0;
    gammaP_ = 0.0;
    delayD_ = 0.0;
    weightMin_ = 0.0;
    weightMax_ = 0.0;
    weightScale_ = 1.0;
    noisy_ = false;
    noiseSD_ = 0.0;
    bistable_ = true;
    normalGenerator_.setMethod(BOX_MUELLER); // the default ALIAS method is 1000x slower!
}

GraupnerBrunel2012CaPlasticitySynHandler::~GraupnerBrunel2012CaPlasticitySynHandler()
{ ; }

GraupnerBrunel2012CaPlasticitySynHandler& GraupnerBrunel2012CaPlasticitySynHandler::operator=\
        ( const GraupnerBrunel2012CaPlasticitySynHandler& ssh)
{
	synapses_ = ssh.synapses_;
	for ( vector< Synapse >::iterator
					i = synapses_.begin(); i != synapses_.end(); ++i )
			i->setHandler( this );

	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();
	while( !delayDPreEvents_.empty() )
		events_.pop();
	while( !postEvents_.empty() )
		postEvents_.pop();

	return *this;
}

void GraupnerBrunel2012CaPlasticitySynHandler::vSetNumSynapses( const unsigned int v )
{
	unsigned int prevSize = synapses_.size();
	synapses_.resize( v );
	for ( unsigned int i = prevSize; i < v; ++i )
		synapses_[i].setHandler( this );
}

unsigned int GraupnerBrunel2012CaPlasticitySynHandler::vGetNumSynapses() const
{
	return synapses_.size();
}

Synapse* GraupnerBrunel2012CaPlasticitySynHandler::vGetSynapse( unsigned int i )
{
	static Synapse dummy;
	if ( i < synapses_.size() )
		return &synapses_[i];
	cout << "Warning: GraupnerBrunel2012CaPlasticitySynHandler::getSynapse: index: " << i <<
		" is out of range: " << synapses_.size() << endl;
	return &dummy;
}

void GraupnerBrunel2012CaPlasticitySynHandler::addSpike(
				unsigned int index, double time, double weight )
{
	assert( index < synapses_.size() );
	events_.push( PreSynEvent( index, time, weight ) );
	delayDPreEvents_.push( PreSynEvent( index, time+delayD_, weight ) );
}

void GraupnerBrunel2012CaPlasticitySynHandler::addPostSpike( const Eref& e, double time )
{
	postEvents_.push( PostSynEvent( time ) );
}

weightFactors GraupnerBrunel2012CaPlasticitySynHandler::updateCaWeightFactors( double currTime )
{
        double CaOld = Ca_;
        double deltaT = currTime-lastCaUpdateTime_;
        // Update Ca, but CaPre and CaPost are added in vProcess()
        Ca_ *= exp(-deltaT/tauCa_);
        lastCaUpdateTime_ = currTime;
        weightFactors wUp; // by default all are set to 0.0

        // calculate/approximate time spent above potentiation and depression thresholds
        // see pg 13 of Higgins et al | October 2014 | Volume 10 | Issue 10 | e1003834 | PLOS Comp Biol
        // starting from bottom condition, going upwards in the algorithm given in above paper
        if (CaOld <= thetaD_) {
        } else if (CaOld <= thetaP_) {
            //cout << "tD<Caold<tP" << "\n";
            if (Ca_ <= thetaD_) {
                wUp.tD = tauCa_*log(CaOld/thetaD_);
                //cout << "Ca<tD" << "\n";
            } else {
                wUp.tD = deltaT;
                //cout << "Ca>tD" << "\n";
            }
        } else {
            //cout << "Caold>tP" << "\n";
            if (Ca_ <= thetaD_) {
                wUp.tP = tauCa_*log(CaOld/thetaP_);
                wUp.tD = tauCa_*log(thetaP_/thetaD_);
                //cout << "Ca<tD" << "\n";
                //cout << "Caold = " << CaOld << "thetaP = " << thetaP_ << "\n";
            } else if (Ca_ <= thetaP_) {
                wUp.tP = tauCa_*log(CaOld/thetaP_);
                wUp.tD = deltaT - wUp.tP;
                //cout << "Ca<tP" << "\n";
            } else {
                wUp.tP = deltaT;
                //cout << "Ca>tP" << "\n";
            }
        }
        wUp.t0 = deltaT - wUp.tP - wUp.tD;

        // Depending on tP and tD, I return A,B,C factors for weight update
        // (see page 13 of Higgins et al 2014).
        // A,B,C,D,E are used to compute the weight change for one or multiple synapses
        // A is weight independent, B,D are weight dependent and C,E are accumulated noise
        if (wUp.tP > 0) {
            double gPgD = gammaP_+gammaD_;
            wUp.A = gammaP_/gPgD*(1.0-exp(-wUp.tP*gPgD/tauSyn_));
            wUp.B = exp(-wUp.tP*gPgD/tauSyn_);
            if (noisy_) {
                wUp.C = noiseSD_ * normalGenerator_.getNextSample() *
                        sqrt( ( 1.0-exp(-2*gPgD*wUp.tP/tauSyn_) ) / gPgD );
                //cout << "A = " << wUp.A << " B = " << wUp.B << " C = " << wUp.C << "\n";
            } else {
                wUp.C = 0.0;
            }
        }

        if (wUp.tD > 0) {
            wUp.D = exp(-wUp.tD*gammaD_/tauSyn_);
            if (noisy_) {
                wUp.E = noiseSD_ * normalGenerator_.getNextSample() *
                        sqrt( ( 1.0-exp(-2*gammaD_*wUp.tD/tauSyn_) ) / 2.0/gammaD_ );
                //cout << "D = " << wUp.D << " E = " << wUp.E << "\n";
            } else{
                wUp.E = 0.0;
            }
        }

        //cout << currTime  << " tD = " << wUp.tD << " tP = " << wUp.tP << "\n";
        // tP, tD, A, B, C, D, E of wUp
        // are set to 0.0 by default in struct constructor
        return wUp;     // return by value, i.e. copies the struct, so wUp going out of scope doesn't matter
                         // malloc and returning pointer is more expensive for small structs
                         // see: http://stackoverflow.com/questions/9590827/is-it-safe-to-return-a-struct-in-c-c
}

void GraupnerBrunel2012CaPlasticitySynHandler::updateWeight( Synapse* synPtr, weightFactors *wFacPtr )
{
    double newWeight = synPtr->getWeight();
    //cout << " oldweight = " << newWeight << "\n";
    if (wFacPtr->tP > 0.0) {
        newWeight = wFacPtr->A + wFacPtr->B*newWeight + wFacPtr->C;
        //cout << " midweight = " << newWeight << "\n";
    }
    if (wFacPtr->tD > 0.0) {
        newWeight = wFacPtr->D*newWeight + wFacPtr->E; // update the weight again
        //cout << " newweight = " << newWeight << "\n";
    }

    // potential is usually ignorable when t0 is small,
    // i.e. Ca is mostly above thetaD or thetaP
    //cout << "before bistable newWeight = " << newWeight << "\n";
    if (bistable_) {
        double chi0 = pow((newWeight-0.5),2.0) / (newWeight*(newWeight-1));
        double weightDeviation = 0.5*sqrt( 1.0 + 1.0/(chi0*exp(wFacPtr->t0/2.0/tauSyn_)-1.0) );
        if (newWeight<0.5) {
            newWeight = 0.5 - weightDeviation;
        } else {
            newWeight = 0.5 + weightDeviation;
        }
    }
    //cout << "after bistable newWeight = " << newWeight << "\n";

    // clip weight within [weightMin,weightMax]
    newWeight = std::max(weightMin_, std::min(newWeight, weightMax_));
    //cout << " clipweight = " << newWeight << "\n";
    //cout << " A = " << wFacPtr->A << " B = " << wFacPtr->B << " C = " << wFacPtr->C
    //    << " D = " << wFacPtr->D << " E = " << wFacPtr->E << " newW = "<< newWeight << "\n";
    synPtr->setWeight( newWeight );
}

void GraupnerBrunel2012CaPlasticitySynHandler::vProcess( const Eref& e, ProcPtr p )
{
	double activation = 0.0;
    double currTime = p->currTime;
    bool CaFactorsUpdated = false; // Ca-decay and weight Factors updation is done only once
                                    // if any pre-spike, pre-spike+delayD or post-spike occurs
                                    // Ca is bumped by CaPre and CaPost on
                                    // pre-spike+delayD and post-spike respectively
                                    // At the end, if any event above occurred,
                                    // change all pre-synaptic weights based on Ca
    weightFactors wFacs;

    // process pre-synaptic spike events for activation, Ca and weight update
	while( !events_.empty() && events_.top().time <= currTime ) {
        PreSynEvent currEvent = events_.top();

        unsigned int synIndex = currEvent.synIndex;
        // Warning, coder! 'STDPSynapse currSyn = synapses_[synIndex];' is wrong,
        // it creates a new, shallow-copied object.
        // We want only to point to the same object.
        Synapse* currSynPtr = &synapses_[synIndex];

        // activate the synapse for every pre-spike
        // If the synapse has a delay, the weight could be updated during the delay!
        // currEvent.weight is the weight before the delay!
        // Might be better to use currSynPtr->getWeight()
        //      or even the latest updated weight below?!
        // Using currSynPtr->getWeight().
        // Still, weight update is event driven and not continuous
        //      so using currSynPtr->getWeight() takes care of
        //      weight updates due to events/spikes during the delay
        //      but doesn't take care of continuous weight evolution during the delay.
        // The weight update is done
        //          after sending the current weight to activation
        // Maybe I should use the updated weight to send to activation?!

        // See: http://www.genesis-sim.org/GENESIS/Hyperdoc/Manual-26.html#synchan
        // Send out weight / dt for every spike
        //      Since it is an impulse active only for one dt,
        //      need to send it divided by dt.
        // Can connect activation to SynChan (double exp)
        //      or to LIF as an impulse to voltage.
		//activation += currEvent.weight * weightScale_ / p->dt;
        activation += currSynPtr->getWeight() * weightScale_ / p->dt;

        // update only once for this time-step if an event occurs
        if (!CaFactorsUpdated) {
            // update Ca and weightFactors
            wFacs = updateCaWeightFactors( currTime );
            CaFactorsUpdated = true;
        }

		events_.pop();
	}
	if ( activation != 0.0 )
		SynHandlerBase::activationOut()->send( e, activation );

    // process delayed pre-synaptic spike events for Ca and weight update
    // delayD after pre-spike accounts for NMDA rise time
	while( !delayDPreEvents_.empty() && delayDPreEvents_.top().time <= currTime ) {
        // Update Ca, and add CaPre
        // update only once for this time-step if an event occurs
        if (!CaFactorsUpdated) {
            // update Ca and weightFactors
            wFacs = updateCaWeightFactors( currTime );
            CaFactorsUpdated = true;
        }
        Ca_ += CaPre_;

		delayDPreEvents_.pop();
	}

    // process post-synaptic spike events for Ca and weight update
	while( !postEvents_.empty() && postEvents_.top().time <= currTime ) {
        // update Ca, then add CaPost
        // update only once for this time-step if an event occurs
        if (!CaFactorsUpdated) {
            // update Ca and weightFactors
            wFacs = updateCaWeightFactors( currTime );
            CaFactorsUpdated = true;
        }
        Ca_ += CaPost_;

		postEvents_.pop();
	}

    // If any event has happened, update all pre-synaptic weights
    // If you want individual Ca for each pre-synapse
    // create individual SynHandlers for each
    if (CaFactorsUpdated) {
        // Change weight of all synapses
        for (unsigned int i=0; i<synapses_.size(); i++) {
            // Warning, coder! 'Synapse currSyn = synapses_[i];' is wrong,
            // it creates a new, shallow-copied object.
            // We want only to point to the same object.
            Synapse* currSynPtrHere = &synapses_[i];
            updateWeight(currSynPtrHere,&wFacs);
        }
    }

}

void GraupnerBrunel2012CaPlasticitySynHandler::vReinit( const Eref& e, ProcPtr p )
{
	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();
	while( !delayDPreEvents_.empty() )
		events_.pop();
	while( !postEvents_.empty() )
		postEvents_.pop();
    Ca_ = CaInit_;
}

unsigned int GraupnerBrunel2012CaPlasticitySynHandler::addSynapse()
{
	unsigned int newSynIndex = synapses_.size();
	synapses_.resize( newSynIndex + 1 );
	synapses_[newSynIndex].setHandler( this );
	return newSynIndex;
}


void GraupnerBrunel2012CaPlasticitySynHandler::dropSynapse( unsigned int msgLookup )
{
	assert( msgLookup < synapses_.size() );
	synapses_[msgLookup].setWeight( -1.0 );
}

void GraupnerBrunel2012CaPlasticitySynHandler::setCa( const double v )
{
	Ca_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getCa() const
{
	return Ca_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setCaInit( const double v )
{
	CaInit_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getCaInit() const
{
	return CaInit_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setCaPre( const double v )
{
	CaPre_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getCaPre() const
{
	return CaPre_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setCaPost( const double v )
{
	CaPost_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getCaPost() const
{
	return CaPost_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setThetaP( const double v )
{
	thetaP_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getThetaP() const
{
	return thetaP_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setThetaD( const double v )
{
	thetaD_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getThetaD() const
{
	return thetaD_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setGammaD( const double v )
{
	gammaD_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getGammaD() const
{
	return gammaD_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setGammaP( const double v )
{
	gammaP_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getGammaP() const
{
	return gammaP_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setDelayD( const double v )
{
	delayD_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getDelayD() const
{
	return delayD_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setTauCa( const double v )
{
	if ( rangeWarning( "tauCa", v ) ) return;
	tauCa_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getTauCa() const
{
	return tauCa_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setTauSyn( const double v )
{
	if ( rangeWarning( "tauSyn", v ) ) return;
	tauSyn_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getTauSyn() const
{
	return tauSyn_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setNoisy( const bool v )
{
	noisy_ = v;
}

bool GraupnerBrunel2012CaPlasticitySynHandler::getNoisy() const
{
	return noisy_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setNoiseSD( const double v )
{
	noiseSD_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getNoiseSD() const
{
	return noiseSD_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setBistable( const bool v )
{
	bistable_ = v;
}

bool GraupnerBrunel2012CaPlasticitySynHandler::getBistable() const
{
	return bistable_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setWeightMax( const double v )
{
	weightMax_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getWeightMax() const
{
	return weightMax_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setWeightMin( const double v )
{
	weightMin_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getWeightMin() const
{
	return weightMin_;
}

void GraupnerBrunel2012CaPlasticitySynHandler::setWeightScale( const double v )
{
	weightScale_ = v;
}

double GraupnerBrunel2012CaPlasticitySynHandler::getWeightScale() const
{
	return weightScale_;
}
