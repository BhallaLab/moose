/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * Binds MsgId to FuncIds.
 */
class MsgFuncBinding
{
	public:
		MsgFuncBinding()
			: mid(), fid( 0 )
		{;}

		MsgFuncBinding( ObjId m, FuncId f )
			: mid( m ), fid( f )
		{;}

		bool operator==( const MsgFuncBinding& other ) const {
			return ( mid == other.mid && fid == other.fid );
		}

		ObjId mid;
		FuncId fid;

	private:
};
