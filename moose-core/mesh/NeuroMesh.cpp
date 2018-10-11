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
#include "../utility/Vec.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "ChemCompt.h"
#include "MeshCompt.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
#include "NeuroMesh.h"
#include "SpineEntry.h"
#include "SpineMesh.h"
#include "EndoMesh.h"
#include "../utility/numutil.h"
#include "../utility/strutil.h"
#include "../shell/Wildcard.h"

static SrcFinfo3< vector< Id >, vector< Id >, vector< unsigned int > >*
spineListOut()
{
    static SrcFinfo3< vector< Id >, vector< Id >, vector< unsigned int > >
    spineListOut(
        "spineListOut",
        "Request SpineMesh to construct self based on list of electrical "
        "compartments that this NeuroMesh has determined are spine shaft "
        "and spine head respectively. Also passes in the info about where "
        "each spine is connected to the NeuroMesh. "
        "Arguments: shaft compartment Ids, head compartment Ids,"
        "index of matching parent voxels for each spine"
    );
    return &spineListOut;
}

static SrcFinfo3< vector< double >, vector< Id >, vector< unsigned int > >*
psdListOut()
{
    static SrcFinfo3< vector< double >,
           vector< Id >, vector< unsigned int > >
           psdListOut(
               "psdListOut",
               "Tells PsdMesh to build a mesh. "
               "Arguments: (Cell Id, Coordinates of each psd, "
               "Id of electrical compartment mapped to each voxel, "
               "index of matching parent voxels for each spine.) "
               "The coordinates each have 8 entries:"
               "xyz of centre of psd, xyz of vector perpendicular to psd, "
               "psd diameter, "
               " diffusion distance from parent compartment to PSD"
           );
    return &psdListOut;
}

