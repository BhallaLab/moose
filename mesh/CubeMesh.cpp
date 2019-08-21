/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "../basecode/header.h"
#include "../basecode/SparseMatrix.h"
#include "../basecode/ElementValueFinfo.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "VoxelJunction.h"
// #include "Stencil.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "EndoMesh.h"

const unsigned int CubeMesh::EMPTY = ~0;
const unsigned int CubeMesh::SURFACE = ~1;
const unsigned int CubeMesh::ABUTX = ~2;
const unsigned int CubeMesh::ABUTY = ~3;
const unsigned int CubeMesh::ABUTZ = ~4;
const unsigned int CubeMesh::MULTI = ~5;

typedef pair< unsigned int, unsigned int > PII;

const Cinfo* CubeMesh::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ValueFinfo< CubeMesh, double > x0(
        "x0",
        "X coord of one end",
        &CubeMesh::setX0,
        &CubeMesh::getX0
    );
    static ValueFinfo< CubeMesh, double > y0(
        "y0",
        "Y coord of one end",
        &CubeMesh::setY0,
        &CubeMesh::getY0
    );
    static ValueFinfo< CubeMesh, double > z0(
        "z0",
        "Z coord of one end",
        &CubeMesh::setZ0,
        &CubeMesh::getZ0
    );
    static ValueFinfo< CubeMesh, double > x1(
        "x1",
        "X coord of other end",
        &CubeMesh::setX1,
        &CubeMesh::getX1
    );
    static ValueFinfo< CubeMesh, double > y1(
        "y1",
        "Y coord of other end",
        &CubeMesh::setY1,
        &CubeMesh::getY1
    );
    static ValueFinfo< CubeMesh, double > z1(
        "z1",
        "Z coord of other end",
        &CubeMesh::setZ1,
        &CubeMesh::getZ1
    );

    static ValueFinfo< CubeMesh, double > dx(
        "dx",
        "X size for mesh",
        &CubeMesh::setDx,
        &CubeMesh::getDx
    );
    static ValueFinfo< CubeMesh, double > dy(
        "dy",
        "Y size for mesh",
        &CubeMesh::setDy,
        &CubeMesh::getDy
    );
    static ValueFinfo< CubeMesh, double > dz(
        "dz",
        "Z size for mesh",
        &CubeMesh::setDz,
        &CubeMesh::getDz
    );

    static ValueFinfo< CubeMesh, unsigned int > nx(
        "nx",
        "Number of subdivisions in mesh in X",
        &CubeMesh::setNx,
        &CubeMesh::getNx
    );
    static ValueFinfo< CubeMesh, unsigned int > ny(
        "ny",
        "Number of subdivisions in mesh in Y",
        &CubeMesh::setNy,
        &CubeMesh::getNy
    );
    static ValueFinfo< CubeMesh, unsigned int > nz(
        "nz",
        "Number of subdivisions in mesh in Z",
        &CubeMesh::setNz,
        &CubeMesh::getNz
    );

    static ValueFinfo< CubeMesh, bool > isToroid(
        "isToroid",
        "Flag. True when the mesh should be toroidal, that is,"
        "when going beyond the right face brings us around to the"
        "left-most mesh entry, and so on. If we have nx, ny, nz"
        "entries, this rule means that the coordinate (x, ny, z)"
        "will map onto (x, 0, z). Similarly,"
        "(-1, y, z) -> (nx-1, y, z)"
        "Default is false",
        &CubeMesh::setIsToroid,
        &CubeMesh::getIsToroid
    );

    static ValueFinfo< CubeMesh, bool > preserveNumEntries(
        "preserveNumEntries",
        "Flag. When it is true, the numbers nx, ny, nz remain"
        "unchanged when x0, x1, y0, y1, z0, z1 are altered. Thus"
        "dx, dy, dz would change instead. When it is false, then"
        "dx, dy, dz remain the same and nx, ny, nz are altered."
        "Default is true",
        &CubeMesh::setPreserveNumEntries,
        &CubeMesh::getPreserveNumEntries
    );

    static ValueFinfo< CubeMesh, bool > alwaysDiffuse(
        "alwaysDiffuse",
        "Flag. When it is true, the mesh matches up sequential "
        "mesh entries for diffusion and chmestry. This is regardless "
        "of spatial location, and is guaranteed to set up at least "
        "the home reaction system"
        "Default is false",
        &CubeMesh::setAlwaysDiffuse,
        &CubeMesh::getAlwaysDiffuse
    );

    static ElementValueFinfo< CubeMesh, vector< double > > coords(
        "coords",
        "Set all the coords of the cuboid at once. Order is:"
        "x0 y0 z0   x1 y1 z1   dx dy dz"
        "When this is done, it recalculates the numEntries since "
        "dx, dy and dz are given explicitly."
        "As a special hack, you can leave out dx, dy and dz and use "
        "a vector of size 6. In this case the operation assumes that "
        "nx, ny and nz are to be preserved and dx, dy and dz will "
        "be recalculated. ",
        &CubeMesh::setCoords,
        &CubeMesh::getCoords
    );

    static ValueFinfo< CubeMesh, vector< unsigned int > > meshToSpace(
        "meshToSpace",
        "Array in which each mesh entry stores spatial (cubic) index",
        &CubeMesh::setMeshToSpace,
        &CubeMesh::getMeshToSpace
    );

    static ValueFinfo< CubeMesh, vector< unsigned int > > spaceToMesh(
        "spaceToMesh",
        "Array in which each space index (obtained by linearizing "
        "the xyz coords) specifies which meshIndex is present."
        "In many cases the index will store the EMPTY flag if there is"
        "no mesh entry at that spatial location",
        &CubeMesh::setSpaceToMesh,
        &CubeMesh::getSpaceToMesh
    );

    static ValueFinfo< CubeMesh, vector< unsigned int > > surface(
        "surface",
        "Array specifying surface of arbitrary volume within the "
        "CubeMesh. All entries must fall within the cuboid. "
        "Each entry of the array is a spatial index obtained by "
        "linearizing the ix, iy, iz coordinates within the cuboid. "
        "So, each entry == ( iz * ny + iy ) * nx + ix"
        "Note that the voxels listed on the surface are WITHIN the "
        "volume of the CubeMesh object",
        &CubeMesh::setSurface,
        &CubeMesh::getSurface
    );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////

    static DestFinfo buildMesh( "buildMesh",
                                "Build cubical mesh for geom surface specified by Id, using"
                                "specified x y z coords as an inside point in mesh",
                                new OpFunc4< CubeMesh, Id, double, double, double >(
                                    &CubeMesh::buildMesh )
                              );

    //////////////////////////////////////////////////////////////
    // Field Elements
    //////////////////////////////////////////////////////////////

    static Finfo* cubeMeshFinfos[] =
    {
        &isToroid,		// Value
        &preserveNumEntries,		// Value
        &alwaysDiffuse,		// Value
        &x0,			// Value
        &y0,			// Value
        &z0,			// Value
        &x1,			// Value
        &y1,			// Value
        &z1,			// Value
        &dx,			// Value
        &dy,			// Value
        &dz,			// Value
        &nx,			// Value
        &ny,			// Value
        &nz,			// Value
        &coords,		// Value
        &meshToSpace,	// Value
        &spaceToMesh,	// Value
        &surface,		// Value
    };

    static string doc[] =
    {
        "Name", "CubeMesh",
        "Author", "Upi Bhalla",
        "Description", "Chemical compartment with cuboid grid. "
        "Defaults to a cube of size 10 microns, with mesh size "
        "also 10 microns, so that there is just 1 cubic voxel. "
        "These defaults are similar to that of a typical cell. "
        "Can be configured to have different x,y,z dimensions and "
        "also different dx,dy,dz voxel sizes. ",
    };
    static Dinfo< CubeMesh > dinfo;
    static Cinfo cubeMeshCinfo (
        "CubeMesh",
        ChemCompt::initCinfo(),
        cubeMeshFinfos,
        sizeof( cubeMeshFinfos ) / sizeof ( Finfo* ),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string)
    );

    return &cubeMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* cubeMeshCinfo = CubeMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
