/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SINGLE_MSG_H
#define _SINGLE_MSG_H

/**
 * This is a message from a single source object to a single target
 * object. The source object must be a DataEntry. The target object
 * may be on a FieldElement, and the target specification includes the
 * index of the field object.
 */

class SingleMsg: public Msg
{
	friend unsigned int Msg::initMsgManagers(); // for initializing Id.
	public:
		SingleMsg( const Eref& e1, const Eref& e2, unsigned int msgIndex );
		~SingleMsg();

		Eref firstTgt( const Eref& src ) const;

		void sources( vector< vector< Eref > >& v ) const;
		void targets( vector< vector< Eref > >& v ) const;

		DataId i1() const;
		DataId i2() const;

		// returns the id of the managing Element.
		Id managerId() const;

		ObjId findOtherEnd( ObjId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;

		void setI1( DataId di );
		DataId getI1() const;

		void setI2( DataId di );
		DataId getI2() const;

		void setTargetField( unsigned int f );
		unsigned int getTargetField() const;

		/// Msg lookup functions
		static unsigned int numMsg();
		static char* lookupMsg( unsigned int index );

		static const Cinfo* initCinfo();
	private:
		DataId i1_;
		DataId i2_;
		unsigned int f2_; // Field for target. Note asymmetry
		static Id managerId_;
		static vector< SingleMsg* > msg_;
};

#endif // _SINGLE_MSG_H
