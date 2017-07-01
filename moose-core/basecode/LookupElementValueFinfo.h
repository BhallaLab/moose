/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _LOOKUP_ELEMENT_VALUE_FINFO_H
#define _LOOKUP_ELEMENT_VALUE_FINFO_H

/**
 * This class manages field access for fields that have an index, or other
 * lookup handle.
 * Here T is the parent class
 * L is the lookup class
 * F is the field class.
 */
template < class T, class L, class F > class LookupElementValueFinfo: public LookupValueFinfoBase
{
	public:
		~LookupElementValueFinfo() {
			delete set_;
			delete get_;
		}

		LookupElementValueFinfo( const string& name, const string& doc,
			void ( T::*setFunc )( const Eref&, L, F ),
			F ( T::*getFunc )( const Eref&, L ) const )
			: LookupValueFinfoBase( name, doc )
		{
				string setname = "set" + name;
				setname[3] = std::toupper( setname[3] );
				set_ = new DestFinfo(
					setname,
					"Assigns field value.",
					new EpFunc2< T, L, F >( setFunc ) );

				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc1< T, L, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( set_ );
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const {
			string fieldPart = field.substr( 0, field.find( "[" ) );
			string indexPart = field.substr( field.find( "[" ) + 1, field.find( "]" ) );
			return LookupField< L, F >::innerStrSet(
							tgt.objId(), fieldPart, indexPart, arg );
		}

		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const {
			string fieldPart = field.substr( 0, field.find( "[" ) );
			string indexPart = field.substr( field.find( "[" ) + 1, field.find( "]" ) );
			return LookupField< L, F >::innerStrGet(
				tgt.objId(), fieldPart, indexPart, returnValue );
		}

		string rttiType() const {
			return Conv<L>::rttiType() + "," + Conv<F>::rttiType();
		}

	private:
		DestFinfo* set_;
		DestFinfo* get_;
};

template < class T, class L, class F >
	class ReadOnlyLookupElementValueFinfo: public LookupValueFinfoBase
{
	public:
		~ReadOnlyLookupElementValueFinfo() {
			delete get_;
		}

		ReadOnlyLookupElementValueFinfo(
			const string& name, const string& doc,
			F ( T::*getFunc )( const Eref& e, L ) const )
			: LookupValueFinfoBase( name, doc )
		{
				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc1< T, L, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const {
			return 0;
		}

		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const {
			string fieldPart = field.substr( 0, field.find( "[" ) );
			string indexPart = field.substr( field.find( "[" ) + 1, field.find( "]" ) );
			return LookupField< L, F >::innerStrGet(
					tgt.objId(), fieldPart, indexPart, returnValue );
		}

		string rttiType() const {
                        return Conv<L>::rttiType() + "," + Conv<F>::rttiType();
		}

	private:
		DestFinfo* get_;
};

#endif // _LOOKUP_ELEMENT_VALUE_FINFO_H
