/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _DIFSHELL_BASE_H
#define _DIFSHELL_BASE_H
/*This is base class for DifShell*/

class DifShellBase
{
 public:
  DifShellBase();
  /////////////////////////////////////////////////////////////
  // Dest functions
  /////////////////////////////////////////////////////////////
  void reinit( const Eref & e, ProcPtr p );
  void process(const Eref & e, ProcPtr p );
  void buffer( const Eref& e, double kf, double kb, double bFree, double bBound );
  void fluxFromOut(const Eref& e, double outerC, double outerThickness );
  void fluxFromIn(const Eref& e, double innerC, double innerThickness );
  void influx(const Eref& e, double I );
  void outflux(const Eref& e,  double I );
  void fInflux(const Eref& e, double I, double fraction );
  void fOutflux(const Eref& e, double I, double fraction );
  void storeInflux(const Eref& e, double flux );
  void storeOutflux(const Eref& e, double flux );
  void tauPump(const Eref& e,  double kP, double Ceq );
  void eqTauPump(const Eref& e, double kP );
  void mmPump(const Eref& e, double vMax, double Kd );
  void hillPump(const Eref& e, double vMax, double Kd, unsigned int hill);

  virtual void vReinit(const Eref & e, ProcPtr p ) = 0;
  virtual void vProcess(const Eref & e, ProcPtr p ) = 0;
  virtual void vBuffer(const Eref& e, double kf, double kb, double bFree, double bBound ) = 0;
  virtual void vFluxFromOut(const Eref& e, double outerC, double outerThickness ) = 0;
  virtual void vFluxFromIn(const Eref& e, double innerC, double innerThickness ) = 0;
  virtual void vInflux(const Eref& e, double I ) = 0;
  virtual void vOutflux(const Eref& e,  double I ) = 0;
  virtual void vFInflux(const Eref& e, double I, double fraction ) = 0;
  virtual void vFOutflux(const Eref& e, double I, double fraction ) = 0;
  virtual void vStoreInflux(const Eref& e, double flux ) = 0;
  virtual void vStoreOutflux(const Eref& e, double flux ) = 0;
  virtual void vTauPump(const Eref& e, double kP, double Ceq ) = 0;
  virtual void vEqTauPump(const Eref& e, double kP ) = 0;
  virtual void vMMPump(const Eref& e, double vMax, double Kd ) = 0;
  virtual void vHillPump(const Eref& e, double vMax, double Kd, unsigned int hill ) = 0;
  /////////////////////////////////////////////////////////////
  // Field access functions
  /////////////////////////////////////////////////////////////

  void setC(const Eref& e,double C);
  double getC( const Eref& e) const;

  void setCeq(const Eref& e,double Ceq );
  double getCeq(const Eref& e) const;

  void setD(const Eref& e, double D );
  double getD(const Eref& e) const;

  void setValence(const Eref& e, double valence );
  double getValence(const Eref& e) const;

  void setLeak(const Eref& e, double leak );
  double getLeak(const Eref& e) const;

  void setShapeMode(const Eref& e, unsigned int shapeMode );
  unsigned int getShapeMode(const Eref& e) const;

  void setLength(const Eref& e, double length );
  double getLength(const Eref& e) const;

  void setDiameter(const Eref& e, double diameter );
  double getDiameter(const Eref& e) const;

  void setThickness(const Eref& e, double thickness );
  double getThickness(const Eref& e) const;

  void setVolume(const Eref& e, double volume );
  double getVolume(const Eref& e) const;

  void setOuterArea(const Eref& e, double outerArea );
  double getOuterArea(const Eref& e) const;

  void setInnerArea(const Eref& e, double innerArea );
  double getInnerArea(const Eref& e) const;

  virtual void vSetC(const Eref& e,double C) = 0;
  virtual double vGetC( const Eref& e) const = 0;

  virtual void vSetCeq(const Eref& e,double Ceq ) = 0;
  virtual double vGetCeq(const Eref& e) const = 0;

  virtual void vSetD(const Eref& e, double D ) = 0;
  virtual double vGetD(const Eref& e) const = 0;

  virtual void vSetValence(const Eref& e, double valence ) = 0;
  virtual double vGetValence(const Eref& e) const = 0;

  virtual void vSetLeak(const Eref& e, double leak ) = 0;
  virtual double vGetLeak(const Eref& e) const = 0;

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



  static SrcFinfo1< double >* concentrationOut();
  static SrcFinfo2< double, double >* innerDifSourceOut();
  static SrcFinfo2< double, double >* outerDifSourceOut();
  static const Cinfo * initCinfo();

 private:



};

#endif //_DIFSHELL_BASE_H
