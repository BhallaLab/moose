/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "../randnum/randnum.h"
#include "../shell/Shell.h"

// Initializing static variables
Id SparseMsg::managerId_;
vector< SparseMsg* > SparseMsg::msg_;

//////////////////////////////////////////////////////////////////
//    MOOSE wrapper functions for field access.
//////////////////////////////////////////////////////////////////

const Cinfo* SparseMsg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////////////////
	static ReadOnlyValueFinfo< SparseMsg, unsigned int > numRows(
		"numRows",
		"Number of rows in matrix.",
		&SparseMsg::getNumRows
	);
	static ReadOnlyValueFinfo< SparseMsg, unsigned int > numColumns(
		"numColumns",
		"Number of columns in matrix.",
		&SparseMsg::getNumColumns
	);
	static ReadOnlyValueFinfo< SparseMsg, unsigned int > numEntries(
		"numEntries",
		"Number of Entries in matrix.",
		&SparseMsg::getNumEntries
	);

	static ValueFinfo< SparseMsg, vector< unsigned int > > connectionList(
		"connectionList",
		"Pairwise specification of connection matrix where each x,y value "
		"represents a connection from src[x] to dest[y]. "
		"The (x,y) entries are ordered in a single vector as \n"
		"(x0, x1,... x_n-1, y0, y1,... y_n-1)\n",
		&SparseMsg::setEntryPairs,
		&SparseMsg::getEntryPairs
	);

    /// Connection matrix entries to manipulate in Python.
    static ReadOnlyValueFinfo< SparseMsg, vector< unsigned int > >
    matrixEntry(
        "matrixEntry",
        "The non-zero matrix entries in the sparse matrix. Their"
        "column indices are in a separate vector and the row"
        "informatino in a third",
        &SparseMsg::getMatrixEntry
    );
    /// connection matrix column indices to manipulate in Python.
    static ReadOnlyValueFinfo< SparseMsg, vector< unsigned int > >
    columnIndex(
        "columnIndex",
        "Column Index of each matrix entry",
        &SparseMsg::getColIndex
    );
    /// connection matrix rowStart to manipulate in Python.
    static ReadOnlyValueFinfo< SparseMsg, vector< unsigned int > >
    rowStart(
        "rowStart",
        "Row start for each block of entries and column indices",
        &SparseMsg::getRowStart
    );

	static ValueFinfo< SparseMsg, double > probability(
		"probability",
		"connection probability for random connectivity.",
		&SparseMsg::setProbability,
		&SparseMsg::getProbability
	);

	static ValueFinfo< SparseMsg, long > seed(
		"seed",
		"Random number seed for generating probabilistic connectivity.",
		&SparseMsg::setSeed,
		&SparseMsg::getSeed
	);

////////////////////////////////////////////////////////////////////////
// DestFinfos
////////////////////////////////////////////////////////////////////////

	static DestFinfo setRandomConnectivity( "setRandomConnectivity",
		"Assigns connectivity with specified probability and seed",
		new OpFunc2< SparseMsg, double, long >(
		&SparseMsg::setRandomConnectivity ) );

	static DestFinfo setEntry( "setEntry",
		"Assigns single row,column value",
		new OpFunc3< SparseMsg, unsigned int, unsigned int, unsigned int >(
		&SparseMsg::setEntry ) );

	static DestFinfo unsetEntry( "unsetEntry",
		"Clears single row,column entry",
		new OpFunc2< SparseMsg, unsigned int, unsigned int >(
		&SparseMsg::unsetEntry ) );

	static DestFinfo clear( "clear",
		"Clears out the entire matrix",
		new OpFunc0< SparseMsg >(
		&SparseMsg::clear ) );

	static DestFinfo transpose( "transpose",
		"Transposes the sparse matrix",
		new OpFunc0< SparseMsg >(
		&SparseMsg::transpose ) );

	static DestFinfo pairFill( "pairFill",
		"Fills entire matrix using pairs of (x,y) indices to indicate "
		"presence of a connection. If the target is a FieldElement it"
		"automagically assigns FieldIndices.",
		new OpFunc2< SparseMsg,
			vector< unsigned int >, vector< unsigned int> >(
		&SparseMsg::pairFill ) );

	static DestFinfo tripletFill( "tripletFill",
		"Fills entire matrix using triplets of (x,y,fieldIndex) to fully "
		"specify every connection in the sparse matrix.",
		new OpFunc3< SparseMsg,
			vector< unsigned int >, vector< unsigned int>,
			vector< unsigned int >	>(
		&SparseMsg::tripletFill ) );

	static DestFinfo tripletFill1( "tripletFill1",
		"Single contiguous array to fill entire connection matrix using "
		"triplets of (x,y, fieldindex) ordered as \n"
		"(x0, x1,... xn-1, y0, y1,... yn-1, fi0, fi1,... fi_n-1)\n",
		new OpFunc1< SparseMsg, vector< unsigned int > >(
		&SparseMsg::tripletFill1 ) );

