/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ONE_TO_ONE_MSG_H
#define _ONE_TO_ONE_MSG_H

/**
 * Manages a projection where each entry in source array
 * connects to the corresponding entry (with same index)
 * in dest array.
 * If there is a mismatch in number of entries, the overhang is ignored.
 * If the dest array is a FieldElement, then it uses its internal DataId
 * i2_ to fill in the DataIndex for the dest. The OneToOne matching is
 * assumed to be between DataIndex on e1 and FieldIndex on e2.
 */
class OneToOneMsg: public Msg
{
	friend unsigned int Msg::initMsgManagers(); // for initializing Id.
	public:
		OneToOneMsg( const Eref& e1, const Eref& e2, unsigned int msgIndex);
		~OneToOneMsg();

		Eref firstTgt( const Eref& src ) const;

		void sources( vector< vector< Eref > >& v ) const;
		void targets( vector< vector< Eref > >& v ) const;

		Id managerId() const;

		ObjId findOtherEnd( ObjId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;

		/// Msg lookup functions
		static unsigned int numMsg();
		static char* lookupMsg( unsigned int index );

		/// Setup function for Element-style access to Msg fields.
		static const Cinfo* initCinfo();
	private:
		DataId i1_;
		DataId i2_;
		static Id managerId_;
		static vector< OneToOneMsg* > msg_;
};

#endif // _ONE_TO_ONE_MSG_H
