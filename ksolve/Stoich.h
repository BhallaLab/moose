/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STOICH_H
#define _STOICH_H

/**
 * Stoich is the class that handles the stoichiometry matrix for a
 * reaction system, and also setting up the computations for reaction 
 * rates. It has to coordinate the operation of a number of model 
 * definition classes, most importantly: Pools, Reacs and Enz(yme)s.
 * It also coordinates the setup of a large number of numerical solution
 * engines, or solvers: Ksolve, Gsolve, Dsolve, and SteadyState.
 * The setup process has to follow a tight order, most of which is
 * internally manged by the Stoich.
 * The Stoich itself does not do any 'process' functions. It just sets up
 * data structures for the other objects that do the crunching.
 *
 * 1. Compartment is set up to a cell (neuroMesh) or volume (other meshes)
 * 2. Compartment assigned to Stoich. Here it assigns unique vols.  
 * 3. Dsolve and Ksolve assigned to Stoich using setKsolve and setDsolve.
 * 	 3.1 At this point the Stoich::useOneWay_ flag is set if it is a Gsolve.
 * 4. Call Stoich::setPath. All the rest happens internally, done by Stoich:
 * 		4.1 assign compartment to Dsolve and Ksolve.
 * 		4.2 assign numPools and compts to Dsolve and Ksolve. 
 * 		4.3 During Zombification, zeroth vector< RateTerm* > is built.  
 * 		4.4 afterZombification, stoich builds rateTerm vector for all vols.
 * 		4.5 Stoich assigns itself to Dsolve and Ksolve.  
 * 			- Ksolve sets up volIndex on each VoxelPool 
 * 			- Dsolve gets vector of pools, extracts DiffConst and MotorConst
 * 		4.6 Dsolve assigned to Ksolve::dsolve_ field by the Stoich.
 * 	5. Reinit, 
 * 		5.1 Dsolve builds matrix, provided dt has changed. It needs dt.  
 * 		5.2 Ksolve builds solvers if not done already, assigning initDt 
 *
 * As seen by the user, this reduces to just 4 stages:
 * - Make the objects.
 * - Assign compartment, Ksolve and Dsolve to stoich.
 * - Set the stoich path.
 * - Reinit.
 */

class Stoich
{
	public: 
		Stoich();
		~Stoich();

		//////////////////////////////////////////////////////////////////
		// Field assignment stuff
		//////////////////////////////////////////////////////////////////

		void setOneWay( bool v );
		bool getOneWay() const;

		/// Returns number of local pools that are updated by solver
		unsigned int getNumVarPools() const;

		/// Returns number of local buffered pools.
		unsigned int getNumBufPools() const;

		/**
		 *  Returns total number of pools. Includes the pools whose
		 *  actual calculations happen on another solver, but are given a
		 *  proxy here in order to handle cross-compartment reactions.
		 */
		unsigned int getNumAllPools() const;

		/**
		 * Returns number of proxy pools here for
		 * cross-compartment reactions
		 */
		unsigned int getNumProxyPools() const;

		/** 
		 * Map to look up the index of the pool from its Id.  
		 * poolIndex = poolIdMap[ Id::value() - poolOffset ] 
		 * where the poolOffset is the smallest Id::value.  
		 * poolOffset is passed back as the last entry of this vector.  
		 * Any Ids that are not pools return EMPTY=~0. 
		 */
		vector< unsigned int > getPoolIdMap() const;

		/**
		 * Take the provided wildcard path to build the list of elements
		 * managed by this solver.
		 */
		void setPath( const Eref& e, string path );
		string getPath( const Eref& e ) const;

		/// assigns kinetic solver: Ksovle or GSSAsolve.
		void setKsolve( Id v ); 
		Id getKsolve() const;

		/// assigns diffusion solver: Dsovle or a Gillespie voxel stepper
		void setDsolve( Id v ); 
		Id getDsolve() const;

		/// assigns compartment occupied by Stoich.
		void setCompartment( Id v ); 
		Id getCompartment() const;

		/**
		 * Utility function to return # of rates_ entries. This includes
		 * the cross-solver reactions.
		 */
		unsigned int getNumRates() const;