////////////////////////////////////////////////////////////////////////
// Assemble it all.
////////////////////////////////////////////////////////////////////////

	static Finfo* sparseMsgFinfos[] = {
		&numRows,			// readonly value
		&numColumns,		// readonly value
		&numEntries,		// readonly value
		&connectionList,	// value
                &matrixEntry,		// ReadOnlyValue
                &columnIndex,		// ReadOnlyValue
                &rowStart,		// ReadOnlyValue
		&probability,		// value
		&seed,				// value
		&setRandomConnectivity,	// dest
		&setEntry,			// dest
		&unsetEntry,		//dest
		&clear,				//dest
		&transpose,			//dest
		&pairFill,			//dest
		&tripletFill,		//dest
		&tripletFill1,		//dest
	};

	static Dinfo< short > dinfo;
	static Cinfo sparseMsgCinfo (
		"SparseMsg",					// name
		Msg::initCinfo(),			// base class
		sparseMsgFinfos,
		sizeof( sparseMsgFinfos ) / sizeof( Finfo* ),	// num Fields
		&dinfo
	);

	return &sparseMsgCinfo;
}

static const Cinfo* sparseMsgCinfo = SparseMsg::initCinfo();

//////////////////////////////////////////////////////////////////
//    Value Fields
//////////////////////////////////////////////////////////////////
void SparseMsg::setProbability ( double probability )
{
	p_ = probability;
	mtseed( seed_ );
	randomConnect( probability );
}

double SparseMsg::getProbability ( ) const
{
	return p_;
}

void SparseMsg::setSeed ( long seed )
{
	seed_ = seed;
	mtseed( seed_ );
	randomConnect( p_ );
}

long SparseMsg::getSeed () const
{
	return seed_;
}

unsigned int SparseMsg::getNumRows() const
{
	return matrix_.nRows();
}

unsigned int SparseMsg::getNumColumns() const
{
	return matrix_.nColumns();
}

unsigned int SparseMsg::getNumEntries() const
{
	return matrix_.nEntries();
}

vector< unsigned int > SparseMsg::getMatrixEntry() const
{
    return matrix_.matrixEntry();
}

vector< unsigned int > SparseMsg::getColIndex() const
{
    return matrix_.colIndex();
}

vector< unsigned int > SparseMsg::getRowStart() const
{
    return matrix_.rowStart();
}

void SparseMsg::setEntryPairs( vector< unsigned int > v )
{
	vector< unsigned int > src( v.begin(), v.begin() + v.size()/2 );
	vector< unsigned int > dest( v.begin() + v.size()/2, v.end() );
	pairFill( src, dest );
}

