/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <fstream>
#include "../utility/strutil.h"
#include "TableBase.h"

const Cinfo* TableBase::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< TableBase, vector< double > > vec(
			"vector",
			"vector with all table entries",
			&TableBase::setVec,
			&TableBase::getVec
		);

		static ReadOnlyValueFinfo< TableBase, double > outputValue(
			"outputValue",
			"Output value holding current table entry or output of a calculation",
			&TableBase::getOutputValue
		);

		static ReadOnlyValueFinfo< TableBase, unsigned int > size(
			"size",
			"size of table. Note that this is the number of x divisions +1"
			"since it must represent the largest value as well as the"
			"smallest",
			&TableBase::getVecSize
		);

		static ReadOnlyLookupValueFinfo< TableBase, unsigned int, double > y(
			"y",
			"Value of table at specified index",
			&TableBase::getY
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////

                static DestFinfo linearTransform( "linearTransform",
                        "Linearly scales and offsets data. Scale first, then offset.",
                        new OpFunc2< TableBase, double, double >( &TableBase::linearTransform ) );

		static DestFinfo xplot( "xplot",
			"Dumps table contents to xplot-format file. "
			"Argument 1 is filename, argument 2 is plotname",
			new OpFunc2< TableBase, string, string >( &TableBase::xplot ) );

		static DestFinfo plainPlot( "plainPlot",
			"Dumps table contents to single-column ascii file. "
			"Uses scientific notation. "
			"Argument 1 is filename",
			new OpFunc1< TableBase, string >( &TableBase::plainPlot ) );

		static DestFinfo loadCSV( "loadCSV",
			"Reads a single column from a CSV file. "
			"Arguments: filename, column#, starting row#, separator",
			new OpFunc4< TableBase, string, int, int, char >( 
				&TableBase::loadCSV ) );

		static DestFinfo loadXplot( "loadXplot",
			"Reads a single plot from an xplot file. "
			"Arguments: filename, plotname"
			"When the file has 2 columns, the 2nd column is loaded.",
			new OpFunc2< TableBase, string, string >( 
				&TableBase::loadXplot ) );

		static DestFinfo loadXplotRange( "loadXplotRange",
			"Reads a single plot from an xplot file, and selects a "
			"subset of points from it. "
			"Arguments: filename, plotname, startindex, endindex"
			"Uses C convention: startindex included, endindex not included."
			"When the file has 2 columns, the 2nd column is loaded.",
			new OpFunc4< TableBase, string, string, unsigned int, unsigned int >( 
				&TableBase::loadXplotRange ) );

		static DestFinfo compareXplot( "compareXplot",
			"Reads a plot from an xplot file and compares with contents of TableBase."
			"Result is put in 'output' field of table."
			"If the comparison fails (e.g., due to zero entries), the "
			"return value is -1."
			"Arguments: filename, plotname, comparison_operation"
			"Operations: rmsd (for RMSDifference), rmsr (RMSratio ), "
			"dotp (Dot product, not yet implemented).",
			new OpFunc3< TableBase, string, string, string >( 
				&TableBase::compareXplot ) );

		static DestFinfo compareVec( "compareVec",
			"Compares contents of TableBase with a vector of doubles."
			"Result is put in 'output' field of table."
			"If the comparison fails (e.g., due to zero entries), the "
			"return value is -1."
			"Arguments: Other vector, comparison_operation"
			"Operations: rmsd (for RMSDifference), rmsr (RMSratio ), "
			"dotp (Dot product, not yet implemented).",
			new OpFunc2< TableBase, vector< double >, string >(
				&TableBase::compareVec ) );
                
                static DestFinfo clearVec(
                        "clearVec",
                        "Handles request to clear the data vector",
                        new OpFunc0< TableBase > (&TableBase::clearVec));


	static Finfo* tableBaseFinfos[] = {
		&vec,			// Value
		&outputValue,	// ReadOnlyValue
		&size,			// ReadOnlyValue
		&y,				// ReadOnlyLookupValue
		&linearTransform,	// DestFinfo
		&xplot,			// DestFinfo
		&plainPlot,			// DestFinfo
		&loadCSV,			// DestFinfo
		&loadXplot,			// DestFinfo
		&loadXplotRange,	// DestFinfo
		&compareXplot,		// DestFinfo
		&compareVec,		// DestFinfo
                &clearVec,
	};

	static Dinfo< TableBase > dinfo;
	static Cinfo tableCinfo (
		"TableBase",
		Neutral::initCinfo(),
		tableBaseFinfos,
		sizeof( tableBaseFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &tableCinfo;
}

//////////////////////////////////////////////////////////////
// Basic class Definitions
//////////////////////////////////////////////////////////////

static const Cinfo* tableBaseCinfo = TableBase::initCinfo();

TableBase::TableBase() : output_( 0 )
{ 
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void TableBase::linearTransform( double scale, double offset )
{
    for ( vector< double >::iterator i = vec_.begin(); i != vec_.end(); ++i)
        *i = *i * scale + offset;
}

void TableBase::plainPlot( string fname )
{
	ofstream fout( fname.c_str(), ios_base::out );
	fout.precision( 18 );
	fout.setf( ios::scientific, ios::floatfield );
	for ( vector< double >::iterator i = vec_.begin(); i != vec_.end(); ++i)
		fout << *i << endl;
	fout << "\n";
}

void TableBase::xplot( string fname, string plotname )
{
	ofstream fout( fname.c_str(), ios_base::app );
	fout << "/newplot\n";
	fout << "/plotname " << plotname << "\n";
	for ( vector< double >::iterator i = vec_.begin(); i != vec_.end(); ++i)
		fout << *i << endl;
	fout << "\n";
}

bool isNamedPlot( const string& line, const string& plotname )
{
	static const unsigned int len = strlen( "/plotname" ) ;
	if ( line.size() < len + 2 )
		return 0;
	if ( line[0] == '/' && line[1] == 'p' ) {
		string name = line.substr( strlen( "/plotname" ) );
		string::size_type pos = name.find_first_not_of( " 	" );
		if ( pos == string::npos ) {
			cout << "TableBase::loadXplot: Malformed plotname line '" <<
				line << "'\n";
			return 0;
		}
		name = name.substr( pos );
		if ( plotname == name )
			return 1;
	}
	return 0;
}

/*
bool lineHasExactlyTwoNumericalColumns( const string& line )
{
	istringstream sstream( line );
	double y;

	if ( sstream >> y ) {
		if ( sstream >> y ) {
			if ( sstream >> y ) {
				return 0;
			} else {
				return 1;
			}
		}
	}
	return 0;
}
*/

double getYcolumn( const string& line )
{
	istringstream sstream( line );
	double y1 = 0.0;
	double y2;
	double y3;

	if ( sstream >> y1 ) {
		if ( sstream >> y2 ) {
			if ( sstream >> y3 ) {
				return y1;
			} else {
				return y2;
			}
		}
	}
	return y1;
}

bool innerLoadXplot( string fname, string plotname, vector< double >& v )
{
	ifstream fin( fname.c_str() );
	if ( !fin.good() ) {
		cout << "TableBase::innerLoadXplot: Failed to open file " << fname <<endl;
		return 0;
	}

	string line;
	// Here we advance to the first numerical line.
	if ( plotname == "" ) // Just load starting from the 1st numerical line.
	{
		while ( fin.good() ) { // Break out of this loop if we find a number
			getline( fin, line );
                        line = moose::trim(line);
			if ( isdigit( line[0] ) )
				break;;
			if ( line[0] == '-' && line.length() > 1 && isdigit( line[1] ) )
				break;
		}
	} else { // Find plotname and then begin loading.
		while ( fin.good() ) {
			getline( fin, line );
                        line = moose::trim(line);
			if ( isNamedPlot ( line, plotname ) ) {
				if ( !getline ( fin, line ) )
					return 0;
				break;
			}
		}
	}

	v.resize( 0 );
	do {
		if ( line.length() == 0 || line == "/newplot" || line == "/plotname" )
			break;
		v.push_back( getYcolumn( line ) );
		getline( fin, line );
                line = moose::trim(line);
	} while ( fin.good() );

	return ( v.size() > 0 );
}

void TableBase::loadXplot( string fname, string plotname )
{
	if ( !innerLoadXplot( fname, plotname, vec_ ) ) {
		cout << "TableBase::loadXplot: unable to load data from file " << fname <<endl;
		return;
	}
}

void TableBase::loadXplotRange( string fname, string plotname, 
	unsigned int start, unsigned int end )
{
	vector< double > temp;
	if ( !innerLoadXplot( fname, plotname, temp ) ) {
		cout << "TableBase::loadXplot: unable to load data from file " << fname <<endl;
		return;
	}
	if ( start > end || end > temp.size() ) {
		cout << "TableBase::loadXplotRange: Bad range (" << start << 
		", " << end << "] for table of size " << temp.size() <<
		" from file " << fname << endl;
		return;
	}
	vec_.clear();
	vec_.insert( vec_.end(), temp.begin() + start, temp.begin() + end );
}

void TableBase::loadCSV( 
	string fname, int startLine, int colNum, char separator )
{
    cout << "TODO: Not implemented yet" << endl;
}

double getRMSDiff( const vector< double >& v1, const vector< double >& v2 )
{
	unsigned int size = ( v1.size() < v2.size() ) ? v1.size() : v2.size();
	if ( size == 0 )
		return -1;

	double sumsq = 0;
	for ( unsigned int i = 0; i < size; ++i ) {
		double temp = v1[i] - v2[i];
		sumsq += temp * temp;
	}
	return sqrt( sumsq / size );
}

double getRMS( const vector< double >& v )
{
	double sumsq = 0;
	unsigned int size = v.size();
	if ( size == 0 )
		return -1;
	for ( vector< double >::const_iterator i = v.begin(); i != v.end(); ++i)
		sumsq += *i * *i;
	
	return sqrt( sumsq / size );
}

double getRMSRatio( const vector< double >& v1, const vector< double >& v2 )
{
	double r1 = getRMS( v1 );
	double r2 = getRMS( v2 );
	if ( v1.size() == 0 || v2.size() == 0 )
		return -1;
	if ( r1 + r2 > 1e-20 )
		return getRMSDiff( v1, v2 ) / (r1 + r2);
	return -1;
}

string headop( const string& op )
{
	const unsigned int len = 5;
	char temp[len];
	unsigned int i = 0;
	for ( i = 0; i < op.length() && i < len-1 ; ++i )
		temp[i] = tolower( op[i] );
	temp[i] = '\0';
	return string( temp );
}

void TableBase::compareXplot( string fname, string plotname, string op )
{
	vector< double > temp;
	if ( !innerLoadXplot( fname, plotname, temp ) ) {
		cout << "TableBase::compareXplot: unable to load data from file " << fname <<endl;
	}

	string hop = headop( op );

	if ( hop == "rmsd" ) { // RMSDifference
		output_ = getRMSDiff( vec_, temp );
	}

	if ( hop == "rmsr" ) { // RMS ratio
		output_ = getRMSRatio( vec_, temp );
	}

	if ( hop == "dotp" )
		cout << "TableBase::compareXplot: DotProduct not yet done\n";
}

void TableBase::compareVec( vector< double > temp, string op )
{
	// Note that this line below is illegal: it causes a race condition
	// vector< double > temp = Field< vector< double > >::get( other, "vec" );

	string hop = headop( op );

	if ( hop == "rmsd" ) { // RMSDifference
		output_ = getRMSDiff( vec_, temp );
	}

	if ( hop == "rmsr" ) { // RMS ratio
		output_ = getRMSRatio( vec_, temp );
	}

	if ( hop == "dotp" )
		cout << "TableBase::compareVec: DotProduct not yet done\n";
}

void TableBase::clearVec()
{
    vec_.resize( 0 );
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

double TableBase::getOutputValue() const
{
	return output_;
}

void TableBase::setOutputValue( double v )
{
	output_ = v;
}

double TableBase::getY( unsigned int index ) const
{
	if ( index < vec_.size() )
		return ( vec_[index] );
	return 0;
}

double TableBase::interpolate( double xmin, double xmax, double input ) 
	const
{
	if ( vec_.size() == 0 )
		return 0;
	if ( vec_.size() == 1 || input < xmin || xmin >= xmax )
		return vec_[0];
	if ( input > xmax )
		return ( vec_.back() );
	
	unsigned int xdivs = vec_.size() - 1;

	double fraction = ( input - xmin ) / ( xmax - xmin );
	if ( fraction < 0 )
		return vec_[0];

	unsigned int j = xdivs * fraction;
	if ( j >= ( vec_.size() - 1 ) )
		return vec_.back();
	
	double dx = (xmax - xmin ) / xdivs;
	double lowerBound = xmin + j * dx;
	double subFraction = ( input - lowerBound ) / dx;

	double y = vec_[j] + ( vec_[j + 1] - vec_[j] ) * subFraction;
	return y;
}

//////////////////////////////////////////////////////////////
// Element Field Definitions
//////////////////////////////////////////////////////////////

void TableBase::setVecSize( unsigned int num )
{
	vec_.resize( num );
}

unsigned int TableBase::getVecSize() const
{
	return vec_.size();
}

vector< double > TableBase::getVec() const
{
	return vec_;
}

void TableBase::setVec( vector< double >  val )
{
	vec_ = val;
}

vector< double >& TableBase::vec()
{
	return vec_;
}
