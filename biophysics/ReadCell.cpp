/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"
#include "ReadCell.h"
#include "../utility/utility.h"
#include "../utility/numutil.h"
#include "CompartmentBase.h"
#include "Compartment.h"
#include "SymCompartment.h"
#include <fstream>

double calcSurf( double, double );

// ReadCell::ReadCell( const vector< double >& globalParms )
ReadCell::ReadCell()
	:
	RM_( 10.0 ),
	CM_( 0.01 ),
	RA_( 1.0 ),
	EREST_ACT_( -0.065 ),
	ELEAK_( -0.065 ),
	
#if 0
        // Unused privates. Causes error when -Werror is enabled. Uncomment them
        // appropriately when needed.
	dendrDiam( 0.0 ),
	aveLength( 0.0 ),
	spineSurf( 0.0 ),
	spineDens( 0.0 ),
	spineFreq( 0.0 ),
	membFactor( 0.0 ),
#endif

	erestFlag_( 0 ),
	eleakFlag_( 0 ),
	
	cell_( Id() ),
	currCell_( Id() ),
	
	lastCompt_( Id() ),
	protoCompt_( Id() ),
	
	numCompartments_( 0 ),
	numChannels_( 0 ),
	numOthers_( 0 ),
	
	numProtoCompts_( 0 ),
	numProtoChans_( 0 ),
	numProtoOthers_( 0 ),
	
	graftFlag_( 0 ),
	polarFlag_( 0 ),
	relativeCoordsFlag_( 0 ),
	doubleEndpointFlag_( 0 ),
	symmetricFlag_( 0 ),
	
	shell_( reinterpret_cast< Shell* >( Id().eref().data() ) )
{
	/*
	assert( globalParms.size() == 5 );
	if ( !isEqual( globalParms[0], 0.0 ) )
		CM_ = globalParms[0];
	if ( !isEqual( globalParms[1], 0.0 ) )
		RM_ = globalParms[1];
	if ( !isEqual( globalParms[2], 0.0 ) )
		RA_ = globalParms[2];
	if ( !isEqual( globalParms[3], 0.0 ) )
		EREST_ACT_ = globalParms[3];
	if ( !isEqual( globalParms[4], 0.0 ) )
		ELEAK_ = globalParms[4];
	else
		ELEAK_ = EREST_ACT_;
	*/
	
	string libPath = "/library";
	Id libId( libPath );

	if ( libId.path() != libPath ) {
		cerr << "Warning: ReadCell: No library for channels.\n";
		return;
	}

	vector< Id > chanList =
		Field< vector< Id > >::get(
			ObjId( libId ), "children" );
	
	vector< Id >::iterator i;
	for ( i = chanList.begin(); i != chanList.end(); ++i ) {
		Id id = (*i);
		string name = id.element()->getName();
		
		chanProtos_[ name ] = id;
	}
}

/**
 * The readcell function implements the old GENESIS cellreader
 * functionality.
 */
Id ReadCell::read(
	const string& fileName,
	const string& cellName,
	Id parent )
{
	fileName_ = fileName;
	
	ifstream fin( fileName.c_str() );
	if ( !fin ) {
		cerr << "ReadCell::read -- could not open file " << fileName << ".\n";
		return Id();
	}
	
	/*
	// Search for file in list of paths.
	PathUtility pathUtil( Property::getProperty( Property::SIMPATH ) );
	ifstream fin( filename.c_str() );
	for (unsigned int i = 0; i < pathUtil.size() && !fin; ++i )
	{
		string path = pathUtil.makeFilePath(filename, i);
		fin.clear( );
		fin.open( path.c_str() );
	}
	if ( !fin ) {
		cerr << "ReadCell::read -- could not open file " << filename << endl;
		return Id();
	}
	*/
	
	unsigned int size =  1;
	if ( parent.element()->cinfo()->isA( "Neuron" ) ) {
		cell_ = parent;
		currCell_ = cell_;
	} else {
		cell_ = shell_->doCreate( "Neuron", parent, 
						cellName, size, MooseGlobal );
		currCell_ = cell_;
	}
	
	if ( innerRead( fin ) ) {
		return cell_;
	} else {
		cerr << "Readcell failed.\n";
		return Id();
	}
}