vector< unsigned int > SparseMsg::getEntryPairs() const
{
	vector< unsigned int > cols = matrix_.colIndex();
	vector< unsigned int > y;
	for ( unsigned int row = 0; row < matrix_.nRows(); ++row ) {
		unsigned int begin = matrix_.rowStart()[row];
		unsigned int end = matrix_.rowStart()[row+1];
		for ( unsigned int j = begin; j < end; ++j )
			y.push_back( row );
	}
	assert( cols.size() == y.size() );
	y.insert( y.end(), cols.begin(), cols.end() );
	return y;
}

//////////////////////////////////////////////////////////////////
//    DestFields
//////////////////////////////////////////////////////////////////

void SparseMsg::setRandomConnectivity( double probability, long seed )
{
	p_ = probability;
	seed_ = seed;
	mtseed( seed );
	randomConnect( probability );
}

void SparseMsg::setEntry(
	unsigned int row, unsigned int column, unsigned int value )
{
	matrix_.set( row, column, value );
}

void SparseMsg::unsetEntry( unsigned int row, unsigned int column )
{
	matrix_.unset( row, column );
}

void SparseMsg::clear()
{
	matrix_.clear();
}

void SparseMsg::transpose()
{
	matrix_.transpose();
	e1()->markRewired();
	e2()->markRewired();
}

void SparseMsg::updateAfterFill()
{
	unsigned int startData = e2_->localDataStart();
	unsigned int endData = startData + e2_->numLocalData();
	SparseMatrix< unsigned int > temp( matrix_);
	temp.transpose();
	for ( unsigned int i = 0; i < temp.nRows(); ++ i ) {
		const unsigned int* colIndex;
		const unsigned int* entry;
		unsigned int num = temp.getRow( i, &entry, &colIndex );
		if ( i >= startData && i < endData ) {
			// Inefficient. Better to do it in one pass after getting
			// the max num
			e2_->resizeField( i - startData, num + 1 );
		}
	}
	e1()->markRewired();
	e2()->markRewired();
}

void SparseMsg::pairFill( vector< unsigned int > src,
			vector< unsigned int> dest )
{
	// Sanity check
	vector< unsigned int >::const_iterator i;
	for ( i = src.begin(); i != src.end(); ++i ) {
		if (*i >= e1()->numData() ) {
			cout << "Warning: SparseMsg::pairFill: Src index " << *i <<
				   " exceeds Src array size " << e1()->numData() <<
				   ". Aborting\n";
			return;
		}
	}
	for ( i = dest.begin(); i != dest.end(); ++i ) {
		if (*i >= e2()->numData() ) {
			cout << "Warning: SparseMsg::pairFill: Dest index " << *i <<
				   " exceeds Dest array size " << e2()->numData() <<
				   ". Aborting\n";
			return;
		}
	}

	vector< unsigned int > numAtDest( dest.size(), 0 );
	vector< unsigned int > fieldIndex( dest.size(), 0 );
	for ( unsigned int i = 0; i < dest.size(); ++i ) {
		fieldIndex[i] = numAtDest[ dest[i] ];
		// Could do on previous line, but clarity
		++numAtDest[ dest[i] ];
	}

	/**
	 * We set retainSize flag to true since the # of src/dest objects
	 * doesn't change. We can explicitly assign it elsewhere if needed.
	 */
	matrix_.tripletFill( src, dest, fieldIndex, true );
	updateAfterFill();
}

void SparseMsg::tripletFill( vector< unsigned int > src,
			vector< unsigned int> destDataIndex,
			vector< unsigned int> destFieldIndex )
{
	// We set retainSize flag to true
	matrix_.tripletFill( src, destDataIndex, destFieldIndex, true );
	updateAfterFill();
}

void SparseMsg::tripletFill1( vector< unsigned int > v )
{
	unsigned int s3 = v.size() / 3;
	vector< unsigned int > src( v.begin(), v.begin() + s3 );
	vector< unsigned int > dest( v.begin() + s3, v.begin() + 2 * s3 );
	vector< unsigned int > fieldIndex( v.begin() + 2 * s3, v.end() );
	tripletFill( src, dest, fieldIndex );
}

