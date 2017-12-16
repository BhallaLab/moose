/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NEURO_NODE_H
#define _NEURO_NODE_H

/**
 * Helper class for the NeuroMesh. Defines the geometry of the branching
 * neuron.
 */

class NeuroNode: public CylBase
{
	public:
		/**
		 * This function explicitly fills in all fields of the NeuroNode
		 */
		NeuroNode( const CylBase& cb,
			unsigned int parent, const vector< unsigned int >& children,
			unsigned int startFid_, Id elecCompt,
			bool isSphere );
		/**
		 * This builds the node using info from the compartment. But the
		 * parent and children have to be filled in later
		 */
		NeuroNode( Id elecCompt );
		/**
		 * Empty constructor for vectors
		 */
		NeuroNode();


		unsigned int parent() const;
		unsigned int startFid() const;
		Id elecCompt() const;

		/**
		 * True when this is a dummy node to represent the coordinates
		 * of the start end of a compartment. For example, the start coords
		 * of a compartment
		 * sitting on a spherical soma, or the start coords of a spine neck
		 * along a longer dendritic compartment.
		 * In all other cases the start coordinates are just those of the
		 * end of the parent compartment.
		 *
		 * When the isDummyNode is true, the elecCompt represents the Id of
		 * the compartment whose start it is.
		 */
		bool isDummyNode() const; // True if CylBase::numDivs is zero.
		bool isSphere() const;
		bool isStartNode() const; // True if startFid_ == 0
		const vector< unsigned int >& children() const;


		/**
		 * Fills in child vector
		 */
		void addChild( unsigned int child );

		/**
		 * Zeroes out the child vector
		 */
		void clearChildren();

		/**
		 * Assigns parent node info
		 */
		void setParent( unsigned int parent );

		/**
		 * Assignes startFid
		 */
		void setStartFid( unsigned int f );

		/**
		 * Calculates and returns compartment length, from parent xyz to
		 * self xyz. Assigns own length as a side-effect.
		 */
		double calculateLength( const CylBase& parent );

		//////////////////////////////////////////////////////////////
		// A set of calls for safely generating a tree by traversing
		// compartment messages. Designed to be somewhat resilient, even
		// when given a badly set up neuron.
		//////////////////////////////////////////////////////////////
		/**
 		 * Finds all the compartments connected to current node, put them
 		 * all into the 'children' vector even if they may be 'parent' by
		 * the messaging. This is because this function has to be robust
		 * enough to sort this out
 		 */
		void findConnectedCompartments(
							const map< Id, unsigned int >& nodeMap,
							const vector< NeuroNode >& nodes );

		/**
 		 * Go through nodes vector and eliminate entries that have zero
		 * children, that is, are not connected to any others.
 		 * Need to clean up 'children_' list after this is called.
 		 */
		static unsigned int removeDisconnectedNodes(
						vector< NeuroNode >& nodes );

		/**
		 * Find the start node, typically the soma, of a model. In terms of
		 * the solution, this should be the node at the root of the tree.
		 * Returns index in nodes vector.
		 * Technically the matrix solution could begin from any terminal
		 * branch, but it helps to keep the soma identical to the root of
		 * the tree.
		 *
		 * Uses two heuristics to locate the start node: Looks for the
		 * node with the largest diameter, and also looks for node(s)
		 * with 'soma' in their name. If these disagree then it goes
		 * with the 'soma' node. If there are
		 * many of the soma nodes, it goes with the fattest.
		 */
		static unsigned int findStartNode(
						const vector< NeuroNode >& nodes );

		/**
		 * Traverses the nodes list starting from the 'start' node, and
		 * sets up correct parent-child information. This involves removing
		 * the identified 'parent' node from the 'children_' vector and
		 * assigning it to the parent_ field.
		 * Then it redoes the entire nodes vector (with due care for
		 * indexing of children and parents)
		 * so that it is in the correct order for a depth-first traversal.
		 * This means that you can take any entry in the list, and the
		 * immediately following entries will be all the descendants,
		 * if any.
		 */
		static void traverse(
						vector< NeuroNode >& nodes, unsigned int start );

		/**
		 * Helper recursive function for traversing nodes to build tree.
		 */
		void innerTraverse(
				vector< NeuroNode >& tree,
				const vector< NeuroNode >& nodes,
				vector< unsigned int >& seen
				) const;

		/**
 		* This function takes a list of elements that include connected
 		* compartments, and constructs a tree of nodes out of them. The
 		* generated nodes vector starts with the soma, and is a depth-first
 		* sequence of nodes. This is meant to be insensitive to vagaries
 		* in how the user has set up the compartment messaging, provided
		* that there is at least one recognized message between connected
		* compartments.
 		*/
		static void buildTree( vector< NeuroNode >& nodes,
						vector< ObjId > elist );
		static void buildSpinyTree(
					vector< ObjId >& elist, vector< NeuroNode >& nodes,
					vector< Id >& shaftId, vector< Id >& headId,
					vector< unsigned int >& spineParent );
		void setParentAndChildren( unsigned int index, int dendParent,
				vector< NeuroNode >& nodes,
				const unordered_map< Id, unsigned int >& dendMap );

		/**
		 * Trims off all spines from tree. Does so by identifying a set of
		 * reasonable names: shaft, head, spine, and variants in capitals.
		 * Having done this it builds two matching vectors of vector of
		 * shafts and heads, which is a hack that assumes that there are
		 * no sub-branches in spines.
		 * The returned nodes vector has non spine/shaft compartments only.
		 * The returned shaftId vector has all the shaft compartments.
		 * The returned headId vector has all the shaft compartments.
		 * The returned parent vector has the indices of the parent
		 * node for each shaft.
		 * There should be exactly the same number of entries in the
		 * shaftId, headId and parent vectors.
		 */
		static void filterSpines( vector< NeuroNode >& nodes,
				vector< Id >& shaftId, vector< Id >& headId,
				vector< unsigned int >& parent );
	private:
		/**
		 * Index of parent NeuroNode, typically a diffusive compartment.
		 * In the special case where the junction to the parent electrical
		 * compartment is NOT at the end of the parent compartment, this
		 * refers instead to a dummy NeuroNode which has the coordinates.
		 *
		 * One of the nodes, typically the soma, will have no parent.
		 * This is indicated by the value ~0U.
		 */
		unsigned int parent_;

		/**
		 * Index of children of this NeuroNode.
		 */
		vector< unsigned int >children_;

		/**
		 * Index of starting MeshEntry handled by this NeuroNode. Assumes
		 * a block of contiguous fids are handled by each NeuroNode.
		 */
		unsigned int startFid_;


		/// Id of electrical compartment in which this diffusive compt lives
		Id elecCompt_;

		/**
		 * Special case for soma, perhaps for spine heads.
		 * When true, xyz are centre, and dia is dia.
		 */
		bool isSphere_;

	// For spines we will also need a double to indicate position along
	// parent dendrite.
	//
};

#endif	// _NEURO_NODE_H