bool ReadCell::innerRead( ifstream& fin )
{
    string line;
	lineNum_ = 0;
	
	ParseStage parseMode = DATA;
	string::size_type pos;
	
	while ( getline( fin, line ) ) {
		line = moose::trim( line );
		lineNum_++;
		
		if ( line.length() == 0 )
			continue;
		
		pos = line.find_first_not_of( "\t " );
		if ( pos == string::npos )
			continue;
		else
			line = line.substr( pos );
		
		if ( line.substr( 0, 2 ) == "//" )
			continue;
		
		if ( ( pos = line.find( "//" ) ) != string::npos ) 
			line = line.substr( 0, pos );
		
		if ( line.substr( 0, 2 ) == "/*" ) {
			parseMode = COMMENT;
		} else if ( line.find( "*/" ) != string::npos ) {
			parseMode = DATA;
			continue;
		} else if ( line[ 0 ] == '*' ) {
			parseMode = SCRIPT;
		}
		
		if ( parseMode == COMMENT ) {
			pos = line.find( "*/" );
			if ( pos != string::npos ) {
				parseMode = DATA;
				if ( line.length() > pos + 2 )
					line = line.substr( pos + 2 );
			}
		}
		
		if ( parseMode == DATA ) {
			// For now not keeping it strict. Ignoring return status, and
			// continuing even if there was error in processing this line.
			readData( line );
		} else if ( parseMode == SCRIPT ) {
			// For now not keeping it strict. Ignoring return status, and
			// continuing even if there was error in processing this line.
			readScript( line );
			parseMode = DATA;
		}
	}
	
	cout <<
		"ReadCell: " <<
		numCompartments_ << " compartments, " << 
		numChannels_ << " channels, " << 
		numOthers_ << " others\n";
	
	return 1;
}

bool ReadCell::readScript( const string& line )
{
	vector< string > argv;
	string delimiters( "\t " );
        moose::tokenize( line, delimiters, argv ); 
	
	if ( argv[ 0 ] == "*cartesian" ) {
		polarFlag_ = 0;
	} else if ( argv[ 0 ] == "*polar" ) {
		polarFlag_ = 1;
	} else if ( argv[ 0 ] == "*relative" ) {
		relativeCoordsFlag_ = 1;
	} else if ( argv[ 0 ] == "*absolute" ) {
		relativeCoordsFlag_ = 0;
	} else if ( argv[ 0 ] == "*symmetric" ) {
		symmetricFlag_ = 1;
	} else if ( argv[ 0 ] == "*asymmetric" ) {
        symmetricFlag_ = 0;
    } else if ( argv[ 0 ] == "*set_global" || argv[ 0 ] == "*set_compt_param" ) {
		if ( argv.size() != 3 ) {
			cerr << "Error: ReadCell: Bad line: " <<
				"File: " << fileName_ <<
				"Line: " << lineNum_ << "\n";
			return 0;
		}
		
		if ( argv[ 1 ] == "RM" )
			RM_ = atof( argv[ 2 ].c_str() );
		if ( argv[ 1 ] == "RA" )
			RA_ = atof( argv[ 2 ].c_str() );
		if ( argv[ 1 ] == "CM" )
			CM_ = atof( argv[ 2 ].c_str() );
		if ( argv[ 1 ] == "EREST_ACT" ) {
			EREST_ACT_ = atof( argv[ 2 ].c_str() );
			erestFlag_ = 1;
		} if (argv[ 1 ] == "ELEAK" ) {
			ELEAK_ = atof( argv[ 2 ].c_str() );
			eleakFlag_ = 1;
		}
	} else if ( argv[ 0 ] == "*start_cell" ) {
		if ( argv.size() == 1 ) {
			graftFlag_ = 0;
			currCell_ = cell_;
		} else if ( argv.size() == 2 ) {
			graftFlag_ = 1;
			currCell_ = startGraftCell( argv[ 1 ] );
			if ( currCell_ == Id() )
				return 0;
		} else {
			cerr << "Error: ReadCell: Bad line: " <<
				"File: " << fileName_ <<
				"Line: " << lineNum_ << "\n";
			return 0;
		}
	} else if ( argv[ 0 ] == "*compt" ) {
		if ( argv.size() != 2 ) {
			cerr << "Error: ReadCell: Bad line: " <<
				"File: " << fileName_ <<
				"Line: " << lineNum_ << "\n";
			return 0;
		}
		
		Id protoId( argv[ 1 ] );
		if ( protoId.path() != argv[ 1 ] ) {
			cerr << "Error: ReadCell: Bad path: " << argv[ 1 ] << " " <<
				"File: " << fileName_ <<
				"Line: " << lineNum_ << "\n";
			return 0;
		}
		
		protoCompt_ = protoId;
		countProtos();
	} else if ( argv[ 0 ] == "*double_endpoint" ) {
		doubleEndpointFlag_ = 1;
	} else if ( argv[ 0 ] == "*double_endpoint_off" ) {
		doubleEndpointFlag_ = 0;
	} else if ( argv[ 0 ] == "*makeproto" ) {
		; // Nothing to be done.
	} else {
		cerr << "Warning: ReadCell: Command " <<
			argv[ 0 ] << " not recognized. Ignoring. " <<
			"File: " << fileName_ <<
			"Line: " << lineNum_ << "\n";
	}
	
	return 1;
}

