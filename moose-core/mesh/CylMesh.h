/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _CYL_MESH_H
#define _CYL_MESH_H

/**
 * The CylMesh represents a chemically identified compartment shaped
 * like an extended cylinder. This is psuedo-1 dimension: Only the
 * axial dimension is considered for diffusion and subdivisions.
 * Typically used in modelling small segments of dendrite
 */
class CylMesh: public MeshCompt
{
public:
    CylMesh();
    ~CylMesh();
    //////////////////////////////////////////////////////////////////
    //  Utility func
    //////////////////////////////////////////////////////////////////
    /**
     * Recomputes all local coordinate and meshing data following
     * a change in any of the coord parameters
     */

    void updateCoords( const Eref& e, const std::vector< double >& childConcs );

    //////////////////////////////////////////////////////////////////
    // Field assignment stuff
    //////////////////////////////////////////////////////////////////

    void setX0( const Eref& e, double v );
    double getX0( const Eref& e ) const;
    void setY0( const Eref& e, double v );
    double getY0( const Eref& e ) const;
    void setZ0( const Eref& e, double v );
    double getZ0( const Eref& e ) const;
    void setR0( const Eref& e, double v );
    double getR0( const Eref& e ) const;

    void setX1( const Eref& e, double v );
    double getX1( const Eref& e ) const;
    void setY1( const Eref& e, double v );
    double getY1( const Eref& e ) const;
    void setZ1( const Eref& e, double v );
    double getZ1( const Eref& e ) const;
    void setR1( const Eref& e, double v );
    double getR1( const Eref& e ) const;

    void innerSetCoords( const Eref& e, const std::vector< double >& v);
    void setCoords( const Eref& e, std::vector< double > v );
    std::vector< double > getCoords( const Eref& e ) const;

    void setDiffLength( const Eref& e, double v );
    double getDiffLength( const Eref& e ) const;

    double getTotLength() const;

    unsigned int innerGetDimensions() const;

    //////////////////////////////////////////////////////////////////
    // FieldElement assignment stuff for MeshEntries
    //////////////////////////////////////////////////////////////////

    /// Virtual function to return MeshType of specified entry.
    unsigned int getMeshType( unsigned int fid ) const;
    /// Virtual function to return dimensions of specified entry.
    unsigned int getMeshDimensions( unsigned int fid ) const;
    /// Virtual function to return volume of mesh Entry.
    double getMeshEntryVolume( unsigned int fid ) const;
    /// Virtual function to return coords of mesh Entry.
    std::vector< double > getCoordinates( unsigned int fid ) const;
    /// Virtual function to return diffusion X-section area
    std::vector< double > getDiffusionArea( unsigned int fid ) const;
    /// Virtual function to return scale factor for diffusion. 1 here.
    std::vector< double > getDiffusionScaling( unsigned int fid ) const;
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
    std::vector< unsigned int > getParentVoxel() const;
    const std::vector< double >& vGetVoxelVolume() const;
    const std::vector< double >& vGetVoxelMidpoint() const;
    const std::vector< double >& getVoxelArea() const;
    const std::vector< double >& getVoxelLength() const;

    /// Inherited virtual. Returns entire volume of compartment.
    double vGetEntireVolume() const;

    /// Inherited virtual. Resizes len and dia of each voxel.
    bool vSetVolumeNotRates( double volume );

    //////////////////////////////////////////////////////////////////
    // Dest funcs
    //////////////////////////////////////////////////////////////////

    /// Virtual func to make a mesh with specified Volume and numEntries
    void innerBuildDefaultMesh( const Eref& e,
            double volume, unsigned int numEntries
            );

    void innerHandleRequestMeshStats(
        const Eref& e,
        const SrcFinfo2< unsigned int, std::vector< double > >*
        meshStatsFinfo
    );

    void innerHandleNodeInfo(
        const Eref& e,
        unsigned int numNodes, unsigned int numThreads );

    void transmitChange( const Eref& e );

    void buildStencil();

    //////////////////////////////////////////////////////////////////
    // inherited virtual funcs for Boundary
    //////////////////////////////////////////////////////////////////

    void matchMeshEntries( const ChemCompt* other,
                           std::vector< VoxelJunction > & ret ) const;

    double nearest( double x, double y, double z,
                    unsigned int& index ) const;

    double nearest( double x, double y, double z,
                    double& linePos, double& r ) const;

    void indexToSpace( unsigned int index,
                       double& x, double& y, double& z ) const;

    //////////////////////////////////////////////////////////////////
    // Inner specific functions needed by matchMeshEntries.
    //////////////////////////////////////////////////////////////////
    void matchCylMeshEntries( const CylMesh* other,
                              std::vector< VoxelJunction >& ret ) const;
    void matchCubeMeshEntries( const CubeMesh* other,
                               std::vector< VoxelJunction >& ret ) const;
    void matchNeuroMeshEntries( const NeuroMesh* other,
                                std::vector< VoxelJunction >& ret ) const;

    // Selects a grid volume for generating intersection with CubeMesh.
    double selectGridVolume( double h ) const;

    //////////////////////////////////////////////////////////////////

    static const Cinfo* initCinfo();

private:
    unsigned int numEntries_; // Number of subdivisions to use
    bool useCaps_; // Flag: Should the ends have hemispherical caps?
    bool isToroid_; // Flag: Should the ends loop around mathemagically?

    double x0_; /// coords
    double y0_; /// coords
    double z0_; /// coords

    double x1_; /// coords
    double y1_; /// coords
    double z1_; /// coords

    double r0_;	/// Radius at one end
    double r1_; /// Radius at other end

    double diffLength_;	/// Length constant for diffusion. Equal to dx.

    /**
     * surfaceGranularity_ decides how finely to subdivide DiffLength
     * or cubic mesh side, when computing surface area of intersections
     * between them when diffusing. Defaults to 0.1
     */
    double surfaceGranularity_;

    double totLen_;	/// Utility value: Total length of cylinder
    double rSlope_;	/// Utility value: dr/dx
    double lenSlope_; /// Utility value: dlen/dx
};

#endif	// _CYL_MESH_H