const Cinfo* NeuroMesh::initCinfo()
{
    //////////////////////////////////////////////////////////////
    // Field Definitions
    //////////////////////////////////////////////////////////////
    static ElementValueFinfo< NeuroMesh, vector< ObjId > > subTree(
        "subTree",
        "Set of compartments in which to embed chemical reaction "
        "systems. If the compartments happen to be contiguous"
        "then also set up diffusion between them. Can also"
        "handle cases where the same cell is divided into multiple"
        "non-diffusively-coupled compartments",
        &NeuroMesh::setSubTree,
        &NeuroMesh::getSubTree
    );
    static ElementValueFinfo< NeuroMesh, string > subTreePath(
        "subTreePath",
        "Set of compartments to model, defined as a path string. "
        "If they happen to be contiguous "
        "then also set up diffusion between the compartments. Can also"
        "handle cases where the same cell is divided into multiple"
        "non-diffusively-coupled compartments",
        &NeuroMesh::setSubTreePath,
        &NeuroMesh::getSubTreePath
    );
    static ValueFinfo< NeuroMesh, bool > separateSpines(
        "separateSpines",
        "Flag: when separateSpines is true, the traversal separates "
        "any compartment with the strings "
        "'spine', 'head', 'shaft' or 'neck' in its name,"
        "Allows to set up separate mesh for spines, based on the "
        "same cell model. Requires for the spineListOut message to"
        "be sent to the target SpineMesh object.",
        &NeuroMesh::setSeparateSpines,
        &NeuroMesh::getSeparateSpines
    );
    static ReadOnlyValueFinfo< NeuroMesh, unsigned int > numSegments(
        "numSegments",
        "Number of cylindrical/spherical segments in model",
        &NeuroMesh::getNumSegments
    );
    static ReadOnlyValueFinfo< NeuroMesh, unsigned int > numDiffCompts(
        "numDiffCompts",
        "Number of diffusive compartments in model",
        &NeuroMesh::getNumDiffCompts
    );
    static ReadOnlyValueFinfo< NeuroMesh, vector< unsigned int > > parentVoxel(
        "parentVoxel",
        "Vector of indices of parents of each voxel.",
        &NeuroMesh::getParentVoxel
    );
    static ReadOnlyValueFinfo< NeuroMesh, vector< Id > > elecComptMap(
        "elecComptMap",
        "Vector of Ids of electrical compartments that map to each "
        "voxel. This is necessary because the order of the IDs may "
        "differ from the ordering of the voxels. Additionally, there "
        "are typically many more voxels than there are electrical "
        "compartments. So many voxels point to the same elecCompt.",
        &NeuroMesh::getElecComptMap
    );
    static ReadOnlyValueFinfo< NeuroMesh, vector< Id > > elecComptList(
        "elecComptList",
        "Vector of Ids of all electrical compartments in this "
        "NeuroMesh. Ordering is as per the tree structure built in "
        "the NeuroMesh, and may differ from Id order. Ordering "
        "matches that used for startVoxelInCompt and endVoxelInCompt",
        &NeuroMesh::getElecComptList
    );
    static ReadOnlyValueFinfo< NeuroMesh, vector< unsigned int > > startVoxelInCompt(
        "startVoxelInCompt",
        "Index of first voxel that maps to each electrical "
        "compartment. Each elecCompt has one or more voxels. "
        "The voxels in a compartment are numbered sequentially.",
        &NeuroMesh::getStartVoxelInCompt
    );
    static ReadOnlyValueFinfo< NeuroMesh, vector< unsigned int > > endVoxelInCompt(
        "endVoxelInCompt",
        "Index of end voxel that maps to each electrical "
        "compartment. In keeping with C and Python convention, this "
        "is one more than the last voxel. "
        "Each elecCompt has one or more voxels. "
        "The voxels in a compartment are numbered sequentially.",
        &NeuroMesh::getEndVoxelInCompt
    );

    static ReadOnlyValueFinfo< NeuroMesh, vector< int > > spineVoxelOnDendVoxel(
        "spineVoxelOnDendVoxel",
        "Voxel index of spine voxel on each dend voxel. Assume that "
        "there is never more than one spine per dend voxel. If no "
        "spine present, the entry is -1. Note that the "
        "same index is used both for spine head and PSDs.",
        &NeuroMesh::getSpineVoxelOnDendVoxel
    );

    static ReadOnlyLookupValueFinfo< NeuroMesh, ObjId,
           vector< unsigned int > > dendVoxelsOnCompartment(
               "dendVoxelsOnCompartment",
               "Returns vector of all chem voxels on specified electrical "
               "compartment of the dendrite. Returns empty vec if none "
               "found, or if the compartment isn't on the dendrite.",
               &NeuroMesh::getDendVoxelsOnCompartment
           );

    static ReadOnlyLookupValueFinfo< NeuroMesh, ObjId,
           vector< unsigned int > > spineVoxelsOnCompartment(
               "spineVoxelsOnCompartment",
               "Returns vector of all chem voxels on specified electrical "
               "compartment, which should be a spine head or shaft . "
               "Returns empty vec if no chem voxels "
               "found, or if the compartment isn't on the dendrite. "
               "Note that spine and PSD voxel indices are the same for a "
               "given spine head.",
               &NeuroMesh::getSpineVoxelsOnCompartment
           );

    static ValueFinfo< NeuroMesh, double > diffLength(
        "diffLength",
        "Diffusive length constant to use for subdivisions. "
        "The system will"
        "attempt to subdivide cell using diffusive compartments of"
        "the specified diffusion lengths as a maximum."
        "In order to get integral numbers"
        "of compartments in each segment, it may subdivide more "
        "finely."
        "Uses default of 0.5 microns, that is, half typical lambda."
        "For default, consider a tau of about 1 second for most"
        "reactions, and a diffusion const of about 1e-12 um^2/sec."
        "This gives lambda of 1 micron",
        &NeuroMesh::setDiffLength,
        &NeuroMesh::getDiffLength
    );

    static ValueFinfo< NeuroMesh, string > geometryPolicy(
        "geometryPolicy",
        "Policy for how to interpret electrical model geometry (which "
        "is a branching 1-dimensional tree) in terms of 3-D constructs"
        "like spheres, cylinders, and cones."
        "There are three options, default, trousers, and cylinder:"
        "default mode:"
        " - Use frustrums of cones. Distal diameter is always from compt dia."
        " - For linear dendrites (no branching), proximal diameter is "
        " diameter of the parent compartment"
        " - For branching dendrites and dendrites emerging from soma,"
        " proximal diameter is from compt dia. Don't worry about overlap."
        " - Place somatic dendrites on surface of spherical soma, or at ends"
        " of cylindrical soma"
        " - Place dendritic spines on surface of cylindrical dendrites, not"
        " emerging from their middle."
        "trousers mode:"
        " - Use frustrums of cones. Distal diameter is always from compt dia."
        " - For linear dendrites (no branching), proximal diameter is "
        " diameter of the parent compartment"
        " - For branching dendrites, use a trouser function. Avoid overlap."
        " - For soma, use some variant of trousers. Here we must avoid overlap"
        " - For spines, use a way to smoothly merge into parent dend. Radius of"
        " curvature should be similar to that of the spine neck."
        " - Place somatic dendrites on surface of spherical soma, or at ends"
        " of cylindrical soma"
        " - Place dendritic spines on surface of cylindrical dendrites, not"
        " emerging from their middle."
        "cylinder mode:"
        " - Use cylinders. Diameter is just compartment dia."
        " - Place somatic dendrites on surface of spherical soma, or at ends"
        " of cylindrical soma"
        " - Place dendritic spines on surface of cylindrical dendrites, not"
        " emerging from their middle."
        " - Ignore spatial overlap.",
        &NeuroMesh::setGeometryPolicy,
        &NeuroMesh::getGeometryPolicy
    );

    //////////////////////////////////////////////////////////////
    // MsgDest Definitions
    //////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // Field Elements
    //////////////////////////////////////////////////////////////

    static Finfo* neuroMeshFinfos[] =
    {
        &subTree,		// Value
        &subTreePath,		// Value
        &separateSpines,	// Value
        &numSegments,		// ReadOnlyValue
        &numDiffCompts,		// ReadOnlyValue
        &parentVoxel,			// ReadOnlyValue
        &elecComptList,			// ReadOnlyValue
        &elecComptMap,			// ReadOnlyValue
        &startVoxelInCompt,			// ReadOnlyValue
        &endVoxelInCompt,			// ReadOnlyValue
        &spineVoxelOnDendVoxel,		// ReadOnlyValue
        &dendVoxelsOnCompartment,	// ReadOnlyLookupValue
        &spineVoxelsOnCompartment,	// ReadOnlyLookupValue
        &diffLength,			// Value
        &geometryPolicy,		// Value
        spineListOut(),			// SrcFinfo
        psdListOut(),			// SrcFinfo
    };

    static Dinfo< NeuroMesh > dinfo;
    static Cinfo neuroMeshCinfo (
        "NeuroMesh",
        ChemCompt::initCinfo(),
        neuroMeshFinfos,
        sizeof( neuroMeshFinfos ) / sizeof ( Finfo* ),
        &dinfo
    );

    return &neuroMeshCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* neuroMeshCinfo = NeuroMesh::initCinfo();

//////////////////////////////////////////////////////////////////
// Class stuff.
//////////////////////////////////////////////////////////////////
NeuroMesh::NeuroMesh()
    :
    nodes_(1),
    subTreePath_( "Undefined" ),
    nodeIndex_(1, 0 ),
    vs_( 1, NA * 1e-9 ),
    area_( 1, 1.0e-12 ),
    length_( 1, 1.0e-6 ),
    diffLength_( 1.0e-6 ),
    separateSpines_( false ),
    geometryPolicy_( "default" ),
    surfaceGranularity_( 0.1 ),
    parentVoxel_(1, -1)
{
    nodes_[0].setLength( diffLength_ );
    nodes_[0].setDia( diffLength_ );
    nodes_[0].setNumDivs( 1 );
}

NeuroMesh::NeuroMesh( const NeuroMesh& other )
    :
    diffLength_( other.diffLength_ ),
    separateSpines_( other.separateSpines_ ),
    geometryPolicy_( other.geometryPolicy_ ),
    surfaceGranularity_( other.surfaceGranularity_ )
{
    ;
}

NeuroMesh& NeuroMesh::operator=( const NeuroMesh& other )
{
    nodes_ = other.nodes_;
    nodeIndex_ = other.nodeIndex_;
    vs_ = other.vs_;
    area_ = other.area_;
    length_ = other.length_;
    diffLength_ = other.diffLength_;
    separateSpines_ = other.separateSpines_;
    geometryPolicy_ = other.geometryPolicy_;
    return *this;
}

NeuroMesh::~NeuroMesh()
{
    ;
}

//////////////////////////////////////////////////////////////////
// Field assignment stuff
//////////////////////////////////////////////////////////////////

/**
 * This assumes that lambda is the quantity to preserve, over numEntries.
 * So when the compartment changes size, so does numEntries.
 * Assumes that the soma node is at index 0.
 */
void NeuroMesh::updateCoords()
{
    unsigned int startFid = 0;
    if ( nodes_.size() <= 1 ) // One for soma and one for dummy pa of soma
    {
        buildStencil();
        return;
    }
    for ( vector< NeuroNode >::iterator i = nodes_.begin();
            i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() )
        {
            double len = i->getLength();
            unsigned int numDivs = floor( 0.5 + len / diffLength_ );
            if ( numDivs < 1 )
                numDivs = 1;
            i->setNumDivs( numDivs );
            i->setStartFid( startFid );
            startFid += numDivs;
        }
    }
    nodeIndex_.resize( startFid );
    for ( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        if ( !nodes_[i].isDummyNode() )
        {
            unsigned int end = nodes_[i].startFid() +nodes_[i].getNumDivs();
            assert( end <= startFid );
            assert( nodes_[i].getNumDivs() > 0 );
            for ( unsigned int j = nodes_[i].startFid(); j < end; ++j )
                nodeIndex_[j] = i;
        }
    }
    // Assign volumes and areas
    vs_.resize( startFid );
    area_.resize( startFid );
    length_.resize( startFid );
    for ( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        const NeuroNode& nn = nodes_[i];
        if ( !nn.isDummyNode() )
        {
            assert( nn.parent() < nodes_.size() );
            const NeuroNode& parent = nodes_[ nn.parent() ];
            for ( unsigned int j = 0; j < nn.getNumDivs(); ++j )
            {
                vs_[j + nn.startFid()] = nn.voxelVolume( parent, j );
                area_[j + nn.startFid()] = nn.getMiddleArea( parent, j);
                length_[j + nn.startFid()] = nn.getVoxelLength();
            }
        }
    }
    buildStencil();
}

