/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_STRUCT_H
#define _HSOLVE_STRUCT_H

typedef double ( *PFDD )( double, double );

struct CompartmentStruct
{
	double CmByDt;
	double EmByRm;
};

/**
 * InjectStruct stores two different kinds of injected currents - one is a
 * "basal" injection, which the user sets at the start of the simulation,
 * and remains constant as long as the simulation is running. The user
 * could potentially run the simulation for N time steps, stop, change
 * the value, and then resume, however.
 * The second kind is a "varying" injection, meaning that it comes
 * from some other part of moose via a message. So this thing has an
 * associated destFinfo. injectVarying is set to zero at every time
 * step.
 */
struct InjectStruct
{
	InjectStruct()
		:
		injectVarying( 0.0 ),
		injectBasal( 0.0 )
	{ ; }

	double injectVarying;
	double injectBasal;
};

/**
 * Channel-specific current struct. Used as the structure for the vector
 * current_ (in HSolveActive).
 */
struct CurrentStruct
{
	double Gk;
	double Ek;
};

/** Structure for a channel. */
struct ChannelStruct
{
public:
	double Gbar_;
	PFDD takeXpower_;		// Some clever method of actually applying the
	PFDD takeYpower_;		// power on the fraction of gates.
	PFDD takeZpower_;
	double Xpower_;			// Actual powers for each gate.
	double Ypower_;
	double Zpower_;

	/**
	 * Instantaneously change conductance - conductance follows no kinetics
	 * It's like tau = 0 => conductance change directly mirrors voltage
	 * step.
	 */
	int instant_;

	/**
	 * Scale factor for conductance, used when there is biochemical and
	 * other multiscale modulation of conductance.
	 */
	double modulation_;

	/**
	 * Sets the powers and accordingly sets the takePower_ functions.
	 */
	void setPowers( double Xpower, double Ypower, double Zpower );

	/**
	 * Finds the fraction for each gate by raising the "state" to the
	 * appropriate power. current.Gk is then set to Gbar_ times the
	 * calculated fraction. Note, "current" is a parameter.
	 */
	void process( double*& state, CurrentStruct& current );

private:
	static PFDD selectPower( double power );

	/** The aforementioned clever stuff. */
	static double power1( double x, double p ) {
		return x;
	}
	static double power2( double x, double p ) {
		return x * x;
	}
	static double power3( double x, double p ) {
		return x * x * x;
	}
	static double power4( double x, double p ) {
		return power2( x * x, p );
	}
	static double powerN( double x, double p );
};

/**
 * Contains information about the spikegens that the HSolve object needs to
 * talk with
 */
struct SpikeGenStruct
{
	SpikeGenStruct( double* Vm, Eref e )
		:
		Vm_( Vm ),
		e_( e )
	{ ; }

	double* Vm_;
	Eref e_;

	/** Finds the spikegen object using e_ and calls reinit on the spikegen */
	void reinit( ProcPtr info );
	void send( ProcPtr info );
};

struct SynChanStruct
{
	// Index of parent compartment
	unsigned int compt_;
	Id elm_;
};

struct CaConcStruct
{
	double c_;			///> Dynamic calcium concentration, over CaBasal_
	double CaBasal_;	///> Reference calcium concentration
	double factor1_;	///> Both these factors are functions of tau, B and dt.
	double factor2_;
	double ceiling_;	///> Ceiling and floor for lookup tables
	double floor_;

	CaConcStruct();
	CaConcStruct(
		double Ca,
		double CaBasal,
		double tau,
		double B,
		double ceiling,
		double floor,
		double dt );

	void setCa( double Ca );	// c_ = Ca - CaBasal_

	/** change CaBasal_ and update c_ accordingly. */
	void setCaBasal( double CaBasal );

	/** Sets the factors using the appropriate functions. */
	void setTauB( double tau, double B, double dt );

	/**
	 * Compute Ca concentration from factors and activation value.
	 * Also takes care of Ca concetration exceeding min and max values.
	 */
	double process( double activation );
};

#endif // _HSOLVE_STRUCT_H