CubeMesh::CubeMesh()
    :
    isToroid_( 0 ),
    preserveNumEntries_( 1 ),
    alwaysDiffuse_( false ),
    x0_( 0.0 ),
    y0_( 0.0 ),
    z0_( 0.0 ),
    x1_( 10e-6 ),
    y1_( 10e-6 ),
    z1_( 10e-6 ),
    dx_( 10e-6 ),
    dy_( 10e-6 ),
    dz_( 10e-6 ),
    nx_( 1 ),
    ny_( 1 ),
    nz_( 1 ),
    m2s_( 1, 0 ),
    s2m_( 1, 0 )
{
    updateCoords();
}

CubeMesh::~CubeMesh()
{
    ;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////

// Swaps x0 and x1 if x0 > x1
void swapIfBackward( double& x0, double& x1 )
{
    if ( x0 > x1 )
    {
        double temp = x0;
        x0 = x1;
        x1 = temp;
    }
}

void CubeMesh::fillTwoDimSurface()
{
    unsigned int size = nx_ * ny_ * nz_;
    if ( nx_ == 1 )
    {
        for ( unsigned int j = 0; j < ny_; ++j )
            surface_.push_back( j  );
        for ( unsigned int j = size - ny_; j < size; ++j )
            surface_.push_back( j  );
        for ( unsigned int i = 1; i < nz_ - 1; ++i )
            surface_.push_back( i * ny_  );
        for ( unsigned int i = 1; i < nz_ - 1; ++i )
            surface_.push_back( ny_ - 1 + i * ny_  );
    }
    else if ( ny_ == 1 )
    {
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( k  );
        for ( unsigned int k = size - nx_; k < size; ++k )
            surface_.push_back( k  );
        for ( unsigned int i = 1; i < nz_ - 1; ++i )
            surface_.push_back( i * nx_  );
        for ( unsigned int i = 1; i < nz_ - 1; ++i )
            surface_.push_back( nx_ - 1 + i * nx_  );
    }
    else if ( nz_ == 1 )
    {
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( k  );
        for ( unsigned int k = size - nx_; k < size; ++k )
            surface_.push_back( k  );
        for ( unsigned int j = 1; j < ny_ - 1; ++j )
            surface_.push_back( j * nx_  );
        for ( unsigned int j = 1; j < ny_ - 1; ++j )
            surface_.push_back( j * nx_ + nx_ - 1  );
    }
    // Ah, C++ STL. Look on my works, ye mighty, and despair.
    sort( surface_.begin(), surface_.end() );
    surface_.erase( unique( surface_.begin(), surface_.end() ),
                    surface_.end() );
}

void CubeMesh::fillThreeDimSurface() // Need to fix duplicate points.
{
    unsigned int size = nx_ * ny_ * nz_;
    // z == 0 plane
    for ( unsigned int j = 0; j < ny_; ++j )
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( j * nx_ + k );
    // z == nz_-1 plane
    unsigned int offset = size - nx_ * ny_;
    for ( unsigned int j = 0; j < ny_; ++j )
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( offset + j * nx_ + k );

    // y == 0 plane
    for ( unsigned int i = 0; i < nz_; ++i )
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( i * nx_ * ny_ + k );
    // y == ny_-1 plane
    offset = nx_ * ( ny_ - 1 );
    for ( unsigned int i = 0; i < nz_; ++i )
        for ( unsigned int k = 0; k < nx_; ++k )
            surface_.push_back( offset + i * nx_ * ny_ + k );

    // x == 0 plane
    for ( unsigned int i = 0; i < nz_; ++i )
        for ( unsigned int j = 0; j < ny_; ++j )
            surface_.push_back( ( i * ny_ + j ) * nx_ );
    // x == nx_-1 plane
    offset = nx_ - 1;
    for ( unsigned int i = 0; i < nz_; ++i )
        for ( unsigned int j = 0; j < ny_; ++j )
            surface_.push_back( offset + ( i * ny_ + j ) * nx_ );

    sort( surface_.begin(), surface_.end() );
    surface_.erase( unique( surface_.begin(), surface_.end() ),
                    surface_.end() );
}

/**
 * This assumes that dx, dy, dz are the quantities to preserve, over
 * numEntries.
 * So when the compartment changes volume, so does numEntries. dx, dy, dz
 * do not change, some of the sub-cuboids will partially be outside.
 */
void CubeMesh::updateCoords()
{
    swapIfBackward( x0_, x1_ );
    swapIfBackward( y0_, y1_ );
    swapIfBackward( z0_, z1_ );
    if ( preserveNumEntries_ )
    {
        dx_ = ( x1_ - x0_ ) / nx_;
        dy_ = ( y1_ - y0_ ) / ny_;
        dz_ = ( z1_ - z0_ ) / nz_;
    }
    else
    {
        nx_ = round( (x1_ - x0_) / dx_ );
        ny_ = round( (y1_ - y0_) / dy_ );
        nz_ = round( (z1_ - z0_) / dz_ );

        if ( nx_ == 0 ) nx_ = 1;
        if ( ny_ == 0 ) ny_ = 1;
        if ( nz_ == 0 ) nz_ = 1;
    }

    /// Temporarily fill out the whole cube for m2s and s2m. These
    // will change for none-cube geometries.
    unsigned int size = nx_ * ny_ * nz_;
    m2s_.resize( size );
    s2m_.resize( size );
    for ( unsigned int i = 0; i < size; ++i )
    {
        m2s_[i] = s2m_[i] = i;
    }

    // Fill out surface vector
    surface_.resize( 0 );
    /*
    if ( numDims() == 0 ) {
    	surface_.push_back( 0 );
    } else if ( numDims() == 1 ) {
    	surface_.push_back( 0 );
    	if ( size > 1 )
    		surface_.push_back( size - 1 );
    } else if ( numDims() == 2 ) {
    	fillTwoDimSurface();
    } else if ( numDims() == 3 ) {
    	fillThreeDimSurface();
    }
    */
    fillThreeDimSurface();

    // volume_ = ( x1_ - x0_ ) * ( y1_ - y0_ ) * ( z1_ - z0_ );
    assert( size >= 0 );

    buildStencil();
}

void CubeMesh::setX0( double v )
{
    x0_ = v;
    updateCoords();
}

double CubeMesh::getX0() const
{
    return x0_;
}

void CubeMesh::setY0( double v )
{
    y0_ = v;
    updateCoords();
}

double CubeMesh::getY0() const
{
    return y0_;
}

void CubeMesh::setZ0( double v )
{
    z0_ = v;
    updateCoords();
}

double CubeMesh::getZ0() const
{
    return z0_;
}

void CubeMesh::setX1( double v )
{
    x1_ = v;
    updateCoords();
}

double CubeMesh::getX1() const
{
    return x1_;
}

void CubeMesh::setY1( double v )
{
    y1_ = v;
    updateCoords();
}

double CubeMesh::getY1() const
{
    return y1_;
}

void CubeMesh::setZ1( double v )
{
    z1_ = v;
    updateCoords();
}

double CubeMesh::getZ1() const
{
    return z1_;
}

void CubeMesh::setDx( double v )
{
    dx_ = v;
    updateCoords();
}

double CubeMesh::getDx() const
{
    return dx_;
}


void CubeMesh::setDy( double v )
{
    dy_ = v;
    updateCoords();
}

double CubeMesh::getDy() const
{
    return dy_;
}


void CubeMesh::setDz( double v )
{
    dz_ = v;
    updateCoords();
}

double CubeMesh::getDz() const
{
    return dz_;
}

void CubeMesh::setNx( unsigned int v )
{
    nx_ = v;
    updateCoords();
}

unsigned int CubeMesh::getNx() const
{
    return nx_;
}


void CubeMesh::setNy( unsigned int v )
{
    ny_ = v;
    updateCoords();
}

unsigned int CubeMesh::getNy() const
{
    return ny_;
}

void CubeMesh::setNz( unsigned int v )
{
    nz_ = v;
    updateCoords();
}

unsigned int CubeMesh::getNz() const
{
    return nz_;
}


void CubeMesh::setIsToroid( bool v )
{
    isToroid_ = v;
}

bool CubeMesh::getIsToroid() const
{
    return isToroid_;
}

void CubeMesh::setPreserveNumEntries( bool v )
{
    preserveNumEntries_ = v;
}

bool CubeMesh::getPreserveNumEntries() const
{
    return preserveNumEntries_;
}

void CubeMesh::setAlwaysDiffuse( bool v )
{
    alwaysDiffuse_ = v;
}

bool CubeMesh::getAlwaysDiffuse() const
{
    // alwaysDiffuse is normally false.
    return alwaysDiffuse_;
}

void CubeMesh::innerSetCoords( const vector< double >& v)
{
    if ( v.size() < 6 )
        return;

    x0_ = v[0];
    y0_ = v[1];
    z0_ = v[2];

    x1_ = v[3];
    y1_ = v[4];
    z1_ = v[5];

    bool temp = preserveNumEntries_;
    if ( v.size() >= 9 )
    {
        dx_ = v[6];
        dy_ = v[7];
        dz_ = v[8];
        preserveNumEntries_ = 0;
    }
    else
    {
        preserveNumEntries_ = 1;
    }
    updateCoords();
    preserveNumEntries_ = temp;

}

void CubeMesh::setCoords( const Eref& e, vector< double > v)
{
    // double oldVol = getMeshEntryVolume( 0 );
    innerSetCoords( v );
    ChemCompt::voxelVolOut()->send( e, vGetVoxelVolume() );
}

vector< double > CubeMesh::getCoords( const Eref& e ) const
{
    vector< double > ret( 9 );

    ret[0] = x0_;
    ret[1] = y0_;
    ret[2] = z0_;

    ret[3] = x1_;
    ret[4] = y1_;
    ret[5] = z1_;

    ret[6] = dx_;
    ret[7] = dy_;
    ret[8] = dz_;

    return ret;
}

void CubeMesh::setMeshToSpace( vector< unsigned int > v )
{
    m2s_ = v;
    deriveS2mFromM2s();
}

vector< unsigned int > CubeMesh::getMeshToSpace() const
{
    return m2s_;
}

void CubeMesh::setSpaceToMesh( vector< unsigned int > v )
{
    s2m_ = v;
    deriveM2sFromS2m();
}

vector< unsigned int > CubeMesh::getSpaceToMesh() const
{
    return s2m_;
}

void CubeMesh::setSurface( vector< unsigned int > v )
{
    surface_ = v;
}

vector< unsigned int > CubeMesh::getSurface() const
{
    return surface_;
}

unsigned int CubeMesh::innerGetDimensions() const
{
    return 3;
}


//////////////////////////////////////////////////////////////////
// DestFinfos
//////////////////////////////////////////////////////////////////

void CubeMesh::buildMesh( Id geom, double x, double y, double z )
{
    ;
}

/**
 * Builds something as close to a cube as can get. This needs a
 * smarter boundary handling code than I have here. For now, goes for
 * the nearest cube
 */
void CubeMesh::innerBuildDefaultMesh( const Eref& e,
                                      double volume, unsigned int numEntries )
{
    double approxN = numEntries;
    approxN = pow( approxN, 1.0 / 3.0 );
    unsigned int smaller = floor( approxN );
    unsigned int bigger = ceil( approxN );
    unsigned int numSide;
    if ( smaller != bigger )
    {
        numSide = smaller;
    }
    else
    {
        unsigned int smallerVol = smaller * smaller * smaller;
        unsigned int biggerVol = bigger * bigger * bigger;
        if ( numEntries - smallerVol < biggerVol - numEntries )
            numSide = smaller;
        else
            numSide = bigger;
    }
    double side = pow( volume, 1.0 / 3.0 );
    vector< double > coords( 9, side );
    coords[0] = coords[1] = coords[2] = 0;
    coords[6] = coords[7] = coords[8] = side / numSide;
    nx_ = ny_ = nz_ = numSide;
    setCoords( e, coords );
}

/// More inherited virtual funcs: request comes in for mesh stats
void CubeMesh::innerHandleRequestMeshStats( const Eref& e,
        const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo )
{
    vector< double > meshVolumes( 1, dx_ * dy_ * dz_ );
    meshStatsFinfo->send( e, nx_ * ny_ * nz_, meshVolumes);
}

/// Generate node decomposition of mesh, send it out along
/// meshSplitFinfo msg
void CubeMesh::innerHandleNodeInfo(
    const Eref& e,
    unsigned int numNodes, unsigned int numThreads )
{
    /*
    unsigned int numEntries = nx_ * ny_ * nz_ ;
    vector< double > vols( numEntries, dx_ * dy_ * dz_ );
    vector< unsigned int > localEntries( numEntries );
    vector< vector< unsigned int > > outgoingEntries;
    vector< vector< unsigned int > > incomingEntries;
    double oldvol = getMeshEntryVolume( 0 );
    meshSplit()->send( e,
    	oldvol,
    	vols, localEntries,
    	outgoingEntries, incomingEntries );
    	*/
}

double CubeMesh::vGetEntireVolume() const
{
    return fabs( (x1_ - x0_) * (y1_ - y0_) * (z1_ - z0_) );
}


//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/// Virtual function to return MeshType of specified entry.
unsigned int CubeMesh::getMeshType( unsigned int fid ) const
{
    return CUBOID;
}

/// Virtual function to return dimensions of specified entry.
unsigned int CubeMesh::getMeshDimensions( unsigned int fid ) const
{
    return 3;
}

/// Virtual function to return volume of mesh Entry.
double CubeMesh::getMeshEntryVolume( unsigned int fid ) const
{
    return dx_ * dy_ * dz_;
}

/// Virtual function to return volume of mesh Entry, including
// for diffusively coupled voxels from other solvers.
double CubeMesh::extendedMeshEntryVolume( unsigned int fid ) const
{
    if ( fid >= m2s_.size() )
    {
        return MeshCompt::extendedMeshEntryVolume( fid - m2s_.size() );
    }
    return dx_ * dy_ * dz_;
}

/// Virtual function to return coords of mesh Entry.
/// For Cuboid mesh, coords are x1y1z1 x2y2z2
vector< double > CubeMesh::getCoordinates( unsigned int fid ) const
{
    assert( fid < m2s_.size() );
    unsigned int spaceIndex = m2s_[fid];

    unsigned int ix = spaceIndex % nx_;
    unsigned int iy = (spaceIndex / nx_) % ny_;
    unsigned int iz = (spaceIndex / ( nx_ * ny_ )) % nz_;

    vector< double > ret( 6 );
    ret[0] = x0_ + ix * dx_;
    ret[1] = y0_ + iy * dy_;
    ret[2] = z0_ + iz * dz_;

    ret[3] = x0_ + ix * dx_ + dx_;
    ret[4] = y0_ + iy * dy_ + dx_;
    ret[5] = z0_ + iz * dz_ + dx_;

    return ret;
}

unsigned int CubeMesh::neighbor( unsigned int spaceIndex,
                                 int dx, int dy, int dz ) const
{
    int ix = spaceIndex % nx_;
    int iy = (spaceIndex / nx_) % ny_;
    int iz = (spaceIndex / ( nx_ * ny_ )) % nz_;

    ix += dx;
    iy += dy;
    iz += dz;

    if ( ix < 0 || ix >= static_cast< int >( nx_ ) )
        return EMPTY;
    if ( iy < 0 || iy >= static_cast< int >( ny_ ) )
        return EMPTY;
    if ( iz < 0 || iz >= static_cast< int >( nz_ ) )
        return EMPTY;

    unsigned int nIndex = ( ( iz * ny_ ) + iy ) * nx_ + ix;

    return s2m_[nIndex];
}

/// Virtual function to return diffusion X-section area for each neighbor
vector< double > CubeMesh::getDiffusionArea( unsigned int fid ) const
{
    assert( fid < m2s_.size() );

    vector< double > ret;
    unsigned int spaceIndex = m2s_[fid];

    unsigned int nIndex = neighbor( spaceIndex, 0, 0, 1 );
    if ( nIndex != EMPTY )
        ret.push_back( dx_ * dy_ );

    nIndex = neighbor( spaceIndex, 0, 0, -1 );
    if ( nIndex != EMPTY )
        ret.push_back( dx_ * dy_ );

    nIndex = neighbor( spaceIndex, 0, 1, 0 );
    if ( nIndex != EMPTY )
        ret.push_back( dz_ * dx_ );

    nIndex = neighbor( spaceIndex, 0, -1, 0 );
    if ( nIndex != EMPTY )
        ret.push_back( dz_ * dx_ );

    nIndex = neighbor( spaceIndex, 1, 0, 0 );
    if ( nIndex != EMPTY )
        ret.push_back( dy_ * dz_ );

    nIndex = neighbor( spaceIndex, -1, 0, 0 );
    if ( nIndex != EMPTY )
        ret.push_back( dy_ * dz_ );

    return ret;
}

/// Virtual function to return scale factor for diffusion. 1 here.
vector< double > CubeMesh::getDiffusionScaling( unsigned int fid ) const
{
    return vector< double >( 6, 1.0 );
}

//////////////////////////////////////////////////////////////////

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int CubeMesh::innerGetNumEntries() const
{
    return m2s_.size();
}

/**
 * Inherited virtual func. Assigns number of MeshEntries.
 */
void CubeMesh::innerSetNumEntries( unsigned int n )
{
    cout << "Warning: CubeMesh::innerSetNumEntries is readonly.\n";
}

// We assume this is linear diffusion for now. Fails for 2 or 3-D diffusion
vector< unsigned int > CubeMesh::getParentVoxel() const
{
    unsigned int numEntries = innerGetNumEntries();
    vector< unsigned int > ret( numEntries );
    if ( numEntries > 0 )
        ret[0] = static_cast< unsigned int >( -1 );
    for (unsigned int i = 1; i < numEntries; ++i )
        ret[i] = i-1;

    return ret;
}

const vector< double >& CubeMesh::vGetVoxelVolume() const
{
    static vector< double > vol;
    vol.clear();
    vol.resize( nx_ * ny_ * nz_, dx_ * dy_ * dz_ );
    return vol;
}

const vector< double >& CubeMesh::vGetVoxelMidpoint() const
{
    static vector< double > midpoint;
    midpoint.resize( m2s_.size() * 3 );
    for ( unsigned int i = 0; i < m2s_.size(); ++i ) // x coords. Lowest.
        midpoint[i] = x0_ + ( 0.5 + (m2s_[i] % nx_ ) ) * dx_;
    for ( unsigned int i = 0; i < m2s_.size(); ++i )   // y coords. Middle.
    {
        unsigned int k = i + m2s_.size();
        midpoint[k] = y0_ + ( 0.5 + ( (m2s_[i] / nx_) % ny_ ) ) * dy_;
    }
    for ( unsigned int i = 0; i < m2s_.size(); ++i )   // z coords. Top.
    {
        unsigned int k = i + m2s_.size() * 2;
        midpoint[k] = z0_ + ( 0.5 + ( m2s_[i] / ( nx_ * ny_ ) ) ) * dz_;
    }
    return midpoint;
}

const vector< double >& CubeMesh::getVoxelArea() const
{
    static vector< double > area;
    if ( nx_ * ny_ == 1 )
        area.resize( nz_, dx_ * dy_ );
    else if ( nx_ * nz_ == 1 )
        area.resize( ny_, dx_ * dz_ );
    else if ( ny_ * nz_ == 1 )
        area.resize( nx_, dy_ * dz_ );
    else
        area.resize( nx_, dy_ * dz_ ); // Just put in a number.
    assert( area.size() == nx_ * ny_ * nz_ );
    return area;
}

const vector< double >& CubeMesh::getVoxelLength() const
{
    static vector< double > length;
    if ( dx_ > dy_ && dx_ > dz_ )
        length.assign( innerGetNumEntries(), dx_ );
    else if ( dy_ > dz_ )
        length.assign( innerGetNumEntries(), dy_ );
    else
        length.assign( innerGetNumEntries(), dz_ );
    return length;
}

bool CubeMesh::vSetVolumeNotRates( double vol )
{
    // Leave x0,y0.z0 and nx,ny,nz the same. Do NOT update any rates.
    double oldvol = vGetEntireVolume();
    double linscale = pow( vol / oldvol, 1.0 / 3.0 );
    dx_ *= linscale;
    dy_ *= linscale;
    dz_ *= linscale;
    x1_ = x0_ + dx_;
    y1_ = y0_ + dy_;
    z1_ = z0_ + dz_;

    return true;
}

//////////////////////////////////////////////////////////////////

bool CubeMesh::isInsideCuboid( double x, double y, double z ) const
{
    return ( x >= x0_ && x < x1_ && y >= y0_ && y < y1_ &&
             z >= z0_ && z < z1_ );
}

bool CubeMesh::isInsideSpheroid( double x, double y, double z ) const
{
    double cx = ( x0_ + x1_ ) / 2.0;
    double cy = ( y0_ + y1_ ) / 2.0;
    double cz = ( z0_ + z1_ ) / 2.0;

    double rx = ( x - cx ) / fabs( x1_ - x0_ ) / 2.0;
    double ry = ( y - cy ) / fabs( y1_ - y0_ ) / 2.0;
    double rz = ( z - cz ) / fabs( z1_ - z0_ ) / 2.0;

    return ( ( rx * rx + ry * ry + rz * rz ) < 1.0 );
}

void CubeMesh::fillSpaceToMeshLookup()
{
    static const unsigned int flag = EMPTY;
    unsigned int num = 0;
    unsigned int q = 0;
    m2s_.clear();
    s2m_.resize( nx_ * ny_ * nz_, flag );
    for( unsigned int k = 0; k < nz_; ++k )
    {
        double z = k * dz_ + z0_;
        for( unsigned int j = 0; j < ny_; ++j )
        {
            double y = j * dy_ + y0_;
            for( unsigned int i = 0; i < nx_; ++i )
            {
                double x = i * dx_ + x0_;
                if ( isInsideCuboid( x, y, z ) )
                {
                    s2m_[q] = num;
                    m2s_.push_back( q );
                    ++num;
                }
                else
                {
                    s2m_[q] = flag;
                }
                ++q;
            }
        }
    }
    assert( m2s_.size() == num );
}

// Reads off s2m to build m2s.
void CubeMesh::deriveM2sFromS2m()
{
    m2s_.clear();
    assert( s2m_.size() == nx_ * ny_ * nz_ );
    for ( unsigned int i = 0; i < s2m_.size(); ++i )
    {
        if ( s2m_[i] != EMPTY )
        {
            m2s_.push_back( i );
            assert( m2s_.size() == s2m_[i] + 1 );
        }
    }
    buildStencil();
}

void CubeMesh::deriveS2mFromM2s()
{
    s2m_.clear();
    s2m_.resize( nx_ * ny_ * nz_, EMPTY );
    for ( unsigned int i = 0; i < m2s_.size(); ++i )
    {
        s2m_[ m2s_[i] ] = i;
    }
    buildStencil();
}

// This is a general version of the function, just relies on the
// contents of the s2m_ and m2s_ vectors to do its job.
// Assumes that entire volume is bounded by nx_, ny_, nz.

void CubeMesh::buildStencil()
{
    static const unsigned int flag = EMPTY;


    // fillSpaceToMeshLookup();
    unsigned int num = m2s_.size();
    setStencilSize( num, num );
    for ( unsigned int i = 0; i < num; ++i )
    {
        unsigned int q = m2s_[i];
        unsigned int ix = q % nx_;
        unsigned int iy = ( q / nx_ ) % ny_;
        unsigned int iz = ( q / ( nx_ * ny_ ) ) % nz_;
        vector< double > entry;
        vector< unsigned int > colIndex;
        vector< Ecol > e;

        if ( ix > 0 && s2m_[q-1] != flag )
        {
            e.push_back( Ecol( dy_ * dz_ / dx_, s2m_[q-1] ) );
        }
        if ( ( ix < nx_ - 1 ) && s2m_[q+1] != flag )
        {
            e.push_back( Ecol( dy_ * dz_ / dx_, s2m_[q+1] ) );
        }
        if ( iy > 0 && s2m_[ q-nx_ ] != flag )
        {
            assert( q >= nx_ );
            e.push_back( Ecol( dx_ * dz_ / dy_, s2m_[q-nx_] ) );
        }
        if ( iy < ny_ - 1 && s2m_[ q+nx_ ] != flag )
        {
            assert( q+nx_ < s2m_.size() );
            e.push_back( Ecol( dx_ * dz_ / dy_, s2m_[q+nx_] ) );
        }
        if ( iz > 0 && s2m_[ q - nx_*ny_ ] != flag )
        {
            assert( q >= nx_ * ny_ );
            e.push_back( Ecol( dx_ * dy_ / dz_, s2m_[q - nx_ * ny_] ) );
        }
        if ( iz < nz_ - 1 && s2m_[ q + nx_*ny_ ] )
        {
            assert( q+nx_ < s2m_.size() );
            e.push_back( Ecol( dx_ * dy_ / dz_, s2m_[q + nx_ * ny_] ) );
        }
        sort( e.begin(), e.end() );
        for ( vector< Ecol >::iterator j = e.begin(); j != e.end(); ++j )
        {
            entry.push_back( j->e_ );
            colIndex.push_back( j->col_ );
        }
        addRow( i, entry, colIndex );
    }
    innerResetStencil();
}

//////////////////////////////////////////////////////////////////

const vector< unsigned int >& CubeMesh::surface() const
{
    return surface_;
}

// For now: Just brute force through the surface list.
// Surface list applies only if 2 or 3 D.
void CubeMesh::matchMeshEntries(const ChemCompt* other, vector< VoxelJunction >& ret) const
{
    const CubeMesh* cm = dynamic_cast< const CubeMesh* >( other );

    if ( cm )
    {
        if ( alwaysDiffuse_ )
            matchAllEntries( cm, ret );
        else
            matchCubeMeshEntries( cm, ret );
        /*
        if ( compareMeshSpacing( cm ) == 0 ) { // Equal spacing.
        		matchSameSpacing( cm, ret );
        } else if ( compareMeshSpacing( cm ) == -1 ) { // self is finer
        		cout << "Warning:CubeMesh::matchMeshEntries: cannot yet handle unequal meshes\n";
        } else { // other is finer.
        		cout << "Warning:CubeMesh::matchMeshEntries: cannot yet handle unequal meshes\n";
        }
        */
        return;
    }
    const EndoMesh* em = dynamic_cast< const EndoMesh* >( other );
    if ( em )
    {
        em->matchMeshEntries( this, ret );
        flipRet( ret );
        return;
    }
    cout << "Warning:CubeMesh::matchMeshEntries: cannot yet handle Neuro or Cyl meshes.\n";
}

unsigned int CubeMesh::numDims() const
{
    return ( ( nx_ > 1 ) + ( ny_ > 1 ) + ( nz_ > 1 ) );
}


void CubeMesh::indexToSpace( unsigned int index,
                             double& x, double& y, double& z ) const
{
    assert ( index < nx_ * ny_ * nz_ );

    // index = m2s_[index];
    unsigned int ix = index % nx_;
    index /= nx_;
    unsigned int iy = index % ny_;
    index /= ny_;
    unsigned int iz = index % nz_;

    x = x0_ + ix * dx_ + dx_ * 0.5;
    y = y0_ + iy * dy_ + dy_ * 0.5;
    z = z0_ + iz * dz_ + dz_ * 0.5;
}

unsigned int CubeMesh::spaceToIndex( double x, double y, double z ) const
{
    if ( x > x0_ && x < x1_ && y > y0_ && y < y1_ && z > z0_ && z < z1_ )
    {
        unsigned int ix = ( x - x0_ ) / dx_;
        unsigned int iy = ( y - y0_ ) / dy_;
        unsigned int iz = ( z - z0_ ) / dz_;
        unsigned int index = ( iz * ny_ + iy ) * nx_ + ix;
        unsigned int innerIndex = s2m_[ index ];
        return innerIndex;
    }
    return EMPTY;
}

double CubeMesh::nearest( double x, double y, double z,
                          unsigned int& index ) const
{
    if ( x > x0_ && x < x1_ && y > y0_ && y < y1_ && z > z0_ && z < z1_ )
    {
        unsigned int ix = ( x - x0_ ) / dx_;
        unsigned int iy = ( y - y0_ ) / dy_;
        unsigned int iz = ( z - z0_ ) / dz_;
        index = ( iz * ny_ + iy ) * nx_ + ix;
        unsigned int innerIndex = s2m_[ index ];
        if ( innerIndex != EMPTY )   // Inside filled volume
        {
            index = innerIndex;
            double tx = x0_ + ix * dx_ + dx_ * 0.5;
            double ty = y0_ + iy * dy_ + dy_ * 0.5;
            double tz = z0_ + iz * dz_ + dz_ * 0.5;
            return distance( x - tx, y - ty, z - tz );
        }
        else     // Outside volume. Look over surface for nearest.
        {
            double rmin = 1e99;
            for ( vector< unsigned int >::const_iterator
                    i = surface_.begin(); i != surface_.end(); ++i )
            {
                double tx, ty, tz;
                indexToSpace( *i, tx, ty, tz );
                double r = distance( tx - x, ty - y, tz - z );
                if ( rmin > r )
                {
                    rmin = r;
                    index = *i;
                }
            }
            return -rmin; // Negative distance indicates xyz is outside vol
        }
    }
    // Should really figure out nearest corner anyway.
    index = 0;
    return -1.0;
}

int CubeMesh::compareMeshSpacing( const CubeMesh* other ) const
{
    if ( doubleApprox( dx_, other->dx_ ) &&
            doubleApprox( dy_, other->dy_ ) &&
            doubleApprox( dz_, other->dz_ ) )
        return 0; // equal

    if ( dx_ >= other->dx_ &&
            dy_ >= other->dy_ &&
            dz_ >= other->dz_ )
        return 1; // bigger

    if ( dx_ <= other->dx_ &&
            dy_ <= other->dy_ &&
            dz_ <= other->dz_ )
        return -1; // smaller than other.


    cout << "Warning: CubeMesh::compareMeshSpacing: inconsistent spacing\n";
    return 0;
}

void CubeMesh::defineIntersection( const CubeMesh* other,
                                   double& xmin, double &xmax,
                                   double& ymin, double &ymax,
                                   double& zmin, double &zmax )
const
{
    const double meshSlop = 0.2;
    xmin = ( x0_ > other->x0_ ) ? x0_ : other->x0_;
    xmax = ( x1_ < other->x1_ ) ? x1_ : other->x1_;
    ymin = ( y0_ > other->y0_ ) ? y0_ : other->y0_;
    ymax = ( y1_ < other->y1_ ) ? y1_ : other->y1_;
    zmin = ( z0_ > other->z0_ ) ? z0_ : other->z0_;
    zmax = ( z1_ < other->z1_ ) ? z1_ : other->z1_;
    // Align to coarser mesh
    double temp = ( xmin - x0_) / dx_;
    if ( temp - floor( temp ) > meshSlop )
        xmin = floor( temp ) * dx_;
    temp = ( ymin - y0_) / dy_;
    if ( temp - floor( temp ) > meshSlop )
        ymin = floor( temp ) * dy_;
    temp = ( zmin - z0_) / dz_;
    if ( temp - floor( temp ) > meshSlop )
        zmin = floor( temp ) * dz_;

    // Provide 1 voxel padding all around.
    xmin -= dx_;
    xmax += dx_;
    ymin -= dy_;
    ymax += dy_;
    zmin -= dz_;
    zmax += dz_;
    swapIfBackward( xmin, xmax );
    swapIfBackward( ymin, ymax );
    swapIfBackward( zmin, zmax );
}

/**
 * The intersect pairs are always meshIndex in first, and a flag in
 * the second. The flag can be:
 * 	EMPTY: empty
 * 	SURFACE: on the surface
 * 	ABUT[X,Y,Z]: One place removed from the surface, only one entry
 * 	MULTI: Multiple surface voxels are only one place removed from here.
 * 	a number: Special case where 2 surface voxels abut. The number is the
 * 	meshIndex of the second surface voxel.
 */

/**
 * setAbut assigns the voxel flags based on what was already there
 */
void setAbut( PII& voxel, unsigned int meshIndex, unsigned int axis )
{
    if ( voxel.second == CubeMesh::SURFACE ) // Don't touch surface voxels.
        return;
    if ( voxel.second == CubeMesh::EMPTY )
        voxel = PII( meshIndex, axis );
    else // 1 or more indices are already here.
        voxel.second = CubeMesh::MULTI;
}

void setIntersectVoxel(
    vector< PII >& intersect,
    unsigned int ix, unsigned int iy, unsigned int iz,
    unsigned int nx, unsigned int ny, unsigned int nz,
    unsigned int meshIndex )
{
    assert( ix < nx && iy < ny && iz < nz );
    unsigned int index = ( iz * ny + iy ) * nx + ix;
    assert( index < intersect.size() );
    intersect[index] = PII( meshIndex, CubeMesh::SURFACE );
    if ( ix > 0 )
        setAbut( intersect[ (iz*ny + iy) * nx + ix-1 ], meshIndex,
                 CubeMesh::ABUTX );
    if ( ix + 1 < nx )
        setAbut( intersect[ (iz*ny + iy) * nx + ix+1 ], meshIndex,
                 CubeMesh::ABUTX );

    if ( iy > 0 )
        setAbut( intersect[ ( iz*ny + iy-1 ) * nx + ix ], meshIndex,
                 CubeMesh::ABUTY);
    if ( iy + 1 < ny )
        setAbut( intersect[ ( iz*ny + iy+1 ) * nx + ix ], meshIndex,
                 CubeMesh::ABUTY);

    if ( iz > 0 )
        setAbut( intersect[ ( (iz-1)*ny + iy ) * nx + ix ], meshIndex,
                 CubeMesh::ABUTZ);
    if ( iz + 1 < nz )
        setAbut( intersect[ ( (iz+1)*ny + iy ) * nx + ix ], meshIndex,
                 CubeMesh::ABUTZ);
}


/**
 * checkAbut checks the intersect vector for the current position
 * ix, iy, iz, to determine how many diffusion terms to extract. It
 * then puts each of the extracted terms into the ret vector. There is
 * a minor efficiency for one and two diffusion terms as they are
 * encoded within the intersect vector. Higher-order surface alignments
 * require an in-line scan of neighboring voxels.
 * In all casesl the function inserts a flag indicating surface direction
 * into the diffScale
 * field of the VoxelJunction. 0 = x; 1 = y; 2 = z.
 */
void checkAbut(
    const vector< PII >& intersect,
    unsigned int ix, unsigned int iy, unsigned int iz,
    unsigned int nx, unsigned int ny, unsigned int nz,
    unsigned int meshIndex,
    vector< VoxelJunction >& ret
)
{
    unsigned int index = ( iz * ny + iy ) * nx + ix;
    unsigned int localFlag = intersect[index].second;

    if ( localFlag == CubeMesh::EMPTY || localFlag == CubeMesh::SURFACE )
        return; // Nothing to put into the ret vector
    if ( localFlag == CubeMesh::ABUTX )
    {
        ret.push_back(
            VoxelJunction( intersect[index].first, meshIndex, 0 ));
    }
    else if ( localFlag == CubeMesh::ABUTY )
    {
        ret.push_back(
            VoxelJunction( intersect[index].first, meshIndex, 1 ));
    }
    else if ( localFlag == CubeMesh::ABUTZ )
    {
        ret.push_back(
            VoxelJunction( intersect[index].first, meshIndex, 2 ));
    }
    else if ( localFlag == CubeMesh::MULTI )     // go through all 6 cases.
    {
        if ( ix > 0 )
        {
            index = ( iz * ny + iy ) * nx + ix - 1;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 0 ));
        }
        if ( ix + 1 < nx )
        {
            index = ( iz * ny + iy ) * nx + ix + 1;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 0 ));
        }
        if ( iy > 0 )
        {
            index = ( iz * ny + iy -1 ) * nx + ix;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 1 ));
        }
        if ( iy + 1 < ny )
        {
            index = ( iz * ny + iy + 1 ) * nx + ix;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 1 ));
        }
        if ( iz > 0 )
        {
            index = ( (iz-1) * ny + iy ) * nx + ix;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 2 ));
        }
        if ( iz + 1 < nz )
        {
            index = ( (iz+1) * ny + iy ) * nx + ix;
            if ( intersect[index].second == CubeMesh::SURFACE )
                ret.push_back(
                    VoxelJunction( intersect[index].first, meshIndex, 2 ));
        }
    }
}