void NeuroMesh::setDiffLength( double v )
{
    diffLength_ = v;
    updateCoords();
}

double NeuroMesh::getDiffLength() const
{
    return diffLength_;
}

void NeuroMesh::setGeometryPolicy( string v )
{
    // STL magic! Converts the string v to lower case. Unfortunately
    // someone has overloaded tolower so it can have one or two args, so
    // this marvellous construct is useless.
    // std::transform( v.begin(), v.end(), v.begin(), std::tolower );
    for( string::iterator i = v.begin(); i != v.end(); ++i )
        *i = tolower( *i );

    if ( !( v == "cylinder" || v == "trousers" || v == "default" ) )
    {
        cout << "Warning: NeuroMesh::setGeometryPolicy( " << v <<
             " ):\n Mode must be one of cylinder, trousers, or default."
             "Using default\n";
        v = "default";
    }

    if ( v == geometryPolicy_ )
        return;
    geometryPolicy_ = v;
    bool isCylinder = ( v == "cylinder" );
    for ( vector< NeuroNode >::iterator
            i = nodes_.begin(); i != nodes_.end(); ++i )
        i->setIsCylinder( isCylinder );
}

string NeuroMesh::getGeometryPolicy() const
{
    return geometryPolicy_;
}

unsigned int NeuroMesh::innerGetDimensions() const
{
    return 3;
}

Id tryParent( Id id, const string& msgName )
{
    const Finfo* finfo = id.element()->cinfo()->findFinfo( msgName );
    if ( !finfo )
        return Id();
    vector< Id > ret;
    id.element()->getNeighbors( ret, finfo );
    assert( ret.size() <= 1 );
    if ( ret.size() == 1 )
        return ret[0];
    return Id();
}

Id getParentFromMsg( Id id )
{
    if ( id.element()->cinfo()->isA( "Compartment" ) )
        return tryParent( id, "axialOut" );
    if ( id.element()->cinfo()->isA( "SymCompartment" ) )
        return tryParent( id, "proximalOut" );
    return Id();
}

Id NeuroMesh::putSomaAtStart( Id origSoma, unsigned int maxDiaIndex )
{
    Id soma = origSoma;
    if ( nodes_[maxDiaIndex].elecCompt() == soma )   // Happy, happy
    {
        ;
    }
    else if ( soma == Id() )
    {
        soma = nodes_[maxDiaIndex].elecCompt();
    }
    else     // Disagreement. Ugh.
    {
        string name = nodes_[ maxDiaIndex ].elecCompt().element()->getName();
        // OK, somehow this model has more than one soma compartment.
        if ( moose::strncasecmp( name.c_str(), "soma", 4 ) == 0 )
        {
            soma = nodes_[maxDiaIndex].elecCompt();
        }
        else
        {
            cout << "Warning: NeuroMesh::putSomaAtStart: named 'soma' compartment isn't biggest\n";
            soma = nodes_[maxDiaIndex].elecCompt();
        }
    }
    // Move soma to start of nodes_ vector.
    if ( maxDiaIndex != 0 )
    {
        NeuroNode temp = nodes_[0];
        nodes_[0] = nodes_[maxDiaIndex];
        nodes_[maxDiaIndex] = temp;
    }
    return soma;
}

