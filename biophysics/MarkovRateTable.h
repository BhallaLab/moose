/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MARKOVRATETABLE_H
#define _MARKOVRATETABLE_H

////////////////////////////////////////////////////////
//Class : MarkovRateTable
//Author : Vishaka Datta S, 2011, NCBS
//
//Presents a unified interface to deal with transition rates that are dependent
//on one or two parameters, or are constant.
//Ther Interpol2D class is used for carrying out 2-D rate lookups. The one
//parameter lookup functions are based on the lookup tables used in the HHGate
//class.
//
//The class consists of a table of pointers to the VectorTable class and the
//Interpol2D class. Since these classes are user-defined and possess
//constructors, destructors, etc., using a union is not possible (but would've
//been more efficient).
//
//For a given rate entry (i,j) either one of these pointers may be set, but not
//both. If both pointers are null, the rate is assumed to be zero i.e. there
//is no transition from state i to state j.
//
//In case a rate is constant, it is stored in a VectorTable class whose lookup
//table is of size 1.
//
/////////////////////////////////////////////////////

template <class T>
vector< vector< T > > resize( vector< vector< T > >table, unsigned int n, T init )
{
	table.resize( n );

	for ( unsigned int i = 0; i < n; ++i )
		table[i].resize( n, init );

	return table;
}

class MarkovRateTable {
	public :
	MarkovRateTable();

	~MarkovRateTable();

	//One parameter rate table set and get functions.
	VectorTable* getVtChildTable( unsigned int, unsigned int ) const;
	void innerSetVtChildTable( unsigned int, unsigned int, VectorTable, unsigned int );
	//Two parameter rate table set and get functions.
	Interpol2D* getInt2dChildTable( unsigned int, unsigned int ) const;
	void innerSetInt2dChildTable( unsigned int, unsigned int, Interpol2D );

	//Lookup functions.
	double lookup1dValue( unsigned int, unsigned int, double );
	double lookup1dIndex( unsigned int, unsigned int, unsigned int );
	double lookup2dValue( unsigned int, unsigned int, double, double );
	double lookup2dIndex( unsigned int, unsigned int, unsigned int, unsigned int );

	//Rate update and initialization functions.
	void updateRates();
	void initConstantRates();

	vector< unsigned int > getListOf1dRates();
	vector< unsigned int > getListOf2dRates();
	vector< unsigned int > getListOfLigandRates();
	vector< unsigned int > getListOfVoltageRates();
	vector< unsigned int > getListOfConstantRates();

	//Overloading the >> operator. This is done so that VectorTable and
	//Interpol2D classes can be used to declare OpFuncs.
	friend istream& operator>>( istream&, MarkovRateTable& );

	/////////////////////////
	//*ValueFinfo related functions.
	////////////////////////
	vector< vector< double > > getQ() const;
	unsigned int getSize() const;

	double getVm( ) const;
	void setVm( double );

	double getLigandConc( ) const;
	void setLigandConc( double );

	//////////////////
	//Helper functions
	/////////////////

	//Returns true if the (i,j)'th rate is zero i.e. no transitions between states
	//i and j. When this is the case, both the  (i,j)'th pointers in vtTables_
	// and int2dTables_ are not set i.e. both are zero.
	bool isRateZero( unsigned int, unsigned int ) const ;

	//Returns true if the (i,j)'th rate is a constant. This is true when the
	//vtTables_[i][j] pointer points to a 1D lookup table of size 1.
	bool isRateConstant( unsigned int, unsigned int ) const ;

	//Returns true if the (i,j)'th rate is varies with only one parameter.
	bool isRate1d( unsigned int, unsigned int ) const ;

	//Returns true if the (i,j)'th rate is dependent on ligand concentration and
	//not membrane voltage, and is set with a 1D lookup table.
	bool isRateLigandDep( unsigned int, unsigned int) const;

	//Returns true if the (i,j)'th rate varies with two parameters.
	bool isRate2d( unsigned int, unsigned int ) const ;

	//Returns true if a table is being accessed at an invalid address.
	bool areIndicesOutOfBounds( unsigned int, unsigned int ) const ;

	///////
	//These functions return general information about the whole rate table
	//itself. Useful for the MarkovSolver class.
	//////

	//Returns true if all the rates in the transition matrix are constant.
	bool areAllRatesConstant();

	//Returns true if at least one of the rates are 1D i.e. vary with only
	//one parameter.
	bool areAnyRates1d();
	bool areAllRates1d();

	//Returns true if at least one of the rates are 2D i.e. vary with two
	//parameters.
	bool areAnyRates2d();
	bool areAllRates2d();

	//Returns true if at least one of the rates are ligand dependent.
	bool areAnyRatesLigandDep();
	bool areAllRatesLigandDep();

	//Returns true if at least one of the rates are voltage dependent.
	bool areAnyRatesVoltageDep();
	bool areAllRatesVoltageDep();

	static const Cinfo* initCinfo();

	/////////////////////
	//MsgDest functions.
	/////////////////////
	void process( const Eref&, ProcPtr );
	void reinit( const Eref&, ProcPtr );

	//Initialization of object.
	void init( unsigned int );

	//Handling incoming messages containing voltage and concentration information.
	void handleVm( double );
	void handleLigandConc( double );

	//Setting rate tables for 1-D and 2-D rate transitions.
	void setVtChildTable( unsigned int, unsigned int, Id, unsigned int );
	void setInt2dChildTable( unsigned int, unsigned int, Id );

	//Setting a constant rate.
	void setConstantRate( unsigned int, unsigned int, double );

	private :
	//Family of 1D lookup table pointers. Each entry is a pointer to a 1-D
	//lookup table (VectorTable type) that contains the values of the rates of
	//transition at a given voltage or ligand concentration.
	vector< vector< VectorTable* > > vtTables_;

	//Family of 2D lookup table pointers. Each entry is a pointer to a 2-D
	//lookup table of type Interpol2D. These lookup tables deal with rates that
	//vary with voltage and ligand concentration.
	vector< vector< Interpol2D* > > int2dTables_;

	//In the case of 1-D lookup tables, this matrix determines which rates are
	//ligand dependent. When the (i,j)'th value is "true", the ligand
	//concentration is used instead of voltage for rate lookups.
	vector< vector< unsigned int > > useLigandConc_;

	//Maintains a list of the indices of 1D,2D,constant rates, voltage and
	//ligand dependent rates. Makes updating rates a whole lot easier and
	//more elegant than blindly scanning through the entire rate matrix
	//each time.
	vector< unsigned int > listOf1dRates_;
	vector< unsigned int > listOf2dRates_;
	vector< unsigned int > listOfConstantRates_;
	vector< unsigned int > listOfLigandRates_;
	vector< unsigned int > listOfVoltageRates_;

	//The instantaneous rate matrix.
	vector< vector< double > > Q_;

	//Membrane voltage and ligand concentration. Needed for rate lookups.
	//Admittedly, this is a bit of a deviation from the usual scheme where
	//channels are the only destinations for Vm and conc.
	double Vm_;
	double ligandConc_;

	//Time step of simulation. The rates are specified in sec^(-1). All the rates
	//have to be proportionally scaled for an arbitrary time step.
	//Is not a MOOSE field, and does not need an explicit setup call either.
	//It is set during the call to reinit.

	unsigned int size_;

	//Check to see if the table has been initialized.
	bool isInitialized() const;
};
#endif
