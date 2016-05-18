/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _M_STRING_H
#define _M_STRING_H

class Mstring
{
	public: 
		Mstring();
		Mstring( string other );
		
		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////
		
		void setThis( string v );
		string getThis() const;

		////////////////////////////////////////////////////////////////
		// Utility stuff
		////////////////////////////////////////////////////////////////
		// const Mstring& operator=( const Mstring& other );
		// string operator=( const Mstring& other );
		// string operator=( const string& other );
	
		////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		string value_;
};

#endif // _M_STRING_H