void NeuroMesh::insertSingleDummy(
    unsigned int parent, unsigned int self,
    double x, double y, double z	)
{
    static const double EPSILON = 1e-8;
    NeuroNode dummy( nodes_[ self ] );
    dummy.clearChildren();
    dummy.setNumDivs( 0 );
    bool isCylinder = (geometryPolicy_ == "cylinder" );
    dummy.setIsCylinder( isCylinder );
    dummy.setX( x );
    dummy.setY( y );
    dummy.setZ( z );
    // Now insert the dummy as a surrogate parent.
    dummy.setParent( parent );
    dummy.addChild( self );
    nodes_[ self ].setParent( nodes_.size() );
    // Idiot check for a bad dimensioned compartment.
    if ( nodes_[self].calculateLength( dummy ) < EPSILON )
    {
        double length = nodes_[self].getLength();
        dummy.setX( x - length );
        assert( doubleEq( nodes_[self].calculateLength( dummy ), length) );
    }
    nodes_.push_back( dummy );
}

void NeuroMesh::insertDummyNodes()
{
    // First deal with the soma, always positioned at node 0.
    unsigned int num = nodes_.size();
    for ( unsigned int i = 0; i < num; ++i )
    {
        if ( nodes_[i].parent() == ~0U )
        {
            Id elec = nodes_[i].elecCompt();
            double x = Field< double >::get( elec, "x0" );
            double y = Field< double >::get( elec, "y0" );
            double z = Field< double >::get( elec, "z0" );
            insertSingleDummy( ~0U, i, x, y, z );
        }
    }

    // Second pass: insert dummy nodes for children.
    // Need to know if parent has multiple children, because each of
    // them will need a dummyNode to connect to.
    // In all the policies so far, the dummy nodes take the same diameter
    // as the children that they host.
    for ( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        vector< unsigned int > kids = nodes_[i].children();
        if ( (!nodes_[i].isDummyNode()) && kids.size() > 1 )
        {
            for( unsigned int j = 0; j < kids.size(); ++j )
            {
                double x = nodes_[i].getX(); // use coords of parent.
                double y = nodes_[i].getY();
                double z = nodes_[i].getZ();
                insertSingleDummy( i, kids[j], x, y, z );
                // Replace the old kid entry with the dummy
                kids[j] = nodes_.size() - 1;
            }
            // Connect up the parent to the dummy nodes.
            nodes_[i].clearChildren();
            for( unsigned int j = 0; j < kids.size(); ++j )
                nodes_[i].addChild( kids[j] );
        }
    }
}

bool NeuroMesh::filterSpines( Id compt )
{
    if ( compt.element()->getName().find( "shaft" ) != string::npos ||
            compt.element()->getName().find( "neck" ) != string::npos )
    {
        shaft_.push_back( compt );
        return true;
    }
    if ( compt.element()->getName().find( "spine" ) != string::npos ||
            compt.element()->getName().find( "head" ) != string::npos )
    {
        head_.push_back( compt );
        return true;
    }
    return false;
}


void NeuroMesh::transmitSpineInfo( const Eref& e )
{
    spineListOut()->send( e, shaft_, head_, parent_ );

    vector< double > ret;
    vector< double > psdCoords;
    vector< unsigned int > index( head_.size(), 0 );
    if ( e.element()->hasMsgs( psdListOut()->getBindIndex() ) )
    {
        // Later can refine to deal with spineless PSDs.
        for ( unsigned int i = 0; i < head_.size(); ++i )
        {
            SpineEntry se =
                SpineEntry( shaft_[i], head_[i], parent_[i] );
            ret = se.psdCoords();
            assert( ret.size() == 8 );
            psdCoords.insert( psdCoords.end(), ret.begin(), ret.end() );
            index[i] = i;
        }
        // ids.clear();
        // e.element()->getNeighbors( ids, psdListOut() );
        psdListOut()->send( e, psdCoords, head_, index );
    }
}

/**
 * converts the parents_ vector from identifying the parent NeuroNode
 * to identifying the parent voxel, for each shaft entry.
 */
void NeuroMesh::updateShaftParents()
{
    assert( shaft_.size() == parent_.size() );
    vector< unsigned int > pa = parent_;
    for ( unsigned int i = 0; i < shaft_.size(); ++i )
    {
        const NeuroNode& nn = nodes_[ pa[i] ];
        double x0 = Field< double >::get( shaft_[i], "x0" );
        double y0 = Field< double >::get( shaft_[i], "y0" );
        double z0 = Field< double >::get( shaft_[i], "z0" );
        const NeuroNode& pn = nodes_[ nn.parent() ];
        unsigned int index = 0;
        double r = nn.nearest( x0, y0, z0, pn, index );
        if ( r >= 0.0 )
        {
            parent_[i] = index + nn.startFid();
        }
        else
        {
            cout << "Warning: NeuroMesh::updateShaftParents: may be"
                 " misaligned on " << i << ", r=" << r <<
                 "\n pt=(" << x0 << "," << y0 << "," << z0 << ")" <<
                 "pa=(" << nn.getX() << "," << nn.getY() << "," << nn.getZ() << ")" <<
                 "\n";
            parent_[i] = index + nn.startFid();
        }
    }
}

