/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_UTILS_H
#define _HSOLVE_UTILS_H

#include "../basecode/header.h"
#include "../basecode/global.h"
#include "../utility/print_function.hpp"
#include "../utility/simple_test.hpp"

#include "../biophysics/HHGate.h"
#include "../biophysics/ChanBase.h"
#include "../biophysics/ChanCommon.h"
#include "../biophysics/HHChannelBase.h"
#include "../biophysics/HHChannel.h"
#include "../basecode/OpFunc.h"


class HSolveUtils
{
public:
    static void initialize( Id object );
    static int adjacent( Id compartment, vector< Id >& ret );
    static int adjacent( Id compartment, Id exclude, vector< Id >& ret );
    static int children( Id compartment, vector< Id >& ret );
    static int channels( Id compartment, vector< Id >& ret );
    static int hhchannels( Id compartment, vector< Id >& ret );
    static int gates( Id channel, vector< Id >& ret, bool getOriginals = true );
    static int spikegens( Id compartment, vector< Id >& ret );
    static int synchans( Id compartment, vector< Id >& ret );
    static int leakageChannels( Id compartment, vector< Id >& ret );
    static int caTarget( Id channel, vector< Id >& ret );
    static int caDepend( Id channel, vector< Id >& ret );

    class Grid
    {
    public:
        Grid( double min, double max, unsigned int divs )
            :
            min_( min ),
            max_( max ),
            divs_( divs )
        {
            dx_ = ( max_ - min_ ) / divs_;
        }

        unsigned int size();
        double entry( unsigned int i );

        bool operator ==( const Grid& other )
        {
            return
                min_ == other.min_ &&
                max_ == other.max_ &&
                divs_ == other.divs_;
        }

        double min_;
        double max_;
        unsigned int divs_;
        double dx_;
    };

    // Functions for accessing gates' lookup tables.
    //~ static int domain(
    //~ Id gate,
    //~ double& min,
    //~ double& max );
    static void rates(
        Id gate,
        Grid grid,
        vector< double >& A,
        vector< double >& B );
    //~ static int modes(
    //~ Id gate,
    //~ int& AMode,
    //~ int& BMode );

    static int targets(
        Id object,
        string msg,
        vector< Id >& target,
        string filter = "",
        bool include = true );

    static int targets(
        Id object,
        string msg,
        vector< Id >& target,
        const vector< string >& filter,
        bool include = true );
};

#endif // _HSOLVE_UTILS_H
