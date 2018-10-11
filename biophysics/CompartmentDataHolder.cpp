/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "../basecode/header.h"
#include "CompartmentBase.h"
#include "CompartmentDataHolder.h"
// static func

using namespace moose;

CompartmentDataHolder::CompartmentDataHolder()
		:
			Cm_( 1.0 ),
			Ra_( 1.0 ),
			Rm_( 1.0 ),
			Em_( -0.06 ),
			initVm_( -0.06 ),
			inject_( 0.0 ),

			diameter_( 1.0e-6 ),
			length_( 100.0e-6 ),
			x0_( 0.0 ),
			y0_( 0.0 ),
			z0_( 0.0 ),
			x_( 0.0 ),
			y_( 0.0 ),
			z_( 0.0 )
{;}

void CompartmentDataHolder::readData( const CompartmentBase* cb, const Eref& er )
{
		Cm_ = cb->getCm( er );
		Rm_ = cb->getRm( er );
		Ra_ = cb->getRa( er );
		Em_ = cb->getEm( er );
		initVm_ = cb->getInitVm( er );
		inject_ = cb->getInject( er );

		x0_ = cb->getX0();
		y0_ = cb->getY0();
		z0_ = cb->getZ0();
		x_ = cb->getX();
		y_ = cb->getY();
		z_ = cb->getZ();
		length_ = cb->getLength();
		diameter_ = cb->getDiameter();
}


void CompartmentDataHolder::writeData( CompartmentBase* cb, const Eref& er )
{
		cb->setCm( er, Cm_ );
		cb->setRm( er, Rm_ );
		cb->setRa( er, Ra_ );
		cb->setEm( er, Em_ );
		cb->setInitVm( er, initVm_ );
		cb->setInject( er, inject_ );

		cb->setX0( x0_ );
		cb->setY0( y0_ );
		cb->setZ0( z0_ );
		cb->setX( x_ );
		cb->setY( y_ );
		cb->setZ( z_ );
		cb->setLength( length_ );
		cb->setDiameter( diameter_ );
}
