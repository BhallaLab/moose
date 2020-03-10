/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include <algorithm>
#include <vector>
#include <map>
#include <cassert>
#include <string>
#include <iostream>
using namespace std;

#include "../basecode/SparseMatrix.h"
#include "DiffPoolVec.h"

/**
 * Default is to create it with a single compartment, independent of any
 * solver, so that we can set it up as a dummy DiffPool for the Pool to
 * work on in single-compartment models.
 */
DiffPoolVec::DiffPoolVec()
    : id_( 0 ), n_( 1, 0.0 ), nInit_( 1, 0.0 ),
      diffConst_( 1.0e-12 ), motorConst_( 0.0 )
{
    ;
}

double DiffPoolVec::getNinit( unsigned int voxel ) const
{
    assert( voxel < nInit_.size() );
    return nInit_[ voxel ];
}

void DiffPoolVec::setNinit( unsigned int voxel, double v )
{
    assert( voxel < nInit_.size() );
    nInit_[ voxel ] = v;
}

double DiffPoolVec::getN( unsigned int voxel ) const
{
    assert( voxel < n_.size() );
    return n_[ voxel ];
}

void DiffPoolVec::setN( unsigned int voxel, double v )
{
    assert( voxel < n_.size() );
    n_[ voxel ] = v;
}

double DiffPoolVec::getPrev( unsigned int voxel ) const
{
    assert( voxel < n_.size() );
    return prev_[ voxel ];
}

const vector< double >& DiffPoolVec::getNvec() const
{
    return n_;
}

void DiffPoolVec::setNvec( const vector< double >& vec )
{
    assert( vec.size() == n_.size() );
    n_ = vec;
}

void DiffPoolVec::setNvec( unsigned int start, unsigned int num,
        vector< double >::const_iterator q )
{
    assert( start + num <= n_.size() );
    vector< double >::iterator p = n_.begin() + start;
    for ( unsigned int i = 0; i < num; ++i )
        *p++ = *q++;
}

void DiffPoolVec::setPrevVec()
{
    prev_ = n_;
}

double DiffPoolVec::getDiffConst() const
{
    return diffConst_;
}

void DiffPoolVec::setDiffConst( double v )
{
    diffConst_ = v;
}

double DiffPoolVec::getMotorConst() const
{
    return motorConst_;
}

void DiffPoolVec::setMotorConst( double v )
{
    motorConst_ = v;
}

void DiffPoolVec::setNumVoxels( unsigned int num )
{
    nInit_.resize( num, 0.0 );
    n_.resize( num, 0.0 );
}

unsigned int DiffPoolVec::getNumVoxels() const
{
    return n_.size();
}

void DiffPoolVec::setId( unsigned int id )
{
    id_ = id;
}

unsigned int DiffPoolVec::getId() const
{
    return id_;
}

void DiffPoolVec::setOps(const vector< Triplet< double > >& ops,
        const vector< double >& diagVal )
{
    if ( ops.size() > 0 )
    {
        assert( diagVal.size() == n_.size() );
        ops_ = ops;
        diagVal_ = diagVal;
    }
    else
    {
        ops_.clear();
        diagVal_.clear();
    }
}

void DiffPoolVec::advance( double dt )
{
    if ( ops_.size() == 0 ) return;

    for (auto i = ops_.cbegin(); i != ops_.end(); ++i )
        n_[i->c_] -= n_[i->b_] * i->a_;

    assert( n_.size() == diagVal_.size() );

    auto iy = n_.begin();
    for ( auto i = diagVal_.cbegin(); i != diagVal_.end(); ++i )
        *iy++ *= *i;
}

void DiffPoolVec::reinit() // Not called by the clock, but by parent.
{
    assert( n_.size() == nInit_.size() );
    prev_ = n_ = nInit_;
}
