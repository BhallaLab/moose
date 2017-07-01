/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _VALUE_FINFO_H
#define _VALUE_FINFO_H

/**
 * This is the base class for all ValueFinfo classes. Used for doing
 * inspection using dynamic casts.
 */
class ValueFinfoBase: public Finfo
{
	public:
		~ValueFinfoBase()
		{;}

		ValueFinfoBase( const string& name, const string& doc );

		DestFinfo* getFinfo() const;

		///////////////////////////////////////////////////////////////
		// Override the default virtual function for the set/get destfinfos
		///////////////////////////////////////////////////////////////

		vector< string > innerDest() const;
	protected:
		DestFinfo* set_;
		DestFinfo* get_;
};

template < class T, class F > class ValueFinfo: public ValueFinfoBase
{
	public:
		~ValueFinfo() {
			delete set_;
			delete get_;
		}

		ValueFinfo( const string& name, const string& doc,
			void ( T::*setFunc )( F ),
			F ( T::*getFunc )() const )
			: ValueFinfoBase( name, doc )
		{
				string setname = "set" + name;
				setname[3] = std::toupper( setname[3] );
				set_ = new DestFinfo(
					setname,
					"Assigns field value.",
					new OpFunc1< T, F >( setFunc ) );

				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetOpFunc< T, F >( getFunc ) );
		}


		void registerFinfo( Cinfo* c ) {
			c->registerFinfo( set_ );
			c->registerFinfo( get_ );
		}

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const {
			return Field< F >::innerStrSet( tgt.objId(), field, arg );
		}

		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const {
			return Field< F >::innerStrGet( tgt.objId(), field, returnValue );
		}

		string rttiType() const {
			return Conv<F>::rttiType();
		}
	private:
};

template < class T, class F > class ReadOnlyValueFinfo: public ValueFinfoBase
{
	public:
		~ReadOnlyValueFinfo() {
			delete get_;
		}

		ReadOnlyValueFinfo( const string& name, const string& doc,
			F ( T::*getFunc )() const )
			: ValueFinfoBase( name, doc )
		{
				string getname = "get" + name;
				getname[3] = std::toupper( getname[3] );
				get_ = new DestFinfo(
					getname,
					"Requests field value. The requesting Element must "
					"provide a handler for the returned value.",
					new GetOpFunc< T, F >( getFunc ) );
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

#endif // _VALUE_FINFO_H
