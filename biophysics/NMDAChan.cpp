/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "ChanBase.h"
#include "ChanCommon.h"
#include "SynChan.h"
#include "NMDAChan.h"

const double EPSILON = 1.0e-12;
const double NMDAChan::valency_ = 2.0;

SrcFinfo1< double >* ICaOut()
{
	static SrcFinfo1< double > ICaOut( "ICaOut",
	"Calcium current portion of the total current carried by the NMDAR" );
	return &ICaOut;
}

const Cinfo* NMDAChan::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// Shared messages
	/////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Dest definitions
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
	static ValueFinfo< NMDAChan, double > KMg_A( "KMg_A",
			"1/eta",
			&NMDAChan::setKMg_A,
			&NMDAChan::getKMg_A
		);
	static ValueFinfo< NMDAChan, double > KMg_B( "KMg_B",
			"1/gamma",
			&NMDAChan::setKMg_B,
			&NMDAChan::getKMg_B
		);
	static ValueFinfo< NMDAChan, double > CMg( "CMg",
			"[Mg] in mM",
			&NMDAChan::setCMg,
			&NMDAChan::getCMg
		);
	static ValueFinfo< NMDAChan, double > temperature( "temperature",
			"Temperature in degrees Kelvin.",
			&NMDAChan::setTemperature,
			&NMDAChan::getTemperature
		);
	static ValueFinfo< NMDAChan, double > extCa( "extCa",
			"External concentration of Calcium in millimolar",
			&NMDAChan::setExtCa,
			&NMDAChan::getExtCa
		);
	static ValueFinfo< NMDAChan, double > intCa( "intCa",
			"Internal concentration of Calcium in millimolar."
			"This is the final value used by the internal calculations, "
			"and may also be updated by the assignIntCa message after "
			"offset and scaling.",
			&NMDAChan::setIntCa,
			&NMDAChan::getIntCa
		);
	static ValueFinfo< NMDAChan, double > intCaScale( "intCaScale",
			"Scale factor for internal concentration of Calcium in mM, "
			"applied to values coming in through the assignIntCa message. "
			"Required because in many models the units of calcium may "
			"differ. ",
			&NMDAChan::setIntCaScale,
			&NMDAChan::getIntCaScale
		);
	static ValueFinfo< NMDAChan, double > intCaOffset( "intCaOffset",
			"Offsetfor internal concentration of Calcium in mM, "
			"applied _after_ the scaling to mM is done. "
			"Applied to values coming in through the assignIntCa message. "
			"Required because in many models the units of calcium may "
			"differ. ",
			&NMDAChan::setIntCaOffset,
			&NMDAChan::getIntCaOffset
		);
	static ValueFinfo< NMDAChan, double > condFraction( "condFraction",
			"Fraction of total channel conductance that is due to the "
			"passage of Ca ions. This is related to the ratio of "
			"permeabilities of different ions, and is typically in "
			"the range of 0.02. This small fraction is largely because "
			"the concentrations of Na and K ions are far larger than that "
			"of Ca. Thus, even though the channel is more permeable to "
			"Ca, the conductivity and hence current due to Ca is smaller. ",
			&NMDAChan::setCondFraction,
			&NMDAChan::getCondFraction
		);
	static ReadOnlyValueFinfo< NMDAChan, double > ICa( "ICa",
			"Current carried by Ca ions",
			&NMDAChan::getICa
		);
	static ElementValueFinfo< ChanBase, double > permeability(
			"permeability",
			"Permeability. Alias for Gbar. Note that the mapping is not"
			" really correct. Permeability is in units of m/s whereas "
			"Gbar is 1/ohm. Some nasty scaling is involved in the "
			"conversion, some of which itself involves concentration "
			"variables. Done internally. ",
			&ChanBase::setGbar,
			&ChanBase::getGbar
		);
	/////////////////////////////////////////////////////////////////////
		static DestFinfo assignIntCa( "assignIntCa",
			"Assign the internal concentration of Ca. The final value "
			"is computed as: "
			"     intCa = assignIntCa * intCaScale + intCaOffset ",
			new OpFunc1< NMDAChan,  double >( &NMDAChan::assignIntCa )
		);
	/////////////////////////////////////////////////////////////////////
	static Finfo* NMDAChanFinfos[] =
	{
		&KMg_A,			// Value
		&KMg_B,			// Value
		&CMg,			// Value
		&temperature,			// Value
		&extCa,			// Value
		&intCa,			// Value
		&intCaScale,			// Value
		&intCaOffset,			// Value
		&condFraction,			// Value
		&ICa,			// ReadOnlyValue
		&permeability,			// ElementValue
		&assignIntCa,			// Dest
		ICaOut(),			// Src
	};

	static string doc[] =
	{
		"Name", "NMDAChan",
		"Author", "Upinder S. Bhalla, 2007, NCBS",
		"Description", "NMDAChan: Ligand-gated ion channel incorporating "
				"both the Mg block and a GHK calculation for Calcium "
				"component of the current carried by the channel. "
				"Assumes a steady reversal potential regardless of "
				"Ca gradients. "
				"Derived from SynChan. "
	};
	static Dinfo< NMDAChan > dinfo;
	static Cinfo NMDAChanCinfo(
		"NMDAChan",
		SynChan::initCinfo(),
		NMDAChanFinfos,
		sizeof( NMDAChanFinfos )/sizeof(Finfo *),
		&dinfo,
		doc,
		sizeof( doc ) / sizeof( string )
	);

	return &NMDAChanCinfo;
}

