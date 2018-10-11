/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/ElementValueFinfo.h"
#include "MeshEntry.h"
#include "Boundary.h"
// #include "Stencil.h"
#include "ChemCompt.h"

static SrcFinfo5< double, unsigned int, unsigned int, vector< unsigned int>, vector< double > >  *remeshOut()
{
	static SrcFinfo5< double, unsigned int, unsigned int, vector< unsigned int>, vector< double > >  remeshOut(
	"remeshOut",
	"Tells the target pool or other entity that the compartment subdivision"
	"(meshing) has changed, and that it has to redo its volume and "
	"memory allocation accordingly."
	"Arguments are: oldvol, numTotalEntries, startEntry, localIndices, vols"
	"The vols specifies volumes of each local mesh entry. It also specifies"
	"how many meshEntries are present on the local node."
	"The localIndices vector is used for general load balancing only."
	"It has a list of the all meshEntries on current node."
	"If it is empty, we assume block load balancing. In this second"
	"case the contents of the current node go from "
	"startEntry to startEntry + vols.size()."
	);
	return &remeshOut;
}

static SrcFinfo0 *remeshReacsOut()
{
	static SrcFinfo0 remeshReacsOut(
		"remeshReacsOut",
		"Tells connected enz or reac that the compartment subdivision"
		"(meshing) has changed, and that it has to redo its volume-"
		"dependent rate terms like numKf_ accordingly."
	);
	return &remeshReacsOut;
}

