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
		double getC( ) const;

		void setCeq(double Ceq );
		double getCeq() const;

		void setD( double D );
		double getD() const;

		void setValence( double valence );
		double getValence() const;

		void setLeak( double leak );
		double getLeak() const;

		void setShapeMode( unsigned int shapeMode );
		unsigned int getShapeMode() const;

		void setLength( double length );
		double getLength() const;

		void setDiameter( double diameter );
		double getDiameter() const;

		void setThickness( double thickness );
		double getThickness() const;

		void setVolume( double volume );
		double getVolume() const;

		void setOuterArea( double outerArea );
		double getOuterArea() const;

		void setInnerArea( double innerArea );
		double getInnerArea() const;

		/////////////////////////////////////////////////////////////
		// Dest functions
		/////////////////////////////////////////////////////////////
		void reinit_0( const Eref & e, ProcPtr p );

		void process_0(const Eref & e, ProcPtr p );

                void process_1(const Eref & e, ProcPtr p );

                void reinit_1(const Eref & e, ProcPtr p ); // dummyFunc
                
		void buffer(
		 double kf,
		 double kb,
		 double bFree,
		 double bBound );

		void fluxFromOut(
		 double outerC,
		 double outerThickness );

		void fluxFromIn(
		 double innerC,
		 double innerThickness );

		void influx(
		 double I );

		void outflux(
		 double I );

		void fInflux(
		 double I,
		 double fraction );

		void fOutflux(
		 double I,
		 double fraction );

		void storeInflux(
		 double flux );

		void storeOutflux(
		 double flux );

		void tauPump(
		 double kP,
		 double Ceq );

		void eqTauPump(
		 double kP );

		void mmPump(
		 double vMax,
		 double Kd );

		void hillPump(
                    double vMax,
                    double Kd,
                    unsigned int hill );

                static const Cinfo * initCinfo();
                
	private:
		void localReinit_0(  const Eref & e, ProcPtr p );
		void localProcess_0( const Eref & e, ProcPtr p );
		void localProcess_1( const Eref & e, ProcPtr p );
		void localBuffer( double kf, double kb, double bFree, double bBound );
		void localFluxFromOut( double outerC, double outerThickness );
		void localFluxFromIn( double innerC, double innerThickness );
		void localInflux(	double I );
		void localOutflux( double I );
		void localFInflux( double I, double fraction );
		void localFOutflux( double I, double fraction );
		void localStoreInflux( double flux );
		void localStoreOutflux( double flux );
		void localTauPump( double kP, double Ceq );
		void localEqTauPump( double kP );
		void localMMPump( double vMax, double Kd );
		void localHillPump( double vMax, double Kd, unsigned int hill );

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
