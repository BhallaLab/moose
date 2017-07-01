/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STIMULUS_TABLE_H
#define _STIMULUS_TABLE_H

/**
 * Receives and records inputs. Handles plot and spiking data in batch mode.
 */
class StimulusTable: public TableBase
{
	public:
		StimulusTable();
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setStartTime( double v );
		double getStartTime() const;
		void setStopTime( double v );
		double getStopTime() const;
		void setLoopTime( double v );
		double getLoopTime() const;
		void setStepSize( double v );
		double getStepSize() const;
		void setStepPosition( double v );
		double getStepPosition() const;
		void setDoLoop( bool v );
		bool getDoLoop() const;

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );

		//////////////////////////////////////////////////////////////////
		// Lookup funcs for table
		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		double start_;
		double stop_;
		double loopTime_;
		double stepSize_;
		double stepPosition_;
		bool doLoop_;
};

#endif	// _STIMULUS_TABLE_H
