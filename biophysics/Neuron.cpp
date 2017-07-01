/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "LookupElementValueFinfo.h"
#include "shell/Shell.h"
#include "shell/Wildcard.h"
#include "ReadCell.h"
#include "utility/Vec.h"
#include "SwcSegment.h"
#include "Spine.h"
#include "Neuron.h"
#include "basecode/global.h"

#include "muParser.h"

class nuParser: public mu::Parser
{
public:
    nuParser( const string& expr ):
        mu::Parser(),
        p(0.0), // geometrical path distance wound through dendrite
        g(0.0), // geometrical path distance direct from soma.
        L(0.0), // electrical distance arg
        len(0.0), // Length of compt in metres
        dia(0.0), // Diameter of compt in metres
        maxP(0.0), // Maximum value of *p* for this neuron.
        maxG(0.0), // Maximum value of *g* for this neuron.
        maxL(0.0), // Maximum value of *L* for this neuron.
        x(0.0), // X position of segment.
        y(0.0), // Y position of segment.
        z(0.0), // Z position of segment.
        oldVal(0.0), // Original value of field, if needed.
        useOldVal( false ) // is the 'orig' field needed?
    {
        DefineVar( "p", &p );
        DefineVar( "g", &g );
        DefineVar( "L", &L );
        DefineVar( "len", &len );
        DefineVar( "dia", &dia );
        DefineVar( "maxP", &maxP );
        DefineVar( "maxG", &maxG );
        DefineVar( "maxL", &maxL );
        DefineVar( "x", &x );
        DefineVar( "y", &y );
        DefineVar( "z", &z );
        DefineVar( "oldVal", &oldVal );
        DefineFun( "H", nuParser::H );
        if ( expr.find( "oldVal" ) != string::npos )
            useOldVal = true;
        SetExpr( expr );
    }

    /// Defines the order of arguments in the val array.
    enum valArgs { EXPR, P, G, EL, LEN, DIA, MAXP, MAXG, MAXL,
                   X, Y, Z,	OLDVAL
                 };

    static double H( double arg )   // Heaviside function.
    {
        return ( arg > 0.0);
    }

    double eval( vector< double >::const_iterator arg0 )
    {
        p = *(arg0 + nuParser::P );
        g = *(arg0 + nuParser::G );
        L = *(arg0 + nuParser::EL );
        len = *(arg0 + nuParser::LEN );
        dia = *(arg0 + nuParser::DIA );
        maxP = *(arg0 + nuParser::MAXP );
        maxG = *(arg0 + nuParser::MAXG );
        maxL = *(arg0 + nuParser::MAXL );
        x = *(arg0 + nuParser::X );
        y = *(arg0 + nuParser::Y );
        z = *(arg0 + nuParser::Z );
        oldVal = *(arg0 + nuParser::OLDVAL );
        return Eval();
    }

    static const unsigned int numVal;
    double p; // geometrical path distance arg
    double g; // geometrical path distance arg
    double L; // electrical distance arg
    double len; // Length of compt in metres
    double dia; // Diameter of compt in metres
    double maxP; // Maximum value of p for this neuron.
    double maxG; // Maximum value of p for this neuron.
    double maxL; // Maximum value of L for this neuron.
    double x; // X position of segment in metres
    double y; // Y position of segment in metres
    double z; // Z position of segment in metres
    double oldVal; // Old value of the field, used for relative scaling
    bool useOldVal; // is the 'oldVal' field needed?
};

const unsigned int nuParser::numVal = 13;