void CubeMesh::assignVoxels( vector< PII >& intersect,
                             double xmin, double xmax,
                             double ymin, double ymax,
                             double zmin, double zmax
                           ) const
{
    unsigned int nx = 0.5 + ( xmax - xmin ) / dx_;
    unsigned int ny = 0.5 + ( ymax - ymin ) / dy_;
    unsigned int nz = 0.5 + ( zmax - zmin ) / dz_;
    // offset terms for nx, ny, nz from coord system for coarser grid.
    // Note that these can go negative if the intersect grid goes past
    // the coarse grid.
    int ox = round( ( xmin - x0_ ) / dx_ );
    int oy = round( ( ymin - y0_ ) / dy_ );
    int oz = round( ( zmin - z0_ ) / dz_ );

    // Scan through mesh surface using coarser grid, assign cuboid voxels
    for ( vector< unsigned int >::const_iterator i = surface_.begin();
            i != surface_.end(); ++i )
    {
        unsigned int index = *i;
        double x, y, z;
        indexToSpace( index, x, y, z );
        if ( x >= xmin && x <= xmax && y >= ymin && y <= ymax &&
                z >= zmin && z <= zmax )
        {
            int ix = index % nx_ - ox;
            assert( ix >= 0 );
            unsigned int uix = ix;
            index /= nx_;
            int iy = index % ny_ - oy;
            assert( iy >= 0 );
            unsigned int uiy = iy;
            index /= ny_;
            int iz = index % nz_ - oz;
            assert( iz >= 0 );
            unsigned int uiz = iz;
            unsigned int meshIndex = s2m_[ *i ];

            setIntersectVoxel( intersect, uix, uiy, uiz, nx, ny, nz,
                               meshIndex );
        }
    }
}

