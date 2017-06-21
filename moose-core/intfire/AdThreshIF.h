/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _AdThreshIF_H
#define _AdThreshIF_H

namespace moose
{
/**
 * The IntFire class sets up an integrate-and-fire compartment.
 */
class AdThreshIF: public IntFireBase
{
	public:
			AdThreshIF();
			virtual ~AdThreshIF();

			void setThreshAdaptive( const Eref& e,  double val );
			double getThreshAdaptive( const Eref& e  ) const;
			void setTauThresh( const Eref& e,  double val );
			double getTauThresh( const Eref& e  ) const;
			void setA0( const Eref& e,  double val );
			double getA0( const Eref& e  ) const;
			void setThreshJump( const Eref& e,  double val );
			double getThreshJump( const Eref& e  ) const;

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
            double threshAdaptive_;
            double tauThresh_;
            double a0_;
            double threshJump_;
};
}

#endif // _AdThreshIF_H
