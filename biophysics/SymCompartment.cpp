/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"

#include "CompartmentBase.h"
#include "Compartment.h"
#include "SymCompartment.h"

static SrcFinfo2< double, double > *distalOut() {
	static SrcFinfo2< double, double > distalOut( "distalOut",
			"Sends out Ra and Vm on each timestep, on the distal end"
			" of a compartment. This end should be pointed away from the"
			" soma. Mathematically the same as proximalOut, but gives"
			" an orientation to the dendrite and helps traversal.");
	return &distalOut;
}

static SrcFinfo2< double, double > *proximalOut() {
	static SrcFinfo2< double, double > proximalOut( "proximalOut",
			"Sends out Ra and Vm on each timestep, on the proximal"
		   " end of a compartment. That is, this end should be "
		   " pointed toward the soma. Mathematically the same as raxialOut"
		   " but provides a logical orientation of the dendrite."
		   " One can traverse proximalOut messages to get to the soma." );
	return &proximalOut;
}

static SrcFinfo2< double, double > *cylinderOut() {
	static SrcFinfo2< double, double > cylinderOut( "cylinderOut",
			" Sends out Ra and Vm to compartments (typically spines) on the"
			" curved surface of a cylinder. Ra is set to nearly zero,"
			" since we assume that the resistance from axis to surface is"
			" negligible." );
	return &cylinderOut;
}

static SrcFinfo1< double > *sumRaxialOut() {
	static SrcFinfo1< double > sumRaxialOut( "sumRaxialOut",
		"Sends out Ra" );
	return &sumRaxialOut;
}

