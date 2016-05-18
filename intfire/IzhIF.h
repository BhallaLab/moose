/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _IzhIF_H
#define _IzhIF_H

namespace moose
{
/**
 * The IntFire class sets up an integrate-and-fire compartment.
 */
class IzhIF: public IntFireBase
{
	public:
			IzhIF();
			virtual ~IzhIF();

			void setA0( const Eref& e,  double val );
			double getA0( const Eref& e  ) const;
			void setB0( const Eref& e,  double val );
			double getB0( const Eref& e  ) const;
			void setC0( const Eref& e,  double val );
			double getC0( const Eref& e  ) const;
			void setA( const Eref& e,  double val );
			double getA( const Eref& e  ) const;
			void setB( const Eref& e,  double val );
			double getB( const Eref& e  ) const;
			void setD( const Eref& e,  double val );
			double getD( const Eref& e  ) const;
			void setVPeak( const Eref& e,  double val );
			double getVPeak( const Eref& e  ) const;
			void setU( const Eref& e,  double val );
			double getU( const Eref& e  ) const;
			void setUInit( const Eref& e,  double val );
			double getUInit( const Eref& e  ) const;

			/**
			 * The process function does the object updating and sends out
			 * messages to channels, nernsts, and so on.
			 */
			void vProcess( const Eref& e, ProcPtr p );

			/**
			 * The reinit function reinitializes all fields.
			 */
			void vReinit( const Eref& e, ProcPtr p );

			/**
			 * Initializes the class info.
			 */
			static const Cinfo* initCinfo();

    private:
            double a0_;
            double b0_;
            double c0_;
            double a_;
            double b_;
            double d_;
            double vPeak_;
            double u_;
            double uInit_;
};
}

#endif // _IzhIF_H
