// GapJunction.h ---
//
// Filename: GapJunction.h
// Description:
// Author: Subhasis Ray
// Maintainer:
// Created: Tue Jul  2 11:22:05 2013 (+0530)
// Version:
// Last-Updated: Tue Jul  2 12:39:14 2013 (+0530)
//           By: subha
//     Update #: 27
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// Implementation of simple gap junction
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

class GapJunction {
  public:
    GapJunction();
    ~GapJunction();
    void setVm1(double Vm);
    void setVm2(double Vm);
    // Sometimes one may want to set g=0, i.e. R=inf - e.g. in Traub model
    void setGk(double g);
    double getGk() const;

    // Dest function definitions.
    /**
     * The process function does the object updating and sends out
     * messages to channels, nernsts, and so on.
     */
    void process( const Eref& e, ProcPtr p );

    /**
     * The reinit function reinitializes all fields.
     */
    void reinit( const Eref& e, ProcPtr p );

    /**
     * Initializes the class info.
     */
    static const Cinfo* initCinfo();

  protected:
    double Vm1_;
    double Vm2_;
    double Gk_;
};
//
// GapJunction.h ends here