static const Cinfo* NMDAChanCinfo = NMDAChan::initCinfo();

///////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////
NMDAChan::NMDAChan()
	:
		KMg_A_( 1.0 ), // These are NOT the same as the A, B state
		KMg_B_( 1.0 ), //. variables used for Exp Euler integration.
		CMg_( 1.0 ), 	/// Extracellular Conc of Mg in mM
		temperature_( 300 ),
		Cout_( 1.5 ),	/// Extracellular conc of Ca in mM
		Cin_( 0.0008 ),
		intCaScale_( 1.0 ),	/// Assume already in mM
		intCaOffset_( 0.0 ),	/// Assume already in mM
		condFraction_( 0.02 ),	/// Assume indept of Vm.
		ICa_( 0.0 ),
		const_( FaradayConst * valency_ / (GasConst * temperature_ ) )
{;}

NMDAChan::~NMDAChan()
{;}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void NMDAChan::setKMg_A( double KMg_A )
{
	if ( KMg_A < EPSILON ) {
		cout << "Error: KMg_A=" << KMg_A << " must be > 0. Not set.\n";
	} else {
		KMg_A_ = KMg_A;
	}
}
double NMDAChan::getKMg_A() const
{
	return KMg_A_;
}
void NMDAChan::setKMg_B( double KMg_B )
{
	if ( KMg_B < EPSILON ) {
		cout << "Error: KMg_B=" << KMg_B << " must be > 0. Not set.\n";
	} else {
		KMg_B_ = KMg_B;
	}
}
double NMDAChan::getKMg_B() const
{
	return KMg_B_;
}
void NMDAChan::setCMg( double CMg )
{
	if ( CMg < EPSILON ) {
		cout << "Error: CMg = " << CMg << " must be > 0. Not set.\n";
	} else {
		CMg_ = CMg;
	}
}
double NMDAChan::getCMg() const
{
	return CMg_;
}

void NMDAChan::setExtCa( double Cout )
{
	if ( Cout < EPSILON ) {
		cout << "Error: Cout = " << Cout << " must be > 0. Not set.\n";
	} else {
		Cout_ = Cout;
	}
}
double NMDAChan::getExtCa() const
{
	return Cout_;
}

void NMDAChan::setIntCa( double Cin )
{
	if ( Cin < 0.0 ) {
		cout << "Error: IntCa = " << Cin << " must be > 0. Not set.\n";
	} else {
		Cin_ = Cin;
	}
}

double NMDAChan::getIntCa() const
{
	return Cin_;
}

void NMDAChan::setIntCaScale( double v )
{
	intCaScale_ = v;
}

double NMDAChan::getIntCaScale() const
{
	return intCaScale_;
}

void NMDAChan::setIntCaOffset( double v )
{
	intCaOffset_ = v;
}

double NMDAChan::getIntCaOffset() const
{
	return intCaOffset_;
}

void NMDAChan::setCondFraction( double v )
{
	condFraction_ = v;
}