//////////////////////////////////////////////////////////////////
//    Here are the actual class functions
//////////////////////////////////////////////////////////////////


SparseMsg::SparseMsg( Element* e1, Element* e2, unsigned int msgIndex )
	: Msg( ObjId( managerId_, (msgIndex != 0) ? msgIndex: msg_.size() ),
					e1, e2 ),
				numThreads_( 1 ),
				nrows_( 0 ),
				p_( 0.0 ), seed_( 0 )
{
	unsigned int nrows = 0;
	unsigned int ncolumns = 0;
	nrows = e1->numData();
	ncolumns = e2->numData();
	matrix_.setSize( nrows, ncolumns );
	if ( msgIndex == 0 ) {
		msg_.push_back( this );
	} else {
		if ( msg_.size() <= msgIndex )
			msg_.resize( msgIndex + 1 );
		msg_[ msgIndex ] = this;
	}

	// cout << Shell::myNode() << ": SparseMsg constructor between " << e1->getName() << " and " << e2->getName() << endl;
}

SparseMsg::~SparseMsg()
{
	assert( mid_.dataIndex < msg_.size() );
	msg_[ mid_.dataIndex ] = 0; // ensure deleted ptr isn't reused.
}

unsigned int rowIndex( const Element* e, const DataId& d )
{
	// FieldDataHandlerBase* fdh = dynamic_cast< FieldDataHandlerBase* >( e->dataHandler() );

	return d;
}


Eref SparseMsg::firstTgt( const Eref& src ) const
{
	if ( matrix_.nEntries() == 0 )
		return Eref( 0, 0 );

	if ( src.element() == e1_ ) {
		const unsigned int* fieldIndex;
		const unsigned int* colIndex;
		unsigned int n = matrix_.getRow( src.dataIndex(),
			&fieldIndex, &colIndex );
		if ( n != 0 ) {
			return Eref( e2_, colIndex[0], fieldIndex[0] );
		}
	} else if ( src.element() == e2_ ) {
		return Eref( e1_, 0 );
	}
	return Eref( 0, 0 );
}

/**
 * Returns number of synapses formed.
 * Fills it in transpose form, because we need to count and index the
 * number of synapses on the target, so we need to iterate over the sources
 * in the inner loop. Once full, does the transpose.
 * Should really have a seed argument for the random number.
 * Later need a way to fast-forward mtrand to just the entries we
 * need to fill.
 */
unsigned int SparseMsg::randomConnect( double probability )
{
	unsigned int nRows = matrix_.nRows(); // Sources
	unsigned int nCols = matrix_.nColumns();	// Destinations
	matrix_.clear();
	unsigned int totalSynapses = 0;
	vector< unsigned int > sizes( nCols, 0 );
	unsigned int totSynNum = 0;
	Element* syn = e2_;
	unsigned int startData = syn->localDataStart();
	unsigned int endData = startData + syn->numLocalData();

	assert( nCols == syn->numData() );

	matrix_.transpose();
	for ( unsigned int i = 0; i < nCols; ++i ) {
		vector< unsigned int > synIndex;
		// This needs to be obtained from current size of syn array.
		// unsigned int synNum = sizes[ i ];
		unsigned int synNum = 0;
		for ( unsigned int j = 0; j < nRows; ++j ) {
			double r = mtrand(); // Want to ensure it is called each time round the loop.
			if ( r < probability ) {
				synIndex.push_back( synNum );
				++synNum;
				++totSynNum;
			} else {
				synIndex.push_back( ~0 );
			}
		}

		if ( i >= startData && i < endData ) {
			e2_->resizeField( i - startData, synNum );
		}
			totalSynapses += synNum;
			matrix_.addRow( i, synIndex );
		/*
		 * This is the correct form, but I need to implement something
		 * to check up for target nodes in order to use this.
		if ( i >= startData && i < endData ) {
			e2_->resizeField( i - startData, synNum );
			totalSynapses += synNum;
			matrix_.addRow( i, synIndex );
		} else {
			synIndex.resize( 0 );
			synIndex.assign( nRows, ~0 );
			matrix_.addRow( i, synIndex );
		}
		*/
	}

	matrix_.transpose();
	// cout << Shell::myNode() << ": sizes.size() = " << sizes.size() << ", ncols = " << nCols << ", startSynapse = " << startSynapse << endl;
	e1()->markRewired();
	e2()->markRewired();
	return totalSynapses;
}