// Uses all compartments, and if they have spines on them adds those too.
void NeuroMesh::setSubTree( const Eref& e, vector< ObjId > compts )
{
    sort( compts.begin(), compts.end() );
    if ( separateSpines_ )
    {
        NeuroNode::buildSpinyTree( compts, nodes_, shaft_, head_, parent_);
        insertDummyNodes();
        updateCoords();
        updateShaftParents();
        transmitSpineInfo( e );
    }
    else
    {
        NeuroNode::buildTree( nodes_, compts );
        insertDummyNodes();
        updateCoords();
    }
    subTreePath_ = "Undefined: subTree set as a compartment list";
}

vector< ObjId > NeuroMesh::getSubTree( const Eref& e ) const
{
    vector< Id > temp = getElecComptList();
    vector< ObjId > ret( temp.size() );
    for ( unsigned int i = 0; i < ret.size(); ++i )
        ret[i] = temp[i];
    return ret;
}

// Uses all compartments, and if they have spines on them adds those too.
void NeuroMesh::setSubTreePath( const Eref& e, string path )
{
    vector< ObjId > compts;
    wildcardFind( path, compts );
    setSubTree( e, compts );
    subTreePath_ = path;
}

string NeuroMesh::getSubTreePath( const Eref& e ) const
{
    return subTreePath_;
}

void NeuroMesh::setSeparateSpines( bool v )
{
    if ( v != separateSpines_ )
    {
        separateSpines_ = v;
        updateCoords();
    }
}

bool NeuroMesh::getSeparateSpines() const
{
    return separateSpines_;
}

unsigned int NeuroMesh::getNumSegments() const
{
    unsigned int ret = 0;
    for ( vector< NeuroNode >::const_iterator i = nodes_.begin();
            i != nodes_.end(); ++i )
        ret += !i->isDummyNode();
    return ret;
}

unsigned int NeuroMesh::getNumDiffCompts() const
{
    return nodeIndex_.size();
}

vector< unsigned int > NeuroMesh::getParentVoxel() const
{
    return parentVoxel_;
}

vector< Id > NeuroMesh::getElecComptMap() const
{
    vector< Id > ret( nodeIndex_.size() );
    for ( unsigned int i = 0; i < nodeIndex_.size(); ++i )
    {
        ret[i] = nodes_[nodeIndex_[i]].elecCompt();
        // cout << i << "	" << nodeIndex_[i] << "	" << ret[i] << endl;

    }
    return ret;
}

vector< Id > NeuroMesh::getElecComptList() const
{
    vector< Id > ret;
    for ( vector< NeuroNode >::const_iterator
            i = nodes_.begin(); i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() )
            ret.push_back( i->elecCompt() );
    }
    return ret;
}

vector< unsigned int > NeuroMesh::getStartVoxelInCompt() const
{
    vector< unsigned int > ret;
    for ( vector< NeuroNode >::const_iterator
            i = nodes_.begin(); i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() )
            ret.push_back( i->startFid() );
    }
    return ret;
}

vector< unsigned int > NeuroMesh::getEndVoxelInCompt() const
{
    vector< unsigned int > ret;
    for ( vector< NeuroNode >::const_iterator
            i = nodes_.begin(); i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() )
            ret.push_back( i->startFid() + i->getNumDivs() );
    }
    return ret;
}

vector< int > NeuroMesh::getSpineVoxelOnDendVoxel() const
{
    vector< int > ret( nodeIndex_.size(), -1 ); //-1 means no spine present
    for ( unsigned int i = 0; i < parent_.size(); ++i )
    {
        assert( parent_[i] < ret.size() );
        ret[ parent_[i] ] = i;
    }
    return ret;
}

vector< unsigned int > NeuroMesh::getDendVoxelsOnCompartment(ObjId compt) const
{
    vector< unsigned int > ret;
    for ( vector< NeuroNode >::const_iterator
            i = nodes_.begin(); i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() && i->elecCompt() == compt.id )
        {
            for ( unsigned int j = 0; j < i->getNumDivs(); ++j )
            {
                ret.push_back( j + i->startFid() );
            }
        }
    }
    return ret;
}

vector< unsigned int > NeuroMesh::getSpineVoxelsOnCompartment(ObjId compt) const
{
    vector< unsigned int > ret;
    assert( shaft_.size() == head_.size() );
    for ( unsigned int i = 0; i < shaft_.size(); ++i )
    {
        if ( shaft_[i] == compt.id || head_[i] == compt.id )
            ret.push_back(i);
    }
    return ret;
}

const vector< double >& NeuroMesh::vGetVoxelVolume() const
{
    return vs_;
}

const vector< double >& NeuroMesh::vGetVoxelMidpoint() const
{
    static vector< double > midpoint;
    unsigned int num = vs_.size();
    midpoint.resize( num * 3 );
    vector< double >::iterator k = midpoint.begin();
    for ( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        const NeuroNode& nn = nodes_[i];
        if ( !nn.isDummyNode() )
        {
            assert( nn.parent() < nodes_.size() );
            const NeuroNode& parent = nodes_[ nn.parent() ];
            for ( unsigned int j = 0; j < nn.getNumDivs(); ++j )
            {
                vector< double > coords = nn.getCoordinates( parent, j );
                *k = ( coords[0] + coords[3] ) / 2.0;
                *(k + num ) = ( coords[1] + coords[4] ) / 2.0;
                *(k + 2 * num ) = ( coords[2] + coords[5] ) / 2.0;
                k++;
            }
        }
    }
    return midpoint;
}

const vector< double >& NeuroMesh::getVoxelArea() const
{
    return area_;
}

const vector< double >& NeuroMesh::getVoxelLength() const
{
    return length_;
}

double NeuroMesh::vGetEntireVolume() const
{
    double ret = 0.0;
    for ( vector< double >::const_iterator i =
                vs_.begin(); i != vs_.end(); ++i )
        ret += *i;
    return ret;
}

