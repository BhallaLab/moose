/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _NEURON_H
#define _NEURON_H

/**
 * The Neuron class to hold the Compartment class elements.
 */

class Neuron
{
	public:
		Neuron();
		Neuron( const Neuron& other );
		void setRM( double v );
		double getRM() const;
		void setRA( double v );
		double getRA() const;
		void setCM( double v );
		double getCM() const;
		void setEm( double v );
		double getEm() const;
		void setTheta( double v );
		double getTheta() const;
		void setPhi( double v );
		double getPhi() const;
		void setSourceFile( string v );
		string getSourceFile() const;
		void setCompartmentLengthInLambdas( double v );
		double getCompartmentLengthInLambdas() const;
		unsigned int getNumCompartments() const;
		unsigned int getNumBranches() const;
		vector< double> getPathDistFromSoma() const;
		vector< double> getGeomDistFromSoma() const;
		vector< double> getElecDistFromSoma() const;
		vector< ObjId > getCompartments() const;
		vector< ObjId > getExprElist( const Eref& e, string line ) const;
		vector< double > getExprVal( const Eref& e, string line ) const;
		vector< ObjId > getSpinesFromExpression(
							const Eref& e, string line ) const;
		vector< ObjId > getSpinesOnCompartment(
				const Eref& e, ObjId compt ) const;
		ObjId getParentCompartmentOfSpine( const Eref& e, ObjId compt )
				const;
		void setChannelDistribution( const Eref& e, vector< string > v );
		vector< string > getChannelDistribution( const Eref& e ) const;
		void setPassiveDistribution( const Eref& e, vector< string > v );
		vector< string > getPassiveDistribution( const Eref& e ) const;
		void setSpineDistribution( const Eref& e, vector< string > v );
		vector< string > getSpineDistribution( const Eref& e ) const;

		void buildSegmentTree( const Eref& e );
		void setSpineAndPsdMesh( Id spineMesh, Id psdMesh );
		void setSpineAndPsdDsolve( Id spineDsolve, Id psdDsolve );

		///////////////////////////////////////////////////////////////////
		// MechSpec set
		///////////////////////////////////////////////////////////////////
		void updateSegmentLengths();
		void installSpines( const vector< ObjId >& elist,
			const vector< double >& val, const vector< string >& line );
		void makeSpacingDistrib(
			const vector< ObjId >& elist, const vector< double >& val,
			vector< unsigned int >& seglistIndex,
			vector< unsigned int >& elistIndex,
			vector< double >& pos,
			const vector< string >& line ) const;
		void parseMechSpec( const Eref& e );
		void installMechanism(  const string& name,
			const vector< ObjId >& elist, const vector< double >& val,
			const vector< string >& line );
		void buildElist(
				const Eref& e,
				const vector< string >& line, vector< ObjId >& elist,
				vector< double >& val );

		void evalExprForElist( const vector< ObjId >& elist,
			const string& expn, vector< double >& val ) const;

		///////////////////////////////////////////////////////////////////
		// Interface for Spine class, used mostly in resizing spines.
		///////////////////////////////////////////////////////////////////
		Spine* lookupSpine( unsigned int index );
		void setNumSpines( unsigned int num );
		unsigned int getNumSpines() const;

		const vector< Id >& spineIds( unsigned int index ) const;
		void scaleBufAndRates( unsigned int spineNum,
				double lenScale, double diaScale ) const;
		void scaleShaftDiffusion( unsigned int spineNum,
						double len, double dia) const;
		void scaleHeadDiffusion( unsigned int spineNum,
						double len, double dia) const;

		/**
		 * Initializes the class info.
		 */
		static const Cinfo* initCinfo();
	private:
		double RM_;
		double RA_;
		double CM_;
		double Em_;
		double theta_;
		double phi_;
		double maxP_; // Maximum value of path dist from soma for this cell
		double maxG_; // Maximum value of geom dist from soma for this cell
		double maxL_; // Maximum value of elec dist from soma for this cell
		Id soma_;
		string sourceFile_;
		double compartmentLengthInLambdas_;
		vector< string > channelDistribution_;
		vector< string > passiveDistribution_;
		vector< string > spineDistribution_;

		/// Map to look up Seg index from Id of associated compt.
		map< Id, unsigned int > segIndex_;
		/// Look up seg index of parent compartment, from index of spine.
		vector< unsigned int > spineParentSegIndex_;
		vector< vector< Id > > spines_; /// Id of each compt in each spine.

		/// Ids of all spines on each compt, looked up by segIndex of compt.
		vector< vector< Id > > allSpinesPerCompt_;

		/// Id of stoich associated with each spine. Typically all the same.
		vector< Id > spineStoich_;
		/// Id of stoich associated with each PSD. Typically all the same.
		vector< Id > psdStoich_;
		/// looks up spine/psd mesh index from FieldIndex of selected spine.
		vector< unsigned int > spineToMeshOrdering_;

		Id headDsolve_; /// Id of the Dsolve for the head compt.
		Id psdDsolve_; /// Id of the Dsolve for the PSD compt.
		// looks up spine/psd Dsolve::DiffJunction::VoxelJunction index
		//from FieldIndex of selected spine.
		// Turns out this is the same as spineToMeshOrdering.
		//vector< unsigned int > spineToVoxelJunctionOrdering_;

		/// Holder for spine operations. Contains pointer to current Neuron.
		Spine spineEntry_;

		vector< Id > segId_; /// Id of compartment in each Seg entry, below.
		vector< SwcSegment > segs_;
		vector< SwcBranch > branches_;

};

//

#endif //