		/**
		 * Utility function to return # of core rates for reacs which are
		 * entirely located on current compartment, including all reactants
		 */
		unsigned int getNumCoreRates() const;

		/// Utility function to return a rates_ entry
		const RateTerm* rates( unsigned int i ) const;

		/// Returns a reference to the entire rates_ vector.
		const vector< RateTerm* >& getRateTerms() const;

		unsigned int getNumFuncs() const;
		const FuncTerm* funcs( unsigned int i ) const;

		vector< int > getMatrixEntry() const;
		vector< unsigned int > getColIndex() const;
		vector< unsigned int > getRowStart() const;

		vector< Id > getProxyPools( Id i ) const;

		/**
		 * getStatus(): Flag to track status of Stoich object.
		 * -1: No path yet assigned.
		 *  0: Success
		 *  1: Warning: Missing reactant in Reac or Enz
		 *  2: Warning: Missing substrate in MMenz
		 *  4: Warning: Compartment not defined
		 *  8: Warning: Neither Ksolve nor Dsolve defined
		 *  16: Warning: No objects found on path
		 */
		int getStatus() const;
		//////////////////////////////////////////////////////////////////
		// Model traversal and building functions
		//////////////////////////////////////////////////////////////////
		/**
		 * Internal function which sets up the model based on the provided
		 * elist of all elements managed by this solver.
		 */
		void setElist( const Eref& e, const vector< ObjId >& elist );

		/**
		 * Scans through elist to find any reactions that connect to
		 * pools not located on solver. Removes these reactions from the
		 * elist and maintains Ids of the affected reactions, and their 
		 * off-solver pools, in offSolverReacs_ and offSolverPools_.
		 */
		void locateOffSolverReacs( Id myCompt, vector< Id >& elist );
	
		/**
		 * Builds the objMap vector, which maps all Ids to 
		 * the internal indices for pools and reacs that are used in the
		 * solver. In addition to the elist, it also scans through the
		 * offSolverPools and offSolverReacs to build the map.
		void allocateObjMap( const vector< Id >& elist );
		 */

		/// Using the computed array sizes, now allocate space for them.
		void resizeArrays();
		/// Identifies and allocates objects in the Stoich.
		void allocateModelObject( Id id );
		/// Calculate sizes of all arrays, and allocate them.
		void allocateModel( const vector< Id >& elist );

		/// Functions to build the maps between Ids and internal indices
		void buildPoolLookup();
		void buildRateTermLookup();
		void buildFuncLookup();

		/**
		 * This function is used when the stoich class is employed by a 
		 * Gsolver for doing stochastic calculations.
		 * Here we fix the issue of having a single substrate at
		 * more than first order. As the first molecule of the substrate is
		 * consumed, the number is depleted by one and so its forward rate 
		 * is reduced. And so on. This also protects against going negative
		 * in mol number or concentration.
		 */
		void convertRatesToStochasticForm();

		/**
		 * Builds cross-reaction terms between current stoich and 
		 * otherStoich. The function scans the voxels at which there
		 * are jucntions between different compartments, and orchestrates
		 * setup of interfaces between the Ksolves that implement the
		 * cross-reactions at these junctions.
		 */
		void buildXreacs( const Eref& e, Id otherStoich );

		void filterXreacs();

		/// Used to handle run-time size updates for spines.
		void scaleBufsAndRates( unsigned int index, double volScale );

		/**
		 * Expands out list of compartment mappings of proxy reactions to
		 * the appropriate entries on the rates_vector.
		void comptsOnCrossReacTerms( vector< pair< Id, Id > >& xr ) const;
		 */

		/**
		 * Used to set up and update all cross solver reac terms and
		 * their rates, if there has been a change in volumes. Should
		 * also work if there has been a change in voxelization.
		 */
		void setupCrossSolverReacVols() const;

		/**
		 * Finds all the input molecules contributing to any of the
		 * Function cases: poolFunc, incrementFunc or reacFunc
		void inputsToPoolFuncs( 
				vector< pair< Id, vector< unsigned int > > >& ret ) const;
		 */
		//////////////////////////////////////////////////////////////////
		// Zombification functions.
		//////////////////////////////////////////////////////////////////

