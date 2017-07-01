/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CYL_BASE_H
#define _CYL_BASE_H

/**
 * Base class for cylinder calculations.
 */

class CylBase
{
	public:
		CylBase( double x, double y, double z,
						double dia, double length, unsigned int numDivs );
		CylBase();

		void setX( double v );
		double getX() const;
		void setY( double v );
		double getY() const;
		void setZ( double v );
		double getZ() const;
		void setDia( double v );
		double getDia() const;
		void setLength( double v );
		double getLength() const;
		void setNumDivs( unsigned int v );
		unsigned int getNumDivs() const;
		void setIsCylinder( bool v );
		bool getIsCylinder() const;

		/// Returns vol of current node. Usually needs to refer to parent.
		double volume( const CylBase& parent ) const;

		/**
		 * Returns volume of specified voxel (MeshEntry)
		 */
		double voxelVolume( const CylBase& parent, unsigned int fid ) const;

		/**
		 * Returns a vector with the coordinates of the specified MeshEntry.
		 * The MeshEntry (entry argument) is a voxel within this CylBase,
		 * 'entry' is indexed from 0 to numDivs.
		 * The parent is the parent CylBase with its own set of coords.
		 * The function does interpolation from the ends of the Node.
		 * These are detailed coordinates representative of the shape of
		 * the MeshEntry. For cylinders it is  x1y1z1 x2y2z2 r0 r1 phi0 phi1
		 * For spheres it is xyzdia.
		 */
		vector< double > getCoordinates( const CylBase& parent,
					   	unsigned int entry ) const;

		/**
 		* Returns diffusion cross-section from specified index to next.
 		* For index 0, this is cross-section to parent.
 		* For index numDivs-1, it is the cross-section from the second-last
 		* to the last voxel in this CylBase.
 		* Thus there is no valid value for (index == numDivs - 1), it has
 		* to be computed external to the CylBase, typically by calling the
 		* getDiffusionArea for the child CylBase.
 		*/
		double getDiffusionArea(
			const CylBase& parent, unsigned int index ) const;

		/// Return cross-section area of middle of specified voxel.
		double getMiddleArea(
			const CylBase& parent, unsigned int index ) const;

		/// Return length of voxel. All are equal.
		double getVoxelLength() const;

		/**
		 * Utility commands for building interface to cube mesh
		 */
		// Selects a grid size for generating intersection with CubeMesh.
		double selectGridSize( double h, double dia1, double granularity )
				const;

		void matchCubeMeshEntries( const ChemCompt* other,
			const CylBase& parent,
			unsigned int startIndex,
			double granularity,
			vector< VoxelJunction >& ret,
		   bool useCylinderCurve, bool useCylinderCap ) const;

		double nearest( double x, double y, double z,
				const CylBase& parent,
				double& linePos, double& r ) const;

		double nearest( double x, double y, double z,
				const CylBase& parent,
				unsigned int& index ) const;

	private:
		/// end of the node. The start is given by parent coords.
		double x_;
		double y_;
		double z_;

		double dia_; /// Diameter of node end.
		double length_; /// Length of compartment.
		unsigned int numDivs_;	/// Number of subdivisions of cylinder.
		bool isCylinder_;	/// True if this acts like a regular cylinder.
};

#endif	// _CYL_BASE_H
