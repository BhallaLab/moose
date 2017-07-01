/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _ARITH_H
#define _ARITH_H

class Arith
{
	friend void testCopyMsgOps();
	friend void testTicks();

	public:
		Arith();
		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );

		/**
 		 * Inserts an event into the pendingEvents queue for spikes.
 		 */
		void addSpike( unsigned int synIndex, const double time );

		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////

		void setFunction( string v );
		string getFunction() const;
		void setOutput( double v );
		double getOutput() const;

		void setIdentifiedArg( unsigned int i, double val );
		double getIdentifiedArg( unsigned int i ) const;

		////////////////////////////////////////////////////////////////
		// Dest Func
		////////////////////////////////////////////////////////////////

		void arg1( double v );
		void arg2( double v );
		void arg3( double v );

		void arg1x2( double arg1, double arg2 );

		////////////////////////////////////////////////////////////////
		// Helper func for tests
		////////////////////////////////////////////////////////////////

		double getArg1() const;
		////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		string function_; // String representation of operation to do.
		double output_; // Store the output so we can examine it.
		double arg1_;	// Argument 1
		double arg2_;	// Argument 2
		double arg3_;	// Argument 2
};

#endif // _ARITH_H
