/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _Nernst_h
#define _Nernst_h

/**
 * This class does the Nernst equation calculations
 */
class Nernst
{
	public:
		Nernst();

	///////////////////////////////////////////////////
	// Field function definitions
	///////////////////////////////////////////////////
	double getE() const;

	void setTemperature( double value );
	double getTemperature() const;

	void setValence( int value );
	int getValence() const;

	void setCin( double value );
	double getCin() const;

	void setCout( double value );
	double getCout() const;

	void setScale( double value );
	double getScale() const;

	///////////////////////////////////////////////////
	// Dest function definitions
	///////////////////////////////////////////////////

	void handleCin( const Eref& e, double conc );
	void handleCout( const Eref& e, double conc );

	static const Cinfo* initCinfo();
	private:
		void updateE( );
		double E_;
		double Temperature_;
		int valence_;
		double Cin_;
		double Cout_;
		double scale_;
		double factor_;
		static const double R_OVER_F;
		static const double ZERO_CELSIUS;
};

// Used by solver, readcell, etc.

#endif // _Nernst_h
