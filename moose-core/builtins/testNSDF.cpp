// testNSDF.cpp ---
//
// Filename: testNSDF.cpp
// Description:
// Author: subha
// Maintainer:
// Created: Tue Aug 25 22:36:07 2015 (-0400)
// Version:
// Last-Updated: Sat Aug 29 12:34:08 2015 (-0400)
//           By: subha
//     Update #: 43
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

#include "hdf5.h"
#include "hdf5_hl.h"

#include <ctime>
#include <deque>
#include <cstdio>

#include "header.h"
#include "../utility/utility.h"

#include "HDF5WriterBase.h"
#include "HDF5DataWriter.h"

#include "NSDFWriter.h"
#include "InputVariable.h"

#define STR_DSET_NAME "vlenstr_dset"
#define STR_DSET_LEN 4

void testCreateStringDataset()
{
    const char * data[STR_DSET_LEN] = {"You have to", "live", "life", "to the limit"};
    hid_t file, memtype, dset;
    hsize_t size = STR_DSET_LEN;
    herr_t status;
    HDF5WriterBase writer;
    string h5Filename = std::tmpnam( NULL );
    file = H5Fcreate(h5Filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    dset = writer.createStringDataset(file, STR_DSET_NAME, size, size);
    assert(dset >= 0);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, H5T_VARIABLE);
    assert(status >= 0);
    status = H5Dwrite(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    assert(status >= 0);
    status = H5Dclose(dset);
    H5Tclose(memtype);
    H5Fclose(file);
}

#else // dummy function
void testCreateStringDataset()
{
    ;
}
#endif // USE_HDF5

void testNSDF()
{
    testCreateStringDataset();
}

//
// testNSDF.cpp ends here