double NMDAChan::getCondFraction() const
{
	return condFraction_;
}

double NMDAChan::getICa() const
{
	return ICa_;
}

double NMDAChan::getTemperature() const
{
	return temperature_;
}
void NMDAChan::setTemperature( double temperature )
{
	if ( temperature < EPSILON ) {
		cout << "Error: temperature = " << temperature << " must be > 0. Not set.\n";
		return;
	}
	temperature_ = temperature;
	const_ = ( FaradayConst / GasConst ) * valency_ / temperature_;
}

///////////////////////////////////////////////////////////
// MsgDest function
///////////////////////////////////////////////////////////
void NMDAChan::assignIntCa( double v )
{
	Cin_ = v * intCaScale_ + intCaOffset_;
}

/**
 * Note the implicit mapping between permeability and conductance.
 * From the GENESIS source code:
 * A thought about the units, by EDS 6/95 (based on Hille 92):
 * Units:
 * 		p in M/s
 * 		EF/RT and z are dimensionless
 * 		F in C/mol
 * 		Cin and Cout in mM==mol/m^3
 * Then Ik is really in units of C/s/m^2==A/m^2, so we compute a
 * current density.  As we replace in practice p by Gbar, which has
 * already been scaled for surface, we get rid of the density factor.
 */


///////////////////////////////////////////////////////////
// Process functions
///////////////////////////////////////////////////////////

