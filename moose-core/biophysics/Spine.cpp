/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2015 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "../utility/Vec.h"
#include "SwcSegment.h"
#include "Spine.h"
#include "Neuron.h"

const Cinfo* Spine::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ElementValueFinfo< Spine, double > shaftLength (
			"shaftLength",
			"Length of spine shaft.",
			&Spine::setShaftLength,
			&Spine::getShaftLength
		);
		static ElementValueFinfo< Spine, double > shaftDiameter (
			"shaftDiameter",
			"Diameter of spine shaft.",
			&Spine::setShaftDiameter,
			&Spine::getShaftDiameter
		);
		static ElementValueFinfo< Spine, double > headLength (
			"headLength",
			"Length of spine head.",
			&Spine::setHeadLength,
			&Spine::getHeadLength
		);
		static ElementValueFinfo< Spine, double > headDiameter (
			"headDiameter",
			"Diameter of spine head, and also the diameter of the PSD. ",
			&Spine::setHeadDiameter,
			&Spine::getHeadDiameter
		);
		static ElementValueFinfo< Spine, double > psdArea (
			"psdArea",
			"Area of the Post synaptic density, PSD. This is the same as "
			"the cross-section area of spine head, perpendicular to shaft. "
			"Assumes that the head is a cylinder and that its length "
			"does not change. \n"
			"This is useful to scale # of surface molecules on the PSD. ",
			&Spine::setPsdArea,
			&Spine::getPsdArea
		);
		static ElementValueFinfo< Spine, double > headVolume (
			"headVolume",
			"Volume of spine head, treating it as a cylinder. When this is "
			"scaled by the user, both the diameter and the length of the "
			"spine head scale by the cube root of the ratio to the "
			"previous volume. The diameter of the PSD is pegged to the "
			"diameter of the spine head. \n"
			"This is useful to scale total # of molecules in the head. ",
			&Spine::setHeadVolume,
			&Spine::getHeadVolume
		);
		static ElementValueFinfo< Spine, double > totalLength (
			"totalLength",
			"Length of entire spine. Scales both the length of the shaft "
			"and of the spine head, without changing any of the diameters.",
			&Spine::setTotalLength,
			&Spine::getTotalLength
		);
		static ElementValueFinfo< Spine, double > angle (
			"angle",
			"Angle of spine around shaft. Longitude. 0 is away from soma. "
			"Not yet implemented. ",
			&Spine::setAngle,
			&Spine::getAngle
		);
		static ElementValueFinfo< Spine, double > inclination (
			"inclination",
			"inclination of spine with ref to shaft. Normal is 0. "
			"Not yet activated. ",
			&Spine::setInclination,
			&Spine::getInclination
		);
		static ElementValueFinfo< Spine, double > minimumSize (
			"minimumSize",
			"Sanity check for the smallest permitted length or diameter. "
			"Used to avoid unreasonable physiological values, which "
		    "are all too easily reached when simulations run unbounded. "
		 	"Defaults to 20 nanometers, which is somewhat smaller than the "
		 	"30 nm size estimated for synaptic vesicles. "
			"Does *not* retroactively resize anything. ",
			&Spine::setMinimumSize,
			&Spine::getMinimumSize
		);
		static ElementValueFinfo< Spine, double > maximumSize (
			"maximumSize",
			"Sanity check for the largest permitted length or diameter. "
			"Used to avoid unreasonable physiological values, which "
		    "are all too easily reached when simulations run unbounded. "
		 	"Defaults to 10 microns, which is a pretty monstrous spine. "
			"Does *not* retroactively resize anything. ",
			&Spine::setMaximumSize,
			&Spine::getMaximumSize
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////
		// Shared definitions
		///////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		// SrcFinfo Definitions
		//////////////////////////////////////////////////////////////

	static Finfo* spineFinfos[] = {
		&shaftLength,		// Value
		&shaftDiameter,		// Value
		&headLength,		// Value
		&headDiameter,		// Value
		&psdArea,			// Value
		&headVolume,		// Value
		&totalLength,		// Value
	};

	static string doc[] =
	{
			"Name", "Spine",
			"Author", "Upi Bhalla",
			"Description", "Spine wrapper, used to change its morphology "
			"typically by a message from an adaptor. The Spine class "
			"takes care of a lot of resultant scaling to electrical, "
			"chemical, and diffusion properties. "
	};
	static Dinfo< Spine > dinfo;
	static Cinfo spineCinfo (
		"Spine",
		Neutral::initCinfo(),
		spineFinfos,
		sizeof( spineFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof(doc)/sizeof( string ),
		true // This IS a FieldElement, not be be created directly.
	);

	return &spineCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* spineCinfo = Spine::initCinfo();

Spine::Spine()
	: parent_( 0 ),
		minimumSize_( 20.0e-9 ),	// 20 nanometres
		maximumSize_( 10.0e-6 )		// 10 microns
{;}

Spine::Spine( const Neuron* parent )
	: parent_( parent ),
		minimumSize_( 20.0e-9 ),	// 20 nanometres
		maximumSize_( 10.0e-6 )		// 10 microns
{;}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

double Spine::getShaftLength( const Eref& e ) const
{
	const vector< Id >& sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 0 &&
					sl[0].element()->cinfo()->isA( "CompartmentBase" ) )
		return Field< double >::get( sl[0], "length" );
	return 0.0;
}

void Spine::setShaftLength( const Eref& e, double len )
{
	if ( len < minimumSize_ )
		len = minimumSize_;
	else if ( len > maximumSize_ )
		len = maximumSize_;
	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
			sl[0].element()->cinfo()->isA( "CompartmentBase" ) )
	{
		double origDia = Field< double >::get( sl[0], "diameter" );
		double dx = Field< double >::get( sl[0], "x" );
		double dy = Field< double >::get( sl[0], "y" );
		double dz = Field< double >::get( sl[0], "z" );
		SetGet2< double, double >::set(
			sl[0], "setGeomAndElec", len, origDia );

		dx = Field< double >::get( sl[0], "x" ) - dx;
		dy = Field< double >::get( sl[0], "y" ) - dy;
		dz = Field< double >::get( sl[0], "z" ) - dz;

		SetGet3< double, double, double >::set( sl[1], "displace",
			dx, dy, dz );
		// Here we've set the electrical and geometrical stuff. Now to
		// do the diffusion. Chem doesn't come into the picture for the
		// spine shaft.
		// Assume the scaleDiffusion function propagates changes into the
		// VoxelJunctions used by the Dsolve.
		parent_->scaleShaftDiffusion( e.fieldIndex(), len, origDia );
	}
}

