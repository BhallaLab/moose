/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 *****/
#ifndef _DifBuffer_h
#define _DifBuffer_h

class DifBuffer: public DifBufferBase{
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
  void vSetBFree(const Eref& e,double value);

  double vGetBBound(const Eref& e) const;
  void vSetBBound(const Eref& e,double value);

  double vGetBTot(const Eref& e) const;           //  total buffer concentration in mM (free + bound)
  void vSetBTot(const Eref& e,double value);

  double vGetKf(const Eref& e) const;         // forward rate constant in 1/(mM*sec)
  void vSetKf(const Eref& e,double value);

  double vGetKb(const Eref& e) const;         // backward rate constant in 1/sec
  void vSetKb(const Eref& e,double value);

  double vGetD(const Eref& e) const;          // diffusion constant of buffer molecules, m^2/sec
  void vSetD(const Eref& e,double value);
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

  void   calculateVolumeArea(const Eref& e);
  static const Cinfo * initCinfo();

 private:


  double integrate(double state, double dt, double A, double B );

  double activation_; //ion concentration from incoming CONCEN message
  double Af_;
  double Bf_;
  double bFree_; //free buffer concentration
  double bBound_; //bound buffer concentration
  double prevFree_;
  double prevBound_;
  //double prevFree_; //bFree at previous time
  //double prevBound_; //bBound at previous time
  double bTot_; //bFree+bBound
  double kf_; //forward rate constant
  double kb_; //backward rate constant
  double D_; //diffusion constant
   unsigned int shapeMode_;
  double length_;
  double diameter_;
  double thickness_;
  double volume_;
  double outerArea_;
  double innerArea_;
  static const double EPSILON;
};

#endif // _DifBuffer_h
/* DifBuffer.h ends here */
