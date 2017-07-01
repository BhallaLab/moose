/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _STATS_H
#define _STATS_H

class Stats
{
	public:
		Stats();

		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////

		double getMean() const;
		double getSdev() const;
		double getSum() const;
		unsigned int getNum() const;

		double getWmean() const;
		double getWsdev() const;
		double getWsum() const;
		unsigned int getWnum() const;

		void setWindowLength( unsigned int len );
		unsigned int getWindowLength() const;

		////////////////////////////////////////////////////////////////
		// Dest Func
		////////////////////////////////////////////////////////////////
		void input( double v );

		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );

		/// Virtual func for handling process calls for derived classes.
		virtual void vProcess( const Eref& e, ProcPtr p );
		virtual void vReinit( const Eref& e, ProcPtr p );

		////////////////////////////////////////////////////////////////
		// other func
		////////////////////////////////////////////////////////////////
		void doWindowCalculation() const;
		void innerWindowCalculation();

		////////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();
	private:
		double mean_;
		double sdev_;
		double sum_;
		unsigned int num_;
		double wmean_;
		double wsdev_;
		double wsum_;
		unsigned int wnum_;
		double sumsq_;
		double lastt_;
		vector< double > samples_;
		bool isWindowDirty_;
};

#endif // _STATS_H