const Cinfo* MeshEntry::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ReadOnlyElementValueFinfo< MeshEntry, double > volume(
			"volume",
			"Volume of this MeshEntry",
			&MeshEntry::getVolume
		);

		static ReadOnlyElementValueFinfo< MeshEntry, unsigned int >
			dimensions (
			"dimensions",
			"number of dimensions of this MeshEntry",
			&MeshEntry::getDimensions
		);

		static ReadOnlyElementValueFinfo< MeshEntry, unsigned int >
			meshType(
			"meshType",
		 	" The MeshType defines the shape of the mesh entry."
		 	" 0: Not assigned"
		 	" 1: cuboid"
		 	" 2: cylinder"
		 	" 3. cylindrical shell"
		 	" 4: cylindrical shell segment"
		 	" 5: sphere"
		 	" 6: spherical shell"
		 	" 7: spherical shell segment"
		 	" 8: Tetrahedral",
			&MeshEntry::getMeshType
		);

		static ReadOnlyElementValueFinfo< MeshEntry, vector< double >  >
			coordinates (
			"Coordinates",
			"Coordinates that define current MeshEntry. Depend on MeshType.",
			&MeshEntry::getCoordinates
		);

		static ReadOnlyElementValueFinfo< MeshEntry, vector< unsigned int >  >
			neighbors (
			"neighbors",
			"Indices of other MeshEntries that this one connects to",
			&MeshEntry::getNeighbors
		);

		static ReadOnlyElementValueFinfo< MeshEntry, vector< double >  >
			diffusionArea (
			"DiffusionArea",
			"Diffusion area for geometry of interface",
			&MeshEntry::getDiffusionArea
		);

		static ReadOnlyElementValueFinfo< MeshEntry, vector< double >  >
			diffusionScaling (
			"DiffusionScaling",
			"Diffusion scaling for geometry of interface",
			&MeshEntry::getDiffusionScaling
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< MeshEntry >( &MeshEntry::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< MeshEntry >( &MeshEntry::reinit ) );
		///////////////////////////////////////////////////////
		// Shared definitions
		///////////////////////////////////////////////////////
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

		static Finfo* meshShared[] = {
			remeshOut(), volume.getFinfo()
		};

		static SharedFinfo mesh( "mesh",
			"Shared message for updating mesh volumes and subdivisions,"
			"typically controls pool volumes",
			meshShared, sizeof( meshShared ) / sizeof( const Finfo* )
		);


		//////////////////////////////////////////////////////////////
		// SrcFinfo Definitions
		//////////////////////////////////////////////////////////////

	static Finfo* meshFinfos[] = {
		&volume,		// Readonly Value
		&dimensions,	// Readonly Value
		&meshType,		// Readonly Value
		&coordinates,	// Readonly Value
		&neighbors,		// Readonly Value
		&diffusionArea,	// Readonly Value
		&diffusionScaling,	// Readonly Value
		&proc,			// SharedFinfo
		&mesh,			// SharedFinfo
		remeshReacsOut(),	// SrcFinfo
	};

	static string doc[] =
	{
			"Name", "MeshEntry",
			"Author", "Upi Bhalla",
			"Description", "One voxel in a chemical reaction compartment",
	};
	static Dinfo< MeshEntry > dinfo;
	static Cinfo meshEntryCinfo (
		"MeshEntry",
		Neutral::initCinfo(),
		meshFinfos,
		sizeof( meshFinfos ) / sizeof ( Finfo* ),
		&dinfo,
		doc,
		sizeof(doc)/sizeof( string ),
		true // This IS a FieldElement, not be be created directly.
	);

	return &meshEntryCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* meshEntryCinfo = MeshEntry::initCinfo();

MeshEntry::MeshEntry()
	: parent_( 0 )
{;}

MeshEntry::MeshEntry( const ChemCompt* parent )
	: parent_( parent )
{;}

//////////////////////////////////////////////////////////////
// Process operations. Used for reac-diff calculations.
//////////////////////////////////////////////////////////////

/**
 * Update the diffusion rate terms. Note that these stay the same
 * through the entire clock tick, even if the GSL solver takes many
 * intermediate steps. The assumption is that the concs change slowly
 * enough that the diffusion terms are not hugely changed over the duration
 * of one tick. Also assume that diffusion itself is slow. The latter
 * assumption is OK for suitable grid sizes. The first assumption is OK
 * with a sensible tick step.
 */
void MeshEntry::process( const Eref& e, ProcPtr info )
{
	// cout << "updateDiffusion for " << e.fieldIndex() << ", at t = " << info->currTime << ", on thr = " << info->threadIndexInGroup << endl << flush;
	// parent_->updateDiffusion( e.fieldIndex() );
}

/**
 * Assigns the Stoich Id to the parent.
 */
void MeshEntry::reinit( const Eref& e, ProcPtr info )
{
	if ( e.dataIndex() == 0 ) {
		ObjId pa = Neutral::parent( e );
		// parent_->lookupStoich( pa );
	}
}


//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

double MeshEntry::getVolume( const Eref& e ) const
{
	//return parent_->getMeshEntryVolume( e.dataIndex() );
	return parent_->getMeshEntryVolume( e.fieldIndex() );
}

unsigned int MeshEntry::getDimensions( const Eref& e ) const
{
	return parent_->getMeshDimensions( e.fieldIndex() );
}

unsigned int MeshEntry::getMeshType( const Eref& e ) const
{
	return parent_->getMeshType( e.fieldIndex() );
}

vector< double >MeshEntry::getCoordinates( const Eref& e ) const
{
	return parent_->getCoordinates( e.fieldIndex() );
}

vector< unsigned int >MeshEntry::getNeighbors( const Eref& e ) const
{
	return parent_->getNeighbors( e.fieldIndex() );
}


vector< double >MeshEntry::getDiffusionArea( const Eref& e ) const
{
	return parent_->getDiffusionArea( e.fieldIndex() );
}


vector< double >MeshEntry::getDiffusionScaling( const Eref& e ) const
{
	return parent_->getDiffusionScaling( e.fieldIndex() );
}

//////////////////////////////////////////////////////////////
// Utility function to pass on mesh changes
//////////////////////////////////////////////////////////////
void MeshEntry::triggerRemesh( const Eref& e,
	double oldvol,
	unsigned int startEntry, const vector< unsigned int >& localIndices,
	const vector< double >& vols )
{
	// cout << "MeshEntry::triggerRemesh on " << e.element()->getName() << endl;
	remeshOut()->send( e, oldvol, parent_->getNumEntries(),
		startEntry, localIndices, vols );
	remeshReacsOut()->send( e );
}

