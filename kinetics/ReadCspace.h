/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
**           copyright (C) 2003-2004 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _READCSPACE_H
#define _READCSPACE_H

class CspaceMolInfo
{
	public:
		CspaceMolInfo( char name, double conc )
			: name_( name ), conc_( conc )
		{
			;
		}
		CspaceMolInfo( )
			: name_( 'a' ), conc_( 0.0 )
		{
			;
		}

		bool operator<( const CspaceMolInfo& other ) const
		{
			return ( name_ < other.name_ );
		}

		double conc() {
			return conc_;
		}
	private:
		char name_;
		double conc_;
};

class CspaceReacInfo
{
	public:
		CspaceReacInfo( const string& name, double r1, double r2 )
			: name_( name ), r1_( r1 ), r2_( r2 )
		{
			;
		}
		CspaceReacInfo( )
			: name_( "" ), r1_( 0.1 ), r2_( 0.1 )
		{
			;
		}

		bool operator<( const CspaceReacInfo& other ) const
		{
			return ( name_ < other.name_ );
		}

		string name() {
			return name_;
		}
		double r1() {
			return r1_;
		}
		double r2() {
			return r2_;
		}
	private:
		string name_;
		double r1_;
		double r2_;
};

class ReadCspace
{
	public:
		ReadCspace();

		void printHeader();
		void printFooter();


		void printMol(Id id, double conc, double concinit, double vol);

		void printReac( Id id, double kf, double kb);

		void printEnz( Id id, Id cplx, double k1, double k2, double k3);

		Id readModelString( const string& model,
			const string& modelname, Id pa, const string& solverClass );
		// void setupGslRun( double plotdt );
		void makePlots( double plotdt );

		void build( const char* name );
		void expandEnzyme(
			const char* name, int e, int s, int p, int p2 = 0);
		void expandReaction( const char* name, int nm1 );

		void deployParameters();
		void testReadModel( );

		void makeMolecule( char name );

	private:
		static const double SCALE;
		static const double DEFAULT_CONC;
		static const double DEFAULT_RATE;
		static const double DEFAULT_KM;
		static const bool USE_PIPE;

		Id base_; // manager of model tree. A SimManager
		Id compt_; // compartment holding model tree.
		Id mesh_; // mesh entry below compartment
		ostream* fout_;

		// All the molecules, in alphabetical order.
		vector< Id > mol_;
		vector< unsigned int > molseq_;
		// Just a list of reactions and enzymes, in order of occurrence
		vector< Id > reac_;
		// All the model parameters. First are the mol concs, then rates
		vector< double > parms_;
		// Temporary storage for default molecular concs.
		vector< double > molparms_;

		vector< CspaceMolInfo > mollist_;
		vector< CspaceReacInfo > reaclist_;
};

#endif // _READCSPACE_H
