/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _DIFBUFFER_BASE_H
#define _DIFBUFFER_BASE_H
/*This is base class for DifBuffer*/
class DifBufferBase
{
public:
  DifBufferBase();

  void buffer(const Eref& e,double C);
  void reinit( const Eref & e, ProcPtr p );
  void process(const Eref & e, ProcPtr p );
  void fluxFromOut(const Eref& e,double outerC, double outerThickness );
  void fluxFromIn( const Eref& e,double innerC, double innerThickness );
  virtual void vBuffer(const Eref& e,double C) = 0;
  virtual void vReinit( const Eref & e, ProcPtr p ) = 0;
  virtual void vProcess(const Eref & e, ProcPtr p ) = 0;
  virtual void vFluxFromOut(const Eref& e,double outerC, double outerThickness ) = 0;
  virtual void vFluxFromIn( const Eref& e,double innerC, double innerThickness ) = 0;


  double getActivation(const Eref& e) const;
  void setActivation(const Eref& e,double value);

  double getBFree(const Eref& e) const;
  void setBFree(const Eref& e,double value);

  double getBBound(const Eref& e) const;
  void setBBound(const Eref& e,double value);

  double getBTot(const Eref& e) const;           //  total buffer concentration in mM (free + bound)
  void setBTot(const Eref& e,double value);

  double getKf(const Eref& e) const;         // forward rate constant in 1/(mM*sec)
  void setKf(const Eref& e,double value);

  double getKb(const Eref& e) const;         // backward rate constant in 1/sec
  void setKb(const Eref& e,double value);

  double getD(const Eref& e) const;          // diffusion constant of buffer molecules, m^2/sec
  void setD(const Eref& e,double value);


  unsigned int getShapeMode(const Eref& e) const;
  void setShapeMode(const Eref& e,unsigned int value); // variables SHELL=0, SLICE=SLAB=1, USERDEF=3.

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


  virtual double vGetActivation(const Eref& e) const = 0;
  virtual void vSetActivation(const Eref& e,double value) = 0;

  virtual double vGetBFree(const Eref& e) const = 0;
  virtual void vSetBFree(const Eref& e,double value) = 0;

  virtual double vGetBBound(const Eref& e) const = 0;
  virtual void vSetBBound(const Eref& e,double value) = 0;


  virtual double vGetBTot(const Eref& e) const = 0;           //  total buffer concentration in mM (free + bound)
  virtual void vSetBTot(const Eref& e,double value) = 0;

  virtual double vGetKf(const Eref& e) const = 0;         // forward rate constant in 1/(mM*sec)
  virtual void vSetKf(const Eref& e,double value) = 0;

  virtual double vGetKb(const Eref& e) const = 0;         // backward rate constant in 1/sec
  virtual void vSetKb(const Eref& e,double value) = 0;

  virtual double vGetD(const Eref& e) const = 0;          // diffusion constant of buffer molecules, m^2/sec
  virtual void vSetD(const Eref& e,double value) = 0;

  virtual void vSetShapeMode(const Eref& e, unsigned int shapeMode ) = 0;
  virtual unsigned int vGetShapeMode(const Eref& e) const = 0;

  virtual void vSetLength(const Eref& e, double length ) = 0;
  virtual double vGetLength(const Eref& e) const = 0;

  virtual void vSetDiameter(const Eref& e, double diameter ) = 0;
  virtual double vGetDiameter(const Eref& e) const = 0;

  virtual void vSetThickness(const Eref& e, double thickness ) = 0;
  virtual double vGetThickness(const Eref& e) const = 0;

  virtual void vSetVolume(const Eref& e, double volume ) = 0;
  virtual double vGetVolume(const Eref& e) const = 0;

  virtual void vSetOuterArea(const Eref& e, double outerArea ) = 0;
  virtual double vGetOuterArea(const Eref& e) const = 0;

  virtual void vSetInnerArea(const Eref& e, double innerArea ) = 0;
  virtual double vGetInnerArea(const Eref& e) const = 0;

  static SrcFinfo4< double, double, double, double >* reactionOut();
  static SrcFinfo2< double, double >* innerDifSourceOut();
  static SrcFinfo2< double, double >* outerDifSourceOut();
  static const Cinfo * initCinfo();

private:



};




#endif //_DIFBUFFER_BASE_H
