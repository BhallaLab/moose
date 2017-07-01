#ifndef _HHGate_h
#define _HHGate_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This class handles a single gate on an HHChannel. It is equivalent to the
 * m and h terms on the Hodgkin-Huxley Na channel, or the n term on the
 * K channel. It stores the
 * voltage-dependence (sometimes concentration-dependence) of the
 * gating variables for opening the channel. It does so in a tabular form
 * which can be directly filled using experimental data points.
 * It also provides a set of
 * utility functions for defining the gate in functional forms, and
 * accessing those original functional forms.
 * The HHGate is
 * accessed as a FieldElement, which means that it is available as a
 * pointer on the HHChannel. HHGates are typically shared. This means that
 * when you make a copy or a vector of an HHChannel, there is only a single
 * HHGate created, and its pointer is used by all the copies.
 * The lookup functions are thread-safe.
 * Field assignment to the HHGate should be possible only from the
 * original HHChannel, but all the others do have read permission.
 */
class HHGate
{
	friend void testHHGateLookup();
	friend void testHHGateSetup();
	public:
		/**
		 * Dummy constructor, to keep Dinfo happy. Should never be used
		 */
		HHGate();

		/**
		 * This constructor is the one meant to be used. It takes the
		 * originalId of the parent HHChannel as a required argument,
		 * so that any subsequent 'write' functions can be checked to
		 * see if they are legal. Also tracks its own Id.
		 */
		HHGate( Id originalChanId, Id originalGateId );

		//////////////////////////////////////////////////////////
		// LookupValueFinfos
		//////////////////////////////////////////////////////////
		/**
		 * lookupA: Look up the A vector from a double. Typically does
		 * so by direct scaling and offset to an integer lookup, using
		 * a fine enough table granularity that there is little error.
		 * Alternatively uses linear interpolation.
		 * The range of the double is predefined based on knowledge of
		 * voltage or conc ranges, and the granularity is specified by
		 * the xmin, xmax, and invDx fields.
		 */
		double lookupA( double v ) const;

		/**
		 * lookupB: Look up the B vector from a double, similar to lookupA.
		 */
		double lookupB( double v ) const;

		//////////////////////////////////////////////////////////
		// Field functions
		//////////////////////////////////////////////////////////
		void setAlpha( const Eref& e, vector< double > val);
		vector< double > getAlpha( const Eref& e) const;
		void setBeta( const Eref& e, vector< double > val );
		vector< double > getBeta( const Eref& e) const;
		void setTau( const Eref& e, vector< double > val );
		vector< double > getTau( const Eref& e) const;
		void setMinfinity( const Eref& e,
			vector< double > val );
		vector< double > getMinfinity( const Eref& e) const;

		void setMin( const Eref& e, double val );
		double getMin( const Eref& e) const;
		void setMax( const Eref& e, double val );
		double getMax( const Eref& e) const;
		void setDivs( const Eref& e, unsigned int val );
		unsigned int getDivs( const Eref& e) const;

		void setTableA( const Eref& e, vector< double > v);
		vector< double > getTableA( const Eref& e) const;

		void setTableB( const Eref& e, vector< double > v);
		vector< double > getTableB( const Eref& e) const;

		void setUseInterpolation( const Eref& e, bool val );
		bool getUseInterpolation( const Eref& e) const;

		void setupAlpha( const Eref& e, vector< double > parms );
		vector< double > getAlphaParms( const Eref& e ) const;

		//////////////////////////////////////////////////////////
		// DestFinfos
		//////////////////////////////////////////////////////////
		void setupTau( const Eref& e,
			vector< double > parms );
		void tweakAlpha();
		void tweakTau();
		void setupGate( const Eref& e,
			vector< double > parms );
		void setupTables( const vector< double >& parms, bool doTau );
		void tweakTables( bool doTau );

		/**
		 * Single call to get both A and B values by lookup
		 */
		void lookupBoth( double v, double* A, double* B ) const;


		/////////////////////////////////////////////////////////////////
		// Utility funcs
		/////////////////////////////////////////////////////////////////
		/**
		 * Returns looked up value of specified table
		 */
		double lookupTable( const vector< double >& tab, double v ) const;

		/**
		 * Checks if the provided Id is the one that the HHGate was created
		 * on. If true, fine, otherwise complains about trying to set the
		 * field.
		 */
		bool checkOriginal( Id id, const string& field ) const;

		/**
		 * isOriginalChannel returns true if the provided Id is the Id of
		 * the channel on which the HHGate was created.
		 */
		bool isOriginalChannel( Id id ) const;

		/**
		 * isOriginalChannel returns true if the provided Id is the Id of
		 * the Gate created at the same time as the original channel.
		 */
		bool isOriginalGate( Id id ) const;

		/**
		 * Returns the Id of the original Channel.
		 */
		Id originalChannelId() const;

		/**
		 * Returns the Id of the original Gate.
		 */
		Id originalGateId() const;

		/**
		 * tabFill does interpolation and range resizing for
		 * a table representing a lookup function.
		 * newXdivs is one less than the size of the table; it is the number
		 * of subdivisions that the table represents.
		 * Does NOT alter the existing xmin and xmax, but it does resize
		 * the table.
		 */
		void tabFill( vector< double >& table,
			unsigned int newXdivs, double newXmin, double newXmax );

		/**
		 * Update the Tau and Minfinity parameters because the alpha or
		 * beta tables have changed.
		 */
		void updateTauMinf();

		/**
		 * Update the alpha and beta parameters because the tau or
		 * minfinity tables have changed.
		 */
		void updateAlphaBeta();

		/**
		 * Take the current alpha/beta parameters, and xdivs, xmin, xmax;
		 * and rebuild the tables.
		 */
		void updateTables();

		static const Cinfo* initCinfo();
	private:
		/// 5 parameters for alpha
		vector< double > alpha_;

		/// 5 parameters for beta
		vector< double > beta_;

		/// 5 parameters for tau
		vector< double > tau_;

		/// 5 parameters for mInfinity
		vector< double > mInfinity_;

		/// The actual lookup table for calculations. Holds alpha(V).
		vector< double > A_;

		/// The lookup table for calculations. Holds alpha(V) + beta(V).
		vector< double > B_;

		/// Minimum of the voltage (or conc) range.
		double xmin_;

		/// Max of the voltage (or conc) range.
		double xmax_;

		/// increment of the range.
		double invDx_;

		/**
		 * Id of original channel, the one which has actually allocated it,
		 * All other Elements have to treat the values as readonly.
		 */
		Id originalChanId_;

		/**
		 * Id of original Gate, the one which was created with the original
		 * channel.
		 * All other Elements have to treat the values as readonly.
		 */
		Id originalGateId_;

		/**
		 * Flag: Use linear interpolation for lookup if true, use direct
		 * table lookup if false.
		 */
		bool lookupByInterpolation_;

		/**
		 * Flag to indicate if table entries have been assigned directly
		 * (as direct experimental data, or an externally defined V-dep
		 * function). When zero it means that the curve-fit forms from
		 * setupAlpha etc. have been used.
		 */
		bool isDirectTable_;
};

#endif // _HHGate_h