bool ReadCell::readData( const string& line )
{
	vector< string > argv;
	string delimiters( "\t " );
        moose::tokenize( line, delimiters, argv ); 
	
	if ( argv.size() < 6 ) {
		cerr <<	"Error: ReadCell: Too few arguments in line: " << argv.size() <<
				", should be > 6.\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return 0;
	}
	
	double x0 = 0.0;
	double y0 = 0.0;
	double z0 = 0.0;
	double x, y, z;
	double d;
	int argOffset = 0;
	
	string name = argv[ 0 ];
	string parent = argv[ 1 ];
	
	if ( doubleEndpointFlag_ ) {
		argOffset = 3;
		
		x0 = 1.0e-6 * atof( argv[ 2 ].c_str() );
		y0 = atof( argv[ 3 ].c_str() );
		z0 = atof( argv[ 4 ].c_str() );
		if ( polarFlag_ ) {
			double r = x0;
			double theta = y0 * M_PI / 180.0;
			double phi = z0 * M_PI / 180.0;
			x0 = r * sin( phi ) * cos ( theta );
			y0 = r * sin( phi ) * sin ( theta );
			z0 = r * cos( phi );
		} else {
			y0 *= 1.0e-6;
			z0 *= 1.0e-6;
		}
	}
	
	x = 1.0e-6 * atof( argv[ argOffset + 2 ].c_str() );
	y = atof( argv[ argOffset + 3 ].c_str() );
	z = atof( argv[ argOffset + 4 ].c_str() );
	if ( polarFlag_ ) {
		double r = x;
		double theta = y * M_PI / 180.0;
		double phi = z * M_PI / 180.0;
		x = r * sin( phi ) * cos ( theta );
		y = r * sin( phi ) * sin ( theta );
		z = r * cos( phi );
	} else {
		y *= 1.0e-6;
		z *= 1.0e-6;
	}
	
	d = 1.0e-6 * atof( argv[ argOffset + 5 ].c_str() );
	
	double length;
	Id compt =
		buildCompartment( name, parent, x0, y0, z0, x, y, z, d, length, argv );
	
	if ( compt == Id() )
		return 0;
	
	return buildChannels( compt, argv, d, length );
}

