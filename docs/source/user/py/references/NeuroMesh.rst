NeuroMesh
---------

.. py:class:: NeuroMesh


   .. py:method:: setSubTree

      (*destination message field*)      Assigns field value.


   .. py:method:: getSubTree

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSubTreePath

      (*destination message field*)      Assigns field value.


   .. py:method:: getSubTreePath

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setSeparateSpines

      (*destination message field*)      Assigns field value.


   .. py:method:: getSeparateSpines

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumSegments

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumDiffCompts

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getParentVoxel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getElecComptList

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getElecComptMap

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStartVoxelInCompt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getEndVoxelInCompt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSpineVoxelOnDendVoxel

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDendVoxelsOnCompartment

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSpineVoxelsOnCompartment

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setDiffLength

      (*destination message field*)      Assigns field value.


   .. py:method:: getDiffLength

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setGeometryPolicy

      (*destination message field*)      Assigns field value.


   .. py:method:: getGeometryPolicy

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:attribute:: spineListOut

      vector<Id>,vector<Id>,vector<unsigned int> (*source message field*)      Request SpineMesh to construct self based on list of electrical compartments that this NeuroMesh has determined are spine shaft and spine head respectively. Also passes in the info about where each spine is connected to the NeuroMesh. Arguments: shaft compartment Ids, head compartment Ids,index of matching parent voxels for each spine


   .. py:attribute:: psdListOut

      vector<double>,vector<Id>,vector<unsigned int> (*source message field*)      Tells PsdMesh to build a mesh. Arguments: (Cell Id, Coordinates of each psd, Id of electrical compartment mapped to each voxel, index of matching parent voxels for each spine.) The coordinates each have 8 entries:xyz of centre of psd, xyz of vector perpendicular to psd, psd diameter,  diffusion distance from parent compartment to PSD


   .. py:attribute:: subTree

      vector<ObjId> (*value field*)      Set of compartments in which to embed chemical reaction systems. If the compartments happen to be contiguousthen also set up diffusion between them. Can alsohandle cases where the same cell is divided into multiplenon-diffusively-coupled compartments


   .. py:attribute:: subTreePath

      string (*value field*)      Set of compartments to model, defined as a path string. If they happen to be contiguous then also set up diffusion between the compartments. Can alsohandle cases where the same cell is divided into multiplenon-diffusively-coupled compartments


   .. py:attribute:: separateSpines

      bool (*value field*)      Flag: when separateSpines is true, the traversal separates any compartment with the strings 'spine', 'head', 'shaft' or 'neck' in its name,Allows to set up separate mesh for spines, based on the same cell model. Requires for the spineListOut message tobe sent to the target SpineMesh object.


   .. py:attribute:: numSegments

      unsigned int (*value field*)      Number of cylindrical/spherical segments in model


   .. py:attribute:: numDiffCompts

      unsigned int (*value field*)      Number of diffusive compartments in model


   .. py:attribute:: parentVoxel

      vector<unsigned int> (*value field*)      Vector of indices of parents of each voxel.


   .. py:attribute:: elecComptList

      vector<Id> (*value field*)      Vector of Ids of all electrical compartments in this NeuroMesh. Ordering is as per the tree structure built in the NeuroMesh, and may differ from Id order. Ordering matches that used for startVoxelInCompt and endVoxelInCompt


   .. py:attribute:: elecComptMap

      vector<Id> (*value field*)      Vector of Ids of electrical compartments that map to each voxel. This is necessary because the order of the IDs may differ from the ordering of the voxels. Additionally, there are typically many more voxels than there are electrical compartments. So many voxels point to the same elecCompt.


   .. py:attribute:: startVoxelInCompt

      vector<unsigned int> (*value field*)      Index of first voxel that maps to each electrical compartment. Each elecCompt has one or more voxels. The voxels in a compartment are numbered sequentially.


   .. py:attribute:: endVoxelInCompt

      vector<unsigned int> (*value field*)      Index of end voxel that maps to each electrical compartment. In keeping with C and Python convention, this is one more than the last voxel. Each elecCompt has one or more voxels. The voxels in a compartment are numbered sequentially.


   .. py:attribute:: spineVoxelOnDendVoxel

      vector<int> (*value field*)      Voxel index of spine voxel on each dend voxel. Assume that there is never more than one spine per dend voxel. If no spine present, the entry is -1. Note that the same index is used both for spine head and PSDs.


   .. py:attribute:: diffLength

      double (*value field*)      Diffusive length constant to use for subdivisions. The system willattempt to subdivide cell using diffusive compartments ofthe specified diffusion lengths as a maximum.In order to get integral numbersof compartments in each segment, it may subdivide more finely.Uses default of 0.5 microns, that is, half typical lambda.For default, consider a tau of about 1 second for mostreactions, and a diffusion const of about 1e-12 um^2/sec.This gives lambda of 1 micron


   .. py:attribute:: geometryPolicy

      string (*value field*)      Policy for how to interpret electrical model geometry (which is a branching 1-dimensional tree) in terms of 3-D constructslike spheres, cylinders, and cones.There are three options, default, trousers, and cylinder:default mode: - Use frustrums of cones. Distal diameter is always from compt dia. - For linear dendrites (no branching), proximal diameter is  diameter of the parent compartment - For branching dendrites and dendrites emerging from soma, proximal diameter is from compt dia. Don't worry about overlap. - Place somatic dendrites on surface of spherical soma, or at ends of cylindrical soma - Place dendritic spines on surface of cylindrical dendrites, not emerging from their middle.trousers mode: - Use frustrums of cones. Distal diameter is always from compt dia. - For linear dendrites (no branching), proximal diameter is  diameter of the parent compartment - For branching dendrites, use a trouser function. Avoid overlap. - For soma, use some variant of trousers. Here we must avoid overlap - For spines, use a way to smoothly merge into parent dend. Radius of curvature should be similar to that of the spine neck. - Place somatic dendrites on surface of spherical soma, or at ends of cylindrical soma - Place dendritic spines on surface of cylindrical dendrites, not emerging from their middle.cylinder mode: - Use cylinders. Diameter is just compartment dia. - Place somatic dendrites on surface of spherical soma, or at ends of cylindrical soma - Place dendritic spines on surface of cylindrical dendrites, not emerging from their middle. - Ignore spatial overlap.


   .. py:attribute:: dendVoxelsOnCompartment

      ObjId,vector<unsigned int> (*lookup field*)      Returns vector of all chem voxels on specified electrical compartment of the dendrite. Returns empty vec if none found, or if the compartment isn't on the dendrite.


   .. py:attribute:: spineVoxelsOnCompartment

      ObjId,vector<unsigned int> (*lookup field*)      Returns vector of all chem voxels on specified electrical compartment, which should be a spine head or shaft . Returns empty vec if no chem voxels found, or if the compartment isn't on the dendrite. Note that spine and PSD voxel indices are the same for a given spine head.
