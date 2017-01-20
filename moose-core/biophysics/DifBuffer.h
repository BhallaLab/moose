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
  
  //Field access functions

  double getActivation(const Eref& e) const;
  void setActivation(const Eref& e,double value);

  double getBFree(const Eref& e) const;
  double getBBound(const Eref& e) const;

  //double getPrevFree(const Eref& e) const;   // Bfree at previous time step
  //double getPrevBound(const Eref& e) const;  // Bbound at previous time step

  double getBTot(const Eref& e) const;           //  total buffer concentration in mM (free + bound)
  void setBTot(const Eref& e,double value);

  double getKf(const Eref& e) const;         // forward rate constant in 1/(mM*sec)
  void setKf(const Eref& e,double value);

  double getKb(const Eref& e) const;         // backward rate constant in 1/sec
  void setKb(const Eref& e,double value);
  
  double getD(const Eref& e) const;          // diffusion constant of buffer molecules, m^2/sec
  void setD(const Eref& e,double value);

  int getShapeMode(const Eref& e) const; //    Set to one of the predefined global
  void setShapeMode(const Eref& e,int value); // variables SHELL=0, SLICE=SLAB=1, USERDEF=3.

  double getLength(const Eref& e) const; //             shell length
  void setLength(const Eref& e,double value);

  double getDiameter(const Eref& e) const; //            shell diameter
  void setDiameter(const Eref& e,double value);
  
  double getThickness(const Eref& e) const; //           shell thickness
  void setThickness(const Eref& e,double value);
  
  void setOuterArea( const Eref& e,double outerArea );
  double getOuterArea(const Eref& e) const; //         area of upper (outer) shell surface
  
  void setInnerArea( const Eref& e,double innerArea );
  double getInnerArea(const Eref& e) const; //       area of lower (inner) shell surface

  double getVolume(const Eref& e) const; //             shell volume
  void setVolume(const Eref& e,double volume); //
  
  //void concentration();
  void  buffer(const Eref& e,double C);

  void reinit( const Eref & e, ProcPtr p );

  void process(const Eref & e, ProcPtr p );
  
  void fluxFromOut(const Eref& e,double outerC, double outerThickness );

  void fluxFromIn( const Eref& e,double innerC, double innerThickness );
  static SrcFinfo4< double, double, double, double >* reactionOut();
  static SrcFinfo2< double, double >* innerDifSourceOut();
  static SrcFinfo2< double, double >* outerDifSourceOut();
  static const Cinfo * initCinfo();

 private:
  
  void localReinit_0(  const Eref & e, ProcPtr p );
  void localProcess_0( const Eref & e, ProcPtr p );
  
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
  int shapeMode_; //SHELL=0, SLICE=SLAB=1, USERDEF=3
  double diameter_; //shell diameter
  double length_; //shell length
  double thickness_; //shell thickness
  double outerArea_; //area of upper (outer) shell surface
  double innerArea_; //area of lower (inner) shell surface
  double volume_;
};

#endif // _DifBuffer_h
/* DifBuffer.h ends here */
