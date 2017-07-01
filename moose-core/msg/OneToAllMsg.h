/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ONE_TO_ALL_MSG_H
#define _ONE_TO_ALL_MSG_H

/**
 * Manages projection from a single entry in e1 to all
 * array entries in e2.
 */

class OneToAllMsg: public Msg
{
	friend unsigned int Msg::initMsgManagers(); // for initializing Id.
	friend void testGetMsgs(); // test func
	public:
		OneToAllMsg( Eref e1, Element* e2, unsigned int msgIndex );
		~OneToAllMsg();

		Eref firstTgt( const Eref& src ) const;

		void sources( vector< vector< Eref > >& v ) const;
		void targets( vector< vector< Eref > >& v ) const;

		Id managerId() const;

		ObjId findOtherEnd( ObjId end ) const;

		Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const;

		/// Return the first DataId
		DataId getI1() const;
		void setI1( DataId i1 ); /// Assign the first DataId.

		/// Msg lookup functions
		static unsigned int numMsg();
		static char* lookupMsg( unsigned int index );

		/// Setup function for Element-style access to Msg fields.
		static const Cinfo* initCinfo();
	private:
		DataId i1_;
		static Id managerId_;
		static vector< OneToAllMsg* > msg_;
};


#endif // _ONE_TO_ALL_MSG_H
