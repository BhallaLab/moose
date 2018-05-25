/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MESH_ENTRY_H
#define _MESH_ENTRY_H

/**
 * MeshType is a set of possible mesh geometries known at this time to
 * the system
 */
enum MeshType {
	BAD,
	CUBOID,
	CYL, CYL_SHELL, CYL_SHELL_SEG,
	SPHERE, SPHERE_SHELL, SPHERE_SHELL_SEG,
	TETRAHEDRON, DISK, ENDO
};

class ChemCompt;

/**
 * The MeshEntry is a single 'compartment' in the mathematical sense,
 * that is, all properties are assumed homogenous within it.
 * It is a FieldElement, so it gets all its values from the parent
 * ChemCompt.
 */
class MeshEntry
{
	public:
		MeshEntry();
		MeshEntry( const ChemCompt* parent );
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		// volume of this MeshEntry
		double getVolume( const Eref& e ) const;

		/**
		 * returns number of dimension
		 */
		unsigned int getDimensions( const Eref& e ) const;

		/**
		 * The MeshType defines the shape of the mesh entry.
		 * 0: Not assigned
		 * 1: cuboid
		 * 2: cylinder
		 * 3. cylindrical shell
		 * 4: cylindrical shell segment
		 * 5: sphere
		 * 6: spherical shell
		 * 7: spherical shell segment
		 * 8: Tetrahedral
		 * 9: EndoMesh
		 */
		unsigned int getMeshType( const Eref& e ) const;

		/**
		 * Coords that define current MeshEntry. Usually generated on
		 * the fly by passing the current Field Index to the parent
		 * ChemCompt subclass, which will figure it out.
		 */
		vector< double > getCoordinates( const Eref& e )
			const;
		/**
		 * Indices of other Entries that this one connects to, for diffusion
		 */
		vector< unsigned int > getNeighbors( const Eref& e )
			const;

		/**
		 * Diffusion scaling for area
		 */
		vector< double > getDiffusionArea( const Eref& e ) const;
		/**
		 * Diffusion scaling for geometry of interface
		 */
		vector< double > getDiffusionScaling( const Eref& e) const;

		/*
		/// Coords that define current MeshEntry. Usually generated on
		/// the fly.
		vector< double > coordinates() const;

		/// Indices of other Entries that this one connects to, for
		/// diffusion
		vector< unsigned int > connected() const;

		/// Diffusion scaling for area
		vector< double > diffusionArea() const;

		/// Diffusion conc term scaling for geometry of interface
		vector< double > diffusionScaling() const;
		*/

		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		void extent( DataId di, double volume, double area, double perimeter );

		void process( const Eref& e, ProcPtr info );
		void reinit( const Eref& e, ProcPtr info );
		//////////////////////////////////////////////////////////////////
		// Utility func
		//////////////////////////////////////////////////////////////////
		void triggerRemesh( const Eref& e,
			double oldvol,
			unsigned int startEntry,
			const vector< unsigned int >& localIndices,
			const vector< double >& vols );

		//////////////////////////////////////////////////////////////////
		// Lookup funcs for Boundary
		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();
	private:
		double volume_; /// Volume or area
		const ChemCompt* parent_;
};

#endif	// _MESH_ENTRY_H