const Cinfo* Neuron::initCinfo()
{
	/////////////////////////////////////////////////////////////////////
	// ValueFinfos
	/////////////////////////////////////////////////////////////////////
	static ValueFinfo< Neuron, double > RM( "RM",
		"Membrane resistivity, in ohm.m^2. Default value is 1.0.",
		&Neuron::setRM,
		&Neuron::getRM
	);
	static ValueFinfo< Neuron, double > RA( "RA",
		"Axial resistivity of cytoplasm, in ohm.m. Default value is 1.0.",
		&Neuron::setRA,
		&Neuron::getRA
	);
	static ValueFinfo< Neuron, double > CM( "CM",
		"Membrane Capacitance, in F/m^2. Default value is 0.01",
		&Neuron::setCM,
		&Neuron::getCM
	);
	static ValueFinfo< Neuron, double > Em( "Em",
		"Resting membrane potential of compartments, in Volts. "
		"Default value is -0.065.",
		&Neuron::setEm,
		&Neuron::getEm
	);
	static ValueFinfo< Neuron, double > theta( "theta",
		"Angle to rotate cell geometry, around long axis of neuron. "
		"Think Longitude. Units are radians. "
		"Default value is zero, which means no rotation. ",
		&Neuron::setTheta,
		&Neuron::getTheta
	);
	static ValueFinfo< Neuron, double > phi( "phi",
		"Angle to rotate cell geometry, around elevation of neuron. "
		"Think Latitude. Units are radians. "
		"Default value is zero, which means no rotation. ",
		&Neuron::setPhi,
		&Neuron::getPhi
	);

	static ValueFinfo< Neuron, string > sourceFile( "sourceFile",
		"Name of source file from which to load a model. "
		"Accepts swc and dotp formats at present. "
		"Both these formats require that the appropriate channel "
		"definitions should have been loaded into /library. ",
		&Neuron::setSourceFile,
		&Neuron::getSourceFile
	);

	static ValueFinfo< Neuron, double > compartmentLengthInLambdas(
		"compartmentLengthInLambdas",
		"Units: meters (SI). \n"
		"Electrotonic length to use for the largest compartment in the "
		"model. Used to define subdivision of branches into compartments. "
		"For example, if we set *compartmentLengthInLambdas*  to 0.1, "
		"and *lambda* (electrotonic length) is 250 microns, then it "
		"sets the compartment length to 25 microns. Thus a dendritic "
		"branch of 500 microns is subdivided into 20 commpartments. "
		"If the branch is shorter than *compartmentLengthInLambdas*, "
		"then it is not subdivided. "
		"If *compartmentLengthInLambdas* is set to 0 then the original "
		"compartmental structure of the model is preserved. "
		" Note that this routine does NOT merge branches, even if "
		"*compartmentLengthInLambdas* is bigger than the branch. "
		"While all this subdivision is being done, the Neuron class "
		"preserves as detailed a geometry as it can, so it can rebuild "
		"the more detailed version if needed. "
		"Default value of *compartmentLengthInLambdas* is 0. ",
		&Neuron::setCompartmentLengthInLambdas,
		&Neuron::getCompartmentLengthInLambdas
	);

	static ElementValueFinfo< Neuron, vector< string > >
			channelDistribution(
		"channelDistribution",
		"Specification for distribution of channels, CaConcens and "
		"any other model components that are defined as prototypes and "
		"have to be placed on the electrical compartments.\n"
		"Arguments: proto path field expr [field expr]...\n"
		" Each entry is terminated with an empty string. "
		"The prototype is any object created in */library*, "
		"If a channel matching the prototype name already exists, then "
		"all subsequent operations are applied to the extant channel and "
		"a new one is not created. "
		"The paired arguments are as follows: \n"
		"The *field* argument specifies the name of the parameter "
		"that is to be assigned by the expression.\n"
		"The *expression* argument is a mathematical expression in "
		"the muparser framework, which permits most operations including "
		"trig and transcendental ones. Of course it also handles simple "
		"numerical values like 1.0, 1e-10 and so on. "
		"Available arguments for muParser are:\n"
		" p, g, L, len, dia, maxP, maxG, maxL \n"
		"	p: path distance from soma, measured along dendrite, in metres.\n"
		"	g: geometrical distance from soma, in metres.\n"
		"	L: electrotonic distance (# of lambdas) from soma, along dend. No units.\n"
		"	len: length of compartment, in metres.\n"
		"	dia: for diameter of compartment, in metres.\n"
		"	maxP: Maximum value of *p* for this neuron. \n"
		"	maxG: Maximum value of *g* for this neuron. \n"
		"	maxL: Maximum value of *L* for this neuron.\n"
		"The expression for the first field must evaluate to > 0 "
		"for the channel to be installed. For example, for "
		"channels, if Field == Gbar, and func( r, L, len, dia) < 0, \n"
		"then the channel is not installed. This feature is typically "
		"used with the sign() or Heaviside H() function to limit range: "
		"for example: H(1 - L) will only put channels closer than "
		"one length constant from the soma, and zero elsewhere. \n"
		"Available fields are: \n"
		"Channels: Gbar (install), Ek \n"
		"CaConcen: shellDia (install), shellFrac (install), tau, min\n"
		"Unless otherwise noted, all fields are scaled appropriately by "
		"the dimensions of their compartment. Thus the channel "
		"maximal conductance Gbar is automatically scaled by the area "
		"of the compartment, and the user does not need to insert this "
		"scaling into the calculations.\n"
		"All parameters are expressed in SI units. Conductance, for "
		"example, is Siemens/sq metre. "
		"\n\n"
		"Some example function forms might be for a channel Gbar: \n"
		" p < 10e-6 ? 400 : 0.0 \n"
		"		equivalently, \n"
		" H(10e-6 - p) * 400 \n"
		"		equivalently, \n"
		" ( sign(10e-6 - p) + 1) * 200 \n"
		"Each of these forms instruct the function to "
		"set channel Gbar to 400 S/m^2 only within 10 microns path "
		"distance of soma\n"
		"\n"
		" L < 1.0 ? 100 * exp( -L ) : 0.0 \n"
		" ->Set channel Gbar to an exponentially falling function of "
		"electrotonic distance from soma, provided L is under "
		"1.0 lambdas. \n",
		&Neuron::setChannelDistribution,
		&Neuron::getChannelDistribution
	);

	static ElementValueFinfo< Neuron, vector< string > >
			passiveDistribution(
		"passiveDistribution",
		"Specification for distribution of passive properties of cell.\n"
		"Arguments: . path field expr [field expr]...\n"
		"Note that the arguments list starts with a period. "
		" Each entry is terminated with an empty string. "
		"The paired arguments are as follows: \n"
		"The *field* argument specifies the name of the parameter "
		"that is to be assigned by the expression.\n"
		"The *expression* argument is a mathematical expression in "
		"the muparser framework, which permits most operations including "
		"trig and transcendental ones. Of course it also handles simple "
		"numerical values like 1.0, 1e-10 and so on. "
		"Available arguments for muParser are:\n"
		" p, g, L, len, dia, maxP, maxG, maxL \n"
		"	p: path distance from soma, measured along dendrite, in metres.\n"
		"	g: geometrical distance from soma, in metres.\n"
		"	L: electrotonic distance (# of lambdas) from soma, along dend. No units.\n"
		"	len: length of compartment, in metres.\n"
		"	dia: for diameter of compartment, in metres.\n"
		"	maxP: Maximum value of *p* for this neuron. \n"
		"	maxG: Maximum value of *g* for this neuron. \n"
		"	maxL: Maximum value of *L* for this neuron.\n"
		"Available fields are: \n"
		"RM, RA, CM, Rm, Ra, Cm, Em, initVm \n"
		"The first three fields are scaled appropriately by "
		"the dimensions of their compartment. Thus the membrane "
		"resistivity RM (ohms.m^2) is automatically scaled by the area "
		"of the compartment, and the user does not need to insert this "
		"scaling into the calculations to compute Rm."
		"Using the Rm field lets the user directly assign the "
	    "membrane resistance (in ohms), presumably using len and dia.\n"
	    "Similarly, RA (ohms.m) and CM (Farads/m^2) are specific units "
		"and the actual values for each compartment are assigned by "
		"scaling by length and diameter. Ra (ohms) and Cm (Farads) "
		"require explicit evaluation of the expression. "
		"All parameters are expressed in SI units. Conductance, for "
		"example, is Siemens/sq metre.\n"
		"Note that time these calculations do NOT currently include spines\n",
		&Neuron::setPassiveDistribution,
		&Neuron::getPassiveDistribution
	);

	static ElementValueFinfo< Neuron, vector< string > >spineDistribution(
		"spineDistribution",
		"Specification for distribution of spines on dendrite. \n"
		"Arguments: proto path spacing expr [field expr]...\n"
		" Each entry is terminated with an empty string. "
		"The *prototype* is any spine object created in */library*, \n"
		"The *path* is the wildcard path of compartments on which to "
		"place the spine.\n"
		"The *spacing* is the spacing of spines, in metres. \n"
		"The *expression* argument is a mathematical expression in "
		"the muparser framework, which permits most operations including "
		"trig and transcendental ones. Of course it also handles simple "
		"numerical values like 1.0, 1e-10 and so on. "
		"The paired arguments are as follows: \n"
		"The *field* argument specifies the name of the parameter "
		"that is to be assigned by the expression.\n"
		"The *expression* argument is a mathematical expression as above. "
		"Available arguments for muParser are:\n"
		" p, g, L, len, dia, maxP, maxG, maxL \n"
		"	p: path distance from soma, measured along dendrite, in metres.\n"
		"	g: geometrical distance from soma, in metres.\n"
		"	L: electrotonic distance (# of lambdas) from soma, along dend. No units.\n"
		"	len: length of compartment, in metres.\n"
		"	dia: for diameter of compartment, in metres.\n"
		"	maxP: Maximum value of *p* for this neuron. \n"
		"	maxG: Maximum value of *g* for this neuron. \n"
		"	maxL: Maximum value of *L* for this neuron.\n"
		"The expression for the *spacing* field must evaluate to > 0 for "
		"the spine to be installed. For example, if the expresssion is\n"
		"		H(1 - L) \n"
		"then the systemwill only put spines closer than "
		"one length constant from the soma, and zero elsewhere. \n"
		"Available spine parameters are: \n"
		"spacing, minSpacing, size, sizeDistrib "
		"angle, angleDistrib \n",
		&Neuron::setSpineDistribution,
		&Neuron::getSpineDistribution
	);


	static ReadOnlyValueFinfo< Neuron, unsigned int > numCompartments(
		"numCompartments",
		"Number of electrical compartments in model. ",
		&Neuron::getNumCompartments
	);

	static ReadOnlyValueFinfo< Neuron, unsigned int > numSpines(
		"numSpines",
		"Number of dendritic spines in model. ",
		&Neuron::getNumSpines
	);

	static ReadOnlyValueFinfo< Neuron, unsigned int > numBranches(
		"numBranches",
		"Number of branches in dendrites. ",
		&Neuron::getNumBranches
	);

	static ReadOnlyValueFinfo< Neuron, vector< double > > pathDistFromSoma(
		"pathDistanceFromSoma",
		"geometrical path distance of each segment from soma, measured by "
		"threading along the dendrite.",
		&Neuron::getPathDistFromSoma
	);

	static ReadOnlyValueFinfo< Neuron, vector< double > > geomDistFromSoma(
		"geometricalDistanceFromSoma",
		"geometrical distance of each segment from soma.",
		&Neuron::getGeomDistFromSoma
	);

	static ReadOnlyValueFinfo< Neuron, vector< double > > elecDistFromSoma(
		"electrotonicDistanceFromSoma",
		"geometrical distance of each segment from soma, as measured along "
		"the dendrite.",
		&Neuron::getElecDistFromSoma
	);
	static ReadOnlyValueFinfo< Neuron, vector< ObjId > > compartments(
		"compartments",
		"Vector of ObjIds of electrical compartments. Order matches order "
		"of segments, and also matches the order of the electrotonic and "
		"geometricalDistanceFromSoma vectors. ",
		&Neuron::getCompartments
	);

	static ReadOnlyLookupElementValueFinfo< Neuron, string, vector< ObjId > >
			compartmentsFromExpression(
		"compartmentsFromExpression",
		"Vector of ObjIds of electrical compartments that match the "
		"'path expression' pair in the argument string.",
		&Neuron::getExprElist
	);

	static ReadOnlyLookupElementValueFinfo< Neuron, string, vector< double > >
			valuesFromExpression(
		"valuesFromExpression",
		"Vector of values computed for each electrical compartment that "
	   	"matches the 'path expression' pair in the argument string."
		"This has 13 times the number of entries as # of compartments."
		"For each compartment the entries are: \n"
		"val, p, g, L, len, dia, maxP, maxG, maxL, x, y, z, 0",
		&Neuron::getExprVal
	);

	static ReadOnlyLookupElementValueFinfo< Neuron, string, vector< ObjId > >
			spinesFromExpression(
		"spinesFromExpression",
		//"Vector of ObjIds of spines/heads sitting on the electrical "
		//"compartments that match the 'path expression' pair in the "
		//"argument string.",
		"Vector of ObjIds of compartments comprising spines/heads "
		"that match the 'path expression' pair in the "
		"argument string.",
		&Neuron::getSpinesFromExpression
	);

	static ReadOnlyLookupElementValueFinfo< Neuron, ObjId,vector< ObjId > >
			spinesOnCompartment(
		"spinesOnCompartment",
		"Vector of ObjIds of spines shafts/heads sitting on the specified "
		"electrical compartment. If each spine has a shaft and a head,"
		"and there are 10 spines on the compartment, there will be 20 "
		"entries in the returned vector, ordered "
		"shaft0, head0, shaft1, head1, ... ",
		&Neuron::getSpinesOnCompartment
	);

	static ReadOnlyLookupElementValueFinfo< Neuron, ObjId, ObjId >
			parentCompartmentOfSpine(
		"parentCompartmentOfSpine",
		"Returns parent compartment of specified spine compartment."
		"Both the spine head or its shaft will return the same parent.",
		&Neuron::getParentCompartmentOfSpine
	);

	/////////////////////////////////////////////////////////////////////
	// DestFinfos
	/////////////////////////////////////////////////////////////////////
	static DestFinfo buildSegmentTree( "buildSegmentTree",
		"Build the reference segment tree structure using the child "
		"compartments of the current Neuron. Fills in all the coords and "
		"length constant information into the segments, for later use "
		"when we build reduced compartment trees and channel "
		"distributions. Should only be called once, since subsequent use "
	   "on a reduced model will lose the original full cell geometry. ",
		new EpFunc0< Neuron >( &Neuron::buildSegmentTree )
	);
	static DestFinfo setSpineAndPsdMesh( "setSpineAndPsdMesh",
		"Assigns the spine and psd mesh to the Neuron. This is used "
		"to build up a mapping from Spine entries on the Neuron to "
		"chem spines and PSDs, so that volume change operations from "
		"the Spine can propagate to the chem systems.",
		new OpFunc2< Neuron, Id, Id >( &Neuron::setSpineAndPsdMesh )
	);
	static DestFinfo setSpineAndPsdDsolve( "setSpineAndPsdDsolve",
		"Assigns the Dsolves used by spine and PSD to the Neuron. "
		"This is used "
		"to handle the rescaling of diffusion rates when spines are "
		"resized. ",
		new OpFunc2< Neuron, Id, Id >( &Neuron::setSpineAndPsdDsolve )
	);

	/*
	static DestFinfo rotateInSpace( "rotateInSpace",
		theta, phi
	static DestFinfo transformInSpace( "transformInSpace",
		transfMatrix(4x4)
	static DestFinfo saveAsNeuroML( "saveAsNeuroML", fname )
	static DestFinfo saveAsDotP( "saveAsDotP", fname )
	static DestFinfo saveAsSwc( "saveAsSwc", fname )
	*/
	/////////////////////////////////////////////////////////////////////
	// FieldElement
	/////////////////////////////////////////////////////////////////////
	static FieldElementFinfo< Neuron, Spine > spineFinfo(
		"spine",
		"Field Element for spines. Used to handle dynamic "
		"geometry changes in spines. ",
		Spine::initCinfo(),
		&Neuron::lookupSpine,
		&Neuron::setNumSpines,
		&Neuron::getNumSpines,
		false
	);

	/////////////////////////////////////////////////////////////////////
	static Finfo* neuronFinfos[] =
	{
		&RM,						// ValueFinfo
		&RA,						// ValueFinfo
		&CM,						// ValueFinfo
		&Em,						// ValueFinfo
		&theta,						// ValueFinfo
		&phi,						// ValueFinfo
		&sourceFile,				// ValueFinfo
		&compartmentLengthInLambdas,	// ValueFinfo
		&numCompartments,			// ReadOnlyValueFinfo
		&numSpines,					// ReadOnlyValueFinfo
		&numBranches,				// ReadOnlyValueFinfo
		&pathDistFromSoma,			// ReadOnlyValueFinfo
		&geomDistFromSoma,			// ReadOnlyValueFinfo
		&elecDistFromSoma,			// ReadOnlyValueFinfo
		&compartments,				// ReadOnlyValueFinfo
		&channelDistribution,		// ValueFinfo
		&passiveDistribution,		// ValueFinfo
		&spineDistribution,			// ValueFinfo
		// &mechSpec,				// ValueFinfo
		// &spineSpecification,		// ValueFinfo
		&compartmentsFromExpression,	// ReadOnlyLookupValueFinfo
		&valuesFromExpression,		// ReadOnlyLookupValueFinfo
		&spinesFromExpression,  	// ReadOnlyLookupValueFinfo
		&spinesOnCompartment,	  	// ReadOnlyLookupValueFinfo
		&parentCompartmentOfSpine, 	// ReadOnlyLookupValueFinfo
		&buildSegmentTree,			// DestFinfo
		&setSpineAndPsdMesh,		// DestFinfo
		&setSpineAndPsdDsolve,		// DestFinfo
		&spineFinfo,				// FieldElementFinfo
	};
	static string doc[] =
	{
		"Name", "Neuron",
		"Author", "C H Chaitanya, Upi Bhalla",
		"Description", "Neuron - Manager for neurons. "
		"Handles high-level specification of distribution of "
		"spines, channels and passive properties. Also manages "
		"spine resizing through a Spine FieldElement. ",
	};
	static Dinfo<Neuron> dinfo;
	static Cinfo neuronCinfo(
				"Neuron",
				Neutral::initCinfo(),
				neuronFinfos, sizeof( neuronFinfos ) / sizeof( Finfo* ),
				&dinfo,
				doc,
				sizeof(doc)/sizeof(string)
	);

	return &neuronCinfo;
}

