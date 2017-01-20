/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _DifShell_h
#define _DifShell_h

class DifShell
{
 public:
  DifShell();
		
  /////////////////////////////////////////////////////////////
  // Field access functions
  /////////////////////////////////////////////////////////////
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

  /////////////////////////////////////////////////////////////
  // Dest functions
  /////////////////////////////////////////////////////////////
  void reinit( const Eref & e, ProcPtr p );

  void process(const Eref & e, ProcPtr p );
  /*
    void process_1(const Eref & e, ProcPtr p );

    void reinit_1(const Eref & e, ProcPtr p ); // dummyFunc
  */
  void buffer(
	      const Eref& e,
	      double kf,
	      double kb,
	      double bFree,
	      double bBound );

  void fluxFromOut(
		   const Eref& e,
		   double outerC,
		   double outerThickness );

  void fluxFromIn(
		  const Eref& e,
		  double innerC,
		  double innerThickness );

  void influx(
	      const Eref& e,
	      double I );

  void outflux(
	       const Eref& e,  
	       double I );

  void fInflux(
	       const Eref& e,
	       double I,
	       double fraction );

  void fOutflux(
		const Eref& e,
		double I,
		double fraction );

  void storeInflux(
		   const Eref& e, 
		   double flux );

  void storeOutflux(
		    const Eref& e,	  
		    double flux );

  void tauPump(
	       const Eref& e,
	       double kP,
	       double Ceq );

  void eqTauPump(
		 const Eref& e,
		 double kP );

  void mmPump(
	      const Eref& e,	    
	      double vMax,
	      double Kd );

  void hillPump(
		const Eref& e,
		double vMax,
		double Kd,
		unsigned int hill );
  static SrcFinfo1< double >* concentrationOut();
  static SrcFinfo2< double, double >* innerDifSourceOut();
  static SrcFinfo2< double, double >* outerDifSourceOut();
  static const Cinfo * initCinfo();
                
 private:
  void localReinit_0(  const Eref & e, ProcPtr p );
  void localProcess_0( const Eref & e, ProcPtr p );
  //void localProcess_1( const Eref & e, ProcPtr p );
  void localBuffer(const Eref& e, double kf, double kb, double bFree, double bBound );
  void localFluxFromOut( const Eref& e,double outerC, double outerThickness );
  void localFluxFromIn(const Eref& e, double innerC, double innerThickness );
  void localInflux(const Eref& e,	double I );
  void localOutflux( const Eref& e,double I );
  void localFInflux(const Eref& e, double I, double fraction );
  void localFOutflux(const Eref& e, double I, double fraction );
  void localStoreInflux(const Eref& e, double flux );
  void localStoreOutflux(const Eref& e, double flux );
  void localTauPump(const Eref& e, double kP, double Ceq );
  void localEqTauPump(const Eref& e, double kP );
  void localMMPump(const Eref& e, double vMax, double Kd );
  void localHillPump(const Eref& e, double vMax, double Kd, unsigned int hill );

  double dCbyDt_;
  double C_;
  double Ceq_;
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
		
  /// Faraday's constant (Coulomb / Mole)
  static const double F_;
};

#endif // _DifShell_h
