/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _LIF_H
#define _LIF_H

namespace moose
{

/**
 * The IntFire class sets up an integrate-and-fire compartment.
 */
class LIF: public IntFireBase
{
public:
    LIF();
    virtual ~LIF();
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
};

}

#endif // _LIF_H
