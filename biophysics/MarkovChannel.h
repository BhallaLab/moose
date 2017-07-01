#ifndef _MARKOVCHANNEL_H
#define _MARKOVCHANNEL_H

//This class deals with ion channels which can be found in one of multiple
//states, some of which are conducting. This implementation assumes the
//occurence of first order kinetics to calculate the probabilities of the
//channel of being found in all states. Further, the rates of transition
//between these states can be constant, voltage-dependent, ligand dependent
//(only one ligand species) or both. The current flow obtained from the channel
//is calculated in a deterministic method by solving the system of
//differential equations obtained from the assumptions above.
//The implicit assumption is that there are a number of ion channels present in
//the system. */

class MarkovChannel : public ChanCommon
{
	public:
	//Default constructor. Use is not recommended as most of the class members
	//cannot be initialized.
	MarkovChannel();

	//Constructor to be used when number of states and number of open states are
	//known. Use of this constructor is recommended as all the other members of
	//the class can be initialized with the information provided.
	static const Cinfo* initCinfo();

	MarkovChannel( unsigned int, unsigned int);
	~MarkovChannel( );

	double getVm( ) const;
	void setVm( double );

	double getLigandConc( ) const;
	void setLigandConc( double );

	unsigned int getNumStates( ) const;
	void setNumStates( unsigned int );

	unsigned int getNumOpenStates( ) const;
	void setNumOpenStates( unsigned int );

	vector< string > getStateLabels( ) const;
	void setStateLabels( vector< string > );

	//If the (i,j)'th is true, ligand concentration will be used instead of
	//voltage.
	vector< vector< bool > > getLigandGated( ) const;
	void setLigandGated ( vector< vector< bool > > );

	//Probabilities of the channel occupying all possible states.
	vector< double > getState ( ) const;
	void setState(  vector< double >  );

	//The initial state of the channel. State of the channel is reset to this
	//vector during a call to reinit().
	vector< double > getInitialState() const;
	void setInitialState( vector< double > );

	//Conductances associated with each open/conducting state.
	vector< double > getGbars( ) const;
	void setGbars( vector< double > );

	//////////////////////
	//MsgDest functions
	/////////////////////

	void vProcess( const Eref&, const ProcPtr);
	void vReinit( const Eref&, const ProcPtr);
	void handleLigandConc( double );
	void handleState( vector< double > );

	private:
	double g_;												//Expected conductance of the channel.
	double ligandConc_;								//Ligand concentration.
	unsigned int numStates_;					//Total number of states.
	unsigned int numOpenStates_;			//Number of open (conducting) states.

	vector< string > stateLabels_;
	vector< double > state_;					//Probabilities of occupancy of each state.
	vector< double > initialState_;
	vector< double > Gbars_;		//Conductance associated with each open state.
};

#endif
