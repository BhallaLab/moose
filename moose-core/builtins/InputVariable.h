/* InputVariable.h ---
 *
 * Filename: InputVariable.h
 * Description:
 * Author: subha
 * Maintainer:
 * Created: Fri Jun 26 06:36:11 2015 (-0400)
 * Version:
 * Last-Updated: Thu Jul 23 08:54:57 2015 (-0400)
 *           By: subha
 *     Update #: 1
 * URL:
 * Keywords:
 * Compatibility:
 *
 */

/* Commentary:
 *
 *
 *
 */

/* Change log:
 *
 *
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#ifdef USE_HDF5

#ifndef _INPUTVARIABLE_H
#define _INPUTVARIABLE_H

#include "Variable.h"

class NSDFWriter;

/**
   This class is for collecting data and updating a handler object
   with a value, similar to Synapse->SynHandler. Currently only for
   NSDFWriter, should be generalized as the need arises.
 */
class InputVariable: public Variable
{
  public:
    InputVariable();
    ~InputVariable();
    void epSetValue(const Eref &eref, double value);
    void setOwner(NSDFWriter * owner);
    static const Cinfo * initCinfo();
  protected:
    NSDFWriter * owner_;
};

#endif

#endif
/* InputVariable.h ends here */
