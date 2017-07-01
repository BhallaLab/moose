/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "header.h"
#include "Synapse.h"
#include "SynEvent.h"
#include "SynHandlerBase.h"
#include "RollingMatrix.h"
#include "SeqSynHandler.h"
#include "muParser.h"

const Cinfo* SeqSynHandler::initCinfo()
{
	static string doc[] =
	{
		"Name", "SeqSynHandler",
		"Author", "Upi Bhalla",
		"Description",
		"The SeqSynHandler handles synapses that recognize sequentially "
			"ordered input, where the ordering is both in space and time. "
			"It assumes that the N input synapses are ordered and "
			"equally spaced along a single linear vector.\n "
			"To do this it maintains a record of recent synaptic input, "
			"for a duration of *historyTime*, at a time interval *seqDt*. "
			"*SeqDt* is typically longer than the simulation "
			"timestep *dt* for the synapse, and cannot be shorter. "
			"*SeqDt* should represent the characteristic time of advance "
			"of the sequence. \n"
			"The SeqSynHandler uses a 2-D kernel to define how to recognize"
			" a sequence, with dependence both on space and history. "
			"This kernel is defined by the *kernelEquation* as a "
			"mathematical expression in x (synapse number) and t (time)."
			"It computes a vector with the local *response* term for each "
			"point along all inputs, by taking a 2-d convolution of the "
		    "kernel with the history[time][synapse#] matrix."
			"\nThe local response can affect the synapse in three ways: "
			"1. It can sum the entire response vector, scale by the "
			"*responseScale* term, and send to the synapse as a steady "
			"activation. Consider this a cell-wide immediate response to "
			"a sequence that it likes.\n"
			"2. It do an instantaneous scaling of the weight of each "
			"individual synapse by the corresponding entry in the response "
			"vector. It uses the *weightScale* term to do this. Consider "
			"this a short-term plasticity effect on specific synapses. \n"
			"3. It can do long-term plasticity of each individual synapse "
			"using the matched local entries in the response vector and "
			"individual synapse history as inputs to the learning rule. "
			"This is not yet implemented.\n"
	};

	static FieldElementFinfo< SynHandlerBase, Synapse > synFinfo(
		"synapse",
		"Sets up field Elements for synapse",
		Synapse::initCinfo(),
		&SynHandlerBase::getSynapse,
		&SynHandlerBase::setNumSynapses,
		&SynHandlerBase::getNumSynapses
	);

	static ValueFinfo< SeqSynHandler, string > kernelEquation(
			"kernelEquation",
			"Equation in x and t to define kernel for sequence recognition",
			&SeqSynHandler::setKernelEquation,
			&SeqSynHandler::getKernelEquation
	);
	static ValueFinfo< SeqSynHandler, unsigned int > kernelWidth(
			"kernelWidth",
			"Width of kernel, i.e., number of synapses taking part in seq.",
			&SeqSynHandler::setKernelWidth,
			&SeqSynHandler::getKernelWidth
	);
	static ValueFinfo< SeqSynHandler, double > seqDt(
			"seqDt",
			"Characteristic time for advancing the sequence.",
			&SeqSynHandler::setSeqDt,
			&SeqSynHandler::getSeqDt
	);
	static ValueFinfo< SeqSynHandler, double > historyTime(
			"historyTime",
			"Duration to keep track of history of inputs to all synapses.",
			&SeqSynHandler::setHistoryTime,
			&SeqSynHandler::getHistoryTime
	);
	static ValueFinfo< SeqSynHandler, double > responseScale(
			"responseScale",
			"Scaling factor for sustained activation of synapse by seq",
			&SeqSynHandler::setResponseScale,
			&SeqSynHandler::getResponseScale
	);
	static ReadOnlyValueFinfo< SeqSynHandler, double > seqActivation(
			"seqActivation",
			"Reports summed activation of synaptic channel by sequence",
			&SeqSynHandler::getSeqActivation
	);
	static ValueFinfo< SeqSynHandler, double > weightScale(
			"weightScale",
			"Scaling factor for weight of each synapse by response vector",
			&SeqSynHandler::setWeightScale,
			&SeqSynHandler::getWeightScale
	);
	static ReadOnlyValueFinfo< SeqSynHandler, vector< double > >
			weightScaleVec(
			"weightScaleVec",
			"Vector of  weight scaling for each synapse",
			&SeqSynHandler::getWeightScaleVec
	);
	static ReadOnlyValueFinfo< SeqSynHandler, vector< double > > kernel(
			"kernel",
			"All entries of kernel, as a linear vector",
			&SeqSynHandler::getKernel
	);
	static ReadOnlyValueFinfo< SeqSynHandler, vector< double > > history(
			"history",
			"All entries of history, as a linear vector",
			&SeqSynHandler::getHistory
	);

	static Finfo* seqSynHandlerFinfos[] = {
		&synFinfo,					// FieldElement
		&kernelEquation,			// Field
		&kernelWidth,				// Field
		&seqDt,						// Field
		&historyTime,				// Field
		&responseScale,				// Field
		&seqActivation,				// Field
		&weightScale,				// Field
		&weightScaleVec,			// Field
		&kernel,					// ReadOnlyField
		&history					// ReadOnlyField
	};

	static Dinfo< SeqSynHandler > dinfo;
	static Cinfo seqSynHandlerCinfo (
		"SeqSynHandler",
		SynHandlerBase::initCinfo(),
		seqSynHandlerFinfos,
		sizeof( seqSynHandlerFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &seqSynHandlerCinfo;
}

static const Cinfo* seqSynHandlerCinfo = SeqSynHandler::initCinfo();

//////////////////////////////////////////////////////////////////////

SeqSynHandler::SeqSynHandler()
	:
		kernelEquation_( "" ),
		kernelWidth_( 5 ),
		historyTime_( 2.0 ),
		seqDt_ ( 1.0 ),
		responseScale_( 1.0 ),
		weightScale_( 0.0 ),
		seqActivation_( 0.0 )
{
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	history_.resize( numHistory, 0 );
}

SeqSynHandler::~SeqSynHandler()
{ ; }

//////////////////////////////////////////////////////////////////////
SeqSynHandler& SeqSynHandler::operator=( const SeqSynHandler& ssh)
{
	synapses_ = ssh.synapses_;
	for ( vector< Synapse >::iterator
					i = synapses_.begin(); i != synapses_.end(); ++i )
			i->setHandler( this );

	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();

	return *this;
}

void SeqSynHandler::vSetNumSynapses( const unsigned int v )
{
	unsigned int prevSize = synapses_.size();
	synapses_.resize( v );
	for ( unsigned int i = prevSize; i < v; ++i )
		synapses_[i].setHandler( this );

	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	history_.resize( numHistory, v );
	latestSpikes_.resize( v, 0.0 );
	weightScaleVec_.resize( v, 0.0 );
	updateKernel();
}

unsigned int SeqSynHandler::vGetNumSynapses() const
{
	return synapses_.size();
}

Synapse* SeqSynHandler::vGetSynapse( unsigned int i )
{
	static Synapse dummy;
	if ( i < synapses_.size() )
		return &synapses_[i];
	cout << "Warning: SeqSynHandler::getSynapse: index: " << i <<
		" is out of range: " << synapses_.size() << endl;
	return &dummy;
}

//////////////////////////////////////////////////////////////////////
void SeqSynHandler::updateKernel()
{
	if ( kernelEquation_ == "" || seqDt_ < 1e-9 || historyTime_ < 1e-9 )
		return;
	double x = 0;
	double t = 0;
	mu::Parser p;
	p.DefineVar("x", &x);
	p.DefineVar("t", &t);
	p.DefineConst(_T("pi"), (mu::value_type)M_PI);
	p.DefineConst(_T("e"), (mu::value_type)M_E);
	p.SetExpr( kernelEquation_ );
	kernel_.clear();
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	kernel_.resize( numHistory );
	for ( int i = 0; i < numHistory; ++i ) {
		kernel_[i].resize( kernelWidth_ );
		t = i * seqDt_;
		for ( unsigned int j = 0; j < kernelWidth_; ++j ) {
			x = j;
			kernel_[i][j] = p.Eval();
		}
	}
}

//////////////////////////////////////////////////////////////////////
void SeqSynHandler::setKernelEquation( string eq )
{
	kernelEquation_ = eq;
	updateKernel();
}

string SeqSynHandler::getKernelEquation() const
{
	return kernelEquation_;
}

void SeqSynHandler::setKernelWidth( unsigned int v )
{
	kernelWidth_ = v;
	updateKernel();
}

unsigned int SeqSynHandler::getKernelWidth() const
{
	return kernelWidth_;
}

void SeqSynHandler::setSeqDt( double v )
{
	seqDt_ = v;
	updateKernel();
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	history_.resize( numHistory, vGetNumSynapses() );
}

double SeqSynHandler::getSeqDt() const
{
	return seqDt_;
}

void SeqSynHandler::setHistoryTime( double v )
{
	historyTime_ = v;
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	history_.resize( numHistory, vGetNumSynapses() );
	updateKernel();
}

double SeqSynHandler::getHistoryTime() const
{
	return historyTime_;
}

void SeqSynHandler::setResponseScale( double v )
{
	responseScale_ = v;
}

double SeqSynHandler::getResponseScale() const
{
	return responseScale_;
}

double SeqSynHandler::getSeqActivation() const
{
	return seqActivation_;
}

double SeqSynHandler::getWeightScale() const
{
	return weightScale_;
}

vector< double >SeqSynHandler::getWeightScaleVec() const
{
	return weightScaleVec_;
}

void SeqSynHandler::setWeightScale( double v )
{
	weightScale_ = v;
}

vector< double > SeqSynHandler::getKernel() const
{
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	vector< double > ret;
	for ( int i = 0; i < numHistory; ++i ) {
		ret.insert( ret.end(), kernel_[i].begin(), kernel_[i].end() );
	}
	return ret;
}

vector< double > SeqSynHandler::getHistory() const
{
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
	int numX = vGetNumSynapses();
	vector< double > ret( numX * numHistory, 0.0 );
	vector< double >::iterator k = ret.begin();
	for ( int i = 0; i < numHistory; ++i ) {
		for ( int j = 0; j < numX; ++j )
			*k++ = history_.get( i, j );
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////

void SeqSynHandler::addSpike(unsigned int index, double time, double weight)
{
	assert( index < synapses_.size() );
	events_.push( PreSynEvent( index, time, weight ) );
	// Strictly speaking this isn't right. If we have a long time lag
	// then the correct placement of the spike may be in another time
	// slice. For now, to get it going for LIF neurons, this will do.
	// Even in the general case we will probably have a very wide window
	// for the latestSpikes slice.
	latestSpikes_[index] += weight;
}

unsigned int SeqSynHandler::addSynapse()
{
	unsigned int newSynIndex = synapses_.size();
	synapses_.resize( newSynIndex + 1 );
	synapses_[newSynIndex].setHandler( this );
	return newSynIndex;
}

void SeqSynHandler::dropSynapse( unsigned int msgLookup )
{
	assert( msgLookup < synapses_.size() );
	synapses_[msgLookup].setWeight( -1.0 );
}

/////////////////////////////////////////////////////////////////////
void SeqSynHandler::vProcess( const Eref& e, ProcPtr p )
{
	// Here we look at the correlations and do something with them.
	int numHistory = static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );

	// Check if we need to do correlations at all.
	if ( numHistory > 0 && kernel_.size() > 0 ) {
		// Check if timestep rolls over a seqDt boundary
		if ( static_cast< int >( p->currTime / seqDt_ ) >
				static_cast< int >( (p->currTime - p->dt) / seqDt_ ) ) {
			history_.rollToNextRow();
			history_.sumIntoRow( latestSpikes_, 0 );
			latestSpikes_.assign( vGetNumSynapses(), 0.0 );

			// Build up the sum of correlations over time
			vector< double > correlVec( vGetNumSynapses(), 0.0 );
			for ( int i = 0; i < numHistory; ++i )
				history_.correl( correlVec, kernel_[i], i );
			if ( responseScale_ > 0.0 ) { // Sum all responses, send to chan
				seqActivation_ = 0.0;
				for ( vector< double >::iterator y = correlVec.begin();
								y != correlVec.end(); ++y )
					seqActivation_ += *y;

				// We'll use the seqActivation_ to send a special msg.
				seqActivation_ *= responseScale_;
			}
			if ( weightScale_ > 0.0 ) { // Short term changes in individual wts
				weightScaleVec_ = correlVec;
				for ( vector< double >::iterator y=weightScaleVec_.begin();
							y != weightScaleVec_.end(); ++y )
					*y *= weightScale_;
			}
		}
	}

	// Here we go through the regular synapse activation calculations.
	// We can't leave it to the base class vProcess, because we need
	// to scale the weights individually in some cases.
	double activation = seqActivation_; // Start with seq activation
	if ( weightScale_ > 0.0 ) {
		while( !events_.empty() && events_.top().time <= p->currTime ) {
			activation += events_.top().weight *
					weightScaleVec_[ events_.top().synIndex ] / p->dt;
			events_.pop();
		}
	} else {
		while( !events_.empty() && events_.top().time <= p->currTime ) {
			activation += events_.top().weight / p->dt;
			events_.pop();
		}
	}
	if ( activation != 0.0 )
		SynHandlerBase::activationOut()->send( e, activation );
}

void SeqSynHandler::vReinit( const Eref& e, ProcPtr p )
{
	// For no apparent reason, priority queues don't have a clear operation.
	while( !events_.empty() )
		events_.pop();
}
