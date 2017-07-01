/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _ELEMENT_VALUE_FINFO_H
#define _ELEMENT_VALUE_FINFO_H

#include <cctype>

/**
 * This variant of ValueFinfo provides facilities to set and get
 * values of fields that require information about the managing Element
 */
template < class T, class F > class ElementValueFinfo: public ValueFinfoBase
{
	public:
		~ElementValueFinfo() {
			delete set_;
			delete get_;
		}

		ElementValueFinfo( const string& name, const string& doc,
			void ( T::*setFunc )( const Eref&, F ),
			F ( T::*getFunc )( const Eref& ) const )
			: ValueFinfoBase( name, doc )
		{
				string setname = "set" + name;
				setname[3] = std::toupper( setname[3] );
				set_ = new DestFinfo(
					setname,
					"Assigns field value.",
					new EpFunc1< T, F >( setFunc ) );

				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc< T, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( set_ );
			// c->registerElementField( set->getFid() );
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const {
			return Field< F >::innerStrSet( tgt.objId(), field, arg );
		}

		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const {
			return Field< F >::innerStrGet(
							tgt.objId(), field, returnValue );
		}

		string rttiType() const {
			return Conv<F>::rttiType();
		}

	private:
};

template < class T, class F > class ReadOnlyElementValueFinfo: public ValueFinfoBase
{
	public:
		~ReadOnlyElementValueFinfo() {
			delete get_;
		}

		ReadOnlyElementValueFinfo( const string& name, const string& doc,
			F ( T::*getFunc )( const Eref& e ) const )
			: ValueFinfoBase( name, doc )
		{
				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetEpFunc< T, F >( getFunc ) );
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
			return Field< F >::innerStrGet(
							tgt.objId(), field, returnValue );
		}

		string rttiType() const {
			return Conv<F>::rttiType();
		}

	private:
};

#endif // _ELEMENT_VALUE_FINFO_H