const Cinfo* SymCompartment::initCinfo()
{
		// process and init and reinit are inherited from Compartment.
	/////////////////////////////////////////////////////////////////////
	// Dest Finfos
	/////////////////////////////////////////////////////////////////////

        static DestFinfo raxialSphere( "raxialSphere",
                "Expects Ra and Vm from other compartment. This is a special case when"
                " other compartments are evenly distributed on a spherical compartment.",
                new OpFunc2< SymCompartment, double, double >(
                &SymCompartment::raxialSphere)
        );
        static DestFinfo raxialCylinder( "raxialCylinder",
                "Expects Ra and Vm from other compartment. This is a special case when"
                " other compartments are evenly distributed on the curved surface"
                " of the cylindrical compartment, so we assume that the"
                " cylinder does not add any further resistance.",
                new OpFunc2< SymCompartment, double, double >(
                &SymCompartment::raxialCylinder)
        );
	static DestFinfo raxialSym( "raxialSym",
		"Expects Ra and Vm from other compartment.",
		new OpFunc2< SymCompartment, double, double >(
			&SymCompartment::raxialSym )
	);
	static DestFinfo sumRaxial( "sumRaxial",
		"Expects Ra from other compartment.",
		new OpFunc1< SymCompartment, double >(
		&SymCompartment::sumRaxial )
	);
	/////////////////////////////////////////////////////////////////////
	// Shared Finfos.
	/////////////////////////////////////////////////////////////////////
	// The SrcFinfos raxialOut, sumRaxialOut and requestSumAxial
	// are defined above to get them into file-wide scope.

	static Finfo* distalShared[] =
	{
            &raxialSym, &sumRaxial,// &handleSumRaxialRequest,
            distalOut(), sumRaxialOut(), //requestSumAxial()
	};

	static Finfo* proximalShared[] =
	{
            &raxialSym, &sumRaxial,
            proximalOut(), sumRaxialOut(),
	};

	static SharedFinfo proximal( "proximal",
		"This is a shared message between symmetric compartments.\n"
		"It goes from the proximal end of the current compartment to\n"
		"distal end of the compartment closer to the soma.\n",
		proximalShared, sizeof( proximalShared ) / sizeof( Finfo* )
	);

	static SharedFinfo distal( "distal",
       "This is a shared message between symmetric compartments.\n"
       "It goes from the distal end of the current compartment to the \n"
       "proximal end of one further from the soma. \n"
       "The Ra values collected from children and\n"
       "sibling nodes are used for computing the equivalent resistance \n"
       "between each pair of nodes using star-mesh transformation.\n"
       "Mathematically this is the same as the proximal message, but\n"
       "the distinction is important for traversal and clarity.\n",
		distalShared, sizeof( distalShared ) / sizeof( Finfo* )
	);

	static SharedFinfo sibling( "sibling",
		"This is a shared message between symmetric compartments.\n"
		"Conceptually, this goes from the proximal end of the current \n"
		"compartment to the proximal end of a sibling compartment \n"
		"on a branch in a dendrite. However,\n"
		"this works out to the same as a 'distal' message in terms of \n"
		"equivalent circuit.  The Ra values collected from siblings \n"
		"and parent node are used for \n"
		"computing the equivalent resistance between each pair of\n"
         "nodes using star-mesh transformation.\n",
		distalShared, sizeof( distalShared ) / sizeof( Finfo* )
	);

    static Finfo* sphereShared[] = {
            &raxialSphere,
            distalOut(),
        };

    static SharedFinfo sphere( "sphere",
		"This is a shared message between a spherical compartment \n"
        "(typically soma) and a number of evenly spaced cylindrical \n"
		"compartments, typically primary dendrites.\n"
		"The sphere contributes the usual Ra/2 to the resistance\n"
	    "between itself and children. The child compartments \n"
		"do not connect across to each other\n"
		"through sibling messages. Instead they just connect to the soma\n"
		"through the 'proximalOnly' message\n",
        sphereShared, sizeof( sphereShared )/sizeof( Finfo* )
    );
    static Finfo* cylinderShared[] = {
            &raxialCylinder,
            cylinderOut(),
        };

    static SharedFinfo cylinder( "cylinder",
		"This is a shared message between a cylindrical compartment \n"
        "(typically a dendrite) and a number of evenly spaced child \n"
		"compartments, typically dendritic spines, protruding from the\n"
		"curved surface of the cylinder. We assume that the resistance\n"
		"from the cylinder curved surface to its axis is negligible.\n"
		"The child compartments do not need to connect across to each \n"
		"other through sibling messages. Instead they just connect to the\n"
		"parent dendrite through the 'proximalOnly' message\n",
        cylinderShared, sizeof( cylinderShared )/sizeof( Finfo* )
    );

    static Finfo* proximalOnlyShared[] = {
            &raxialSphere,
            proximalOut(),
        };
    static SharedFinfo proximalOnly( "proximalOnly",
		"This is a shared message between a dendrite and a parent\n"
        "compartment whose offspring are spatially separated from each\n"
		"other. For example, evenly spaced dendrites emerging from a soma\n"
		"or spines emerging from a common parent dendrite. In these cases\n"
		"the sibling dendrites do not need to connect to each other\n"
		"through 'sibling' messages. Instead they just connect to the\n"
		"parent compartment (soma or dendrite) through this message\n",
        proximalOnlyShared, sizeof( proximalOnlyShared )/sizeof( Finfo* )
    );

	//////////////////////////////////////////////////////////////////
	static Finfo* symCompartmentFinfos[] =
	{

	//////////////////////////////////////////////////////////////////
	// SharedFinfo definitions
	//////////////////////////////////////////////////////////////////
	    // The inherited process and init messages do not need to be
		// overridden.
		&proximal,
		&distal,
		&sibling,
        &sphere,
        &cylinder,
		&proximalOnly

	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// DestFinfo definitions
	//////////////////////////////////////////////////////////////////
	};

	// static SchedInfo schedInfo[] = { { process, 0, 0 }, { init, 0, 1 } };

	static string doc[] =
	{
		"Name", "SymCompartment",
		"Author", "Upi Bhalla; updated and documented by Subhasis Ray",
		"Description", "SymCompartment object, for branching neuron models. In symmetric\n"
                "compartments the axial resistance is equally divided on two sides of\n"
                "the node. The equivalent circuit of the passive compartment becomes:\n"
                "(NOTE: you must use a fixed-width font like Courier for correct rendition of the diagrams below)::\n"
                "                                       \n"
                "         Ra/2    B    Ra/2               \n"
                "       A-/\\/\\/\\_____/\\/\\/\\-- C           \n"
                "                 |                      \n"
                "             ____|____                  \n"
                "            |         |                 \n"
                "            |         \\                 \n"
                "            |         / Rm              \n"
                "           ---- Cm    \\                 \n"
                "           ----       /                 \n"
                "            |         |                 \n"
                "            |       _____               \n"
                "            |        ---  Em            \n"
                "            |_________|                 \n"
                "                |                       \n"
                "              __|__                     \n"
                "              /////                     \n"
                "                                       \n"
                "                                       \n\n"
                "In case of branching, the B-C part of the parent's axial resistance\n"
                "forms a Y with the A-B part of the children::\n\n"
                "                               B'              \n"
                "                               |               \n"
                "                               /               \n"
                "                               \\              \n"
                "                               /               \n"
                "                               \\              \n"
                "                               /               \n"
                "                               |A'             \n"
                "                B              |               \n"
                "  A-----/\\/\\/\\-----/\\/\\/\\------|C        \n"
                "                               |               \n"
                "                               |A\"            \n"
                "                               /               \n"
                "                               \\              \n"
                "                               /               \n"
                "                               \\              \n"
                "                               /               \n"
                "                               |               \n"
                "                               B\"             \n\n\n"
                "As per basic circuit analysis techniques, the C node is replaced using\n"
                "star-mesh transform. This requires all sibling compartments at a\n"
                "branch point to be connected via 'sibling' messages by the user (or\n"
                "by the cell reader in case of prototypes). For the same reason, the\n"
                "child compartment must be connected to the parent by\n"
                "distal-proximal message pair. The calculation of the\n"
                "coefficient for computing equivalent resistances in the mesh is done\n"
                "at reinit.",
	};
        static Dinfo< SymCompartment > dinfo;

	static Cinfo symCompartmentCinfo(
			"SymCompartment",
			moose::Compartment::initCinfo(),
			symCompartmentFinfos,
			sizeof( symCompartmentFinfos ) / sizeof( Finfo* ),
                        &dinfo,
                        doc, sizeof(doc)/sizeof(string)
	);

	return &symCompartmentCinfo;
}

