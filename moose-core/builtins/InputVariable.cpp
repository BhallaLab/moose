// InputVariable.cpp ---
//
// Filename: InputVariable.cpp
// Description:
// Author: subha
// Maintainer:
// Created: Fri Jun 26 07:21:56 2015 (-0400)
// Version:
// Last-Updated: Thu Jul 23 08:54:24 2015 (-0400)
//           By: subha
//     Update #: 2
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

#ifdef USE_HDF5
#include "../basecode/header.h"
#include "hdf5.h"

#include "NSDFWriter.h"
#include "InputVariable.h"


const Cinfo * InputVariable::initCinfo()
{
    static DestFinfo input("input",
                           "Handles input message, inserts into variable queue on owner.",
                           new EpFunc1< InputVariable, double > (&InputVariable::epSetValue));
    static Finfo * inputVariableFinfos[] = {&input};

    static string doc[] = {
        "Name", "InputVariable",
        "Author", "Subhasis Ray",
        "Description", "Variable for capturing incoming values and updating them in owner object."};

    static Dinfo< InputVariable > dinfo;

    static Cinfo inputVariableCinfo(
        "InputVariable",
        Variable::initCinfo(),
        inputVariableFinfos,
        sizeof(inputVariableFinfos)/sizeof(Finfo*),
        &dinfo,
        doc,
        sizeof(doc)/sizeof(string),
        true);
    return &inputVariableCinfo;
}

static const Cinfo *InputVariableCinfo = InputVariable::initCinfo();

InputVariable::InputVariable(): owner_(0)
{
    ;
}

InputVariable::~InputVariable()
{
    ;
}

void InputVariable::setOwner( NSDFWriter * owner)
{
    owner_ = owner;
}

void InputVariable::epSetValue( const Eref& eref, double value)
{
    if (owner_)
    {
        owner_->setInput(eref.fieldIndex(), value);
    }
}
#endif

//
// InputVariable.cpp ends here
