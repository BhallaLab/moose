/* DifBuffer.h --- 
 * 
 * Filename: DifBuffer.h
 * Description: 
 * Author: Subhasis Ray
 * Maintainer: 
 * Created: Mon Feb 16 11:42:50 2015 (-0500)
 * Version: 
 * Package-Requires: ()
 * Last-Updated: Mon Feb 23 10:49:54 2015 (-0500)
 *           By: Subhasis Ray
 *     Update #: 25
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Code: */

#ifndef _DifBuffer_h
#define _DifBuffer_h

class DifBuffer
{
 public:
  DifBuffer();
  
  void vBuffer(const Eref& e,double C);
  void vReinit( const Eref & e, ProcPtr p );
  void vProcess(const Eref & e, ProcPtr p );
  void vFluxFromOut(const Eref& e,double outerC, double outerThickness );
  void vFluxFromIn( const Eref& e,double innerC, double innerThickness );  
  //Field access functions

  double vGetActivation(const Eref& e) const;
  void vSetActivation(const Eref& e,double value);

  double vGetBFree(const Eref& e) const;
  double vGetBBound(const Eref& e) const;

  double vGetBTot(const Eref& e) const;           //  total buffer concentration in mM (free + bound)
  void vSetBTot(const Eref& e,double value);

  double vGetKf(const Eref& e) const;         // forward rate constant in 1/(mM*sec)
  void vSetKf(const Eref& e,double value);

  double vGetKb(const Eref& e) const;         // backward rate constant in 1/sec
  void vSetKb(const Eref& e,double value);
  
  double vGetD(const Eref& e) const;          // diffusion constant of buffer molecules, m^2/sec
  void vSetD(const Eref& e,double value);
  
  //void concentration();

 
  static const Cinfo * initCinfo();

 private:
  
 
  double integrate(double state, double dt, double A, double B );
  
  double activation_; //ion concentration from incoming CONCEN message
  double Af_;
  double Bf_;
  double bFree_; //free buffer concentration
  double bBound_; //bound buffer concentration
  //double prevFree_; //bFree at previous time
  //double prevBound_; //bBound at previous time
  double bTot_; //bFree+bBound
  double kf_; //forward rate constant
  double kb_; //backward rate constant
  double D_; //diffusion constant
  static const double EPSILON;
};

#endif // _DifBuffer_h
/* DifBuffer.h ends here */