		/**
		 * zombifyModel marches through the specified id list and 
		 * converts all entries into zombies. The first arg e is the
		 * Eref of the Stoich itself.
		 */
		void zombifyModel( const Eref& e, const vector< Id >& elist );

		/**
		 * Converts back to ExpEuler type basic kinetic Elements.
		 */
		void unZombifyModel();

		/// unZombifies Pools. Helper for unZombifyModel.
		void unZombifyPools();

		void zombifyChemCompt( Id compt );

		/**
		 * Utility function to find if incoming message assigns N or conc,
		 * and to appropriately zombify the function and set up its 
		 * parameters including volume scaling.
		 */
		Id zombifyPoolFuncWithScaling( Id pool );

		unsigned int convertIdToReacIndex( Id id ) const;
		unsigned int convertIdToPoolIndex( Id id ) const;
		unsigned int convertIdToFuncIndex( Id id ) const;

		/// Utility function to make a half reac and return the rate term.
		ZeroOrder* makeHalfReaction( 
						double rate, const vector< Id >& reactants );

		/*
		 * This takes the specified Reac and its substrate and product
		 * list, and installs them into the Stoich. It also builds up the
		 * vectors to store which compartment each substrate/product 
		 * belongs to, needed for cross-reaction computations.
		 * This is the high-level interface function.
		 */
		void installReaction( Id reacId,
				const vector< Id >& subs, const vector< Id >& prds );
		/*
		 * This takes the specified subs and prds belonging
		 * to the specified Reac, and builds them into the Stoich.
		 * It is a low-level function used internally.
		 */
		unsigned int innerInstallReaction( Id reacId, 
				const vector< Id >& subs, const vector< Id >& prds );

		/**
		 * This takes the baseclass for an MMEnzyme and builds the
		 * MMenz into the Stoich.
		 */
		void installMMenz( Id enzId, const vector< Id >& enzMolId,
			const vector< Id >& subs, const vector< Id >& prds );
		/**
		 * This is the inner function to do the installation.
		 */
		void installMMenz( MMEnzymeBase* meb, unsigned int rateIndex,
			const vector< Id >& subs, const vector< Id >& prds );

		/*
		 * This takes the specified Reac and its substrate and product
		 * list, and installs them into the Stoich. This is the high-level
		 * interface function.
		 */
		void installEnzyme( Id enzId, Id enzMolId, Id cplxId,
				const vector< Id >& subs, const vector< Id >& prds );
		/**
		 * This takes the forward, backward and product formation half-reacs
		 * belonging to the specified Enzyme, and builds them into the
		 * Stoich. This is the low-level function.
		 */
		void installEnzyme( ZeroOrder* r1, ZeroOrder* r2, ZeroOrder* r3,
			Id enzId, Id enzMolId, const vector< Id >& prds );

		/// This is used when the enzyme lacks sub or prd.
		void installDummyEnzyme( Id enzId, Id enzMolId);

		/**
		 * This installs a FuncTerm, which evaluates a function to specify
		 * the conc of the specific pool. The pool is a BufPool.
		 */
		void installAndUnschedFunc( Id func, Id pool, double volScale );

		/**
		 * This installs a FuncRate, which evaluates a function to specify
		 * the rate of change of conc of the specific pool. 
		 * The pool is a Pool.
		 */
		void installAndUnschedFuncRate( Id func, Id pool );

		/**
		 * This installs a FuncReac, which evaluates a function to specify
		 * the rate (Kf) of the specified reaction. 
		 */
		void installAndUnschedFuncReac( Id func, Id reac );

		//////////////////////////////////////////////////////////////////

		/**
		 * Returns SpeciesId of specified pool
		 */
		unsigned int getSpecies( unsigned int poolIndex ) const;

		/**
		 * Assigns SpeciesId of specified pool
		 */
		void setSpecies( unsigned int poolIndex, unsigned int s );

		/**
		 * Sets the forward rate v (given in millimoloar concentration units)
		 * for the specified reaction throughout the compartment in which the
		 * reaction lives. Internally the stoich uses #/voxel units so this 
		 * involves querying the volume subsystem about volumes for each
		 * voxel, and scaling accordingly.
		 */
		void setReacKf( const Eref& e, double v ) const;