static const Cinfo* neuronCinfo = Neuron::initCinfo();

////////////////////////////////////////////////////////////////////////
Neuron::Neuron()
	:
			RM_( 1.0 ),
			RA_( 1.0 ),
			CM_( 0.01 ),
			Em_( -0.065 ),
			theta_( 0.0 ),
			phi_( 0.0 ),
			maxP_( 0.0 ),
			maxG_( 0.0 ),
			maxL_( 0.0 ),
			sourceFile_( "" ),
			compartmentLengthInLambdas_( 0.2 ),
			spineEntry_( this )
{;}

// When copying Neuron, we next have to rerun buildSegmentTree() and
// setSpineAndPsdMesh
Neuron::Neuron( const Neuron& other )
    :
    RM_( other.RM_ ),
    RA_( other.RA_ ),
    CM_( other.CM_ ),
    Em_( other.Em_ ),
    theta_( other.theta_ ),
    phi_( other.phi_ ),
    maxP_( other.maxP_ ),
    maxG_( other.maxG_ ),
    maxL_( other.maxL_ ),
    sourceFile_( other.sourceFile_ ),
    compartmentLengthInLambdas_(other.compartmentLengthInLambdas_),
    channelDistribution_( other.channelDistribution_ ),
    passiveDistribution_( other.passiveDistribution_ ),
    spineDistribution_( other.spineDistribution_ ),
    spineEntry_( this )
{
    ;
}
////////////////////////////////////////////////////////////////////////
// Some static utility functions.
////////////////////////////////////////////////////////////////////////


bool parseDistrib( vector< vector < string > >& lines,
                   const vector< string >& distrib )
{
    lines.clear();
    vector< string > temp;
    for ( unsigned int i = 0; i < distrib.size(); ++i )
    {
        if ( distrib[i] == "" )
        {
            if ( temp.size() < 4 )
            {
                cout << "Warning: Neuron::parseDistrib: <4 args: " <<
                     temp.size() << endl;
                return false;
            }
            if ( temp.size() % 2 == 1 )
            {
                cout << "Warning: Neuron::parseDistrib: : odd # of args:"
                     << temp.size() << endl;
                return false;
            }
            lines.push_back( temp );
            temp.clear();
        }
        else
        {
            temp.push_back( distrib[i] );
        }
    }
    return true;
}

static void doClassSpecificMessaging( Shell* shell, Id obj, ObjId compt )
{
    if ( obj.element()->cinfo()->isA( "ChanBase" ) )
    {
        shell->doAddMsg( "Single", compt, "channel", obj, "channel" );
        // Add the message to the Ca pool if not defined
        if ( obj.element()->getName().find_first_of( "Ca" ) != string::npos )
        {
            // Don't do it if we have the legacy GENESIS format
            if ( Neutral::child( obj.eref(), "addmsg1" ) == Id() )
            {
                vector< ObjId > elist;
                string path = Neutral::parent( obj ).path() + "/#[ISA=CaConcBase]";
                // cout << "OK2 to Add Ca Msg for " << path << endl;
                wildcardFind( path, elist );
                if ( elist.size() > 0 )
                {
                    // cout << "Added Ca Msg for " << obj.path() << endl;
                    ObjId mid = shell->doAddMsg(
                                    "single", obj, "IkOut", elist[0], "current" );
                    assert( !mid.bad());
                }
            }
        }
    }
    ReadCell::addChannelMessage( obj );
}

static bool buildFromProto(
    const string& name,
    const vector< ObjId >& elist, const vector< double >& val,
    vector< ObjId >& mech )
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    Id proto( "/library/" + name );
    if ( proto == Id() )
    {
        cout << "Warning: Neuron::buildFromProto: proto '"
             << name << "' not found, skipping\n";
        return false;
    }
    mech.clear();
    mech.resize( elist.size() );
    for ( unsigned int i = 0; i < elist.size(); ++i )
    {
        unsigned int j = i * nuParser::numVal;
        if ( val[ j + nuParser::EXPR ] > 0 )
        {
            string name = proto.element()->getName();
            Id obj = Neutral::child( elist[i].eref(), name );
            if ( obj == Id() )   // Need to copy it in from proto.
            {
                obj = shell->doCopy( proto, elist[i], name, 1, false, false );
                doClassSpecificMessaging( shell, obj, elist[i] );
            }
            mech[i] = obj;
        }
    }
    return true;
}

static void assignParam( Id obj, const string& field,
                         double val, double len, double dia )
{
    if ( obj.element()->cinfo()->isA( "ChanBase" ) )
    {
        if ( field == "Gbar" )
        {
            if ( val > 0 )
                Field< double >::set( obj, "Gbar", val * len * dia * PI );
        }
        else if ( field == "Ek" )
        {
            Field< double >::set( obj, "Ek", val );
        }
    }
    else if ( obj.element()->cinfo()->isA( "CaConcBase" ) )
    {
        Field< double >::set( obj, "length", len );
        Field< double >::set( obj, "diameter", dia );
        // cout << "len, dia = " << len << ", " << dia << endl;
        if ( field == "CaBasal" || field == "tau" || field == "thick" ||
                field == "floor" || field == "ceiling" )
        {
            Field< double >::set( obj, field, val );
        }
        else if ( field == "B" )
        {
            // The system obeys dC/dt = B*I_Ca - C/tau
            // So B = arg/( vol * F ). Here the system provides vol and F,
            // so the arg is just a scale factor over this, typically
            // to be thought of in terms of buffering. Small B is more
            // buffering. This field is deprecated but used in legacy
            // GENESIS scripts.
            Field< double >::set( obj, "B", val /
                                  ( FaradayConst * len * dia * dia * PI / 4.0 ) );
        }
    }
}

