/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _PROC_OPFUNC_H
#define _PROC_OPFUNC_H
/**
 * This specialized set of classes is derived from OpFunc, and is used
 * to handle Process type calls which need to bypass the Queueing system.
 * These include process, reinit, and other intermediate stage process ops.
 */

/*
class ProcOpFuncBase: public OpFunc
{
	public:
		// This key extra function does Process calls.
		virtual void proc( char* obj, const Eref& e, ProcPtr p ) const = 0;
	private:
};

template< class T > class ProcOpFunc: public ProcOpFuncBase
{
	public:
		ProcOpFunc( void ( T::*func )( const Eref& e, ProcPtr ) )
			: func_( func )
			{;}

		bool checkFinfo( const Finfo* s ) const {
			return dynamic_cast< const SrcFinfo1< ProcPtr >* >( s );
		}

		bool checkSet( const SetGet* s ) const {
			return dynamic_cast< const SetGet1< ProcPtr >* >( s );
		}

		bool strSet( const Eref& tgt,
			const string& field, const string& arg ) const {
			ProcInfo p;
			stringstream ss( arg );
			double dt = 1;
			ss >> dt;
			if ( dt <= 0 )
				dt = 1;
			p.dt = dt;
			return SetGet1< ProcPtr >::set( tgt.objId(), field, &p );
		}

		void op( const Eref& e, const double* buf ) const {
			Conv< ProcPtr > arg1( buf );
			(reinterpret_cast< T* >( e.data() )->*func_)( e, *arg1 ) ;
		}


		// This key extra function does Process calls.
		void proc( char* obj, const Eref& e, ProcPtr p ) const {
			( reinterpret_cast< T* >( obj )->*func_)( e, p );
		}

		string rttiType() const {
			return "void";
		}

	private:
		void ( T::*func_ )( const Eref& e, ProcPtr );
};
*/
template< class T > class ProcOpFunc: public EpFunc1< T, ProcPtr >
{
	public:
		ProcOpFunc( void ( T::*func )( const Eref& e, ProcPtr ) )
			: EpFunc1< T, ProcPtr >( func )
			{;}

		string rttiType() const {
			return "const ProcInfo*";
		}
};

#endif //_PROC_OPFUNC_H
