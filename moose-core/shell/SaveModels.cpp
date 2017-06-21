/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <fstream>
#include "header.h"
#include "Shell.h"

// Defined in kinetics/WriteKkit.cpp
extern void writeKkit( Id model, const string& fname );
extern void writeCspace (Id model, const string& fname );

/**
 * Saves specified model to specified file, using filetype identified by
 * filename extension. Currently known filetypes are:
 * .g: Kkit model
 * .cspace: cspace model
 *
 * Still to come:
 * .p: GENESIS neuron morphology and channel spec file
 * .sbml: SBML file
 * .nml: NeuroML file
 * .9ml: NineML file
 * .snml: SigNeurML
 */
void Shell::doSaveModel( Id model, const string& fileName, bool qFlag )
	   	const
{
	// string modelFamily = Field< string >::get( model, "modelFamily" );
	// if ( modelFamily != "kinetic" ) {
	// 	cout << "Warning: Shell::doSaveModel: Do not know how to save "
	// 			"model of family '" << modelFamily << "'.\n";
	// 	return;
	// }

	size_t pos = fileName.find( "." );
	string fileType = fileName.substr( pos );

	if ( fileType == ".g" ) { // kkit model requested.
			// cout << "Cannot write kkit model at this point\n";
			writeKkit( model, fileName );
	} else if (fileType == ".cspace"){
            // writeCspace( model, fileName );
            cout << "Cannot write cspace model at this point\n";
    } else {
		cout << "Warning: Shell::doSaveModel: Do not know how to save "
				"model of file type '" << fileType << "'.\n";
	}
}