void CubeMesh::matchCubeMeshEntries( const CubeMesh* other,
                                     vector< VoxelJunction >& ret ) const
{
    // Flip meshes if the current grid is finer.
    // There are still problems here because a finer grid will end
    // up with 2 levels deep defined as being abutting.
    if ( compareMeshSpacing( other ) == -1 )
    {
        other->matchMeshEntries( this, ret );
        flipRet( ret );
        return;
    }
    ret.resize( 0 );
    // Define intersecting cuboid
    double xmin, xmax, ymin, ymax, zmin, zmax;
    defineIntersection( other, xmin, xmax, ymin, ymax, zmin, zmax );

    // Allocate intersecting cuboid
    unsigned int nx = 0.5 + ( xmax - xmin ) / dx_;
    unsigned int ny = 0.5 + ( ymax - ymin ) / dy_;
    unsigned int nz = 0.5 + ( zmax - zmin ) / dz_;
    vector< PII > intersect( nx * ny * nz, PII( EMPTY, EMPTY ) );
    assignVoxels( intersect, xmin, xmax, ymin, ymax, zmin, zmax );

    // Scan through finer mesh surface, check for occupied voxels.
    for ( vector< unsigned int >::const_iterator i =
                other->surface_.begin();
            i != other->surface_.end(); ++i )
    {
        double x, y, z;
        other->indexToSpace( *i, x, y, z );
        if ( x >= xmin && x <= xmax && y >= ymin && y <= ymax &&
                z >= zmin && z <= zmax )
        {
            unsigned int ix = ( x - xmin ) / dx_;
            unsigned int iy = ( y - ymin ) / dy_;
            unsigned int iz = ( z - zmin ) / dz_;
            unsigned int meshIndex = other->s2m_[ *i ];
            checkAbut( intersect, ix, iy, iz, nx, ny, nz, meshIndex, ret );
        }
    }

    // Scan through the VoxelJunctions and populate their diffScale field
    setDiffScale( other, ret );
    // Scan through the VoxelJunctions and populate their volume field
    setJunctionVol( other, ret );
    sort( ret.begin(), ret.end() );
}