Id ReadCell::buildCompartment( 
	const string& name,
	const string& parent,
	double x0, double y0, double z0,
	double x, double y, double z,
	double d,
	double& length, // Length is sent back.
	vector< string >& argv )
{
	static const Finfo* raxial2OutFinfo =
			SymCompartment::initCinfo()->findFinfo( "distalOut" );
	/*
	 * This section determines the parent compartment, to connect up with axial
	 * messages. Here 'parent' refers to the biophysical relationship within 
	 * the neuron's tree, and not to the path hierarchy in the MOOSE element
	 * tree.
	 * 
	 * If the parent is specified as 'none', then the compartment is the root
	 * of the cell's tree, and will not be connected axially to any compartments
	 * except for its children, if any.
	 */
	Id parentId;
	if ( parent == "." ) { // Shorthand: use the previous compartment.
		parentId = lastCompt_;
	} else if ( parent == "none" || parent == "nil" ) {
                parentId = Id();
	} else {
		string parentPath = currCell_.path() + "/" + parent;
		ObjId parentObjId = ObjId( parentPath );
		if ( parentObjId.bad() ) {
			cerr << "Error: ReadCell: could not find parent compt '"
				<< parent
				<< "' for child '" << name << "'.\n";
			cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
			return ObjId(0, BADINDEX);
		}
                parentId = parentObjId;
	}
	
	//~ Id childId;
	//~ bool ret = lookupGet< Id, string >(
		//~ currCell_, "lookupChild", childId, name );
	//~ assert( ret );
	//~ if ( !childId.bad() ) {
		//~ if ( name[ name.length() - 1 ] == ']' ) {
			//~ string::size_type pos = name.rfind( '[' );
			//~ if ( pos == string::npos ) {
				//~ cerr << "Error: ReadCell: bad child name:" << name << endl;
				//~ cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				//~ return 0;
			//~ }
			//~ unsigned int index = 
				//~ atoi( name.substr( pos + 1, name.length() - pos ).c_str() );
			//~ if ( childId.index() == index ) {
				//~ cerr << "Error: ReadCell: duplicate child on parent compt '" <<
						//~ parent << "' for child '" << name << "'\n";
				//~ cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				//~ return 0;
			//~ }
		//~ } else {
			//~ cerr << "Error: ReadCell: duplicate child on parent compt '" <<
					//~ parent << "' for child '" << name << "'\n";
			//~ cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
			//~ return 0;
		//~ }
	//~ }
	
	unsigned int size = 1;
	Id compt;
	if ( graftFlag_ && ( parent == "none" || parent == "nil" ) ) {
		compt = currCell_;
	} else {
            if ( protoCompt_ != Id() ) {
		                compt = shell_->doCopy(
                                protoCompt_,
				currCell_,
				name,
				1,        // n: number of copies
				false,    // toGlobal
				false     // copyExtMsgs
			);
			numCompartments_ += numProtoCompts_;
			numChannels_ += numProtoChans_;
			numOthers_ += numProtoOthers_;
		} else {
			string comptType = ( symmetricFlag_ ) ? 
				"SymCompartment" : "Compartment";
			compt = shell_->doCreate(
                            comptType, currCell_, name, size, MooseGlobal );
			if ( !graftFlag_ )
				++numCompartments_;
		}
	}
	lastCompt_ = compt;
	
	if ( parentId != Id()){
		double px, py, pz;
		double dx, dy, dz;
		
		px = Field< double >::get( parentId, "x" );
		py = Field< double >::get( parentId, "y" );
		pz = Field< double >::get( parentId, "z" );
		
		if ( !doubleEndpointFlag_ ) {
			x0 = px;
			y0 = py;
			z0 = pz;
		}
		
		if ( relativeCoordsFlag_ == 1 ) {
			x += px;
			y += py;
			z += pz;
			if ( doubleEndpointFlag_ ) {
				x0 += px;
				y0 += py;
				z0 += pz;
			}
		}
		dx = x - x0;
		dy = y - y0;
		dz = z - z0;
		
		length = sqrt( dx * dx + dy * dy + dz * dz );
		if ( symmetricFlag_ ) {
			// Now find all sibling compartments on the same parent.
			// They must be connected up using 'sibling'.
			vector< Id > sibs;
			parentId.element()->getNeighbors( sibs, raxial2OutFinfo );
			// Later put in the soma as a sphere, with its special msgs.
			shell_->doAddMsg( "Single",
				parentId, "distal", compt, "proximal" );
			for ( vector< Id >::iterator i = sibs.begin(); 
				i != sibs.end(); ++i ) {
				shell_->doAddMsg( "Single",
					compt, "sibling", *i, "sibling" );
			}

		} else {
			shell_->doAddMsg( "Single", 
				parentId, "axial", compt, "raxial" );
		}
	} else {
		length = sqrt( x * x + y * y + z * z ); 
		// or it could be a sphere.
	}
	
	double Cm, Rm, Ra;
	
	Cm = CM_ * calcSurf( length, d );
	Rm = RM_ / calcSurf( length, d );
	
	if ( length > 0 ) {
		Ra = RA_ * length * 4.0 / ( d * d * M_PI );
	} else {
		Ra = RA_ * 8.0 / ( d * M_PI );
	}

	// Set each of these to the other only if the only one set was other
	double eleak = ( erestFlag_ && !eleakFlag_ ) ? EREST_ACT_ : ELEAK_;
	double erest = ( !erestFlag_ && eleakFlag_ ) ? ELEAK_ : EREST_ACT_;
	
	Field< double >::set( compt, "x0", x0 );
	Field< double >::set( compt, "y0", y0 );
	Field< double >::set( compt, "z0", z0 );
	Field< double >::set( compt, "x", x );
	Field< double >::set( compt, "y", y );
	Field< double >::set( compt, "z", z );
	Field< double >::set( compt, "diameter", d );
	Field< double >::set( compt, "length", length );
	Field< double >::set( compt, "Rm", Rm );
	Field< double >::set( compt, "Ra", Ra );
	Field< double >::set( compt, "Cm", Cm );
	Field< double >::set( compt, "initVm", erest );
	Field< double >::set( compt, "Em", eleak );
	Field< double >::set( compt, "Vm", erest );
	
	return compt;
}

