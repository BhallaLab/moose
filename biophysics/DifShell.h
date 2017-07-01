/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _DIFSHELL_H
#define _DIFSHELL_H

class DifShell: public DifShellBase{
 public:
  DifShell();

  /////////////////////////////////////////////////////////////
  // Dest functions
  /////////////////////////////////////////////////////////////
  void vReinit( const Eref & e, ProcPtr p );
  void vProcess(const Eref & e, ProcPtr p );
  void vBuffer(const Eref& e, double kf, double kb, double bFree, double bBound );
  void vFluxFromOut(const Eref& e, double outerC, double outerThickness );
  void vFluxFromIn(const Eref& e, double innerC, double innerThickness );
  void vInflux(const Eref& e, double I );
  void vOutflux(const Eref& e, double I );
  void vFInflux(const Eref& e, double I, double fraction );
  void vFOutflux(const Eref& e, double I, double fraction );
  void vStoreInflux(const Eref& e, double flux );
  void vStoreOutflux(const Eref& e, double flux );
  void vTauPump(const Eref& e, double kP, double Ceq );
  void vEqTauPump(const Eref& e, double kP );
  void vMMPump(const Eref& e, double vMax, double Kd );
  void vHillPump(const Eref& e, double vMax, double Kd, unsigned int hill );

  /////////////////////////////////////////////////////////////
  // Field access functions
  /////////////////////////////////////////////////////////////

  void vSetC(const Eref& e,double C);
  double vGetC( const Eref& e) const;

  void vSetCeq(const Eref& e,double Ceq );
  double vGetCeq(const Eref& e) const;

  void vSetD(const Eref& e, double D );
  double vGetD(const Eref& e) const;

  void vSetValence(const Eref& e, double valence );
  double vGetValence(const Eref& e) const;

  void vSetLeak(const Eref& e, double leak );
  double vGetLeak(const Eref& e) const;

  void vSetShapeMode(const Eref& e, unsigned int shapeMode );
  unsigned int vGetShapeMode(const Eref& e) const;

  void vSetLength(const Eref& e, double length );
  double vGetLength(const Eref& e) const;

  void vSetDiameter(const Eref& e, double diameter );
  double vGetDiameter(const Eref& e) const;

  void vSetThickness(const Eref& e, double thickness );
  double vGetThickness(const Eref& e) const;

  void vSetVolume(const Eref& e, double volume );
  double vGetVolume(const Eref& e) const;

  void vSetOuterArea(const Eref& e, double outerArea );
  double vGetOuterArea(const Eref& e) const;

  void vSetInnerArea(const Eref& e, double innerArea );
  double vGetInnerArea(const Eref& e) const;

  void calculateVolumeArea(const Eref& e);

  static const Cinfo * initCinfo();


 private:

  double integrate( double state, double dt, double A, double B );

  double dCbyDt_;
  double Cmultiplier_;
  double C_;
  double Ceq_;
  double prevC_;
  double D_;
  double valence_;
  double leak_;
  unsigned int shapeMode_;
  double length_;
  double diameter_;
  double thickness_;
  double volume_;
  double outerArea_;
  double innerArea_;

  static const double EPSILON;
  /// Faraday's constant (Coulomb / Mole)
  static const double F;

};

#endif // _DIFSHELL_H
