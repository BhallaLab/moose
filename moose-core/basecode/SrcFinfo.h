/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _SRC_FINFO_H
#define _SRC_FINFO_H

/**
 * This set of classes define Message Sources. Their main job is to supply
 * a type-safe send operation, and to provide typechecking for it.
 */

class SrcFinfo: public Finfo
{
	public:
		SrcFinfo( const string& name, const string& doc );

		~SrcFinfo() {;}

		void registerFinfo( Cinfo* c );

		///////////////////////////////////////////////////////////////

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const {
			return 0; // always fails
		}

		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const {
			return 0; // always fails
		}

		BindIndex getBindIndex() const;
		void setBindIndex( BindIndex b );

		/**
		 * Checks that the target will work for this Msg.
		 */
		bool checkTarget( const Finfo* target ) const;

		/**
		 * First checks that the target will work, then adds the Msg.
		 */
		bool addMsg( const Finfo* target, ObjId mid, Element* src ) const;

		/**
		 * Sends contents of buffer on to msg targets
		 * Buffer has a header with the TgtInfo.
		 */
		virtual void sendBuffer( const Eref& e, double* buf )
				const = 0;

		static const BindIndex BadBindIndex;
	private:
		/**
		 * Index into the msgBinding_ vector.
		 */
		unsigned short bindIndex_;
};

/**
 * SrcFinfo0 sets up calls without any arguments.
 */
class SrcFinfo0: public SrcFinfo
{
	public:

		SrcFinfo0( const string& name, const string& doc );
		~SrcFinfo0() {;}

		void send( const Eref& e ) const;

		void sendBuffer( const Eref& e, double* buf ) const;

		string rttiType() const {
			return "void";
		}

	private:
};

template< class A > class OpFunc1Base;
// Should specialize for major cases like doubles.

template < class T > class SrcFinfo1: public SrcFinfo
{
	public:
		~SrcFinfo1() {;}

		SrcFinfo1( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		void send( const Eref& er, T arg ) const
		{
			const vector< MsgDigest >& md = er.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc1Base< T >* f =
					dynamic_cast< const OpFunc1Base< T >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numLocalData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg );
					} else  {
						f->op( *j, arg );
						// Need to send stuff offnode too here. The
						// target in this case is just the src Element.
						// Its ObjId gets stuffed into the send buf.
						// On the other node it will execute
						// its own send command with the passed in args.
					}
				}
			}
		}

		void sendTo( const Eref& er, Id tgt, T arg ) const
		{
			const vector< MsgDigest >& md = er.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc1Base< T >* f =
					dynamic_cast< const OpFunc1Base< T >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->element() != tgt.element() )
						continue; // Wasteful unless very few dests.
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numLocalData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg );
					} else  {
						f->op( *j, arg );
						// Need to send stuff offnode too here. The
						// target in this case is just the src Element.
						// Its ObjId gets stuffed into the send buf.
						// On the other node it will execute
						// its own send command with the passed in args.
					}
				}
			}
		}

		/**
		 * Each target gets successive entries from the arg vector.
		 * Rolls over if the # of targets exceeds vector size.
		 * Fails totally if the targets are off-node.
		 */
		void sendVec( const Eref& er, const vector< T >& arg ) const
		{
			if ( arg.size() == 0 )
				return;
			const vector< MsgDigest >& md = er.msgDigest( getBindIndex() );
			unsigned int argPos = 0;
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc1Base< T >* f =
					dynamic_cast< const OpFunc1Base< T >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numLocalData();
						for ( unsigned int k = start; k < end; ++k ) {
							f->op( Eref( e, k ), arg[ argPos++ ] );
							if ( argPos >= arg.size() )
								argPos = 0;
						}
					} else  {
						f->op( *j, arg[ argPos++ ] );
							if ( argPos >= arg.size() )
								argPos = 0;
						// Need to send stuff offnode too here. The
						// target in this case is just the src Element.
						// Its ObjId gets stuffed into the send buf.
						// On the other node it will execute
						// its own send command with the passed in args.
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			send( e, Conv< T >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T>::rttiType();
		}
	private:
};


template< class A1, class A2 > class OpFunc2Base;
// Specialize for doubles.
template < class T1, class T2 > class SrcFinfo2: public SrcFinfo
{
	public:
		~SrcFinfo2() {;}

		SrcFinfo2( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		void send( const Eref& e, const T1& arg1, const T2& arg2 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc2Base< T1, T2 >* f =
					dynamic_cast< const OpFunc2Base< T1, T2 >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg1, arg2 );
					} else  {
						f->op( *j, arg1, arg2 );
					}
				}
			}
		}

		void sendTo( const Eref& e, Id tgt,
						const T1& arg1, const T2& arg2 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc2Base< T1, T2 >* f =
					dynamic_cast< const OpFunc2Base< T1, T2 >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->element() != tgt.element() )
						continue; // Wasteful unless very few dests.
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg1, arg2 );
					} else  {
						f->op( *j, arg1, arg2 );
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			// Note that buf2val alters the buf ptr as a side-effect, in
			// order to advance to the next argument. However, the order
			// in which argument evaluation is called within a multi-
			// argument function is undefined. So we have to explicitly
			// ensure the ordering of argument calls.
			const T1& arg1 = Conv< T1 >::buf2val( &buf );
			send( e, arg1, Conv< T2 >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T1>::rttiType() + "," + Conv< T2 >::rttiType();
		}

	private:
};