static const Cinfo* symCompartmentCinfo = SymCompartment::initCinfo();

//////////////////////////////////////////////////////////////////
// Here we put the SymCompartment class functions.
//////////////////////////////////////////////////////////////////

SymCompartment::SymCompartment():
        coeff_(0.0),
        RaSum_(0.0)
{
	;
}

//////////////////////////////////////////////////////////////////
// Compartment::Dest function definitions.
//////////////////////////////////////////////////////////////////


// Alternates with the 'process' message
void SymCompartment::vInitProc( const Eref& e, ProcPtr p )
{
	// cout << "SymCompartment " << e.id().path() << ":: innerInitProc: A = " << A_ << ", B = " << B_ << endl;
	distalOut()->send( e, Ra_, Vm_ ); // to kids
	proximalOut()->send( e, Ra_, Vm_ ); // to parent
	cylinderOut()->send( e, 1e-6, Vm_ ); // to kids
}

// Virtual func. Must be called after the 'init' phase.
void SymCompartment::vReinit( const Eref& e, ProcPtr p )
{
	moose::Compartment::vReinit( e, p );
        // We don't want to recalculate these every time step - the request... methods are not required
        // requestSumAxial()->send( e );
        // requestSumAxial2()->send( e );
	sumRaxialOut()->send( e, Ra_ );
	// sumRaxial2Out()->send( e, Ra_ );

	// cout << "SymCompartment " << e.id().path() << ":: innerReinit: coeff = " << coeff_ << ", coeff2 = " << coeff2_ << endl;
}

// The Compartment and Symcompartment go through an 'init' and then a 'proc'
// during each clock tick. Same sequence applies during reinit.
// This function is called during 'init' phase to send Raxial info around.
void SymCompartment::vInitReinit( const Eref& e, ProcPtr p )
{
	// cout << "SymCompartment " << e.id().path() << ":: innerInitReinit: coeff = " << coeff_ << ", coeff2 = " << coeff2_ << endl;
	// coeff_ = 0.0;
	// coeff2_ = 0.0;
	// RaSum_ = 0.0;
	// RaSum2_ = 0.0;
	// requestSumAxial()->send( e );
	// requestSumAxial2()->send( e );
}

void SymCompartment::sumRaxial( double Ra )
{
	RaSum_ += Ra_/Ra;
	coeff_ = ( 1 + RaSum_ ) / 2.0;
	// cout << "SymCompartment::sumRaxial: coeff = " << coeff_ << endl;
}

void SymCompartment::raxialSym( double Ra, double Vm)
{
	// cout << "SymCompartment " << ":: raxialSym: Ra = " << Ra << ", Vm = " << Vm << endl;
		/*
	Ra *= coeff_;
	A_ += Vm / Ra;
	B_ += 1.0 / Ra;
	Im_ += ( Vm - Vm_ ) / Ra;
	*/

    double R = Ra * coeff_;
    // cout << "raxialSym:R=" << R << endl;
    A_ += Vm / R;
    B_ += 1.0 / R;
    Im_ += (Vm - Vm_) / R;
	// double invR = 2.0 / ( Ra + Ra_ );
	// A_ += Vm * invR;
	// B_ += invR;
	// Im_ += ( Vm - Vm_ ) * invR;
}

void SymCompartment::raxialSphere( double Ra, double Vm)
{
	double invR = 2.0 / ( Ra + Ra_ );
	A_ += Vm * invR;
	B_ += invR;
	Im_ += ( Vm - Vm_ ) * invR;
}

void SymCompartment::raxialCylinder( double Ra, double Vm)
{
	double invR = 2.0 / Ra;
	A_ += Vm * invR;
	B_ += invR;
	Im_ += ( Vm - Vm_ ) * invR;
}

/////////////////////////////////////////////////////////////////////