Id ReadCell::startGraftCell( const string& cellPath )
{
	/*
	 * If path exists, return with error. This will also catch the case where
	 * cellPath is "/", and we will not have to check for this separately
	 * later.
	 */
	Id cellId( cellPath );
	if ( cellId.path() == cellPath ) {
		cerr << "Warning: ReadCell: cell '" << cellPath << "' already exists.\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return Id();
	}
	
        ObjId parentObjId;
	string cellName;
	string::size_type pos_1 = cellPath.find_first_of( "/" );
	string::size_type pos_2 = cellPath.find_last_of( "/" );
	
	if ( pos_1 != 0 ) {
		cerr << "Error: ReadCell: *start_cell should be given absolute path.\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return Id();
	}
	
	if ( pos_2 == 0 ) {
            parentObjId = ObjId("/");
		cellName = cellPath.substr( 1 );
	} else {
		string parentPath = cellPath.substr( 0, pos_2  );
		parentObjId = ObjId( parentPath );
		if ( parentObjId.bad() ) {
			cerr << "Error: ReadCell: cell path '" << cellPath
				<< "' not found.\n";
			cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
			return Id();
		}
		
		cellName = cellPath.substr( pos_2 + 1 );
	}
	
	unsigned int size = 1;
	return shell_->doCreate( "Compartment", parentObjId, cellName, size, MooseGlobal );
}

Id ReadCell::findChannel( const string& name )
{
	//~ vector< Id >::iterator i;
	//~ for ( i = chanProtos_.begin(); i != chanProtos_.end(); i++ )
		//~ if ( i->name() == name )
			//~ return *i;
	//~ return Id();
	
	map< string, Id >::iterator pos = chanProtos_.find( name );
	if ( pos != chanProtos_.end() )
		return pos->second;
	else
		return Id();
}

double calcSurf( double len, double dia )
{
	double area = 0.0;
	if ( len == 0.0 ) // Spherical. Safe to compare with 0.0.
		area = dia * dia * M_PI;
	else
		area = len * dia * M_PI;
	
	return area;
}