		/**
		 * Sets the reverse rate v (given in millimoloar concentration units)
		 * for the specified reaction throughout the compartment in which the
		 * reaction lives. Internally the stoich uses #/voxel units so this 
		 * involves querying the volume subsystem about volumes for each
		 * voxel, and scaling accordingly.
		 */
		void setReacKb( const Eref& e, double v ) const;

		/**
		 * Sets the Km for MMenz, using appropriate volume conversion to
		 * go from the argument (in millimolar) to #/voxel.
		 * This may do the assignment among many voxels containing the enz
		 * in case there are different volumes.
		 */
		void setMMenzKm( const Eref& e, double v ) const;
		double getMMenzNumKm( const Eref& e ) const;

		/**
		 * Sets the kcat for MMenz. No conversions needed.
		 */
		void setMMenzKcat( const Eref& e, double v ) const;
		double getMMenzKcat( const Eref& e ) const;

		/**
		 * Sets the rate v (given in millimoloar concentration units)
		 * for the forward enzyme reaction of binding substrate to enzyme.
		 * Does this throughout the compartment in which the
		 * enzyme lives. Internally the stoich uses #/voxel units so this 
		 * involves querying the volume subsystem about volumes for each
		 * voxel, and scaling accordingly.
		 */
		void setEnzK1( const Eref& e, double v ) const;
		double getEnzNumK1( const Eref& e ) const;

		/// Set rate k2 (1/sec) for enzyme
		void setEnzK2( const Eref& e, double v ) const;
		/// Get rate k2 (1/sec) for enzyme
		double getEnzK2( const Eref& e ) const;

		/// Set rate k3 (1/sec) for enzyme
		void setEnzK3( const Eref& e, double v ) const;
		/// Get rate k3, aka kcat, for enzyme
		double getEnzK3( const Eref& e ) const;

		/**
		 * Returns the internal rate in #/voxel, for R1, for the specified
		 * Eref.
		 */
		double getR1( const Eref& e ) const;
		/**
		 * Returns internal rate R1 in #/voxel, for the rate term 
		 * following the one directly referred to by the Eref e. Enzymes
		 * define multiple successive rate terms, so we look up the first,
		 * and then select one after it.
		 */
		double getR1offset1( const Eref& e ) const;
		/**
		 * Returns internal rate R1 in #/voxel, for the rate term 
		 * two after the one directly referred to by the Eref e. Enzymes
		 * define multiple successive rate terms, so we look up the first,
		 * and then select the second one after it.
		 */
		double getR1offset2( const Eref& e ) const;

		/**
		 * Returns the internal rate in #/voxel, for R2, for the specified
		 * reacIndex and voxel index. In some cases R2 is undefined, and it
		 * then returns 0.
		 */
		double getR2( const Eref& e ) const;

		/**
		 * Sets the arithmetic expression used in a FuncRate or FuncReac
		 */
		void setFunctionExpr( const Eref& e, string expr );

		/**
		 * This function scans all reacs and enzymes and recalculates the
		 * internal rate terms depending on the reference terms stored in
		 * the original chemical object.
		 */
		void updateRatesAfterRemesh();

		/// Utility function, prints out N_, used for debugging
		void print() const;

		/// Another utility function, prints out all Kf, kf, Kb, kb.
		void printRates() const;
		//////////////////////////////////////////////////////////////////
		// Utility funcs for numeric calculations
		//////////////////////////////////////////////////////////////////

		/**
		 * Updates the yprime array, rate of change of each molecule
		 * The volIndex specifies which set of rates to use, since the
		 * rates are volume dependent.
		 * Moved to VoxelPools
		void updateRates( const double* s, double* yprime,
					   unsigned int volIndex ) const;
		 */
		
		/**
		 * Computes the velocity of each reaction, vel.
		 * The volIndex specifies which set of rates to use, since the
		 * rates are volume dependent.
		 */
		void updateReacVelocities( const double* s, vector< double >& vel,
					   unsigned int volIndex ) const;

		/// Updates the function values, within s.
		void updateFuncs( double* s, double t ) const;

