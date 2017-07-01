/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <cstring>
#include "PrepackedBuffer.h"

PrepackedBuffer::PrepackedBuffer(
	const char* data, unsigned int dataSize, unsigned int numEntries )
	: dataSize_( dataSize ), numEntries_( numEntries )
{
	if ( numEntries == 0 )
		individualDataSize_ = dataSize_;
	else
		individualDataSize_ = dataSize_ / numEntries_;

	data_ = new char[ dataSize ];
	memcpy( data_, data, dataSize );
}

PrepackedBuffer::PrepackedBuffer( const PrepackedBuffer& other )
	: dataSize_( other.dataSize_ ),
		numEntries_( other.numEntries_ )
{
	if ( numEntries_ == 0 )
		individualDataSize_ = dataSize_;
	else
		individualDataSize_ = dataSize_ / numEntries_;
	data_ = new char[ dataSize_ ];
	memcpy( data_, other.data_, dataSize_ );
}

PrepackedBuffer::PrepackedBuffer( const char* buf )
	: dataSize_( *reinterpret_cast< const unsigned int * >( buf ) ),
		numEntries_( *reinterpret_cast< const unsigned int * >(
			buf + sizeof( unsigned int ) ) )
{
	if ( numEntries_ == 0 )
		individualDataSize_ = dataSize_;
	else
		individualDataSize_ = dataSize_ / numEntries_;
	data_ = new char[ dataSize_ ];
	memcpy( data_, buf + 2 * sizeof( unsigned int ), dataSize_ );
}

PrepackedBuffer::PrepackedBuffer() // Used to make StrSet happy
	: dataSize_( 0 ), numEntries_( 0 ), individualDataSize_( 0 )
{
	data_ = new char[1];
	data_[0] = '\0';
}

PrepackedBuffer::~PrepackedBuffer()
{
	delete[] data_;
}

const char* PrepackedBuffer::operator[]( unsigned int index ) const
{
	if ( numEntries_ == 0 )
		return data_ ;
	return data_ + ( index % numEntries_ ) * individualDataSize_;
}

unsigned int PrepackedBuffer::conv2buf( char* buf ) const
{
	*reinterpret_cast< unsigned int* >( buf ) = dataSize_;
	buf += sizeof( unsigned int );
	*reinterpret_cast< unsigned int* >( buf ) = numEntries_;
	buf += sizeof( unsigned int );
	memcpy( buf, data_, dataSize_ );
	return size();
}
