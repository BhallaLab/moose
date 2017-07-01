/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _DINFO_H
#define _DINFO_H

class DinfoBase
{
	public:
		DinfoBase()
			: isOneZombie_( 0 )
		{;}
		DinfoBase( bool isOneZombie )
			: isOneZombie_( isOneZombie )
		{;}
		virtual ~DinfoBase()
		{;}
		virtual char* allocData( unsigned int numData ) const = 0;
		virtual void destroyData( char* d ) const = 0;
		virtual unsigned int size() const = 0;
		virtual unsigned int sizeIncrement() const = 0;

		/**
		 * Return a newly allocated copy of the original data, repeated
		 * copyEntries times. Orig data untouched.
		 * Analogous to copying a vector into a bigger one. Repeat the
		 * original data as many times as possible.
		 */
		virtual char* copyData( const char* orig, unsigned int origEntries,
			unsigned int copyEntries, unsigned int startEntry ) const = 0;

		/**
		 * Assigns data contents from 'orig' over to 'copy'. Tiles the
		 * origEntries onto the copyEntries. So if there are fewer
		 * origEntries, the orig data contents are repeated till the
		 * copy is full.
		 */
		virtual void assignData( char* copy, unsigned int copyEntries,
			const char* orig, unsigned int origEntries ) const = 0;

		/*
		static unsigned int size( const D* value ) const = 0;
		static unsigned int serialize( char* buf, const Data* d ) const = 0;
		static unsigned int unserialize( const char* buf, Data* d ) const = 0;
		*/
		virtual bool isA( const DinfoBase* other ) const = 0;

		bool isOneZombie() const {
			return isOneZombie_;
		}
	private:
		const bool isOneZombie_;
};

template< class D > class Dinfo: public DinfoBase
{
	public:
		Dinfo()
			: sizeIncrement_( sizeof( D ) )
		{;}
		Dinfo( bool isOneZombie )
			: DinfoBase( isOneZombie ),
				sizeIncrement_( isOneZombie ? 0 : sizeof( D ) )
		{;}

		char* allocData( unsigned int numData ) const {
			if ( numData == 0 )
				return 0;
			else
				return reinterpret_cast< char* >( new( nothrow) D[ numData ] );
		}

		char* copyData( const char* orig, unsigned int origEntries,
			unsigned int copyEntries, unsigned int startEntry ) const
		{
			if ( origEntries == 0 )
				return 0;
			if ( isOneZombie() )
				copyEntries = 1;

			D* ret = new( nothrow ) D[copyEntries];
			if ( !ret )
				return 0;
			const D* origData = reinterpret_cast< const D* >( orig );
			for ( unsigned int i = 0; i < copyEntries; ++i ) {
				ret[ i ] = origData[ ( i + startEntry ) % origEntries ];
			}

			/*
			D* ret = new D[ numData * numCopies ];
			const D* origData = reinterpret_cast< const D* >( orig );
			for ( unsigned int i = 0; i < numData; ++i ) {
				for ( unsigned int j = 0; j < numCopies; ++j ) {
					ret[ i * numCopies + j ] = origData[ i ];
				}
			}
			*/
			return reinterpret_cast< char* >( ret );
		}

		void assignData( char* data, unsigned int copyEntries,
			const char* orig, unsigned int origEntries ) const
		{
			if ( origEntries == 0 || copyEntries == 0 ||
				orig == 0 || data == 0 ) {
				return;
			}
			if ( isOneZombie() )
				copyEntries = 1;
			const D* origData = reinterpret_cast< const D* >( orig );
			D* tgt = reinterpret_cast< D* >( data );
			for ( unsigned int i = 0; i < copyEntries; ++i ) {
				tgt[ i ] = origData[ i % origEntries ];
			}

			/*
			for ( unsigned int i = 0; i < copyEntries; i+= origEntries ) {
				unsigned int numCopies = origEntries;
				if ( copyEntries - i < origEntries )
					numCopies = copyEntries - i;
				for ( unsigned int j = 0; j < numCopies; ++j ) {
					tgt[i +j] = origData[j];
				}
				// memcpy( data + i * sizeof( D ), orig, sizeof( D ) * numCopies );
				// Memcpy is fast but it does not permit for C++ to do
				// various constructor and assignment operations that are
				// important if D has pointers in it.
			}
			*/
		}

		void destroyData( char* d ) const {
			delete[] reinterpret_cast< D* >( d );
		}

		unsigned int size()  const {
			return sizeof( D );
		}

		/**
		 * This little hack is used for lookups of a data entry in an array.
		 * Normally sizeIncrement is identical to size. However, if it is
		 * a OneZombie then we always need to return the original pointer,
		 * so here sizeIncrement becomes zero.
		 */
		unsigned int sizeIncrement() const {
			return sizeIncrement_;
		}

		/*
		// Will need to specialize for variable size and pointer-containing
		// D.
		static unsigned int serialize( char* buf, const Data* d ) {
			*reinterpret_cast< D* >( buf ) = *static_cast< const D* >( d );
			return sizeof( D );
		}

		static unsigned int unserialize( const char* buf, Data* d ) {
			*d = *reinterpret_cast< const D* >( buf );
			return sizeof( D );
		}
		// Possible problems of dependence here.
		// static const Cinfo* cinfo;
		*/
		bool isA( const DinfoBase* other ) const {
			return dynamic_cast< const Dinfo< D >* >( other );
		}
	private:
		unsigned int sizeIncrement_;
};

template< class D > class ZeroSizeDinfo: public Dinfo< D >
{
	public:
		unsigned int size()  const {
			return 0;
		}
		unsigned int sizeIncrement()  const {
			return 0;
		}
};

#endif // _DINFO_H
