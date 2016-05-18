/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _EPFUNC_H
#define _EPFUNC_H

/**
 * Utility function to return a pointer of the desired type from the 
 * Eref data. Mainly here because it lets us specialize for Neutrals,
 * below.
 */
template< class T > T* getEpFuncData( const Eref& e )
{
	return reinterpret_cast< T* >( e.data() ) ;
}

/**
 * This is a template specialization for GetEpFunc applied to Neutrals.
 * This is necessary in order to access Element fields of objects that 
 * may not have been allocated (such as synapses), even though their Element
 * has been created and needs to be manipulated.
 * Apparently regular functions with same args will be preferred
 * over templates, let's see if this works. Nope, it doesn't. Good try.
	extern Neutral* getEpFuncData( const Eref& e );
 * Try this form: it doesn't work either.
extern Neutral* dummyNeutral();
template<> Neutral* getEpFuncData< Neutral >( const Eref& e )
{
	return dummyNeutral();
}
 * Try externing the template itself.
 */
template<> Neutral* getEpFuncData< Neutral >( const Eref& e );
// Function is implemented in EpFunc.cpp.



/**
 * This set of classes is derived from OpFunc, and take extra args
 * for the qinfo and Eref.
 */

template< class T > class EpFunc0: public OpFunc0Base
{
	public:
		EpFunc0( void ( T::*func )( const Eref& e ) )
			: func_( func )
			{;}

		void op( const Eref& e ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( e );
		}

	private:
		void ( T::*func_ )( const Eref& e ); 
};

template< class T, class A > class EpFunc1: public OpFunc1Base< A >
{
	public:
		EpFunc1( void ( T::*func )( const Eref&, A ) )
			: func_( func )
			{;}

		void op( const Eref& e, A arg ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( e, arg );
		}

	private:
		void ( T::*func_ )( const Eref& e, A ); 
};

template< class T, class A1, class A2 > class EpFunc2: 
		public OpFunc2Base< A1, A2 >
{
	public:
		EpFunc2( void ( T::*func )( const Eref&, A1, A2 ) )
			: func_( func )
			{;}

		void op( const Eref& e, A1 arg1, A2 arg2 ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( e, arg1, arg2 );
		}

	private:
		void ( T::*func_ )( const Eref& e, A1, A2 ); 
};

template< class T, class A1, class A2, class A3 > class EpFunc3:
	public OpFunc3Base< A1, A2, A3 >
{
	public:
		EpFunc3( void ( T::*func )( const Eref&, A1, A2, A3 ) )
			: func_( func )
			{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3 ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( 
							e, arg1, arg2, arg3 );
		}

	private:
		void ( T::*func_ )( const Eref& e, A1, A2, A3 ); 
};

template< class T, class A1, class A2, class A3, class A4 > class EpFunc4:
	public OpFunc4Base< A1, A2, A3, A4 >
{
	public:
		EpFunc4( void ( T::*func )( const Eref&, A1, A2, A3, A4 ) )
			: func_( func )
			{;}

		void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3, A4 arg4 ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( 
							e, arg1, arg2, arg3, arg4 );
		}

	private:
		void ( T::*func_ )( const Eref& e, A1, A2, A3, A4 ); 
};

template< class T, class A1, class A2, class A3, class A4, class A5 > 
	class EpFunc5: public OpFunc5Base< A1, A2, A3, A4, A5 >
{
	public:
		EpFunc5( void ( T::*func )( const Eref&, A1, A2, A3, A4, A5 ) )
			: func_( func )
			{;}

		void op( const Eref& e, 
				A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5 ) const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( 
							e, arg1, arg2, arg3, arg4, arg5 );
		}
	private:
		void ( T::*func_ )( const Eref& e, A1, A2, A3, A4, A5 ); 
};

template< class T, 
		class A1, class A2, class A3, class A4, class A5, class A6 > 
		class EpFunc6: public OpFunc6Base< A1, A2, A3, A4, A5, A6 >
{
	public:
		EpFunc6( void ( T::*func )( const Eref&, A1, A2, A3, A4, A5, A6 ) )
			: func_( func )
			{;}

		void op( const Eref& e, 
				A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6 )
			   	const {
			( reinterpret_cast< T* >( e.data() )->*func_ )( 
							e, arg1, arg2, arg3, arg4, arg5, arg6 );
		}

	private:
		void ( T::*func_ )( const Eref& e, A1, A2, A3, A4, A5, A6 ); 
};

/**
 * This specialized EpFunc is for returning a single field value.
 * Unlike the regular GetOpFunc, this variant takes the Eref
 * and Qinfo.
 * It generates an opFunc that takes a single argument:
 * FuncId of the function on the object that requested the
 * value. The EpFunc then sends back a message with the info.
 */
template< class T, class A > class GetEpFunc: public GetOpFuncBase< A >
{
	public:
		GetEpFunc( A ( T::*func )( const Eref& e ) const )
			: func_( func )
			{;}

		void op( const Eref& e, vector< A >* ret ) const {
			ret->push_back( returnOp( e ) );
		}
		/*
		void op( const Eref& e, ObjId recipient, FuncId fid ) const {
			const OpFunc *f = recipient.element()->cinfo()->getOpFunc( fid);
			const OpFunc1Base< A >* recvOpFunc =
				dynamic_cast< const OpFunc1Base< A >* >( f );
			assert( recvOpFunc );
			recvOpFunc->op( recipient.eref(), returnOp( e ) );
		}
		*/

		A returnOp( const Eref& e ) const {
			return ( getEpFuncData< T >( e )->*func_ )( e );
		}

	private:
		A ( T::*func_ )( const Eref& e ) const;
};


/**
 * This specialized EpFunc is for returning a single field value,
 * but the field lookup requires an index argument as well.
 * Unlike the regular GetOpFunc, this variant takes the Eref
 * and Qinfo.
 * It generates an opFunc that takes a single argument:
 * FuncId of the function on the object that requested the
 * value. The EpFunc then sends back a message with the info.
 */
template< class T, class L, class A > class GetEpFunc1: 
		public LookupGetOpFuncBase< L, A >
{
	public:
		GetEpFunc1( A ( T::*func )( const Eref& e, L ) const )
			: func_( func )
			{;}


		void op( const Eref& e, L index, ObjId recipient, FuncId fid ) 
				const {
			const OpFunc *f = recipient.element()->cinfo()->getOpFunc( fid);
			const OpFunc1Base< A >* recvOpFunc =
				dynamic_cast< const OpFunc1Base< A >* >( f );
			assert( recvOpFunc );
			recvOpFunc->op( recipient.eref(), returnOp( e, index ) );
		}

		A returnOp( const Eref& e, const L& index ) const {
			return ( reinterpret_cast< T* >( e.data() )->*func_)( 
				e, index );
		}

	private:
		A ( T::*func_ )( const Eref& e, L ) const;
};

#endif //_EPFUNC_H