bool ReadCell::buildChannels(
	Id compt,
	vector< string >& argv,
	double diameter,
	double length )
{
	bool isArgOK;
	int argStart;
	vector< Id > goodChannels;
	
	if ( doubleEndpointFlag_ ) {
		isArgOK = ( argv.size() % 2 ) == 1;
		argStart = 9;
	} else {
		isArgOK = ( argv.size() % 2 ) == 0;
		argStart = 6;
	}
	
	if ( !isArgOK ) {
		cerr << "Error: ReadCell: Bad number of arguments in channel list\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return 0;
	}
	
	for ( unsigned int j = argStart; j < argv.size(); j++ ) {
		// Here we explicitly set compt fields by scaling from the 
		// specific value applied here.
		string chan = argv[ j ];
		
		double value = atof( argv[ ++j ].c_str() );
		if ( chan == "RA" ) {
			double temp;
			if ( length == 0.0 ) // Spherical flag. Assume length = dia.
				temp = 8.0 * value / ( diameter * M_PI );
			else
				temp = 4.0 * value * length / ( diameter * diameter * M_PI );
			Field< double >::set( compt, "Ra", temp );
		} else if ( chan == "RM" ) {
			Field< double >::set( compt, "Rm", value * calcSurf( length, diameter ) );
		} else if ( chan == "CM" ) {
			Field< double >::set( compt, "Cm", value * calcSurf( length, diameter ) );
		} else if ( chan == "Rm" ) {
			Field< double >::set( compt, "Rm", value );
		} else if ( chan == "Ra" ) {
			Field< double >::set( compt, "Ra", value );
		} else if ( chan == "Cm" ) {
			Field< double >::set( compt, "Cm", value );
		} else if ( chan == "kinModel" ) {
			// Need 3 args here: 
			// lambda, name of proto, method
			// We already have lambda from value. Note it is in microns
			if ( j + 2 < argv.size() ) {
				string protoName = argv[ ++j ];
				string method = argv[ ++j ];
				//~ addKinModel( compt, value * 1.0e-6, protoName, method );
			} else {
				cerr << "Error: ReadCell: kinModel needs 3 args\n";
				cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				break;
			}
		} else if ( chan == "m2c" ) {
			// Need 5 args here: 
			// scale factor, mol, moloffset, chan, chanoffset
			// We already have scale factor from value.
			if ( j + 4 < argv.size() ) {
				//~ addM2C( compt, value, argv.begin() + j + 1 ); 
				j += 4;
			} else {
				cerr << "Error: ReadCell: m2c adaptor needs 5 args\n";
				cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				break;
			}
		} else if ( chan == "c2m" ) {
			// Need another 5 args here: 
			// scale factor, chan, chanoffset, mol, moloffset
			if ( j + 4 < argv.size() ) {
				//~ addC2M( compt, value, argv.begin() + j + 1 ); 
				j += 4;
			} else {
				cerr << "Error: ReadCell: c2m adaptor needs 5 args\n";
				cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				break;
			}
		} else {
			Id chanId = findChannel( chan );
			if ( chanId == Id() ) {
				cerr << "Error: ReadCell: Channel '" << chan <<
						"' not found\n";
				cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
				continue;
			}
			
			Id copy = addChannel( compt, chanId, value, diameter, length );
			if ( copy != Id() ) {
				goodChannels.push_back( copy );
			} else {
				cerr << "Error: ReadCell: Could not add " << chan
                                     << " in " << compt.element()->getName() << ".";
				cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
			}
		}
	}
	
	for ( unsigned int i = 0; i < goodChannels.size(); i++ )
		addChannelMessage( goodChannels[ i ] );
	
	return 1;
}

Id ReadCell::addChannel( 
	Id compt,
	Id proto,
	double value,
	double dia,
	double length )
{
	Id copy = shell_->doCopy(
		proto,
		compt,
		"",     // newName: same as old name.
		1,      // n: number of copies
		false,  // toGlobal
		false   // copyExtMsgs
	);
/////////////////////////////
        assert( copy.element()->getName() == proto.element()->getName() );
/////////////////////////////
	assert( copy != Id() );
	
	if ( addCanonicalChannel( compt, copy, value, dia, length ) ) return copy;
	if ( addSpikeGen( compt, copy, value, dia, length ) ) return copy;
	if ( addCaConc( compt, copy, value, dia, length ) ) return copy;
	if ( addNernst( compt, copy, value ) ) return copy;
	
	return Id();
}

/**
 * Adds a typical channel to a compartment:
 *     - Connects up the 'channel' message between chan and compt.
 *     - Sets the Gbar field on the channel.
 * 
 * Typical channels currently are: HHChannel, HHChannel2D and SynChan. All of
 * these have the same "channel" interface, and have a "Gbar" field.
 */
bool ReadCell::addCanonicalChannel(
	Id compt,
	Id chan, 
	double value,
	double dia,
	double length )
{
    string className = chan.element()->cinfo()->name();
	if (
		className == "HHChannel" ||
		className == "HHChannel2D" ||
		className == "SynChan" ||
		className == "NMDAChan"
	) {
		ObjId mid = shell_->doAddMsg(
			"Single",
			compt,
			"channel",
			chan,
			"channel"
		);
		if ( mid.bad() )
			cout << "failed to connect message from compt " << compt << 
					" to channel " << chan << endl;
		
		if ( value > 0 ) {
			value *= calcSurf( length, dia );
		} else {
			value = -value;
		}
		
		if ( !graftFlag_ )
			++numChannels_;
		
		return Field< double >::set( chan, "Gbar", value );
	}
	
	return 0;
}