static void assignSingleCompartmentParams( ObjId compt,
        double val, const string& field, double len, double dia )
{
    if ( field == "initVm" || field == "INITVM" )
    {
        Field< double >::set( compt, "initVm", val );
        return;
    }
    if ( field == "Em" || field == "EM" )
    {
        Field< double >::set( compt, "Em", val );
        return;
    }
    if ( val > 0.0 )
    {
        if ( field == "Rm" || field == "Ra" || field == "Cm" )
        {
            Field< double >::set( compt, field, val );
        }
        else if ( field == "RM" )
        {
            Field< double >::set( compt, "Rm", val / ( len  * dia * PI ) );
        }
        else if ( field == "RA" )
        {
            Field< double >::set( compt, "Ra", val*len*4 / (dia*dia*PI) );
        }
        else if ( field == "CM" )
        {
            Field< double >::set( compt, "Cm", val * len * dia * PI );
        }
        else
        {
            cout << "Warning: setCompartmentParam: field '" << field <<
                 "' not found\n";
        }
    }
}

static void setCompartmentParams(
    const vector< ObjId >& elist, const vector< double >& val,
    const string& field, const string& expr )
{
    try
    {
        nuParser parser( expr );
        for ( unsigned int i = 0; i < elist.size(); ++i )
        {
            unsigned int j = i * nuParser::numVal;
            if ( val[ j + nuParser::EXPR ] > 0 )
            {
                double len = val[j + nuParser::LEN ];
                double dia = val[j + nuParser::DIA ];
                double x = parser.eval( val.begin() + j );
                assignSingleCompartmentParams( elist[i],
                                               x, field, len, dia );
            }
        }
    }
    catch ( mu::Parser::exception_type& err )
    {
        cout << err.GetMsg() << endl;
    }
}

static void setMechParams(
    const vector< ObjId >& mech,
    const vector< ObjId >& elist, const vector< double >& val,
    const string& field, const string& expr )
{
    try
    {
        nuParser parser ( expr );
        for ( unsigned int i = 0; i < elist.size(); ++i )
        {
            unsigned int j = i * nuParser::numVal;
            if ( val[ j + nuParser::EXPR ] > 0 )
            {
                double len = val[j + nuParser::LEN ];
                double dia = val[j + nuParser::DIA ];
                double x = parser.eval( val.begin() + j );
                assignParam( mech[i], field, x, len, dia );
            }
        }
    }
    catch ( mu::Parser::exception_type& err )
    {
        cout << err.GetMsg() << endl;
    }
}

////////////////////////////////////////////////////////////////////////
// ValueFinfos
////////////////////////////////////////////////////////////////////////

void Neuron::setRM( double v )
{
    if ( v > 0.0 )
        RM_ = v;
    else
        cout << "Warning:: Neuron::setRM: value must be +ve, is " << v << endl;
}
double Neuron::getRM() const
{
    return RM_;
}

void Neuron::setRA( double v )
{
    if ( v > 0.0 )
        RA_ = v;
    else
        cout << "Warning:: Neuron::setRA: value must be +ve, is " << v << endl;
}
double Neuron::getRA() const
{
    return RA_;
}

void Neuron::setCM( double v )
{
    if ( v > 0.0 )
        CM_ = v;
    else
        cout << "Warning:: Neuron::setCM: value must be +ve, is " << v << endl;
}
double Neuron::getCM() const
{
    return CM_;
}


void Neuron::setEm( double v )
{
    Em_ = v;
}
double Neuron::getEm() const
{
    return Em_;
}


void Neuron::setTheta( double v )
{
    theta_ = v;
    // Do stuff here for rotating it.
}
double Neuron::getTheta() const
{
    return theta_;
}


void Neuron::setPhi( double v )
{
    phi_ = v;
    // Do stuff here for rotating it.
}
double Neuron::getPhi() const
{
    return phi_;
}


void Neuron::setSourceFile( string v )
{
    sourceFile_ = v;
    // Stuff here for loading it.
}

string Neuron::getSourceFile() const
{
    return sourceFile_;
}


void Neuron::setCompartmentLengthInLambdas( double v )
{
    compartmentLengthInLambdas_ = v;
}
double Neuron::getCompartmentLengthInLambdas() const
{
    return compartmentLengthInLambdas_;
}

unsigned int Neuron::getNumCompartments() const
{
    return segId_.size();
}

/*
unsigned int Neuron::getNumSpines() const
{
	return spineParentIndex_.size();
}
*/

unsigned int Neuron::getNumBranches() const
{
    return branches_.size();
}

vector< double> Neuron::getPathDistFromSoma() const
{
    vector< double > ret( segs_.size(), 0.0 );
    for ( unsigned int i = 0; i < segs_.size(); ++i )
        ret[i] = segs_[i].getPathDistFromSoma();
    return ret;
}

vector< double> Neuron::getGeomDistFromSoma() const
{
    vector< double > ret( segs_.size(), 0.0 );
    for ( unsigned int i = 0; i < segs_.size(); ++i )
        ret[i] = segs_[i].getGeomDistFromSoma();
    return ret;
}

vector< double> Neuron::getElecDistFromSoma() const
{
    vector< double > ret( segs_.size(), 0.0 );
    for ( unsigned int i = 0; i < segs_.size(); ++i )
        ret[i] = segs_[i].getElecDistFromSoma();
    return ret;
}

vector< ObjId > Neuron::getCompartments() const
{
    vector< ObjId > ret( segId_.size() );
    for ( unsigned int i = 0; i < segId_.size(); ++i )
        ret[i] = segId_[i];
    return ret;
}

/**
 * Digest the path and the math expression to obtain an elist of
 * compartments that satisfy both criteria.
 * The argument 'line' has path
 * followed by the math expression, separated by a space.
 */
vector< ObjId > Neuron::getExprElist( const Eref& e, string line ) const
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    vector< ObjId > ret;
    vector< ObjId > elist;
    vector< double > val;
    unsigned long pos = line.find_first_of( " \t" );
    string path = line.substr( 0, pos );
    string expr = line.substr( pos );
    ObjId oldCwe = shell->getCwe();
    shell->setCwe( e.objId() );
    wildcardFind( path, elist );
    shell->setCwe( oldCwe );
    if ( elist.size() == 0 )
        return ret;
    evalExprForElist( elist, expr, val );
    ret.reserve( elist.size() );
    for ( unsigned int i = 0; i < elist.size(); ++i )
    {
        if ( val[i * nuParser::numVal] > 0 )
            ret.push_back( elist[i] );
    }
    return ret;
}

/**
 * Digest the path and the math expression to obtain all the
 * values pertaining to the elements in the path. There are
 * nuParser::numVal * elist.size() entries.
 * The argument 'line' has path
 * followed by the math expression, separated by a space.
 */
vector< double > Neuron::getExprVal( const Eref& e, string line ) const
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< ObjId > elist;
	vector< double > val;
	unsigned long pos = line.find_first_of( " \t" );
	string path = line.substr( 0, pos );
	string expr = line.substr( pos );
	ObjId oldCwe = shell->getCwe();
	shell->setCwe( e.objId() );
	wildcardFind( path, elist );
	shell->setCwe( oldCwe );
	if ( elist.size() == 0 )
		return val;
	evalExprForElist( elist, expr, val );
	return val;
}

vector< ObjId > Neuron::getSpinesFromExpression(
				const Eref& e, string line ) const
{
	unsigned long pos = line.find_first_of( " \t" );
	string path = line.substr( 0, pos );
	string expr = line.substr( pos );

	// Look for all compartments that fit the expression.
	vector< ObjId > temp = getExprElist( e, "# " + expr );
	// indexed by segIndex, includes all compts in all spines.
	/*
	vector< vector< Id > > allSpinesPerCompt( segId_.size() );
	for ( unsigned int i = 0; i < spines_.size(); ++i ) {
		assert( allSpinesPerCompt.size() > spineParentSegIndex_[i] );
		vector< Id >& s = allSpinesPerCompt[ spineParentSegIndex_[i] ];
		s.insert( s.end(), spines_[i].begin(), spines_[i].end() );
	}
	*/
	vector< ObjId >ret;
	if ( allSpinesPerCompt_.size() == 0 )
		return ret;
	for ( vector< ObjId >::iterator
					i = temp.begin(); i != temp.end(); ++i ) {
		map< Id, unsigned int >::const_iterator si =
				segIndex_.find( i->id );
		assert( si != segIndex_.end() );
		assert( si->second < segId_.size() );
		if ( allSpinesPerCompt_.size() > si->second ) {
			const vector< Id >& s = allSpinesPerCompt_[ si->second ];
			for ( vector< Id >::const_iterator j = s.begin(); j != s.end(); ++j ){
				if ( matchBeforeBrace( *j, path ) )
					ret.push_back( *j );
			}
		}
	}
	return ret;
}

