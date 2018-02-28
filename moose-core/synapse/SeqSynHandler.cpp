/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "global.h"
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
			"*sequenceScale* term, and send to the synapse as a steady "
			"activation. Consider this a cell-wide immediate response to "
			"a sequence that it likes.\n"
			"2. It do an instantaneous scaling of the weight of each "
			"individual synapse by the corresponding entry in the response "
			"vector. It uses the *plasticityScale* term to do this. "
			"Consider "
			"this a short-term plasticity effect on specific synapses. \n"
			"3. It can do long-term plasticity of each individual synapse "
			"using the matched local entries in the response vector and "
			"individual synapse history as inputs to the learning rule. "
			"This is not yet implemented.\n"
			"In addition to all these, the SeqSynHandler can act just like "
			"a regular synapse, where it responds to individual synaptic "
			"input according to the weight of the synapse. The size of "
			"this component of the output is scaled by *baseScale*\n"
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
	static ValueFinfo< SeqSynHandler, double > baseScale(
			"baseScale",
			"Basal scaling factor for regular synaptic activation.",
			&SeqSynHandler::setBaseScale,
			&SeqSynHandler::getBaseScale
	);
	static ValueFinfo< SeqSynHandler, double > sequenceScale(
			"sequenceScale",
			"Scaling factor for sustained activation of synapse by seq",
			&SeqSynHandler::setSequenceScale,
			&SeqSynHandler::getSequenceScale
	);
	static ValueFinfo< SeqSynHandler, vector< unsigned int > > synapseOrder(
			"synapseOrder",
			"Mapping of synapse input order to spatial order on syn array."
			"Entries in this vector are indices which must remain smaller "
			"than numSynapses. The system will fix up if you mess up. "
			"It does not insist on unique mappings, but these are "
			"desirable as outcome is undefined for repeated entries.",
			&SeqSynHandler::setSynapseOrder,
			&SeqSynHandler::getSynapseOrder
	);
	static ValueFinfo< SeqSynHandler, int > synapseOrderOption(
			"synapseOrderOption",
			"How to do the synapse order remapping. This rule stays in "
			"place and guarantees safe mappings even if the number of "
			"synapses is altered. Options:\n"
		    "-2: User ordering.\n"
		    "-1: Sequential ordering, 0 to numSynapses-1.\n"
		    "0: Random ordering using existing system seed.\n"
		    ">0: Random ordering using seed specified by this number\n"
			"Default is -1, sequential ordering.",
			&SeqSynHandler::setSynapseOrderOption,
			&SeqSynHandler::getSynapseOrderOption
	);
	static ReadOnlyValueFinfo< SeqSynHandler, double > seqActivation(
			"seqActivation",
			"Reports summed activation of synaptic channel by sequence",
			&SeqSynHandler::getSeqActivation
	);
	static ValueFinfo< SeqSynHandler, double > plasticityScale(
			"plasticityScale",
			"Scaling factor for doing plasticity by scaling each synapse by response vector",
			&SeqSynHandler::setPlasticityScale,
			&SeqSynHandler::getPlasticityScale
	);

	static ValueFinfo< SeqSynHandler, double > sequencePower(
			"sequencePower",
			"Exponent for the outcome of the sequential calculations. "
			"This is needed because linear summation of terms in the kernel"
			"means that a brief stong sequence match is no better than lots"
			"of successive low matches. In other words, 12345 is no better"
			"than 11111. Using an exponent lets us select the former."
			"Defaults to 1.0.",
			&SeqSynHandler::setSequencePower,
			&SeqSynHandler::getSequencePower
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
		&sequenceScale,				// Field
		&baseScale,					// Field
		&synapseOrder,				// Field
		&synapseOrderOption,		// Field
		&seqActivation,				// ReadOnlyField
		&plasticityScale,			// Field
		&sequencePower,				// Field
		&weightScaleVec,			// ReadOnlyField
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
		baseScale_( 0.0 ),
		sequenceScale_( 1.0 ),
		plasticityScale_( 0.0 ),
		sequencePower_( 1.0 ),
		seqActivation_( 0.0 ),
		synapseOrderOption_( -1 ) // sequential ordering
{
	history_.resize( numHistory(), 0 );
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

	history_.resize( numHistory(), v );
	latestSpikes_.resize( v, 0.0 );
	weightScaleVec_.resize( v, 0.0 );
	refillSynapseOrder( v );
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

// Checks for numbers bigger than the size. Replaces with
// values within the range that have not yet been used.
void SeqSynHandler::fixSynapseOrder()
{
	unsigned int sz = synapseOrder_.size();
	vector< unsigned int > availableEntries( sz );
	iota( availableEntries.begin(), availableEntries.end(), 0 );
	for( unsigned int i = 0; i < sz; ++i ) {
		if ( synapseOrder_[i] < sz )
			availableEntries[ synapseOrder_[i] ] = sz;
	}
	vector< unsigned int > ae;
	for( unsigned int i = 0; i < sz; ++i )
		if ( availableEntries[i] < sz )
			ae.push_back( availableEntries[i] );

	auto jj = ae.begin();
	for( unsigned int i = 0; i < sz; ++i ) {
		if ( synapseOrder_[i] >= sz )
			synapseOrder_[i] = *jj++;
	}
}

// Beautiful snippet from Lukasz Wiklendt on StackOverflow. Returns order
// of entries in a vector.
template <typename T> vector<size_t> sort_indexes(const vector<T> &v) {
	// initialize original index locations
	vector<size_t> idx(v.size());
	iota(idx.begin(), idx.end(), 0);
	// sort indexes based on comparing values in v
	sort(idx.begin(), idx.end(),
		[&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
	return idx;
}

void SeqSynHandler::refillSynapseOrder( unsigned int newSize )
{
	if ( synapseOrderOption_ <= -2 ) { // User order
		synapseOrder_.resize( newSize, newSize );
		fixSynapseOrder();
	} else if ( synapseOrderOption_ == -1 ) { // Ordered
		synapseOrder_.resize( newSize );
		for ( unsigned int i = 0 ; i < newSize; ++i )
			synapseOrder_[i] = i;
	} else {
		synapseOrder_.resize( newSize );
		if ( synapseOrderOption_ > 0 ) { // Specify seed explicitly
                    moose::mtseed( synapseOrderOption_ );
		}
		vector< double > x;
		for ( unsigned int i = 0; i < newSize; ++i )
			x.push_back( moose::mtrand() );
		auto idx = sort_indexes< double >( x );
		for ( unsigned int i = 0; i < newSize; ++i )
			synapseOrder_[i] = idx[i];
	}
}

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
	int nh = numHistory();
	kernel_.resize( nh );
	for ( int i = 0; i < nh; ++i ) {
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
	history_.resize( numHistory(), vGetNumSynapses() );
}

double SeqSynHandler::getSeqDt() const
{
	return seqDt_;
}

void SeqSynHandler::setHistoryTime( double v )
{
	historyTime_ = v;
	history_.resize( numHistory(), vGetNumSynapses() );
	updateKernel();
}

double SeqSynHandler::getHistoryTime() const
{
	return historyTime_;
}

void SeqSynHandler::setBaseScale( double v )
{
	baseScale_ = v;
}

double SeqSynHandler::getBaseScale() const
{
	return baseScale_;
}

void SeqSynHandler::setSequenceScale( double v )
{
	sequenceScale_ = v;
}

double SeqSynHandler::getSequenceScale() const
{
	return sequenceScale_;
}

double SeqSynHandler::getSeqActivation() const
{
	return seqActivation_;
}

double SeqSynHandler::getPlasticityScale() const
{
	return plasticityScale_;
}

void SeqSynHandler::setPlasticityScale( double v )
{
	plasticityScale_ = v;
}

double SeqSynHandler::getSequencePower() const
{
	return sequencePower_;
}

void SeqSynHandler::setSequencePower( double v )
{
	sequencePower_ = v;
}

vector< double >SeqSynHandler::getWeightScaleVec() const
{
	return weightScaleVec_;
}

vector< double > SeqSynHandler::getKernel() const
{
	int nh = numHistory();
	vector< double > ret;
	for ( int i = 0; i < nh; ++i ) {
		ret.insert( ret.end(), kernel_[i].begin(), kernel_[i].end() );
	}
	return ret;
}

vector< double > SeqSynHandler::getHistory() const
{
	int nh = numHistory();
	int numX = vGetNumSynapses();
	vector< double > ret( numX * nh, 0.0 );
	vector< double >::iterator k = ret.begin();
	for ( int i = 0; i < nh; ++i ) {
		for ( int j = 0; j < numX; ++j )
			*k++ = history_.get( i, j );
	}
	return ret;
}

void SeqSynHandler::setSynapseOrder( vector< unsigned int > v )
{
	synapseOrder_ = v;
	fixSynapseOrder();
	synapseOrderOption_ = -2; // Set the flag to say it is User defined.
}

vector< unsigned int > SeqSynHandler::getSynapseOrder() const
{
	return synapseOrder_;
}

void SeqSynHandler::setSynapseOrderOption( int v )
{
	synapseOrderOption_ = v;
	refillSynapseOrder( synapseOrder_.size() );
}

int SeqSynHandler::getSynapseOrderOption() const
{
	return synapseOrderOption_;
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
	//
	// Here we reorder the entries in latestSpikes by the synapse order.
	latestSpikes_[ synapseOrder_[index] ] += weight;
}

double SeqSynHandler::getTopSpike( unsigned int index ) const
{
	if ( events_.empty() )
		return 0.0;
	return events_.top().time;
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
	int nh = numHistory();

	// Check if we need to do correlations at all.
	if ( nh > 0 && kernel_.size() > 0 ) {
		// Check if timestep rolls over a seqDt boundary
		if ( static_cast< int >( p->currTime / seqDt_ ) >
				static_cast< int >( (p->currTime - p->dt) / seqDt_ ) ) {
			history_.rollToNextRow();
			history_.sumIntoRow( latestSpikes_, 0 );
			latestSpikes_.assign( vGetNumSynapses(), 0.0 );

			// Build up the sum of correlations over time
			vector< double > correlVec( vGetNumSynapses(), 0.0 );
			for ( int i = 0; i < nh; ++i )
				history_.correl( correlVec, kernel_[i], i );
			if ( sequenceScale_ > 0.0 ) { // Sum all responses, send to chan
				seqActivation_ = 0.0;
				for ( vector< double >::iterator y = correlVec.begin();
								y != correlVec.end(); ++y )
					seqActivation_ += pow( *y, sequencePower_ );

				// We'll use the seqActivation_ to send a special msg.
				seqActivation_ *= sequenceScale_;
			}
			if ( plasticityScale_ > 0.0 ) { // Short term changes in individual wts
				weightScaleVec_ = correlVec;
				for ( vector< double >::iterator y=weightScaleVec_.begin();
							y != weightScaleVec_.end(); ++y )
					*y *= plasticityScale_;
			}
		}
	}

	// Here we go through the regular synapse activation calculations.
	// We can't leave it to the base class vProcess, because we need
	// to scale the weights individually in some cases.
	double activation = seqActivation_; // Start with seq activation
	if ( plasticityScale_ > 0.0 ) {
		while( !events_.empty() && events_.top().time <= p->currTime ) {
			activation += events_.top().weight * baseScale_ *
					weightScaleVec_[ events_.top().synIndex ] / p->dt;
			events_.pop();
		}
	} else {
		while( !events_.empty() && events_.top().time <= p->currTime ) {
			activation += baseScale_ * events_.top().weight / p->dt;
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

int SeqSynHandler::numHistory() const
{
	return static_cast< int >( 1.0 + floor( historyTime_ * (1.0 - 1e-6 ) / seqDt_ ) );
}