double Spine::getShaftDiameter( const Eref& e ) const
{
	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 0 &&
				sl[0].element()->cinfo()->isA( "CompartmentBase" ) )
		return Field< double >::get( sl[0], "diameter" );
	return 0.0;
}

void Spine::setShaftDiameter( const Eref& e, double dia )
{
	if ( dia < minimumSize_ )
		dia = minimumSize_;
	else if ( dia > maximumSize_ )
		dia = maximumSize_;

	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
					sl[0].element()->cinfo()->isA( "CompartmentBase") )
	{
		double origLen = Field< double >::get( sl[0], "length" );
		SetGet2< double, double >::set(
			sl[0], "setGeomAndElec", origLen, dia );
		// Dia is changing, leave the coords alone.
		parent_->scaleShaftDiffusion( e.fieldIndex(), origLen, dia );
	}
}

double Spine::getHeadLength( const Eref& e ) const
{
	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
					sl[1].element()->cinfo()->isA( "CompartmentBase" ) )
		return Field< double >::get( sl[1], "length" );
	return 0.0;
}

void Spine::setHeadLength( const Eref& e, double len )
{
	if ( len < minimumSize_ )
		len = minimumSize_;
	else if ( len > maximumSize_ )
		len = maximumSize_;

	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
					sl[1].element()->cinfo()->isA( "CompartmentBase") )
	{
		double origDia = Field< double >::get( sl[1], "diameter" );
		double origLen = Field< double >::get( sl[1], "length" );
		SetGet2< double, double >::set(
			sl[1], "setGeomAndElec", len, origDia );
		// Here we've set the electrical and geometrical stuff. Now to
		// do the diffusion.
		// Assume the scaleDiffusion function propagates changes into the
		// VoxelJunctions used by the Dsolve.
		parent_->scaleHeadDiffusion( e.fieldIndex(), len, origDia );
		// Now scale the chem stuff. The PSD mesh is assumed to scale only
		// with top surface area of head, so it is not affected here.
		parent_->scaleBufAndRates( e.fieldIndex(), len/origLen, 1.0 );
	}
}