bool ReadCell::addSpikeGen( 
	Id compt,
	Id chan, 
	double value,
	double dia,
	double length )
{
    string className = chan.element()->cinfo()->name();
	if ( className == "SpikeGen" ) {
		shell_->doAddMsg(
			"Single",
			compt,
			"VmSrc",
			chan,
			"Vm"
		);
		
		if ( !graftFlag_ )
			++numOthers_;
		
		return Field< double >::set( chan, "threshold", value );
	}
	
	return 0;
}

/*
 * At present this does not set up any messaging. Traditionally (in GENESIS),
 * information on how to set customized messaging is stored in extra 'addmsg'
 * fields created on channels and other objects. Here the 'addChannelMessage()'
 * function handles these 'addmsg' fields.
 */
bool ReadCell::addCaConc( 
	Id compt,
	Id chan, 
	double value,
	double dia,
	double length )
{
	double thickness = Field< double >::get( chan, "thick" );
	if ( thickness > dia / 2.0 )
		thickness = 0.0;
	
	string className = chan.element()->cinfo()->name();
	if ( className == "CaConc" ) {
		if ( value > 0.0 ) {
			double vol;
			if ( length > 0.0 ){
				if ( thickness > 0.0 ) {
					vol = M_PI * length * ( dia - thickness ) * thickness;
				} else {
					vol = dia * dia * M_PI * length / 4.0;
				}
			} else { // spherical
				if ( thickness > 0.0 ) {
					double inner_dia = dia - 2 * thickness;
					vol = M_PI * (
						dia * dia * dia -
						inner_dia * inner_dia * inner_dia
					) / 6.0; 
				} else {
					vol = M_PI * dia * dia * dia / 6.0;
				}
			}
			if ( vol > 0.0 ) // Scale by volume.
				value /= vol;
		} else {
			value = - value;
		}
		
		if ( !graftFlag_ )
			++numOthers_;
		
		return Field< double >::set( chan, "B", value );
	}
	
	return 0;
}

bool ReadCell::addNernst( 
	Id compt,
	Id chan,
	double value )
{
	if ( !graftFlag_ )
		++numOthers_;
	return 0;
}

/*
void ReadCell::addKinModel( Element* compt, double lambda, 
	string name, string method )
{
	// cout << "addKinModel on " << compt->name() <<
	//		" name= " << name << ", lambda = " << lambda <<
	//		", using " << method << endl;
	
	Element* kinElm = findChannel( name );
	if ( kinElm == 0 ) {
		cerr << "Error:ReadCell: KinProto '" << name << "' not found\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return;
	}
	
	Element* kph = Neutral::create( "KinPlaceHolder", "kinModel", 
		compt->id(), Id::childId( compt->id() ) );
	set< Id, double, string >( kph, "setup", 
		kinElm->id(), lambda, method );
}

void ReadCell::addM2C( Element* compt, double scale, 
	vector< string >::iterator args )
{
	// cout << "addM2C on " << compt->name() << 
	//	" scale= " << scale << 
	//	" mol= " << *args << ", moloff= " << *(args+1) << 
	//	" chan= " << *(args + 2) << ", chanoff= " << *(args+3) << endl;
	
	string molName = *args++;
	double molOffset = atof( ( *args++ ).c_str() );
	string chanName = *args++;
	double chanOffset = atof( ( *args ).c_str() );
	string adaptorName = molName + "_2_" + chanName;
	
	Element* chan = findChannel( chanName );
	if ( chan == 0 ) {
		cerr << "Error:ReadCell: addM2C ': channel" << chanName << 
			"' not found\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return;
	}
	
	Element* adaptor = Neutral::create( "Adaptor", adaptorName,
		compt->id(), Id::childId( compt->id() ) );
	
	Eref( adaptor ).add( "outputSrc", Eref( chan ), "Gbar" );
	set< string, double, double, double >( adaptor, "setup",
		molName, scale, molOffset, chanOffset );
}

void ReadCell::addC2M( Element* compt, double scale, 
	vector< string >::iterator args )
{
	// cout << "addC2M on " << compt->name() << 
	//	" scale= " << scale << 
	//	" chan= " << *args << ", chanoff= " << *(args+1) << 
	//	" mol= " << *(args + 2) << ", moloff= " << *(args+3) <<  endl;
	
	string chanName = *args++;
	double chanOffset = atof( ( *args++ ).c_str() );
	string molName = *args++;
	double molOffset = atof( ( *args++ ).c_str() );
	string adaptorName = "Ca_2_" + molName;
	
	Element* chan = findChannel( chanName );
	if ( chan == 0 ) {
		cerr << "Error:ReadCell: addC2M ': channel" << chanName << 
			"' not found\n";
		cerr << "File: " << fileName_ << " Line: " << lineNum_ << endl;
		return;
	}
	
	Element* adaptor = Neutral::create( "Adaptor", adaptorName,
		compt->id(), Id::childId( compt->id() ) );
	
	Eref( adaptor ).add( "inputRequest", Eref( chan ), "Ca" );
	set< string, double, double, double >( adaptor, "setup",
		molName, scale, chanOffset, molOffset );
}
*/