Id SparseMsg::managerId() const
{
	return SparseMsg::managerId_;
}

void SparseMsg::setMatrix( const SparseMatrix< unsigned int >& m )
{
	matrix_ = m;
}

SparseMatrix< unsigned int >& SparseMsg::getMatrix( )
{
	return matrix_;
}

ObjId SparseMsg::findOtherEnd( ObjId f ) const
{
	if ( f.element() == e1() ) {
		const unsigned int* entry;
		const unsigned int* colIndex;
		unsigned int num = matrix_.getRow( f.dataIndex, &entry, &colIndex );
		if ( num > 0 ) { // Return the first matching entry.
			return ObjId( e2()->id(), colIndex[0] );
		}
		return ObjId( 0, BADINDEX );
	} else if ( f.element() == e2() ) { // Bad! Slow! Avoid!
		vector< unsigned int > entry;
		vector< unsigned int > rowIndex;
		unsigned int num = matrix_.getColumn( f.dataIndex, entry, rowIndex );
		if ( num > 0 ) { // Return the first matching entry.
				return ObjId( e1()->id(), DataId( rowIndex[0] ) );
		}
	}
	return ObjId( 0, BADINDEX );
}

Msg* SparseMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc.element();
	if ( n <= 1 ) {
		SparseMsg* ret = 0;
		if ( orig == e1() ) {
			ret = new SparseMsg( newSrc.element(), newTgt.element(), 0 );
			ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
		} else if ( orig == e2() ) {
			ret = new SparseMsg( newTgt.element(), newSrc.element(), 0 );
			ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
		} else {
			assert( 0 );
		}
		ret->setMatrix( matrix_ );
		ret->nrows_ = nrows_;
		return ret;
	} else {
		// Here we need a SliceMsg which goes from one 2-d array to another.
		cout << "Error: SparseMsg::copy: SparseSliceMsg not yet implemented\n";
		return 0;
	}
}

void fillErefsFromMatrix(
		const SparseMatrix< unsigned int >& matrix,
		vector< vector < Eref > >& v, Element* e1, Element* e2 )
{
	v.clear();
	v.resize( e1->numData() );
	assert( e1->numData() == matrix.nRows() );
	assert( e2->numData() == matrix.nColumns() );
	for ( unsigned int i = 0; i < e1->numData(); ++i ) {
		const unsigned int* entry;
		const unsigned int* colIndex;
		unsigned int num = matrix.getRow( i, &entry, &colIndex );
		v[i].resize( num );
		for ( unsigned int j = 0; j < num; ++j ) {
			v[i][j] = Eref( e2, colIndex[j], entry[j] );
		}
	}
}

void SparseMsg::sources( vector< vector < Eref > >& v ) const
{
	SparseMatrix< unsigned int > temp( matrix_ );
	temp.transpose();
	fillErefsFromMatrix( temp, v, e2_, e1_ );
}

void SparseMsg::targets( vector< vector< Eref > >& v ) const
{
	fillErefsFromMatrix( matrix_, v, e1_, e2_ );
}

/// Static function for Msg access
unsigned int SparseMsg::numMsg()
{
	return msg_.size();
}

/// Static function for Msg access
char* SparseMsg::lookupMsg( unsigned int index )
{
	assert( index < msg_.size() );
	return reinterpret_cast< char* >( msg_[index] );
}