		/// Updates the rates for cross-compartment reactions.
		/*
		void updateJunctionRates( const double* s,
			   const vector< unsigned int >& reacTerms, double* yprime );
			   */

		/** 
		 * Get the rate for a single reaction specified by r, as per all 
		 * the mol numbers in s.
		double getReacVelocity( unsigned int r, const double* s,
					   unsigned int volIndex ) const;
		 */

		/// Returns the stoich matrix. Used by gsolve.
		const KinSparseMatrix& getStoichiometryMatrix() const;
		//////////////////////////////////////////////////////////////////
		// Access functions for cross-node reactions.
		//////////////////////////////////////////////////////////////////
		const vector< Id >& getOffSolverPools() const;

		/**
		 * List all the compartments to which the current compt is coupled,
		 * by cross-compartment reactions. Self not included.
		 */
		vector< Id > getOffSolverCompts() const;

		/**
		 * Looks up the vector of pool Ids originating from the specified
		 * compartment, that are represented on this compartment as proxies.
		 */
		const vector< Id >& offSolverPoolMap( Id compt ) const;

		/**
		 * Returns the index of the matching volume,
		 * which is also the index into the RateTerm vector.
		unsigned int indexOfMatchingVolume( double vol ) const;
		 */
		
		//////////////////////////////////////////////////////////////////
		static const unsigned int PoolIsNotOnSolver;
		static const Cinfo* initCinfo();
	private:
		/**
		 * True if the stoich matrix is set up to handle only one-way 
		 * reactions, as is needed in the case of the Gillespie algorithm.
		 */
		bool useOneWay_;
		string path_;

		/// This contains the Id of the Kinetic solver.
		Id ksolve_;

		/// This contains the Id of the Diffusion solver. Optional.
		Id dsolve_;

		/// Contains Id of compartment holding reac system. Optional
		Id compartment_;

		/// Pointer for ksolve
		ZombiePoolInterface* kinterface_;
		/// Pointer for dsolve
		ZombiePoolInterface* dinterface_;

		/**
		 * Lookup from each molecule to its Species identifer
		 * This will eventually be tied into an ontology reference.
		 */
		vector< unsigned int > species_;

		/**
		 * The RateTerms handle the update operations for reaction rate v_.
		 * This is the master vector of RateTerms, and it is scaled to
		 * a volume such that the 'n' units and the conc units are
		 * identical.
		 * Duplicates of this vector are made in each voxel with a different
		 * volume. The duplicates have rates scaled as per volume.
		 * The RateTerms vector also includes reactions that have 
		 * off-compartment products. These need special volume scaling 
		 * involving all the interactiong compartments.
		 */
		vector< RateTerm* > rates_;

		/**
		 * This tracks the unique volumes handled by the reac system.
		 * Maps one-to-one with the vector of vector of RateTerms.
		vector< double > uniqueVols_;
		 */

		/// Number of voxels in reac system.
		unsigned int numVoxels_;

		/// The FuncTerms handle mathematical ops on mol levels.
		vector< FuncTerm* > funcs_;

		/// N_ is the stoichiometry matrix. All pools * all reac terms.
		KinSparseMatrix N_;

		/**
		 * Maps Ids to objects in the S_, RateTerm, and FuncTerm vectors.
		 * There will be holes in this map, but look up is very fast.
		 * The calling Id must know what it wants to find: all it
		 * gets back is an integer.
		 * The alternative is to have multiple maps, but that is slower.
		 * Assume no arrays. Each Pool/reac etc must be a unique
		 * Element. Later we'll deal with diffusion.
		 */
		// vector< unsigned int > objMap_;
		/**
		 * Minor efficiency: We will usually have a set of objects that are
		 * nearly contiguous in the map. May as well start with the first of
		 * them.
		 */
		// unsigned int objMapStart_;
		//
		///////////////////////////////////////////////////////////
		// Here we have the vectors of the different kinds of objects
		// managed by the Stoich
		///////////////////////////////////////////////////////////

		/**
		 * Vector of variablePool Ids.
		 */
		vector< Id > varPoolVec_;

		/**
		 * Vector of bufPool Ids.
		 */
		vector< Id > bufPoolVec_;