vector< ObjId > Neuron::getSpinesOnCompartment(
				const Eref& e, ObjId compt ) const
{
	vector< ObjId > ret;
	map< Id, unsigned int >::const_iterator pos =
			segIndex_.find( compt.id );
	if ( pos != segIndex_.end() ) {
		assert( pos->second < allSpinesPerCompt_.size() );
		const vector< Id >& spines = allSpinesPerCompt_[pos->second];
		for ( unsigned int i = 0; i < spines.size(); ++i )
			ret.push_back( spines[i] );
	}
	return ret;
}

ObjId Neuron::getParentCompartmentOfSpine(
				const Eref& e, ObjId compt ) const
{
	for ( unsigned int comptIndex = 0; comptIndex < allSpinesPerCompt_.size(); ++comptIndex ) {
		const vector< Id >& v = allSpinesPerCompt_[comptIndex];
		for ( unsigned int j = 0; j < v.size(); j++ )
			if ( v[j] == compt.id )
				return segId_[ comptIndex ];
	}
	return ObjId();
}

void Neuron::buildElist( const Eref& e,
				const vector< string >& line,
				vector< ObjId >& elist,
				vector< double >& val )
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    const string& path = line[1];
    const string& expr = line[3];
    ObjId oldCwe = shell->getCwe();
    shell->setCwe( e.objId() );
    wildcardFind( path, elist );
    shell->setCwe( oldCwe );
    evalExprForElist( elist, expr, val );
}

void Neuron::setChannelDistribution( const Eref& e, vector< string > v )
{
    // Here we should clear the extant channels, if any.
    vector< vector< string > > lines;
    if ( parseDistrib( lines, v ) )
    {
        channelDistribution_ = v;
        for ( unsigned int i = 0; i < lines.size(); ++i )
        {
            vector< string >& temp = lines[i];
            vector< ObjId > elist;
            vector< double > val;
            buildElist( e, temp, elist, val );

            vector< ObjId > mech( elist.size() );
            if ( buildFromProto( temp[0], elist, val, mech ) )
            {
                for ( unsigned int j = 2; j < temp.size(); j += 2 )
                {
                    setMechParams( mech, elist, val, temp[j], temp[j+1] );
                }
            }
        }
    }
}

vector< string > Neuron::getChannelDistribution( const Eref& e ) const
{
    return channelDistribution_;
}

void Neuron::setPassiveDistribution( const Eref& e, vector< string > v )
{
    vector< vector< string > > lines;
    if ( parseDistrib( lines, v ) )
    {
        passiveDistribution_ = v;
        for ( unsigned int i = 0; i < lines.size(); ++i )
        {
            vector< string >& temp = lines[i];
            vector< ObjId > elist;
            vector< double > val;
            buildElist( e, temp, elist, val );
            for ( unsigned int j = 2; j < temp.size(); j += 2 )
            {
                setCompartmentParams( elist, val, temp[j], temp[j+1] );
            }
        }
    }
}

vector< string > Neuron::getPassiveDistribution( const Eref& e ) const
{
    return passiveDistribution_;
}

void Neuron::setSpineDistribution( const Eref& e, vector< string > v )
{
    // Here we should clear the extant spines, if any.
    vector< vector< string > > lines;
    if ( parseDistrib( lines, v ) )
    {
        spineDistribution_ = v;
        for ( unsigned int i = 0; i < lines.size(); ++i )
        {
            vector< ObjId > elist;
            vector< double > val;
            buildElist( e, lines[i], elist, val );
            installSpines( elist, val, lines[i] );
        }
    }
}

vector< string > Neuron::getSpineDistribution( const Eref& e ) const
{
    return spineDistribution_;
}


////////////////////////////////////////////////////////////////////////
// Stuff here for parsing the compartment tree
////////////////////////////////////////////////////////////////////////

static Id getComptParent( Id id )
{
    // raxial points towards soma.
    static const Finfo* raxialFinfo =
        Cinfo::find( "Compartment" )->findFinfo( "raxialOut" );
    static const Finfo* proximalFinfo =
        Cinfo::find( "SymCompartment" )->findFinfo( "proximalOut" );

    if ( id.element()->cinfo()->isA( "CompartmentBase" ) )
    {
        vector< Id > ret;
        id.element()->getNeighbors( ret, raxialFinfo );
        if ( ret.size() == 1 )
            return ret[0];
        // If it didn't find an axial, maybe it is a symCompt
        if ( id.element()->cinfo()->isA( "SymCompartment" ) )
        {
            id.element()->getNeighbors( ret, proximalFinfo );
            if ( ret.size() == 1 )
                return ret[0];
        }
    }
    return Id();
}

// Returns Id of soma
Id fillSegIndex(
    const vector< Id >& kids, map< Id, unsigned int >& segIndex )
{
    Id soma;
    segIndex.clear();
    Id fatty;
    double maxDia = 0.0;
    unsigned int numKids = 0;
    for ( unsigned int i = 0; i < kids.size(); ++i )
    {
        const Id& k = kids[i];
        if ( k.element()->cinfo()->isA( "CompartmentBase" ) )
        {
            segIndex[ k ] = numKids++;
            const string& s = k.element()->getName();
            if ( s.find( "soma" ) != s.npos ||
                    s.find( "Soma" ) != s.npos ||
                    s.find( "SOMA" ) != s.npos )
            {
                soma = k;
            }
            double dia = Field< double >::get( k, "diameter" );
            if ( dia > maxDia )
            {
                maxDia = dia;
                fatty = k;
            }
        }
    }
    if ( soma != Id() )
        return soma;
    return fatty; // Fallback.
}

static void fillSegments( vector< SwcSegment >& segs,
                          const map< Id, unsigned int >& segIndex,
                          const vector< Id >& kids )
{
    segs.clear();
    for ( unsigned int i = 0; i < kids.size(); ++i )
    {
        const Id& k = kids[i];
        if ( k.element()->cinfo()->isA( "CompartmentBase" ) )
        {
            double x = Field< double >::get( k, "x" );
            double y = Field< double >::get( k, "y" );
            double z = Field< double >::get( k, "z" );
            double dia = Field< double >::get( k, "diameter" );
            Id pa = getComptParent( k );
            unsigned int paIndex = ~0U; // soma
            int comptType = 1; // soma
            if ( pa != Id() )
            {
                map< Id, unsigned int >::const_iterator
                j = segIndex.find( pa );
                if ( j != segIndex.end() )
                {
                    paIndex = j->second;
                    comptType = 3; // generic dendrite
                }
            }
            segs.push_back(
                SwcSegment( i, comptType, x, y, z, dia/2.0, paIndex ) );
        }
    }
}

/// Recursive function to fill in cumulative distances from soma.
static void traverseCumulativeDistance(
    SwcSegment& self, vector< SwcSegment >& segs,
    const vector< Id >& lookupId,
    double len, double L, double pSoma, double eSoma )
{
    self.setCumulativeDistance( len, L, pSoma, eSoma );
    for ( unsigned int i = 0; i < self.kids().size(); ++i )
    {
        SwcSegment& kid = segs[ self.kids()[i] ];
        double segmentLength = kid.distance( self );
        double p = pSoma + segmentLength;
        Id kidId = lookupId[ self.kids()[i] ];
        double Rm = Field< double >::get( kidId, "Rm" );
        double Ra = Field< double >::get( kidId, "Ra" );
        // Note that sqrt( Rm/Ra ) = lambda/length = 1/L.
        double electrotonicLength = sqrt( Ra / Rm );
        double e = eSoma + electrotonicLength;
        traverseCumulativeDistance( kid, segs, lookupId,
                                    segmentLength, electrotonicLength, p, e );
    }
}

void Neuron::updateSegmentLengths()
{
    double len = Field< double >::get( soma_, "length" );
    double dia = Field< double >::get( soma_, "diameter" );
    if ( len < dia )
        len = dia;
    double Rm = Field< double >::get( soma_, "Rm" );
    double Ra = Field< double >::get( soma_, "Ra" );
    double L = sqrt( Ra / Rm );

    for ( unsigned int i = 0; i < segs_.size(); ++i )
    {
        segs_[i].setGeometricalDistanceFromSoma( segs_[0] );
    }

    traverseCumulativeDistance( segs_[0], segs_, segId_, len, L, 0, 0 );
    maxL_ = maxG_ = maxP_ = 0.0;
    for ( unsigned int i = 0; i < segs_.size(); ++i )
    {
        double p = segs_[i].getPathDistFromSoma();
        if ( maxP_ < p ) maxP_ = p;
        double g = segs_[i].getGeomDistFromSoma();
        if ( maxG_ < g ) maxG_ = g;
        double L = segs_[i].getElecDistFromSoma();
        if ( maxL_ < L ) maxL_ = L;
    }
}

