/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class TestSched
{
	public:
		/**
		 * This may be created as an array, but only on one thread
		 */
		TestSched()
			: index_( 0 )
		{
			if ( isInitPending_ ) {
				globalIndex_ = 0;
				isInitPending_ = 0;
			}
		}
		~TestSched()
		{
			isInitPending_ = 1;
		}

		void process( const Eref& e, ProcPtr p );

		void zeroIndex() {
			index_ = 0 ;
		}

		static const Cinfo* initCinfo();
	private:
		int index_;
		static int globalIndex_;
		static bool isInitPending_;
};