bool NeuroMesh::vSetVolumeNotRates( double volume )
{
    assert( parentVoxel_.size() == nodeIndex_.size() );
    assert( parentVoxel_.size() == 1 );
    assert( vs_.size() == 1 );
    if ( parentVoxel_.size() > 1 ) // Can't handle multicompartments yet.
        return false;
    NeuroNode& n = nodes_[0];
    double oldvol = n.volume( nodes_[0] );
    double scale = pow( volume / oldvol, 1.0/3.0 );
    n.setLength( n.getLength() * scale );
    n.setDia( n.getDia() * scale );
    vs_[0] *= volume / oldvol;
    area_[0] *= scale * scale;
    length_[0] *= scale;
    diffLength_ = length_[0];

    return true;
}

//////////////////////////////////////////////////////////////////
// FieldElement assignment stuff for MeshEntries
//////////////////////////////////////////////////////////////////

/// Virtual function to return MeshType of specified entry.
unsigned int NeuroMesh::getMeshType( unsigned int fid ) const
{
    assert( fid < nodeIndex_.size() );
    assert( nodeIndex_[fid] < nodes_.size() );
    if ( nodes_[ nodeIndex_[fid] ].isSphere() )
        return SPHERE_SHELL_SEG;

    return CYL;
}

/// Virtual function to return dimensions of specified entry.
unsigned int NeuroMesh::getMeshDimensions( unsigned int fid ) const
{
    return 3;
}

/// Virtual function to return volume of mesh Entry.
double NeuroMesh::getMeshEntryVolume( unsigned int fid ) const
{
    if ( nodeIndex_.size() == 0 )
        return 1.0; // A default value to use before init
    assert( fid < nodeIndex_.size() );
    assert( nodeIndex_[fid] < nodes_.size() );
    const NeuroNode& node = nodes_[ nodeIndex_[fid] ];
    assert( fid >= node.startFid() );
    if ( node.parent() == ~0U )
        return node.voxelVolume( node, fid - node.startFid() );
    assert ( node.parent() < nodes_.size() );
    const NeuroNode& parent = nodes_[ node.parent() ];
    return node.voxelVolume( parent, fid - node.startFid() );
}

/// Virtual function to return coords of mesh Entry.
/// For Cylindrical mesh, coords are x1y1z1 x2y2z2 r0 r1 phi0 phi1
vector< double > NeuroMesh::getCoordinates( unsigned int fid ) const
{
    assert( fid < nodeIndex_.size() );
    assert( nodeIndex_[fid] < nodes_.size() );
    const NeuroNode& node = nodes_[ nodeIndex_[fid] ];
    assert( fid >= node.startFid() );
    assert ( node.parent() < nodes_.size() );
    const NeuroNode& parent = nodes_[ node.parent() ];

    return node.getCoordinates( parent, fid - node.startFid() );
}

/// Virtual function to return diffusion X-section area for each neighbor
vector< double > NeuroMesh::getDiffusionArea( unsigned int fid ) const
{
    assert( fid < nodeIndex_.size() );
    assert( nodeIndex_[fid] < nodes_.size() );
    const NeuroNode& node = nodes_[ nodeIndex_[fid] ];
    assert( fid >= node.startFid() );
    assert ( node.parent() < nodes_.size() );
    const NeuroNode& parent = nodes_[ node.parent() ];
    vector< double > ret;
    vector< unsigned int > neighbors = getNeighbors( fid );
    for ( unsigned int i = 0; i < neighbors.size(); ++i )
    {
        ret.push_back( node.getDiffusionArea( parent, neighbors[ i ] ) );
    }
    return ret;
}

/// Virtual function to return scale factor for diffusion.
/// I think all dendite tips need to return just one entry of 1.
//  Regular points return vector( 2, 1.0 );
vector< double > NeuroMesh::getDiffusionScaling( unsigned int fid ) const
{
    /*
    if ( nodeIndex_.size() <= 1 )
    return vector< double >( 0 );

    if ( !isToroid_ && ( fid == 0 || fid == (nodeIndex_.size() - 1) ) )
    return vector< double >( 1, 1.0 );
    */

    return vector< double >( 2, 1.0 );
}

/// Virtual function to return volume of mesh Entry, including
/// for diffusively coupled voxels from other solvers.
double NeuroMesh::extendedMeshEntryVolume( unsigned int fid ) const
{
    if ( fid < nodeIndex_.size() )
    {
        return getMeshEntryVolume( fid );
    }
    else
    {
        return MeshCompt::extendedMeshEntryVolume( fid - nodeIndex_.size() );
    }
}



//////////////////////////////////////////////////////////////////
// Dest funcsl
//////////////////////////////////////////////////////////////////

#if 0
/// More inherited virtual funcs: request comes in for mesh stats
/// Not clear what this does.
void NeuroMesh::innerHandleRequestMeshStats( const Eref& e,
        const SrcFinfo2< unsigned int, vector< double > >* meshStatsFinfo
                                           )
{
    vector< double > ret( vGetEntireVolume() / nodeIndex_.size() ,1 );
    meshStatsFinfo->send( e, 1, ret );
}
#endif

void NeuroMesh::innerHandleNodeInfo(
    const Eref& e,
    unsigned int numNodes, unsigned int numThreads )
{
    /*
    unsigned int numEntries = nodeIndex_.size();
    vector< double > vols( numEntries, 0.0 );
    double oldVol = getMeshEntryVolume( 0 );
    for ( unsigned int i = 0; i < numEntries; ++i ) {
    assert( nodeIndex_[i] < nodes_.size() );
    NeuroNode& node = nodes_[ nodeIndex_[i] ];
    assert( nodes_.size() > node.parent() );
    NeuroNode& parent = nodes_[ node.parent() ];
    vols[i] = node.voxelVolume( parent, i );
    }
    vector< unsigned int > localEntries( numEntries );
    vector< vector< unsigned int > > outgoingEntries;
    vector< vector< unsigned int > > incomingEntries;
    meshSplit()->send( e,
    oldVol,
    vols, localEntries,
    outgoingEntries, incomingEntries );
    */
}
//////////////////////////////////////////////////////////////////