void ReadCell::addChannelMessage( Id chan )
{
	/*
	* Get child objects of type Mstring, named addmsg1, 2, etc.
	* These define extra messages to be assembled at setup.
	* Similar to what was done with GENESIS.
	*/
	vector< Id > kids;
	Neutral::children( chan.eref(), kids );
	Shell *shell = reinterpret_cast< Shell* >( Id().eref().data() );
	Id cwe = shell->getCwe();
	shell->setCwe( chan );
	for ( vector< Id >::iterator i = kids.begin(); i != kids.end(); ++i )
	{
		// Ignore kid if its name does not begin with "addmsg"..
		const string& name = i->element()->getName();
		if ( name.find( "addmsg", 0 ) != 0 )
			continue;

		string s = Field< string >::get( *i, "value" );
		vector< string > token;
                moose::tokenize( s, " 	", token );
		assert( token.size() == 4 );
		ObjId src = shell->doFind( token[0] );
		ObjId dest = shell->doFind( token[2] );

		// I would like to assert, or warn here, but there are legitimate 
		// cases where not all possible messages are actually available 
		// to set up. So I just bail.
		if ( src.bad() || dest.bad()) {
#ifndef NDEBUG
				/*
			cout << "ReadCell::addChannelMessage( " << chan.path() << 
				"): " << name << " " << s << 
				": Bad src " << src << " or dest " << dest << endl;
				*/
#endif
			continue; 
		}
		ObjId mid = 
			shell->doAddMsg( "single", src, token[1], dest, token[3] );
		assert( !mid.bad());
	}
	shell->setCwe( cwe );
}

/**
 * Count elements under a tree.
 */
void ReadCell::countProtos( )
{
	//~ if ( protoCompt_ == 0 )
		//~ return;
	//~ 
	//~ numProtoCompts_ = 1; // protoCompt_ itself
	//~ numProtoChans_ = 0;
	//~ numProtoOthers_ = 0;
//~ 
	//~ vector< vector< Id > > cstack;
	//~ cstack.push_back( Neutral::getChildList( protoCompt_ ) );
	//~ while ( !cstack.empty() ) {
		//~ vector< Id >& child = cstack.back();
		//~ 
		//~ if ( child.empty() ) {
			//~ cstack.pop_back();
			//~ if ( !cstack.empty() )
				//~ cstack.back().pop_back();
		//~ } else {
			//~ const Id& curr = child.back();
			//~ const Cinfo* currCinfo = curr()->cinfo();
			//~ 
			//~ if ( currCinfo->isA( comptCinfo ) )
				//~ ++numProtoCompts_;
			//~ else if ( currCinfo->isA( chanCinfo ) ||
			          //~ currCinfo->isA( synchanCinfo ) )
				//~ ++numProtoChans_;
			//~ else if ( currCinfo->isA( spikegenCinfo ) ||
			          //~ currCinfo->isA( caconcCinfo ) ||
			          //~ currCinfo->isA( nernstCinfo ) )
				//~ ++numProtoOthers_;
			//~ 
			//~ cstack.push_back( Neutral::getChildList( curr() ) );
		//~ }
	//~ }
}