double Spine::getHeadDiameter( const Eref& e ) const
{
	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
			sl[1].element()->cinfo()->isA( "CompartmentBase" ) )
		return Field< double >::get( sl[1], "diameter" );
	return 0.0;
}

void Spine::setHeadDiameter( const Eref& e, double dia )
{
	if ( dia < minimumSize_ )
		dia = minimumSize_;
	else if ( dia > maximumSize_ )
		dia = maximumSize_;
	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
			sl[0].element()->cinfo()->isA( "CompartmentBase") )
	{
		double origLen = Field< double >::get( sl[1], "length" );
		double origDia = Field< double >::get( sl[1], "diameter" );
		SetGet2< double, double >::set(
			sl[1], "setGeomAndElec", origLen, dia );
		parent_->scaleHeadDiffusion( e.fieldIndex(), origLen, dia );
		parent_->scaleBufAndRates( e.fieldIndex(),
						1.0, dia/origDia );
	}
}

double Spine::getPsdArea( const Eref& e ) const
{
	double ret = getHeadDiameter( e );
	return ret * ret * PI / 4.0;
}

void Spine::setPsdArea( const Eref& e, double area )
{
	if ( area < 0 ) {
		setHeadDiameter( e, minimumSize_ );
	} else  {
		double dia = 2.0 * sqrt( area / PI );
		setHeadDiameter( e, dia );
	}
}

double Spine::getHeadVolume( const Eref& e ) const
{
	double dia = getHeadDiameter( e );
	return getHeadLength( e ) * dia * dia * PI / 4.0;
}

// Handle like a cylinder of equal length and dia. vol = PI*dia*dia*len/4
void Spine::setHeadVolume( const Eref& e, double volume )
{
	if ( volume < 0 )
		volume = 0.0;
	double dia = pow( volume * 4.0 / PI, 1.0/3.0 );
	if ( dia < minimumSize_ )
		volume = pow( minimumSize_, 3.0 ) * PI / 4.0;
	else if ( dia > maximumSize_ )
		volume = pow( maximumSize_, 3.0 ) * PI / 4.0;

	vector< Id > sl = parent_->spineIds( e.fieldIndex() );
	if ( sl.size() > 1 &&
			sl[0].element()->cinfo()->isA( "CompartmentBase") )
	{
		double origLen = Field< double >::get( sl[1], "length" );
		double origDia = Field< double >::get( sl[1], "diameter" );
		double oldVolume = origLen * origDia * origDia * PI / 4.0;
		double ratio = pow( volume / oldVolume, 1.0/3.0 );

		SetGet2< double, double >::set(
			sl[1], "setGeomAndElec", origLen * ratio, origDia * ratio );
		parent_->scaleHeadDiffusion( e.fieldIndex(), origLen * ratio, origDia * ratio );
		parent_->scaleBufAndRates( e.fieldIndex(), ratio, ratio );
	}
}

double Spine::getTotalLength( const Eref& e ) const
{
	return getHeadLength( e ) + getShaftLength( e );
}

void Spine::setTotalLength( const Eref& e, double len )
{
	double shaftLen = getShaftLength( e );
	double headLen = getHeadLength( e );
	double totLen = shaftLen + headLen;

	shaftLen *= len / totLen;
	headLen *= len / totLen;

	/// Don't set this if either of them is out of range.
	if ( shaftLen < minimumSize_ || shaftLen > maximumSize_ ||
		headLen < minimumSize_ || headLen > maximumSize_ )
		return;

	setShaftLength( e, shaftLen );
	setHeadLength( e, headLen );
}

double Spine::getAngle( const Eref& e ) const
{
	return 0;
}

void Spine::setAngle( const Eref& e, double theta )
{
	;
}

double Spine::getInclination( const Eref& e ) const
{
	return 0;
}

void Spine::setInclination( const Eref& e, double theta )
{
	;
}

double Spine::getMinimumSize( const Eref& e ) const
{
	return minimumSize_;
}

void Spine::setMinimumSize( const Eref& e, double len )
{
	minimumSize_ = len;
}

double Spine::getMaximumSize( const Eref& e ) const
{
	return maximumSize_;
}

void Spine::setMaximumSize( const Eref& e, double len )
{
	maximumSize_ = len;
}
