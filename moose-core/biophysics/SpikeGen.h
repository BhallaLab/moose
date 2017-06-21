/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SpikeGen_h
#define _SpikeGen_h

class SpikeGen
{
  public:
    SpikeGen();

	//////////////////////////////////////////////////////////////////
	// Field functions.
	//////////////////////////////////////////////////////////////////
		void setThreshold( double threshold );
		double getThreshold() const;

		void setRefractT( double val );
		double getRefractT() const;

		/*
		void setAmplitude( double val );
		double getAmplitude() const;

		void setState( double val );
		double getState() const;
		*/

        bool getFired() const;
        void setEdgeTriggered( bool yes);
        bool getEdgeTriggered() const;

	//////////////////////////////////////////////////////////////////
	// Message dest functions.
	//////////////////////////////////////////////////////////////////

		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );
		void handleVm( double val );

		static const Cinfo* initCinfo();
	private:
		double threshold_;
		double refractT_;
		double lastEvent_;
		double V_;
		bool fired_;
		bool edgeTriggered_;
};

#endif // _SpikeGen_h
