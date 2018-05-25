/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _ENDO_MESH_H
#define _ENDO_MESH_H

/**
 * The EndoMesh is a cellular compartment entirely contained within another
 * compartment. It supports diffusion only with its immediate surround,
 * of which there can only be one, and with its immediate interior, which 
 * can be one or more 
 * EndoMeshes. Each voxel in the EndoMesh is well mixed, and does not
 * exchange with any other voxels.
 * Typically used in modelling endosomes, ER, mitochondria, and other
 * organelles.
 */
class EndoMesh: public MeshCompt
{
	public:
		EndoMesh();
		~EndoMesh();
		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////
		void setRpower( const Eref& e, double v );
		double getRpower( const Eref& e ) const;
		void setRscale( const Eref& e, double v );
		double getRscale( const Eref& e ) const;
		void setApower( const Eref& e, double v );
		double getApower( const Eref& e ) const;
		void setAscale( const Eref& e, double v );
		double getAscale( const Eref& e ) const;
		void setVpower( const Eref& e, double v );
		double getVpower( const Eref& e ) const;
		void setVscale( const Eref& e, double v );
		double getVscale( const Eref& e ) const;
		void setSurround( const Eref& e, ObjId v );
		ObjId getSurround( const Eref& e ) const;
		void setDoAxialDiffusion( const Eref& e, bool v );
		bool getDoAxialDiffusion( const Eref& e ) const;

		//////////////////////////////////////////////////////////////////
		// FieldElement assignment stuff for MeshEntries
		//////////////////////////////////////////////////////////////////

		/// Virtual function to return MeshType of specified entry.
		unsigned int getMeshType( unsigned int fid ) const;
		/// Virtual function to return dimensions of specified entry.
		unsigned int getMeshDimensions( unsigned int fid ) const;
		unsigned int innerGetDimensions() const;
		/// Virtual function to return volume of mesh Entry.
		double getMeshEntryVolume( unsigned int fid ) const;
		/// Virtual function to return coords of mesh Entry.
		vector< double > getCoordinates( unsigned int fid ) const;
		/// Virtual function to return diffusion X-section area
		vector< double > getDiffusionArea( unsigned int fid ) const;
		/// Virtual function to return scale factor for diffusion. 1 here.
		vector< double > getDiffusionScaling( unsigned int fid ) const;
		/// Volume of mesh Entry including abutting diff-coupled voxels
		double extendedMeshEntryVolume( unsigned int fid ) const;

		//////////////////////////////////////////////////////////////////

		/**
		 * Inherited virtual func. Returns number of MeshEntry in array
		 */
		unsigned int innerGetNumEntries() const;
		/// Inherited virtual func.
		void innerSetNumEntries( unsigned int n );

		/// Inherited virtual, do nothing for now.
		vector< unsigned int > getParentVoxel() const;
		const vector< double >& vGetVoxelVolume() const;
		const vector< double >& vGetVoxelMidpoint() const;
		const vector< double >& getVoxelArea() const;
		const vector< double >& getVoxelLength() const;

		/// Inherited virtual. Returns entire volume of compartment.
		double vGetEntireVolume() const;

		/// Inherited virtual. Resizes len and dia of each voxel.
		bool vSetVolumeNotRates( double volume );
		//////////////////////////////////////////////////////////////////
		// Dest funcs
		//////////////////////////////////////////////////////////////////

		/// Virtual func to make a mesh with specified Volume and numEntries
		void innerBuildDefaultMesh( const Eref& e,
			double volume, unsigned int numEntries );

		void innerHandleRequestMeshStats(
			const Eref& e,
			const SrcFinfo2< unsigned int, vector< double > >*
				meshStatsFinfo
		);

		void innerHandleNodeInfo(
			const Eref& e,
			unsigned int numNodes, unsigned int numThreads );

		//////////////////////////////////////////////////////////////////
		// inherited virtual funcs for Boundary
		//////////////////////////////////////////////////////////////////

		void matchMeshEntries( const ChemCompt* other,
			vector< VoxelJunction > & ret ) const;

		double nearest( double x, double y, double z,
						unsigned int& index ) const;

		void indexToSpace( unsigned int index,
						double& x, double& y, double& z ) const;

		//////////////////////////////////////////////////////////////////

		static const Cinfo* initCinfo();

	private:
		/** 
		 * Surrounding mesh. Use this for calculating
		 * all volume and diffusion terms, don't maintain any local
		 * variables.
		 */
		ObjId surround_;	
		const MeshCompt* parent_;

		Id insideMeshes_;	/// EndoMeshes inside. Used to update.

		/**
		 * The reason why the powers don't have to be 1/3, 1/2 and 1 is
		 * because some organelles are not a simple linear vol relationship
		 * with their surround. I want to keep flexibility.
		 */
		double rPower_; 	/// rEndo = rScale * pow( surroundVol, rPower_)
		double rScale_; 	/// rEndo = rScale * pow( surroundVol, rPower_)
		double aPower_; 	/// aEndo = aScale * pow( surroundVol, aPower_)
		double aScale_; 	/// aEndo = aScale * pow( surroundVol, aPower_)
		double vPower_; 	/// vEndo = vScale * pow( surroundVol, vPower_)
		double vScale_; 	/// vEndo = vScale * pow( surroundVol, vPower_)

		bool doAxialDiffusion_;	/// Flag for axial diffusion
};

#endif	// _ENDO_MESH_H