/**
 * Inherited virtual func. Returns number of MeshEntry in array
 */
unsigned int NeuroMesh::innerGetNumEntries() const
{
    return nodeIndex_.size();
}

/**
 * Inherited virtual func. Assigns number of MeshEntries.
 * This one doesn't set exact number, because each compartment has to
 * fit integral numbers of voxels.
 */
void NeuroMesh::innerSetNumEntries( unsigned int n )
{
    static const unsigned int WayTooLarge = 1000000;
    if ( n == 0 || n > WayTooLarge )
    {
        cout << "Warning: NeuroMesh::innerSetNumEntries( " << n <<
             " ): out of range\n";
        return;
    }
    double totalLength = 0;
    for ( vector< NeuroNode >::iterator i = nodes_.begin();
            i != nodes_.end(); ++i )
    {
        if ( !i->isDummyNode() )
        {
            if ( i->isSphere() )
                totalLength += i->getDia();
            else
                totalLength += i->getLength();
        }
    }
    assert( totalLength > 0 );
    diffLength_ = totalLength / n;
    updateCoords();
}


/**
 * This is a bit odd, effectively asks to build an imaginary neuron and
 * then subdivide it. I'll make do with a ball-and-stick model: Soma with
 * a single apical dendrite with reasonable diameter. I will interpret
 * size as total length of neuron, not as volume.
 * Soma will have a diameter of up to 20 microns, anything bigger than
 * this is treated as soma of 20 microns +
 * dendrite of (specified length - 10 microns) for radius of soma.
 * This means we avoid having miniscule dendrites protruding from soma,
 * the shortest one will be 10 microns.
 */
void NeuroMesh::innerBuildDefaultMesh( const Eref& e,
                                       double size, unsigned int numEntries )
{

    if ( size > 10e-3 )
    {
        cout << "Warning: attempt to build a neuron of dendritic length " <<
             size << " metres.\n Seems improbable.\n" <<
             "Using default of 0.001 m\n";
        size = 1e-3;
    }

    diffLength_ = size / numEntries;

    vector< unsigned int > noChildren( 0 );
    vector< unsigned int > oneChild( 1, 2 );

    if ( size < 20e-6 )
    {
        CylBase cb( 0, 0, 0, size, 0, numEntries );
        NeuroNode soma( cb, 0, noChildren, 0, Id(), true );
        nodes_.resize( 1, soma );
        nodeIndex_.resize( 1, 0 );
    }
    else
    {
        CylBase cb( 0, 0, 0, 20e-6, 0, 1 );
        NeuroNode soma( cb, 0, oneChild, 0, Id(), true );
        nodes_.resize( 1, soma );
        nodeIndex_.resize( 1, 0 );

        CylBase cbDummy( 0, 0, 10e-6, 4e-6, 0, 0 );
        NeuroNode dummy( cbDummy, 0, noChildren, 1, Id(), false );
        nodes_.push_back( dummy );

        CylBase cbDend( 0, 0, size, 2e-6, size - 10e-6, numEntries - 1);
        NeuroNode dend( cbDend, 1, noChildren, 2, Id(), false );
        nodes_.push_back( dend );
        for ( unsigned int i = 1; i < numEntries; ++i )
            nodeIndex_.push_back( 2 );
    }
    updateCoords();
}

//////////////////////////////////////////////////////////////////
// Utility function to set up Stencil for diffusion
//////////////////////////////////////////////////////////////////

double NeuroMesh::getAdx( unsigned int i, unsigned int& parentFid ) const
{
    const NeuroNode &nn = nodes_[ nodeIndex_[i] ];
    if ( nn.isDummyNode() || nn.parent() == ~0U )
        return -1; // No diffusion, bail out.
    const NeuroNode *pa = &nodes_[ nn.parent() ];
    double L1 = nn.getLength() / nn.getNumDivs();
    double L2 = L1;
    parentFid = i - 1;
    if ( nn.startFid() == i )
    {
        // We're at the start of the node, need to refer to parent for L
        const NeuroNode* realParent = pa;
        if ( pa->isDummyNode() )
        {
            if ( pa->parent() == ~0U )
            {
                parentFid = -1;
                return -1; // No diffusion, bail out.
            }
            assert( pa->parent() < nodes_.size() );
            realParent = &nodes_[ realParent->parent() ];
            if ( realParent->isDummyNode() )
            {
                // Still dummy. So we're at a terminus. No diffusion
                return -1;
            }
        }
        L2 = realParent->getLength() / realParent->getNumDivs();
        parentFid = realParent->startFid() +
                    realParent->getNumDivs() - 1;
    }
    assert( parentFid < nodeIndex_.size() );
    double length = 0.5 * (L1 + L2 );
    // Note that we use the parent node here even if it is a dummy.
    // It has the correct diameter.
    double adx = nn.getDiffusionArea( *pa, i - nn.startFid() ) / length;
    return adx;
}

