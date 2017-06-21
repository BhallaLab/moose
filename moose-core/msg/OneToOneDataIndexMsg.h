/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ONE_TO_ONE_DATA_INDEX_MSG_H
#define _ONE_TO_ONE_DATA_INDEX_MSG_H

/**
 * Manages a projection where each entry in source array
 * connects to the corresponding entry (with same index)
 * in dest array.
 * If there is a mismatch in number of entries, the overhang is ignored.
 * This differs from the regular OneToOne msg because even
 * if the dest array is a FieldElement, it still does a mapping between
 * the DataIndex of src and dest.
 */
class OneToOneDataIndexMsg: public Msg
{
	friend unsigned int Msg::initMsgManagers(); // for initializing Id.
	public:
		OneToOneDataIndexMsg( const Eref& e1, const Eref& e2, unsigned int msgIndex);
		~OneToOneDataIndexMsg();

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
		static Id managerId_;
		static vector< OneToOneDataIndexMsg* > msg_;
};

#endif // _ONE_TO_ONE_DATA_INDEX_MSG_H
