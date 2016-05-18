/**

\page HSolveImplementation HSolve Implementation

\section Introduction

This page documents the internals of HSolve, i.e. the data structures and working of the HSolve class and its base classes. This document is meant to serve as a reference for anyone who wants to get a better understanding of how to interface with existing data structures of HSolve, or who wants to extend the existing code. 

\section GettingToHSolve Getting to HSolve

Where are the entry points into HSolve? (from the python scripting perspective)

<ol>

<li>
The first entry point is, of course, when the HSolve object is created from python, with some path. This calls the HSolve constructor, but is otherwise innocuous.
</li>
<br><br>
<li>
The second entry point, and the more important one as far as setup is concerned, is the HSolve::setup function which gets called as soon as the target field of the HSolve object (in python) is set. This is evident in the definition of the HSolve::setPath function. HSolve::setup is where the matrix for the corresponding compartment gets set up, by walking through the dendritic tree and collecting the various compartment elements. Following this, the fields of HSolveActive get populated, such as the HHChannels, gates, Calcium-dependent channels and synapses. Lastly, outgoing messages are redirected as required. The setup process will be elaborated upon shortly.
</li>
<br><br>
<li>
The third entry point is the moose.reinit() call, which automatically calls a series of reinit functions within hsolve, starting with HSolve::reinit. The reinit call basically resets the simulation to start from the beginning (all updated values are discarded and values are reinitialized). The various calls from reinit are once again explained in detail further below.
</li>
<br><br>
<li>
The last entry point, and where the actual work starts is moose.start( sim_dt ), which triggers the calling of HSolveActive::step (via HSolve::process) repeatedly. This is where the actual simulation happens.
</li>
<br><br>

</ol>

\section HSolveMembers HSolve classes and members

\subsection HinesMatrixSetup Hines Matrix Setup and Data Members

For setup, I'm going to take a bottom-up approach, and start with the Hines Matrix itself: how it is organized as a data structure and how it is accessed in the HSolve code. In order to do this, I'm first going to talk about the Hines method itself.

- The Hines Matrix itself is an admittance matrix of the dendritic tree, after the Ek and Gk values of the various channels have been calculated at a given time step. (TODO: explain half-time step at which each operation is performed)

- The Hines Matrix is a predominantly tridiagonal matrix, with off-tridiagonal elements appearing only when there are branches (or junctions) in the dendritic tree. This is ensured by the indexing mechanism:
  - The Hines indexing mechanism starts from a leaf and performs a depth-first-search on the tree.
  - Numbers are assigned "on the way up", after having exhausted all children of a particular node.
  - The position of the soma is not relevant to the indexing scheme.

- Each compartment in the tree contributes to the diagonal of the Hines Matrix. Neighbouring compartments will contribute to corresponding cells in the tridiagonal: for example, consider compartments 2-3-4 to be a linear segment in the tree. Then, compartment 3 will contribute to the diagonal element (3, 3), to elements (3, 2) and (2, 3) by virtue of its being connected to compartment 2, and to elements (3, 4) and (4, 3) by virtue of its connection with compartment 4.

- Each branch in the tree is a Y-network. Consider:
  \verbatim
    2   6
     \ /
      Y
      |
      7      \endverbatim
  This can equivaltently be converted into the corresponding delta:
  \verbatim
    2---6
     \ /
      7      \endverbatim
  Therefore, a Y branch contributes three elements to each of the upper and lower halves of the triangle, 6 elements in total. In this example, these elements are (2,6) and (6,2); (2,7) and (7,2); (6,7) and (7,6). Note that because of the Hines indexing scheme, at least one of these elements will always be a part of the tridiagonal itself. Also, if we designate "parents" and "children" in the process of performing the DFS, then parents will always have a Hines index that is one more than its that of its greatest child.

- Each multi-way branch is more than a Y-network. For a three-way branch, one can create upto six different branches in the equivalent "delta" configuration:
  \verbatim
    2  4  6
     \ | /
      \|/
       Y
       |
       7     \endverbatim
  Which becomes:
  \verbatim
      4
     /|\
    / | \
   2--|--6
    \ | /
     \|/
      7    \endverbatim
  In such a scenario, the resulting electrical network has 6 unknowns (or in general nC2 unknowns, where n is the total number of nodes in the group of compartments involed at the junction). On the other hand, there are only four (or in general, n) constraints: for a given set of node voltages, the node currents must be the same before and after the transformation (or vice versa). The system of equations is therefore underconstrained. However, for the purpose of effecting the transformation, any one solution is sufficient. It can be inferred from inspection upon writing out the respective equations, that the following value of Gij satisfies the constraints:
  \verbatim Gij = Gi * Gj / Gsum \endverbatim
  where Gsum is the sum over all Gi.

- The admittances produced by each compartment due to itself and its linear neighbours is stored in the HinesMatrix::HS_ vector. HS_ is a vector of doubles, consisting of the flattened diagonal and the values against which the Hines matrix is to be inverted (i.e., the external currents). HS_ can be regarded as the flattened version of an Nx4 matrix "Hines", where N is the number of compartments in the neuron.
  - Hines[i][0] (or HS_[ 4*i + 0 ]) contains the diagonal element, after including the effects of external currents.
  - Hines[i][1] contains the element to the right and bottom of Hines[i][0] in the symmetric matrix: element (i,i+1) = element (i+1,i).
  - Hines[i][2] contains the diagonal element due to passive effects alone.
  - Hines[i][3] contains the total external current injected into this compartment.

- The admittances produced at junctions are stored in the HinesMatrix::HJ_ vector. HJ_ is a flattened vector comprising of elements produced by Wyes converted to Deltas. So, in the previous example, HJ_ would store (in that order) Hines[2][6], Hines[6][2], Hines[2][7], Hines[7][2], Hines[6][7], Hines[7][6]. However, the HJ_ vector itself does not store any information regarding the location of its elements within the Hines matrix.

- The information linking the junctions to HJ_ is stored separately in HinesMatrix::junction_ and HinesMatrix::operandBase_ . But in order to understand these, we first need to look at how they were made. This utilizes two further data structures: HinesMatrix::coupled_ and HinesMatrix::groupNumber_ .
  - HinesMatrix::coupled_ is a vector of groups. A group is a vector of unsigned ints. There are as many groups as there are branch-points in the dendritic tree, and each group holds the children (in order of Hines index) followed by the parent. In other words, the group contains a vector of the Hines indices of all compartments surrounding a branch point, in order of Hines index. coupled_ is a vector of all such groups (one for each branch-point).
  - HinesMatrix::groupNumber_ is a map of unsigned ints to unsigned ints. Given the Hines index of a compartment (which is part of a group in coupled_), it tells you the index of its corresponding group into the coupled_ vector. That is, if i is the Hines index of a compartment, then the group it belongs to is coupled_[ groupNumber_[ i ] ].
  - HinesMatrix::junction_ is a vector of JunctionStruct. There is one element in the junction_ vector for each parent-child pair in the dendritic tree. Each element contains the Hines index of the corresponding child compartment, and a rank which denotes the number of compartments <strong>remaining</strong> in its group. "Remaining" here means the number of compartments with Hines index greater than the current compartment itself. The rank therefore tells you how many more elements of the Hines Matrix can be eliminated by this compartment.
  - HinesMatrix::operandBase_ is a map from unsigned ints to an iterator into HJ_. Given the Hines index of a compartment in a group, it gives you the iterator into HJ_ at the point corresponding to where that compartment's eliminates start. "Compartment's eliminates" here refers to the elements that must be eliminated by this compartment. In the above example of the Y branch, operandBase_[2] would point to the element corresponding to (2,6). operandBase_[6] would point to the element in HJ_ corresponding to (6,7). 7 will not be an available index in operandBase_, because there is nothing left to eliminate.

- The way to iterate through HinesMatrix::HJ_, therefore, involves doing the following:
  - Iterate through HinesMatrix::junction_
  - Find the Hines index of the compartment corresponding to this JunctionStruct element.
  - Find the pointer into HJ_ using HinesMatrix::operandBase_ [ index ].
  - Find the rank (the number of elements to be eliminated by this compartment) from the JunctionStruct.
  - Move rank steps forward in HJ_.
  - Repeat.

- In case you want to find the group associated with a certain compartment (for knowing the Vm values of neighbouring compartments, for, instance), then find the group as \verbatim group = coupled_[ groupNumber_[ index ] ] \endverbatim

There are a few more data members of the HinesMatrix class that have not yet been discussed. These are:
- nCompt_: the number of compartments
- dt_: the simulation time step
- VMid_: the voltage values of the compartments (in order of Hines index) at the middle of the time step (t + dt/2).
- operand_: A vector of iterators into HS_, HJ_ and VMid_, which enables easy access to the relevant data during forward elimination.
- backOperand_: A vector of iterators into HJ_ and VMid_, which enables easy access to the relevant data during backward substitution.
- stage_: A variable that represents which of updateMatrix, backwardSubstitution and forwardElimination have been completed.

\subsection HSolvePassiveSetup HSolvePassive methods

HSolvePassive has methods that enable it to build up the compartment network of the neuron by inspecting the messaging strucutre. A path into the compartment network has to be supplied for each neuron. The building of the tree is accomplished by the following three methods:
- HSolvePassive::walkTree
  This fucntion takes a valid compartment Id as input and traverses the message strucutre until a terminal compartment is found. At this point, it performs a depth-first-search with this terminal compartment as the root. Having accumulated all compartments while going down the tree in a vector (compartmentId_), the method reverses the vector so that compartments get automatically arranged in the order of their Hines indices.
- HSolvePassive::initialize
  Initialize pulls out the membrane parameters from each of the compartments: Vm, Cm, Em, Rm and inject. All leakage channels are iterated through, and the effective Em/Rm and Cm/dt are stored in a CompartmentStruct object for each compartment.
- HSolvePassive::storeTree
  This last method actually creates the tree object by going through compartmentId_ once again and storing the initVm, Cm, Em, Rm and Ra values in the tree_ data structure.

Once the tree has been setup, it is given as a parameter to HinesMatrix which then creates the matrix out of it.

HSolvePassive also contains methods to perform integration in a single-time step (backward Euler). This comprises the stages of Gaussian elimination:
- HSolvePassive::updateMatrix
  This method is used to update matrix parameters, subject to changed compartment parameters - Em/Rm, Cm/dt and inject values.
  (Note that this function is not used. HSolveActive::udpateMatrix does everything that this function does, and a bit more)
- HSolvePassive::forwardEliminate
  This function mostly relies on the operand_ structure created at the HinesMatrix level and uses the operands to reduce the matrix to an upper triangular matrix.
- HSolvePassive::backwardSubstitute
  This function solves the matrix equation for the unknown VMid vector by performing a backward substitution process.

\subsection HSolveActiveSetup HSolveActive setup and data members

 HSolveActive inherits from HinesMatrix via HSolvePassive. While HinesMatrix has methods to build up the matrix and HSolvePassive has methods to solve the matrix, HSolveActive has data and methods that allow it to manipulate channels. The three key methods involved are:
- HSolveActive::advanceChannels
  This function recomputes the "state" values of each of the channels. The state_variable is a flattened vector of the fraction of opened gates across all channels across all compartments. To update state values, the values of rate constants are looked up from the lookup tables, depending upon what the membrane voltage and calcium concetration are at this instant.
- HSolveActive::calculateChannelCurrents
  This function is used to re-compute the total current entering each compartment from "state" information. The state values are raised to the required power and multipled with the respective Gbar.
- HSolveActive::advanceCalcium
  - This method pushes forward the caActivation values (total calcium current flowing into each pool) by adding up the contributions from each channel feeding the respective pools.
  - It also updates the calcium conentration in each pool depending upon the calcium current so calculated.
- HSolveActive::updateMatrix
  - This method supersedes HSolvePassive::updateMatrix.
  - Changes in the channel conductances only affect the diagonal values of the Hines matrix (because the channel conductances connect Vm to ground). updateMatrix computes the new values of the diagonal parameters as well as the modified values of the "B" vector - the vector against which the Hines matrix is being inverted.
  - The values of inject are also modified, since injectVarying (obtained via a message) could have changed.
  - Finally, external currents, from channels not handled by HSolve, are added to the "B" vector part of HS_.




*/