template< class A1, class A2, class A3 > class OpFunc3Base;
template < class T1, class T2, class T3 > class SrcFinfo3: public SrcFinfo
{
	public:
		~SrcFinfo3() {;}

		SrcFinfo3( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		void send( const Eref& e,
			const T1& arg1, const T2& arg2, const T3& arg3 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc3Base< T1, T2, T3 >* f =
					dynamic_cast< const OpFunc3Base< T1, T2, T3 >* >(
									i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg1, arg2, arg3 );
					} else  {
						f->op( *j, arg1, arg2, arg3 );
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			const T1& arg1 = Conv< T1 >::buf2val( &buf );
			const T2& arg2 = Conv< T2 >::buf2val( &buf );
			send( e, arg1, arg2, Conv< T3 >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T1>::rttiType() + "," + Conv< T2 >::rttiType() +
				"," + Conv<T3>::rttiType();
		}

	private:
};

template< class A1, class A2, class A3, class A4 > class OpFunc4Base;
template < class T1, class T2, class T3, class T4 > class SrcFinfo4: public SrcFinfo
{
	public:
		~SrcFinfo4() {;}

		SrcFinfo4( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( const Eref& e,
			const T1& arg1, const T2& arg2,
			const T3& arg3, const T4& arg4 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc4Base< T1, T2, T3, T4 >* f =
					dynamic_cast< const OpFunc4Base< T1, T2, T3, T4 >* >(
									i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ), arg1, arg2, arg3, arg4 );
					} else  {
						f->op( *j, arg1, arg2, arg3, arg4 );
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			const T1& arg1 = Conv< T1 >::buf2val( &buf );
			const T2& arg2 = Conv< T2 >::buf2val( &buf );
			const T3& arg3 = Conv< T3 >::buf2val( &buf );
			send( e, arg1, arg2, arg3, Conv< T4 >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T1>::rttiType() + "," + Conv< T2 >::rttiType() +
				"," + Conv<T3>::rttiType() + "," + Conv< T4 >::rttiType();
		}

	private:
};

template< class A1, class A2, class A3, class A4, class A5 >
	class OpFunc5Base;
template < class T1, class T2, class T3, class T4, class T5 > class SrcFinfo5: public SrcFinfo
{
	public:
		~SrcFinfo5() {;}

		SrcFinfo5( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		// Will need to specialize for strings etc.
		void send( const Eref& e,
			const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
			const T5& arg5 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc5Base< T1, T2, T3, T4, T5 >* f =
					dynamic_cast<
					const OpFunc5Base< T1, T2, T3, T4, T5 >* >( i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ),
											arg1, arg2, arg3, arg4, arg5 );
					} else  {
						f->op( *j, arg1, arg2, arg3, arg4, arg5 );
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			const T1& arg1 = Conv< T1 >::buf2val( &buf );
			const T2& arg2 = Conv< T2 >::buf2val( &buf );
			const T3& arg3 = Conv< T3 >::buf2val( &buf );
			const T4& arg4 = Conv< T4 >::buf2val( &buf );
			send( e, arg1, arg2, arg3, arg4, Conv< T5 >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T1>::rttiType() + "," + Conv< T2 >::rttiType() +
				"," + Conv<T3>::rttiType() + "," + Conv< T4 >::rttiType() +
				"," + Conv<T5>::rttiType();
		}

	private:
};


template< class A1, class A2, class A3, class A4, class A5, class A6 >
	class OpFunc6Base;
template < class T1, class T2, class T3, class T4, class T5, class T6 > class SrcFinfo6: public SrcFinfo
{
	public:
		~SrcFinfo6() {;}

		SrcFinfo6( const string& name, const string& doc )
			: SrcFinfo( name, doc )
			{ ; }

		void send( const Eref& e,
			const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
			const T5& arg5, const T6& arg6 ) const
		{
			const vector< MsgDigest >& md = e.msgDigest( getBindIndex() );
			for ( vector< MsgDigest >::const_iterator
				i = md.begin(); i != md.end(); ++i ) {
				const OpFunc6Base< T1, T2, T3, T4, T5, T6 >* f =
					dynamic_cast<
					const OpFunc6Base< T1, T2, T3, T4, T5, T6 >* >(
									i->func );
				assert( f );
				for ( vector< Eref >::const_iterator
					j = i->targets.begin(); j != i->targets.end(); ++j ) {
					if ( j->dataIndex() == ALLDATA ) {
						Element* e = j->element();
						unsigned int start = e->localDataStart();
						unsigned int end = start + e->numData();
						for ( unsigned int k = start; k < end; ++k )
							f->op( Eref( e, k ),
									arg1, arg2, arg3, arg4, arg5, arg6 );
					} else  {
						f->op( *j, arg1, arg2, arg3, arg4, arg5, arg6 );
					}
				}
			}
		}

		void sendBuffer( const Eref& e, double* buf ) const
		{
			const T1& arg1 = Conv< T1 >::buf2val( &buf );
			const T2& arg2 = Conv< T2 >::buf2val( &buf );
			const T3& arg3 = Conv< T3 >::buf2val( &buf );
			const T4& arg4 = Conv< T4 >::buf2val( &buf );
			const T5& arg5 = Conv< T5 >::buf2val( &buf );
			send( e, arg1, arg2, arg3, arg4, arg5, Conv< T6 >::buf2val( &buf ) );
		}

		string rttiType() const {
			return Conv<T1>::rttiType() + "," + Conv< T2 >::rttiType() +
				"," + Conv<T3>::rttiType() + "," + Conv< T4 >::rttiType() +
				"," + Conv<T5>::rttiType() + "," + Conv< T6 >::rttiType();
		}

	private:
};

#endif // _SRC_FINFO_H