/// Fills up vector of segments. First entry is soma.
void Neuron::buildSegmentTree( const Eref& e )
{
    vector< Id > kids;
    Neutral::children( e, kids );

    soma_ = fillSegIndex( kids, segIndex_ );
    if ( kids.size() == 0 || soma_ == Id() )
    {
        cout << "Error: Neuron::buildSegmentTree( " << e.id().path() <<
             " ): \n		Valid neuronal model not found.\n";
        return;
    }
    fillSegments( segs_, segIndex_, kids );
    int numPa = 0;
    for ( unsigned int i = 0; i < segs_.size(); ++i )
    {
        if ( segs_[i].parent() == ~0U )
        {
            numPa++;
        }
        else
        {
            segs_[ segs_[i].parent() ].addChild( i );
        }
    }
    for ( unsigned int i = 0; i < segs_.size(); ++i )
    {
        segs_[i].figureOutType();
    }

    if ( numPa != 1 )
    {
        cout << "Warning: Neuron.cpp: buildTree: numPa = " << numPa << endl;
    }
    segId_.clear();
    segId_.resize( segIndex_.size(), Id() );
    for ( map< Id, unsigned int >::const_iterator
            i = segIndex_.begin(); i != segIndex_.end(); ++i )
    {
        assert( i->second < segId_.size() );
        segId_[ i->second ] = i->first;
    }
    updateSegmentLengths();
}


/// Fills up vector of segments. First entry is soma.
void Neuron::setSpineAndPsdMesh( Id spineMesh, Id psdMesh )
{
    if ( !spineMesh.element()->cinfo()->isA( "SpineMesh" ) )
    {
        cout << "Error: Neuron::setSpineAndPsdMesh: '" <<
             spineMesh.path() << "' is not a SpineMesh\n";
        return;
    }
    if ( !psdMesh.element()->cinfo()->isA( "PsdMesh" ) )
    {
        cout << "Error: Neuron::setSpineAndPsdMesh: '" <<
             psdMesh.path() << "' is not a PsdMesh\n";
        return;
    }
    Id spineStoich = Neutral::child( spineMesh.eref(), "stoich" );
    Id psdStoich = Neutral::child( psdMesh.eref(), "stoich" );
    if ( spineStoich == Id() || psdStoich == Id() )
    {
        cout << "Error: Neuron::setSpineAndPsdMesh: Stoich child not found\n";
        return;
    }

    vector< Id > spineList = Field< vector< Id > >::get(
                                 spineMesh, "elecComptList" );
    vector< Id > psdList = Field< vector< Id > >::get(
                               psdMesh, "elecComptList" );
    assert( spineList.size()== psdList.size() );
    map< Id, unsigned int > spineMap; // spineMap[ SpineOnNeuron ] = index
    for ( unsigned int i = 0; i < spines_.size(); ++i )
    {
        assert( spines_[i].size() > 1 );
        spineMap[ spines_[i][1] ] = i;
    }
    // We don't want to clear this because we can use a single vector
    // to overlay a number of spine meshes, since each will be a distinct
    // subset of the full list of spines. None is used twice.
    // spineToMeshOrdering_.clear();
    for( unsigned int i = 0; i < spineList.size(); ++i )
    {
        map< Id, unsigned int >::iterator j = spineMap.find( spineList[i]);
        if ( j == spineMap.end() )
        {
            cout << "Error: Neuron::setSpineAndPsdMesh: spine '" <<
                 spineList[i].path() << "' not found on Neuron\n";
            return;
        }
        spineToMeshOrdering_[ j->second ] = i;
        spineStoich_[ j->second ] = spineStoich;
        psdStoich_[ j->second ] = psdStoich;
    }
}

void Neuron::setSpineAndPsdDsolve( Id spineDsolve, Id psdDsolve )
{
    headDsolve_ = spineDsolve;
    psdDsolve_ = psdDsolve;
}

/////////////////////////////////////////////////////////////////////////
// Here we set up the more general specification of mechanisms. Each
// line is
// proto path field expr [field expr]...
/////////////////////////////////////////////////////////////////////////

/**
 * Evaluates expn for every CompartmentBase entry in elist. Pushes
 * value, length and dia for each elist entry into the 'val' vector.
 */
void Neuron::evalExprForElist( const vector< ObjId >& elist,
                               const string& expn, vector< double >& val ) const
{
    val.clear();
    val.resize( elist.size() * nuParser::numVal );
    // Build the function
    double len = 0; // Length of compt in metres
    double dia = 0; // Diameter of compt in metres
    unsigned int valIndex = 0;
    try
    {
        nuParser parser( expn );

        // Go through the elist checking for the channels. If not there,
        // build them.
        for ( vector< ObjId >::const_iterator
                i = elist.begin(); i != elist.end(); ++i )
        {
            if ( i->element()->cinfo()->isA( "CompartmentBase" ) )
            {
                map< Id, unsigned int >:: const_iterator j =
                    segIndex_.find( *i );
                if ( j != segIndex_.end() )
                {
                    dia = Field< double >::get( *i, "diameter" );
                    len = Field< double >::get( *i, "length" );
                    assert( j->second < segs_.size() );
                    val[valIndex + nuParser::P] =
                        segs_[j->second].getPathDistFromSoma();
                    val[valIndex + nuParser::G] =
                        segs_[j->second].getGeomDistFromSoma();
                    val[valIndex + nuParser::EL] =
                        segs_[j->second].getElecDistFromSoma();
                    val[valIndex + nuParser::LEN] = len;
                    val[valIndex + nuParser::DIA] = dia;
                    val[valIndex + nuParser::MAXP] = maxP_;
                    val[valIndex + nuParser::MAXG] = maxG_;
                    val[valIndex + nuParser::MAXL] = maxL_;
                    val[valIndex + nuParser::X] = segs_[j->second].vec().a0();
                    val[valIndex + nuParser::Y] = segs_[j->second].vec().a1();
                    val[valIndex + nuParser::Z] = segs_[j->second].vec().a2();
                    // Can't assign oldVal on first arg
                    val[valIndex + nuParser::OLDVAL] = 0.0;

                    val[valIndex + nuParser::EXPR] = parser.eval(
                                                         val.begin() + valIndex );
                }
            }
            valIndex += nuParser::numVal;
        }
    }
    catch ( mu::Parser::exception_type& err )
    {
        cout << err.GetMsg() << endl;
    }
}


////////////////////////////////////////////////////////////////////////
// Stuff here for inserting spines.
////////////////////////////////////////////////////////////////////////

/**
 * Utility function to return a coordinate system where
 * z is the direction of a dendritic segment,
 * x is the direction of spines outward from soma and perpendicular to z
 * and y is the perpendicular to x and z.
 */
static double coordSystem( Id soma, Id dend, Vec& x, Vec& y, Vec& z )
{
    static const double EPSILON = 1e-20;
    double x0 = Field< double >::get( dend, "x0" );
    double y0 = Field< double >::get( dend, "y0" );
    double z0 = Field< double >::get( dend, "z0" );
    double x1 = Field< double >::get( dend, "x" );
    double y1 = Field< double >::get( dend, "y" );
    double z1 = Field< double >::get( dend, "z" );

    Vec temp( x1-x0, y1-y0, z1-z0 );
    double len = temp.length();
    z = Vec( temp.a0()/len, temp.a1()/len, temp.a2()/len );

    double sx0 = Field< double >::get( soma, "x0" );
    double sy0 = Field< double >::get( soma, "y0" );
    double sz0 = Field< double >::get( soma, "z0" );
    Vec temp2( x0 - sx0, y0-sy0, z0-sz0 );
    Vec y2 = temp.crossProduct( z );
    double ylen = y2.length();
    double ytemp = 1.0;
    while ( ylen < EPSILON )
    {
        Vec t( ytemp , sqrt( 2.0 ), 0.0 );
        y2 = t.crossProduct( z );
        ylen = y2.length();
        ytemp += 1.0;
    }
    y = Vec( y2.a0()/ylen, y2.a1()/ylen, y2.a2()/ylen );
    x = z.crossProduct( y );
    assert( doubleEq( x.length(), 1.0 ) );
    return len;
}

/**
 * Utility function to change coords of spine so as to reorient it.
 */