void NeuroMesh::buildStencil()
{
    parentVoxel_.clear();
    setStencilSize( nodeIndex_.size(), nodeIndex_.size() );
    SparseMatrix< double > sm( nodeIndex_.size(), nodeIndex_.size() );
    vector< vector< double > > paEntry( nodeIndex_.size() );
    vector< vector< unsigned int > > paColIndex( nodeIndex_.size() );
    // It is very easy to set up the matrix using the parent as there is
    // only one parent for every voxel.
    for ( unsigned int i = 0; i < nodeIndex_.size(); ++i )
    {
        unsigned int parentFid;
        double adx = getAdx( i, parentFid );
        parentVoxel_.push_back( parentFid );
        if ( adx < 0.0 ) // No diffusion, so don't put in any entry.
            continue;
        /*
        const NeuroNode &nn = nodes_[ nodeIndex_[i] ];
        const NeuroNode *pa = &nodes_[ nn.parent() ];
        double L1 = nn.getLength() / nn.getNumDivs();
        double L2 = L1;
        unsigned int parentFid = i - 1;
        if ( nn.startFid() == i ) {
        	// We're at the start of the node, need to refer to parent for L
        	const NeuroNode* realParent = pa;
        	if ( pa->isDummyNode() ) {
        		realParent = &nodes_[ realParent->parent() ];
        		if ( realParent->isDummyNode() ) {
        			// Still dummy. So we're at a terminus. No diffusion
        			continue;
        		}
        	}
        	L2 = realParent->getLength() / realParent->getNumDivs();
        	parentFid = realParent->startFid() +
        			realParent->getNumDivs() - 1;
        }
        assert( parentFid < nodeIndex_.size() );
        double length = 0.5 * (L1 + L2 );
        // Note that we use the parent node here even if it is a dummy.
        // It has the correct diameter.
        double adx = nn.getDiffusionArea( *pa, i - nn.startFid() ) / length;
        */
        paEntry[ i ].push_back( adx );
        paColIndex[ i ].push_back( parentFid );
        // Now put in the symmetric entries.
        paEntry[ parentFid ].push_back( adx );
        paColIndex[ parentFid ].push_back( i );
    }

    // Now go through the paEntry and paColIndex and build sparse matrix.
    // We have to do this separately because the sparse matrix has to be
    // build up in row order, and sorted, whereas the entries above
    // are random access.
    for ( unsigned int i = 0; i < nodeIndex_.size(); ++i )
    {
        unsigned int num = paColIndex[i].size();
        vector< Ecol > e( num );
        vector< double > entry( num );
        vector< unsigned int > colIndex( num );
        for ( unsigned int j = 0; j < num; ++j )
        {
            e[j] = Ecol( paEntry[i][j], paColIndex[i][j] );
        }
        sort( e.begin(), e.end() );

        for ( unsigned int j = 0; j < num; ++j )
        {
            entry[j] = e[j].e_;
            colIndex[j] = e[j].col_;
        }
        addRow( i, entry, colIndex );
    }
    innerResetStencil();
}


const vector< NeuroNode >& NeuroMesh::getNodes() const
{
    return nodes_;
}

//////////////////////////////////////////////////////////////////
// Utility function for junctions
//////////////////////////////////////////////////////////////////

void NeuroMesh::matchMeshEntries( const ChemCompt* other,
                                  vector< VoxelJunction >& ret ) const
{
    const CubeMesh* cm = dynamic_cast< const CubeMesh* >( other );
    if ( cm )
    {
        matchCubeMeshEntries( other, ret );
        return;
    }
    const EndoMesh* em = dynamic_cast< const EndoMesh* >( other );
    if ( em )
    {
        em->matchMeshEntries( this, ret );
        flipRet( ret );
        return;
    }
    const SpineMesh* sm = dynamic_cast< const SpineMesh* >( other );
    if ( sm )
    {
        sm->matchNeuroMeshEntries( this, ret );
        flipRet( ret );
        return;
    }
    const NeuroMesh* nm = dynamic_cast< const NeuroMesh* >( other );
    if ( nm )
    {
        matchNeuroMeshEntries( other, ret );
        return;
    }
    cout << "Warning: NeuroMesh::matchMeshEntries: unknown class\n";
}

void NeuroMesh::indexToSpace( unsigned int index,
                              double& x, double& y, double& z ) const
{
    if ( index >= innerGetNumEntries() )
        return;
    const NeuroNode& nn = nodes_[ nodeIndex_[ index ] ];
    const NeuroNode& pa = nodes_[ nn.parent() ];
    Vec a( pa.getX(), pa.getY(), pa.getZ() );
    Vec b( nn.getX(), nn.getY(), nn.getZ() );
    double frac = ( ( index - nn.startFid() ) + 0.5 ) / nn.getNumDivs();
    Vec pt = a.pointOnLine( b, frac );
    x = pt.a0();
    y = pt.a1();
    z = pt.a2();
}

double NeuroMesh::nearest( double x, double y, double z,
                           unsigned int& index ) const
{
    double best = 1e12;
    index = 0;
    for( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        const NeuroNode& nn = nodes_[i];
        if ( !nn.isDummyNode() )
        {
            assert( nn.parent() < nodes_.size() );
            const NeuroNode& pa = nodes_[ nn.parent() ];
            double linePos;
            double r;
            double near = nn.nearest( x, y, z, pa, linePos, r );
            if ( linePos >= 0 && linePos < 1.0 )
            {
                if ( best > near )
                {
                    best = near;
                    index = linePos * nn.getNumDivs() + nn.startFid();
                }
            }
        }
    }
    if ( best == 1e12 )
        return -1;
    return best;
}

void NeuroMesh::matchCubeMeshEntries( const ChemCompt* other,
                                      vector< VoxelJunction >& ret ) const
{
    for( unsigned int i = 0; i < nodes_.size(); ++i )
    {
        const NeuroNode& nn = nodes_[i];
        if ( !nn.isDummyNode() )
        {
            assert( nn.parent() < nodes_.size() );
            const NeuroNode& pa = nodes_[ nn.parent() ];
            nn.matchCubeMeshEntries( other, pa, nn.startFid(),
                                     surfaceGranularity_, ret, true, false );
        }
    }
}

void NeuroMesh::matchNeuroMeshEntries( const ChemCompt* other,
                                       vector< VoxelJunction >& ret ) const
{
}