/**
 * Version that assumes all voxels are in contact and able to diffuse
 * with matching voxels on other mesh
 */
void CubeMesh::matchAllEntries(const CubeMesh* other, vector<VoxelJunction>& ret) const
{
    ret.clear();
    unsigned int min = m2s_.size();
    if ( min > other->m2s_.size() )
        min = other->m2s_.size();
    ret.resize( min );
    for ( unsigned int i = 0; i < min; ++i )
    {
        ret[i] = VoxelJunction( i, i );
    }
}

void CubeMesh::matchCylMeshEntries( const ChemCompt* other,
                                    vector< VoxelJunction >& ret ) const
{
    other->matchMeshEntries( this, ret );
    flipRet( ret );
}

void CubeMesh::setDiffScale( const CubeMesh* other,
                             vector< VoxelJunction >& ret ) const
{

    for ( vector< VoxelJunction >::iterator i = ret.begin();
            i != ret.end(); ++i )
    {
        if ( doubleEq( i->diffScale, 0 ) )   // Junction across x plane
        {
            double selfXA = dy_ * dz_;
            double otherXA = other->dy_ * other->dz_;
            if ( selfXA <= otherXA )
                i->diffScale = 2 * selfXA / ( dx_ + other->dx_ );
            else
                i->diffScale = 2 * otherXA / ( dx_ + other->dx_ );
        }
        else if ( doubleEq( i->diffScale, 1 ) )     // across y plane
        {
            double selfXA = dx_ * dz_;
            double otherXA = other->dx_ * other->dz_;
            if ( selfXA <= otherXA )
                i->diffScale = 2 * selfXA / ( dy_ + other->dy_ );
            else
                i->diffScale = 2 * otherXA / ( dy_ + other->dy_ );
        }
        else if ( doubleEq( i->diffScale, 2 ) )     // across z plane
        {
            double selfXA = dx_ * dy_;
            double otherXA = other->dx_ * other->dy_;
            if ( selfXA <= otherXA )
                i->diffScale = 2 * selfXA / ( dz_ + other->dz_ );
            else
                i->diffScale = 2 * otherXA / ( dz_ + other->dz_ );
        }
        else
        {
            assert( 0 );
        }
    }
}

void CubeMesh::setJunctionVol( const CubeMesh* other,
                               vector< VoxelJunction >& ret ) const
{

    double myVol = dx_ * dy_ * dz_;
    double otherVol = other->dx_ * other->dy_ * other->dz_;
    for ( vector< VoxelJunction >::iterator i = ret.begin();
            i != ret.end(); ++i )
    {
        i->firstVol = myVol;
        i->secondVol = otherVol;
    }
}