static void reorientSpine( vector< Id >& spineCompts,
                           vector< Vec >& coords,
                           Vec& parentPos, double pos, double angle,
                           Vec& x, Vec& y, Vec& z )
{
    double c = cos( angle );
    double s = sin( angle );
    double omc = 1.0 - c;

    Vec rot0( 		z.a0()*z.a0()*omc + c,
                    z.a1()*z.a0()*omc - z.a2()*s ,
                    z.a2()*z.a0()*omc + z.a1()*s );

    Vec rot1( 		z.a0()*z.a1()*omc + z.a2()*s,
                    z.a1()*z.a1()*omc + c,
                    z.a2()*z.a1()*omc - z.a0()*s );

    Vec rot2(		z.a0()*z.a2()*omc - z.a1()*s,
                    z.a1()*z.a2()*omc + z.a0()*s,
                    z.a2()*z.a2()*omc + c );

    Vec translation = z * pos + parentPos;
    // Vec translation = parentPos;
    vector< Vec > ret( coords.size() );
    for ( unsigned int i = 0; i < coords.size(); ++i )
    {
        ret[i] = Vec( 	rot0.dotProduct( coords[i] ) + translation.a0(),
                        rot1.dotProduct( coords[i] ) + translation.a1(),
                        rot2.dotProduct( coords[i] ) + translation.a2() );

    }
    assert( spineCompts.size() * 2 == ret.size() );

    for ( unsigned int i = 0; i < spineCompts.size(); ++i )
    {
        unsigned int j = 2 * i;
        Field< double >::set( spineCompts[i], "x0", ret[j].a0() );
        Field< double >::set( spineCompts[i], "y0", ret[j].a1() );
        Field< double >::set( spineCompts[i], "z0", ret[j].a2() );
        // cout << "(" << ret[j].a0() << ", " << ret[j].a1() << ", " << ret[j].a2() << ")";
        j = j + 1;
        Field< double >::set( spineCompts[i], "x", ret[j].a0() );
        Field< double >::set( spineCompts[i], "y", ret[j].a1() );
        Field< double >::set( spineCompts[i], "z", ret[j].a2() );
        // cout << "(" << ret[j].a0() << ", " << ret[j].a1() << ", " << ret[j].a2() << ")\n";
    }
}

/**
 * Utility function to add a single spine to the given parent.
 * Returns vector of added spine contents.
 * parent is parent compartment for this spine.
 * spineProto is just that.
 * pos is position (in metres ) along parent compartment
 * angle is angle (in radians) to rotate spine wrt x in plane xy.
 * Size is size scaling factor, 1 leaves as is.
 * x, y, z are unit vectors. Z is along the parent compt.
 * We first shift the spine over so that it is offset by the parent compt
 * diameter.
 * We then need to reorient the spine which lies along (i,0,0) to
 * lie along x. X is a unit vector so this is done simply by
 * multiplying each coord of the spine by x.
 * Finally we rotate the spine around the z axis by the specified angle
 * k is index of this spine.
 */

static vector< Id > addSpine( Id parentCompt, Id spineProto,
                              double pos, double angle,
                              Vec& x, Vec& y, Vec& z,
                              double size,
                              unsigned int k )
{
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    Id parentObject = Neutral::parent( parentCompt );
    stringstream sstemp;
    sstemp << k;
    string kstr = sstemp.str();
    Id spine = shell->doCopy( spineProto, parentObject, "_spine" + kstr,
                              1, false, false );
    vector< Id > kids;
    Neutral::children( spine.eref(), kids );
    double x0 = Field< double >::get( parentCompt, "x0" );
    double y0 = Field< double >::get( parentCompt, "y0" );
    double z0 = Field< double >::get( parentCompt, "z0" );
    double parentRadius = Field< double >::get( parentCompt, "diameter" )/2;
    Vec ppos( x0, y0, z0 );
    // First, build the coordinates vector for the spine. Assume that its
    // major axis is along the unit vector [1,0,0].
    vector< Vec > coords;
    vector< Id > ret; // Filtered to make sure that it returns only compts
    for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i )
    {
        if ( i->element()->cinfo()->isA( "CompartmentBase" ) )
        {
            i->element()->setName( i->element()->getName() + kstr );
            x0 = Field< double >::get( *i, "x0" ) * size;
            y0 = Field< double >::get( *i, "y0" ) * size;
            z0 = Field< double >::get( *i, "z0" ) * size;
            coords.push_back( Vec( x0 + parentRadius, y0, z0 ) );
            double x = Field< double >::get( *i, "x" ) * size;
            double y = Field< double >::get( *i, "y" ) * size;
            double z = Field< double >::get( *i, "z" ) * size;
            double dia = Field< double >::get( *i, "diameter" ) * size;
            // Field< double >::set( *i, "diameter", dia );

            double len = sqrt(
                             (x-x0)*(x-x0) +
                             (y-y0)*(y-y0) +
                             (z-z0)*(z-z0) );
            SetGet2< double, double >::set( *i, "setGeomAndElec", len, dia );


            coords.push_back( Vec( x + parentRadius, y, z ) );
            // scaleSpineCompt( *i, size );
            shell->doMove( *i, parentObject );
            ret.push_back( *i );
        }
    }
    // Then, take the projection of this along the x vector passed in.
    for( vector< Vec >::iterator i = coords.begin(); i != coords.end(); ++i)
        *i = x * i->a0();
    shell->doDelete( spine ); // get rid of the holder for the spine copy.
    shell->doAddMsg( "Single", parentCompt, "axial", kids[0], "raxial" );
    reorientSpine( kids, coords, ppos, pos, angle, x, y, z );

    return ret;
}

///////////////////////////////////////////////////////////////////////
// Here are the mechSpec format line parsers
///////////////////////////////////////////////////////////////////////

// Return value for field if found, otherwise return a default string.
string findArg( const vector<string>& line, const string& field )
{
    // line has: proto path [field val]...
    assert( (line.size() % 2) != 1 );
    for ( unsigned int i = 2; i < line.size(); i+=2 )
        if ( line[i] == field )
            return line[i+1];
    string ret = "";
    if ( field == "spacing" )
    {
        ret = "1.0e-6";
    }
    else if ( field == "spacingDistrib" )
    {
        ret = "0";
    }
    else if ( field == "size" )
    {
        ret = "1";
    }
    else if ( field == "sizeDistrib" )
    {
        ret = "0";
    }
    else if ( field == "angle" )
    {
        ret = "0";
    }
    else if ( field == "angleDistrib" )
    {
        ret = "6.283185307";
    }
    else if ( field == "rotation" )
    {
        ret = "0";
    }
    else if ( field == "rotationDistrib" )
    {
        ret = "6.283185307"; // 2*PI
    }
    else if ( field == "shaftLen" )
    {
        ret = "1.0e-6";
    }
    else if ( field == "shaftDia" )
    {
        ret = "0.2e-6";
    }
    else if ( field == "headLen" )
    {
        ret = "0.5e-6";
    }
    else if ( field == "headDia" )
    {
        ret = "0.5e-6";
    }
    else if ( field == "theta" )
    {
        ret = "0";
    }
    else if ( field == "phi" )
    {
        ret = "1.5707963268"; // PI/2
    }
    return ret;
}

/// Add entries into the pos vector for a given compartment i.
static void addPos( unsigned int segIndex, unsigned int eIndex,
		double spacing, double minSpacing,
		double dendLength,
		vector< unsigned int >& seglistIndex,
		vector< unsigned int >& elistIndex,
		vector< double >& pos )
{
	if ( minSpacing < spacing * 0.1 && minSpacing < 1e-7 )
		minSpacing = spacing * 0.1;
	if ( minSpacing > spacing * 0.5 )
		minSpacing = spacing * 0.5;
	unsigned int n = 1 + dendLength / minSpacing;
	double dx = dendLength / n;
	for( unsigned int i = 0; i < n; ++i ) {
		if ( moose::mtrand() < dx / spacing ) {
			seglistIndex.push_back( segIndex );
			elistIndex.push_back( eIndex );
			pos.push_back( i * dx + dx*0.5 );
		}
	}
}
/*
 * This version tries to put in Pos using simple increments from the
 * start of each compt. Multiple issues including inability to put
 * spines in small compartments, even if many of them.
 *
static void addPos( unsigned int segIndex, unsigned int eIndex,
		double spacing, double spacingDistrib,
		double dendLength,
		vector< unsigned int >& seglistIndex,
		vector< unsigned int >& elistIndex,
		vector< double >& pos )
{
	if ( spacingDistrib > 0.0 ) {
		double position = spacing * 0.5 +
				( moose::mtrand() - 0.5 ) * spacingDistrib;
		while ( position < dendLength ) {
			seglistIndex.push_back( segIndex );
			elistIndex.push_back( eIndex );
			pos.push_back( position );
			position += spacing + ( moose::mtrand() - 0.5 ) * spacingDistrib;
		}
	} else {
		for ( double position = spacing * 0.5;
				position < dendLength; position += spacing ) {
			seglistIndex.push_back( segIndex );
			elistIndex.push_back( eIndex );
			pos.push_back( position );
		}
	}
}
*/

void Neuron::makeSpacingDistrib( const vector< ObjId >& elist,
		const vector< double >& val,
		vector< unsigned int >& seglistIndex,
		vector< unsigned int >& elistIndex,
		vector< double >& pos,
		const vector< string >& line ) const
{
	string distribExpr = findArg( line, "spacingDistrib"  );
	pos.resize( 0 );
	elistIndex.resize( 0 );

	try {
		nuParser parser( distribExpr );

		for ( unsigned int i = 0; i < elist.size(); ++i ) {
			unsigned int j = i * nuParser::numVal;
			if ( val[ j + nuParser::EXPR ] > 0 ) {
				double spacing = val[ j + nuParser::EXPR ];
				double spacingDistrib = parser.eval( val.begin() + j );
				if ( spacingDistrib > spacing || spacingDistrib < 0 ) {
					cout << "Warning: Neuron::makeSpacingDistrib: " <<
						"0 < " << spacingDistrib << " < " << spacing <<
						" fails on " << elist[i].path() << ". Using 0.\n";
					spacingDistrib = 0.0;
				}
				map< Id, unsigned int>::const_iterator
					lookupDend = segIndex_.find( elist[i] );
				if ( lookupDend != segIndex_.end() ) {
					double dendLength = segs_[lookupDend->second].length();
					addPos( lookupDend->second, i,
								spacing, spacingDistrib, dendLength,
								seglistIndex, elistIndex, pos );
				}
			}
		}
	}
	catch ( mu::Parser::exception_type& err )
	{
		cout << err.GetMsg() << endl;
	}
}

