/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _COMPARTMENT_DATA_HOLDER_H
#define _COMPARTMENT_DATA_HOLDER_H

/**
 */
namespace moose
{

class CompartmentDataHolder
{
	public:
			CompartmentDataHolder();
			void readData( const CompartmentBase* c, const Eref& e );
			void writeData( CompartmentBase* c, const Eref& e );
	private:
			double Cm_;
			double Ra_;
			double Rm_;
			double Em_;
			double initVm_;
			double inject_;

			double diameter_;
			double length_;
			double x0_;
			double y0_;
			double z0_;
			double x_;
			double y_;
			double z_;
};

}

#endif // _COMPARTMENT_DATA_HOLDER_H
