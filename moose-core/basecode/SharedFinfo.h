/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _SHARED_FINFO_H
#define _SHARED_FINFO_H

/**
 * This is a SharedFinfo, which wraps an arbitrary set of regular
 * Src and Dest Messages. Its main job is to do typechecking for setting
 * up multiple data streams to go across the same Msg.
 */

class SharedFinfo: public Finfo
{
	public:
		SharedFinfo( const string& name, const string& doc,
			Finfo** entries, unsigned int numEntries );

		~SharedFinfo() {;}

		void registerFinfo( Cinfo* c );

		bool strSet( const Eref& tgt, const string& field,
			const string& arg ) const;
		bool strGet( const Eref& tgt, const string& field,
			string& returnValue ) const;

		/**
		 * Checks that the type of target Finfo matches self, and is safe
		 * to exchange messages with.
		 */
		bool checkTarget( const Finfo* target ) const;

		/**
		 * First calls checkTarget on all targets, then sets up message.
		 * Returns true on success.
		 */
		bool addMsg( const Finfo* target, ObjId mid, Element* src ) const;

		const vector< SrcFinfo* >& src() const;

		const vector< Finfo* >& dest() const;

		///////////////////////////////////////////////////////////////////
		// Override the default virtual function for the set/get destfinfos
		///////////////////////////////////////////////////////////////////
		vector< string > innerSrc() const;
		vector< string > innerDest() const;

		/// This always returns void. We need to check the subsidiary Finfos
		string rttiType() const;

	private:
		vector< SrcFinfo* > src_;
		vector< Finfo* > dest_;
};

#endif // _SHARED_FINFO_H