static void makeAngleDistrib ( const vector< ObjId >& elist,
		const vector< double >& val,
		vector< unsigned int >& elistIndex,
		vector< double >& theta,
		const vector< string >& line )
{
	string angleExpr = findArg( line, "angle" );
	string angleDistribExpr = findArg( line, "angleDistrib" );
	// I won't bother with rotation and rotation distrb for now.
	// Easy to add, but on reflection they don't make sense.
	theta.clear();
	theta.resize( elistIndex.size(), 0.0 );

	try {
		nuParser angleParser( angleExpr );
		nuParser distribParser( angleDistribExpr );
		unsigned int lastIndex = ~0U;
		double angle = 0;
		double angleDistrib = 0;
		for ( unsigned int k = 0; k < elistIndex.size(); ++k ) {
			unsigned int i = elistIndex[k];
			if ( i != lastIndex ) {
				lastIndex = i;
				unsigned int j = i * nuParser::numVal;
				angle = angleParser.eval( val.begin() + j );
				angleDistrib = distribParser.eval( val.begin() + j);
			}
			if ( angleDistrib > 0 )
				theta[k] = angle + ( moose::mtrand() - 0.5 ) * angleDistrib;
			else
				theta[k] = angle;
		}
	}
	catch ( mu::Parser::exception_type& err )
	{
		cout << err.GetMsg() << endl;
	}
}

static void makeSizeDistrib ( const vector< ObjId >& elist,
		const vector< double >& val,
		vector< unsigned int >& elistIndex,
		vector< double >& size,
		const vector< string >& line )
{
	string sizeExpr = findArg( line, "size" );
	string sizeDistribExpr = findArg( line, "sizeDistrib" );
	size.clear();
	size.resize( elistIndex.size(), 0.0 );

	try {
		nuParser sizeParser( sizeExpr );
		nuParser distribParser( sizeDistribExpr );
		unsigned int lastIndex = ~0U;
		double sz = 1.0;
		double sizeDistrib = 0;
		for ( unsigned int k = 0; k < elistIndex.size(); ++k ) {
			unsigned int i = elistIndex[k];
			if ( i != lastIndex ) {
				lastIndex = i;
				unsigned int j = i * nuParser::numVal;
				sz = sizeParser.eval( val.begin() + j );
				sizeDistrib = distribParser.eval( val.begin() + j);
			}
			if ( sizeDistrib > 0 )
				size[k] = sz + ( moose::mtrand() - 0.5 ) * sizeDistrib;
			else
				size[k] = sz;
		}
	}
	catch ( mu::Parser::exception_type& err )
	{
		cout << err.GetMsg() << endl;
	}
}

void Neuron::installSpines( const vector< ObjId >& elist,
		const vector< double >& val, const vector< string >& line )
{
	Id spineProto( "/library/spine" );

	if ( spineProto == Id() ) {
		cout << "Warning: Neuron::installSpines: Unable to find prototype spine: /library/spine\n";
		return;
	}
	// Look up elist index from pos index, since there may be many
	// spines on each segment.
	vector< unsigned int > elistIndex;
	vector< double > pos; // spacing of the new spines along compt.
	vector< double > theta; // Angle of spines
	vector< double > size; // Size scaling of spines
	pos.reserve( elist.size() );
	elistIndex.reserve( elist.size() );

	makeSpacingDistrib( elist, val,
					spineParentSegIndex_, elistIndex, pos, line);
	makeAngleDistrib( elist, val, elistIndex, theta, line );
	makeSizeDistrib( elist, val, elistIndex, size, line );
	for ( unsigned int k = 0; k < spineParentSegIndex_.size(); ++k ) {
		unsigned int i = spineParentSegIndex_[k];
		Vec x, y, z;
		coordSystem( soma_, segId_[i], x, y, z );
		spines_.push_back(
			addSpine( segId_[i], spineProto, pos[k], theta[k],
				x, y, z, size[k], k )
		);
	}
	spineToMeshOrdering_.clear();
	spineToMeshOrdering_.resize( spines_.size(), 0 );
	spineStoich_.clear();
	spineStoich_.resize( spines_.size() );
	psdStoich_.clear();
	psdStoich_.resize( spines_.size() );

	/// Now fill in allSpinesPerCompt_ vector. First clear it out.
	allSpinesPerCompt_.clear();
   	allSpinesPerCompt_.resize(segId_.size() );
	for ( unsigned int i = 0; i < spines_.size(); ++i ) {
		assert( allSpinesPerCompt_.size() > spineParentSegIndex_[i] );
		vector< Id >& s = allSpinesPerCompt_[ spineParentSegIndex_[i] ];
		s.insert( s.end(), spines_[i].begin(), spines_[i].end() );
	}
}
////////////////////////////////////////////////////////////////////////
// Interface funcs for spines
////////////////////////////////////////////////////////////////////////

Spine* Neuron::lookupSpine( unsigned int index )
{
    return &spineEntry_;
}

void Neuron::setNumSpines( unsigned int num )
{
    /// Ignore it. the number of spines is set when we build the tree.
    ;
}

unsigned int Neuron::getNumSpines() const
{
    return spines_.size();
}

////////////////////////////////////////////////////////////////////////
// Utility funcs for spines
////////////////////////////////////////////////////////////////////////
const vector< Id >& Neuron::spineIds( unsigned int index ) const
{
    static vector< Id > fail;
    if ( index < spines_.size() )
        return spines_[index];
    return fail;
}

void Neuron::scaleBufAndRates( unsigned int spineNum,
                               double lenScale, double diaScale ) const
{
    if ( spineStoich_.size() == 0 )
        // Perhaps no chem stuff in model, but user could have forgotten
        // to assign psd and spine meshes.
        return;

    if ( spineNum > spineStoich_.size() )
    {
        cout << "Error: Neuron::scaleBufAndRates: spineNum too big: " <<
             spineNum << " >= " << spineStoich_.size() << endl;
        return;
    }
    Id ss = spineStoich_[ spineNum ];
    if ( ss == Id() )
    {
        // The chem system for the spine may not have been defined.
        // Later figure out how to deal with cases where there is a psd
        // but no spine.
        return;
    }
    Id ps = psdStoich_[ spineNum ];
    if ( ps == Id() )
    {
        // The chem system for the spine may not have been defined.
        return;
    }
    double volScale = lenScale * diaScale * diaScale;
    SetGet2< unsigned int, double >::set( ss, "scaleBufsAndRates",
                                          spineToMeshOrdering_[spineNum], volScale );
    volScale = diaScale * diaScale;
    SetGet2< unsigned int, double >::set( ps, "scaleBufsAndRates",
                                          spineToMeshOrdering_[spineNum], volScale );
}

/// Scale the diffusion parameters due to a change in the shaft dimensions
void Neuron::scaleShaftDiffusion( unsigned int spineNum,
                                  double len, double dia) const
{
    double diffScale = dia * dia * 0.25 * PI / len;
    SetGet2< unsigned int, double >::set(
        // Note that the buildNeuroMeshJunctions function is called
        // on the dendDsolve with the args smdsolve, pmdsolve.
        headDsolve_, "setDiffScale",
        spineToMeshOrdering_[ spineNum ], diffScale );
}

/// Scale the diffusion parameters due to a change in the head dimensions
void Neuron::scaleHeadDiffusion( unsigned int spineNum,
                                 double len, double dia) const
{
    double vol = len * dia * dia * PI * 0.25;
    double diffScale = dia * dia * 0.25 * PI / len;
    unsigned int meshIndex = spineToMeshOrdering_[ spineNum ];
    Id headCompt = Field< Id >::get( headDsolve_, "compartment" );
    LookupField< unsigned int, double >::set( headCompt, "oneVoxelVolume",
            meshIndex, vol );
    Id psdCompt = Field< Id >::get( psdDsolve_, "compartment" );
    double thick = Field< double >::get( psdCompt, "thickness" );
    double psdVol = thick * dia * dia * PI * 0.25;
    LookupField< unsigned int, double >::set( psdCompt, "oneVoxelVolume",
            meshIndex, psdVol );
    SetGet2< unsigned int, double >::set(
        headDsolve_, "setDiffVol1", meshIndex, vol );
    SetGet2< unsigned int, double >::set(
        psdDsolve_, "setDiffVol2", meshIndex, vol );
    SetGet2< unsigned int, double >::set(
        psdDsolve_, "setDiffVol1", meshIndex, psdVol );
    SetGet2< unsigned int, double >::set(
        psdDsolve_, "setDiffScale", meshIndex, diffScale );
}