void NMDAChan::vProcess( const Eref& e, ProcPtr info )
{
	// First do the regular channel current calculations for the
	// summed ions, in which the NMDAR behaves like a regular channel
	// modulo the Mg block.
	double Gk = SynChan::calcGk();
	double KMg = KMg_A_ * exp(Vm_/KMg_B_);
	Gk *= KMg / (KMg + CMg_);
	ChanBase::setGk( e, Gk );
	ChanCommon::updateIk();

	////////////////////////////////////////////////////////////////////
	// Here we do the calculations for the Ca portion of the current.
	// Total current is still done using a regular reversal potl for chan.
	// Here we use Gk = Permeability * Z * FaradayConst * area / dV.
	// Note that Cin and Cout are in moles/m^3, and FaradayConst is in
	// Coulombs/mole.
	//
	// The GHK flux equation for an ion S (Hille 2001):
	// \Phi_{S} = P_{S}z_{S}^2\frac{V_{m}F^{2}}{RT}\frac{[\mbox{S}]_{i} - [\mbox{S}]_{o}\exp(-z_{S}V_{m}F/RT)}{1 - \exp(-z_{S}V_{m}F/RT)}
	// where
	// \PhiS is the current across the membrane carried by ion S, measured in amperes per square meter (A·m−2)
	// PS is the permeability of the membrane for ion S measured in m·s−1
	// zS is the valence of ion S
	// Vm is the transmembrane potential in volts
	// F is the Faraday constant, equal to 96,485 C·mol−1 or J·V−1·mol−1
	// R is the gas constant, equal to 8.314 J·K−1·mol−1
	// T is the absolute temperature, measured in Kelvin (= degrees Celsius + 273.15)
	// [S]i is the intracellular concentration of ion S, measured in mol·m−3 or mmol·l−1
	// [S]o is the extracellular concentration of ion S, measured in mol·m−3
	//
	// Put this into cleaner text, we get
	//
	// ICa (A/m^2) = perm * Z * Vm * F * const * (Cin-Cout * e2exponent)
	//                                             ----------------------
	//                                                (1-e2exponent)
	//
	// where const = zF/RT (Coulombs/J/K * K ) = Coul/J = 1/V
	// and exponent = const * Vm ( unitless)
	//
	// We want ICa in Amps. We use perm times area which is m^3/s
	// Flux ( mol/sec) = Perm (m/s ) * area (m^2) * (Cout - Cin) (mol/m^3)
	//
	// But we also have
	//
	// Flux (mol/sec) = Gk ( Coul/(sec*V) ) * dV (V) / (zF (Coul/Mol))
	//
	// So Perm( m/s ) * Area = Gk * dV / (Z*F * (cout - cin) )
	//
	// Note that dV should be just the Nernst potential for the ion, since
	// the calculation of flux from permeability assumes no extra flux
	// due to charge gradients.
	//
	// e2exponent = exp( -const * Vm)
	//
	// ICa (A) = (Gk * dV / (zF*(cout-cin) ) * Vm * zF * const * (Cin-Cout * e2exponent)
	//                                             ----------------------
	//                                                (1-e2exponent)
	//
	//			    	  = Gk * dV * Vm * const*(cin - cout*e2exponent)
	//                                     ------------------------------
	//                                     (cout-cin) *  (1-e2exponent)
	//
	// Note how the calculation for Perm in terms of Gk has to use
	// different terms for dV and conc, than the GHK portion.
	//
	// A further factor comes in because we want to relate the Gk for
	// Ca to that for the channel as a whole. We know from Hille that
	// the permeability of NMDAR for Ca is about 4x that for Na.
	//
	// Here is the calculation:
	// Gchan = GNa + GK + GCa									(Eq1)
	// IChan = 0 = GNa*ENa + GK*EK + GCa*ECa					(Eq2)
	//
	// Perm_Ca * area = GCa * ECa / ( ZF * (cout - cin ) )		(Eq3a)
	// Perm_Na * area = GNa * ENa / ( ZF * (cout - cin ) )		(Eq3b)
	// But Perm_Na ~ 0.25 * perm_Ca
	// So GNa*ENa / (F * (cout - cin ) ) ~ 0.25*GCa*ECa/(2F*(cout-cin) )
	// Cancelling
	//
	// GNa * 0.060 / (cout - cin|Na) ~ 0.25*GCa*0.128/(2*(cout-cin|Ca))
	// GNa * 0.060 / 130 ~ GCa * 0.25 * 0.128 / (2*1.5)
	// GNa ~ GCa * 130 * 0.25 * 0.128 / ( 2 * 1.5 * 0.060)
	// GNa ~ 23 GCa (!!!!!!)									(Eq4)
	//
	// Now plug into Eq2:
	//
	// 0 = 23GCa*0.06 - GK*0.08 + GCa*0.128
	// So
	// GK = GCa(0.06*23 + 0.128) / 0.06 = 25*GCa. 	(Not surprised any more)
	//
	// Finally, put into Eq1:
	//
	// Gchan = 23GCa + 25GCa + GCa
	// So GCa = Gchan / 49. So condFraction ~0.02 in the vicinity of 0.0mV.
	//
	// Rather than build in this calculation, I'll just provide the user
	// with a scaling field to use and set it to the default of 0.02
	//
	double ErevCa = log( Cout_ / Cin_ ) / const_;
	double dV = ErevCa;
	//double dV = ErevCa - Vm_;
	// double dV = vGetEk( e ) - Vm_;
	// double dV = Vm_;
	double exponent = const_ * Vm_;
	double e2e = exp( -exponent );
	if ( fabs( exponent) < 0.00001 ) { // Exponent too near zero, use Taylor
		ICa_ = Gk * dV * exponent * ( Cin_ - Cout_ * e2e ) /
				( ( Cin_ - Cout_ ) * (1 - 0.5 * exponent ) );
	} else {
		ICa_ = Gk * dV * exponent * ( Cin_ - Cout_ * e2e ) /
				( ( Cin_ - Cout_ ) * (1 - e2e ) );
	}
	ICa_ *= condFraction_; // Scale Ca current by fraction carried by Ca.

	sendProcessMsgs( e, info );
	ICaOut()->send( e, ICa_ );
}

void NMDAChan::vReinit( const Eref& e, ProcPtr info )
{
	SynChan::vReinit( e, info );
	if ( CMg_ < EPSILON || KMg_B_ < EPSILON || KMg_A_ < EPSILON ) {
		cout << "Error: NMDAChan::innerReinitFunc: fields KMg_A, KMg_B, CMg\nmust be greater than zero. Resetting to 1 to avoid numerical errors\n";
		if ( CMg_ < EPSILON ) CMg_ = 1.0;
		if ( KMg_B_ < EPSILON ) KMg_B_ = 1.0;
		if ( KMg_A_ < EPSILON ) KMg_A_ = 1.0;
	}
	sendReinitMsgs( e, info );
	ICaOut()->send( e, 0.0 );
}

///////////////////////////////////////////////////
// Unit tests
///////////////////////////////////////////////////

