/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * DiffJunction wraps the data needed to calculate fluxes between two
 * Dsolves. The Dsolves may contact each other at many places, and each
 * contact may have many molecules that diffuse. For example, a NeuroMesh
 * touches a SpineMesh at each of the spine necks.
 *
 * The data includes all the pools that diffuse, which are identified
 * by their indices in the respective Dsolves.
 * It also includes the vector of VoxelJunctions, which specifies
 * matching voxel indices in the two compartments, and the diffusion scale
 * factor (based on geometry) for this junction.
 */
class DiffJunction
{
	public:
		unsigned int otherDsolve;
		vector< unsigned int > myPools;
		vector< unsigned int > otherPools;
		vector< VoxelJunction > vj;
};
