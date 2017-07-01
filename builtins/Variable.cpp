// Variable.cpp ---
//
// Filename: Variable.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Fri May 30 19:56:06 2014 (+0530)
// Version:
// Last-Updated:
//           By:
//     Update #: 0
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change log:
//
//
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
//
//

// Code:

#include "header.h"
#include "Variable.h"
#include "Function.h"

const Cinfo * Variable::initCinfo()
{
    static ValueFinfo< Variable, double > value(
        "value",
        "Variable value",
        &Variable::setValue,
        &Variable::getValue);
    static DestFinfo input(
        "input",
        "Handles incoming variable value.",
        new EpFunc1< Variable, double >( &Variable::epSetValue ));

    static Finfo * variableFinfos[] = {
        &value,
        &input
    };
    static string doc[] = {
        "Name", "Variable",
        "Author", "Subhasis Ray",
        "Description", "Variable for storing double values. This is used in Function class."
    };
    static Dinfo< Variable > dinfo;
    static Cinfo variableCinfo("Variable",
                               Neutral::initCinfo(),
                               variableFinfos,
                               sizeof(variableFinfos) / sizeof(Finfo*),
                               &dinfo,
                               doc,
                               sizeof(doc) / sizeof(string),
                               true // is FieldElement, not to be created directly
                               );
    return & variableCinfo;
}

static const Cinfo * variableCinfo = Variable::initCinfo();

// This imitates Synapse::addMsgCallback
// but does not seem to be used anywhere
// - Subha, Tue Sep  9 19:37:11 IST 2014

void Variable::addMsgCallback(const Eref& e, const string& finfoName, ObjId msg, unsigned int msgLookup)
{
    /*
    if (finfoName == "input"){
        ObjId pa = Neutral::parent(e);
        Function * fn = reinterpret_cast< Function *>(pa.data());
        unsigned int varNumber = fn->addVar();
        SetGet2<unsigned int, unsigned int>::set(msg, "fieldIndex", msgLookup, varNumber);
    }
    */
}

// // This imitates Synapse::dropMsgCallback
// void Variable::dropMsgCallback(
//     const Eref& e, const string& finfoName,
//     ObjId msg, unsigned int msgLookup )
// {
// 	if ( finfoName == "input" ) {
// 		ObjId pa = Neutral::parent( e );
// 		SynHandlerBase* sh =
// 				reinterpret_cast< Function* >( pa.data() );
// 		sh->dropVar( msgLookup );
// 	}
// }


//
// Variable.cpp ends here
