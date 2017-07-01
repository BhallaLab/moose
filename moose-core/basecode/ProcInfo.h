/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef  PROCINFO_INC
#define  PROCINFO_INC
class ProcInfo
{
    public:
        ProcInfo()
            : dt( 1.0 ), currTime( 0.0 )
        {;}
        double dt;
        double currTime;
};

typedef const ProcInfo* ProcPtr;
#endif   /* ----- #ifndef PROCINFO_INC  ----- */
