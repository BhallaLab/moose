/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <vector>
using namespace std;
#include "RollingMatrix.h"
#include <cassert>

RollingMatrix::RollingMatrix()
		: nrows_(0), ncolumns_(0), currentStartRow_(0)
{;}


RollingMatrix::~RollingMatrix()
{;}

RollingMatrix& RollingMatrix::operator=( const RollingMatrix& other )
{
	nrows_ = other.nrows_;
	ncolumns_ = other.ncolumns_;
	currentStartRow_ = other.currentStartRow_;
	rows_ = other.rows_;
	return *this;
}


void RollingMatrix::resize( unsigned int nrows, unsigned int ncolumns )
{
	rows_.resize( nrows );
	nrows_ = nrows;
	ncolumns_ = ncolumns;
	for ( unsigned int i = 0; i < nrows; ++i ) {
		rows_[i].resize( ncolumns, 0.0 );
	}
	currentStartRow_ = 0;
}

double RollingMatrix::get( unsigned int row, unsigned int column ) const
{
	unsigned int index = (row + currentStartRow_ ) % nrows_;
	return rows_[index][column];
}

void RollingMatrix::sumIntoEntry( double input, unsigned int row, unsigned int column )
{
	unsigned int index = (row + currentStartRow_ ) % nrows_;
	SparseVector& sv = rows_[index];
	sv[column] += input;
}

void RollingMatrix::sumIntoRow( const vector< double >& input, unsigned int row )
{
	unsigned int index = (row + currentStartRow_) % nrows_;
	SparseVector& sv = rows_[index];

	for (unsigned int i = 0; i < input.size(); ++i )
		sv[i] += input[i];
}


double RollingMatrix::dotProduct( const vector< double >& input,
				unsigned int row, unsigned int startColumn ) const
{
	unsigned int index = (row + currentStartRow_) % nrows_;
	const SparseVector& sv = rows_[index];

	double ret = 0;
	if ( input.size() + startColumn <= sv.size() ) {
		for (unsigned int i = 0; i < input.size(); ++i )
			ret += sv[i + startColumn] * input[i];
	} else if ( sv.size() > startColumn ) {
		unsigned int end = sv.size() - startColumn;
		for (unsigned int i = 0; i < end; ++i )
			ret += sv[i + startColumn] * input[i];
	}
	return ret;
}

void RollingMatrix::correl( vector< double >& ret,
				const vector< double >& input, unsigned int row) const

{
	if ( ret.size() < ncolumns_ )
		ret.resize( ncolumns_, 0.0 );
	for ( unsigned int i = 0; i < ncolumns_; ++i ) {
		ret[i] += dotProduct( input, row, i );
	}
}

void RollingMatrix::zeroOutRow( unsigned int row )
{
	unsigned int index = (row + currentStartRow_) % nrows_;
	rows_[index].assign( rows_[index].size(), 0.0 );
}

void RollingMatrix::rollToNextRow()
{
	if ( currentStartRow_ == 0 )
		currentStartRow_ = nrows_ - 1;
	else
		currentStartRow_--;
	zeroOutRow( 0 );
}
