#ifndef _Adaptor_h
#define _Adaptor_h

/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This is the adaptor class. It is used in interfacing different kinds
 * of solver with each other, especially for electrical to chemical
 * signeur models.
 * The Adaptor class is the core of the API for interfacing between
 * different solution engines. It is currently in use for interfacing
 * between chemical and electrical simulations, but could be used for other
 * cases such as mechanical models.
 *
 *
 * The API for interfacing between solution engines rests on
 * the following capabilities of MOOSE.
 * 1. The object-oriented interface with classes mapped to biological
 * and modeling concepts such as electrical and chemical compartments,
 * ion channels and molecular pools.
 * 2. The invisible mapping of Solvers (Objects implementing numerical
 * engines) to the object-oriented interface. Solvers work behind the
 * scenes to update the objects.
 * 3. The messaging interface which allows any visible field to be
 * accessed and updated from any other object.
 * 4. The clock-based scheduler which drives operations of any subset of
 * objects at any interval. For example, this permits the operations of
 * field access and update to take place at quite different timescales
 * from the numerical engines.
 * 5. The implementation of Adaptor classes. These perform a linear
 * transformation::
 *
 * 	(y = scale * (x + inputOffset) + outputOffset )
 *
 * where y is output and x is the input. The input is the average of
 * any number of sources (through messages) and any number of timesteps.
 * The output goes to any number of targets, again through messages.
 *
 * It is worth adding that messages can transport arbitrary data structures,
 * so it would also be possible to devise a complicated opaque message
 * directly between solvers. The implementation of Adaptors working on
 * visible fields does this much more transparently and gives the user
 * facile control over the scaling transformatoin.
 *
 * These adaptors are used especially in the rdesigneur framework of MOOSE,
 * which enables multiscale reaction-diffusion and electrical signaling
 * models.
 * As an example of this API in operation, I consider two mappings:
 * 1. Calcium mapped from electrical to chemical computations.
 * 2. phosphorylation state of a channel mapped to the channel conductance.
 *
 * 1. Calcium mapping.
 * Problem statement.
 * Calcium is computed in the electrical solver as one or more pools that
 * are fed by calcium currents, and is removed by an exponential
 * decay process. This calcium pool is non-diffusive in the current
 * electrical solver. It has to be mapped to chemical calcium pools at a
 * different spatial discretization, which do diffuse.
 * In terms of the list of capabilities described above, this is how the
 * API works.
 * 	1. The electrical model is partitioned into a number of electrical
 * 		compartments, some of which have the 'electrical' calcium pool
 * 		as child object in a UNIX filesystem-like tree. Thus the
 * 		'electrical' calcium is represented as an object with
 * 		concentration, location and so on.
 * 	2. The Solver computes the time-course of evolution of the calcium
 * 		concentration. Whenever any function queries the 'concentration'
 * 		field of the calcium object, the Solver provides this value.
 *  3. Messaging couples the 'electrical' calcium pool concentration to
 *  	the adaptor (see point 5). This can either be a 'push' operation,
 *  	where the solver pushes out the calcium value at its internal
 *  	update rate, or a 'pull' operation where the adaptor requests
 *  	the calcium concentration.
 *  4. The clock-based scheduler keeps the electrical and chemical solvers
 *  	ticking away, but it also can drive the operations of the adaptor.
 *  	Thus the rate of updates to and from the adaptor can be controlled.
 *  5. The adaptor averages its inputs. Say the electrical solver is
 *  	going at a timestep of 50 usec, and the chemical solver at 5000
 *  	usec. The adaptor will take 100 samples of the electrical
 *  	concentration, and average them to compute the 'input' to the
 *  	linear scaling. Suppose that the electrical model has calcium units
 *  	of micromolar, but has a zero baseline. The chemical model has
 *  	units of millimolar and a baseline of 1e-4 millimolar. This gives:
 *  	y = 0.001x + 1e-4
 *  	At the end of this calculation, the adaptor will typically 'push'
 *  	its output to the chemical solver. Here we have similar situation
 *  	to item (1), where the chemical entities are calcium pools in
 *  	space, each with their own calcium concentration.
 *  	The messaging (3) determines another aspect of the mapping here:
 *  	the fan in or fan out. In this case, a single electrical
 *  	compartment may house 10 chemical compartments. Then the output
 *  	message from the adaptor goes to update the calcium pool
 *  	concentration on the appropriate 10 objects representing calcium
 *  	in each of the compartments.
 *
 * In much the same manner, the phosphorylation state can regulate
 * channel properties.
 *  1. The chemical model contains spatially distributed chemical pools
 *  	that represent the unphosphorylated state of the channel, which in
 *  	this example is the conducting form. This concentration of this
 *  	unphosphorylated state is affected by the various reaction-
 *  	diffusion events handled by the chemical solver, below.
 *  2. The chemical solver updates the concentrations
 *  	of the pool objects as per reaction-diffusion calculations.
 *  3. Messaging couples these concentration terms to the adaptor. In this
 *  	case we have many chemical pool objects for every electrical
 *  	compartment. There would be a single adaptor for each electrical
 *  	compartment, and it would average all the input values for calcium
 *  	concentration, one for each mesh point in the chemical calculation.
 *  	As before, the access to these fields could be through a 'push'
 *  	or a 'pull' operation.
 *  4. The clock-based scheduler oeperates as above.
 *  5. The adaptor averages the spatially distributed inputs from calcium,
 *  	and now does a different linear transform. In this case it converts
 *  	chemical concentration into the channel conductance. As before,
 *  	the 'electrical' channel is an object (point 1) with a field for
 *  	conductance, and this term is mapped into the internal data
 *  	structures of the solver (point 2) invisibly to the user.
 *
 *
 *
 */
class Adaptor
{
	public:
		Adaptor();

		////////////////////////////////////////////////////////////
		// Here are the interface functions for the MOOSE class
		////////////////////////////////////////////////////////////
		void setInputOffset( double offset );
		double getInputOffset() const;
		void setOutputOffset( double offset );
		double getOutputOffset() const;
		void setScale( double scale );
		double getScale() const;
		double getOutput() const;

		////////////////////////////////////////////////////////////
		// Here are the Destination functions
		////////////////////////////////////////////////////////////
		void input( double val );
		// void handleBufInput( PrepackedBuffer pb );
		void innerProcess();
		void process( const Eref& e, ProcPtr p );
		void reinit( const Eref& e, ProcPtr p );
		/*
		static void setup( const Eref& e, const Qinfo* q,
			string molName, double scale,
			double inputOffset, double outputOffset );
		static void build( const Eref& e, const Qinfo* q);
		*/

		////////////////////////////////////////////////////////////
		static const Cinfo* initCinfo();

	private:
		double output_;
		double inputOffset_;
		double outputOffset_;
		double scale_;
		string molName_; /// Used for placeholding in cellreader mode.
		double sum_;
		unsigned int counter_; /// Counts number of inputs received.

		/// Counts number of targets of requestField message
		unsigned int numRequestOut_;
};

#endif // _Adaptor_h