		/**
		 * These are pools that were not in the original scope of the 
		 * solver, but have to be brought in because they are reactants
		 * of one or more of the offSolverReacs.
		 */
		vector< Id > offSolverPoolVec_;

		/**
		 * Vector of reaction ids.
		 */
		vector< Id > reacVec_;
		vector< Id > offSolverReacVec_;

		/**
		 * Map back from enz index to Id. Needed to unzombify
		 */
		vector< Id > enzVec_;
		vector< Id > offSolverEnzVec_;

		/**
		 * Map back from enz index to Id. Needed to unzombify
		 */
		vector< Id > mmEnzVec_;
		vector< Id > offSolverMMenzVec_;

		/**
		 * Vector of funcs controlling pool number, that is N.
		 */
		vector< Id > poolFuncVec_;
		
		/**
		 * Vector of funcs controlling pool increment, that is dN/dt
		 * This is handled as a rateTerm.
		 */
		vector< Id > incrementFuncVec_;

		/**
		 * Vector of funcs controlling reac rate, that is numKf
		 * This is handled as a rateTerm.
		 */
		vector< Id > reacFuncVec_;

		///////////////////////////////////////////////////////////////
		// Here we have maps to look up objects from their ids.
		///////////////////////////////////////////////////////////////
		map< Id, unsigned int > poolLookup_;
		map< Id, unsigned int > rateTermLookup_;
		map< Id, unsigned int > funcLookup_;

		/**
		 * Number of variable molecules that the solver deals with,
		 * that are local to the solver.
		 *
		 */
		// unsigned int numVarPools_;

		/**
		 * Number of variable molecules that the solver deals with,
		 * including the proxy molecules which belong in other compartments.
		 *
		unsigned int totVarPools_;
		 */

		/**
		 * Number of buffered molecules
		 */
		// unsigned int numBufPools_;

		/**
		 * Looks up the rate term from the Id for a reac, enzyme, or func.
		map< Id, unsigned int > rateTermLookup_;
		 */
		
		/**
		 * Number functions, currently only the ones controlling molecule
		 * numbers, like sumtotals.
		unsigned int numFunctions_;
		 */

		/**
		 * Number of reactions in the solver model. This includes 
		 * conversion reactions A + B <---> C
		 * enzyme reactions E + S <---> E.S ---> E + P
		 * and MM enzyme reactions rate = E.S.kcat / ( Km + S )
		 * The enzyme reactions count as two reaction steps.
		unsigned int numReac_;
		 */

		/**
		 * Flag to track status of Stoich object.
		 * -1: No path yet assigned.
		 *  0: Success
		 *  1: Warning: Missing reactant in Reac or Enz
		 *  2: Warning: Missing substrate in MMenz
		 *  4: Warning: Compartment not defined
		 *  8: Warning: Neither Ksolve nore Dsolve defined
		 */
		int status_;

		//////////////////////////////////////////////////////////////////
		// Off-solver stuff
		//////////////////////////////////////////////////////////////////

		/**
		 * Map of vectors of Ids of offSolver pools. 
		 * Each map entry contains the vector of Ids of proxy pools 
		 * coming from the specified compartment.
		 * In use, the junction will copy the pool indices over for 
		 * data transfer.
		 */
		map< Id, vector< Id > > offSolverPoolMap_; 

		/**
		 * Tracks the reactions that go off the current solver.
		vector< Id > offSolverReacs_;
		 */

		/**
		 * Which compartment(s) does the off solver reac connect to?
		 * Usually it is just one, in which case the second id is Id()
		 */
		vector< pair< Id, Id > > offSolverReacCompts_;
		vector< pair< Id, Id > > offSolverEnzCompts_;
		vector< pair< Id, Id > > offSolverMMenzCompts_;

		/**
		 * subComptVec_[rateTermIndex][substrate#]: Identifies compts
		 * for each substrate for each cross-compartment RateTerm in 
		 * the rates_vector.
		 */
		vector< vector< Id > > subComptVec_;

		/**
		 * prdComptVec_[rateTermIndex][product#]: Identifies compts
		 * for each product for each cross-compartment RateTerm in 
		 * the rates_vector.
		 */
		vector< vector< Id > > prdComptVec_;
};

#endif	// _STOICH_H
