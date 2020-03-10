/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _DIFF_POOL_VEC_H
#define _DIFF_POOL_VEC_H

/**
 * This is a FieldElement of the Dsolve class. It manages (ie., zombifies)
 * a specific pool, and the pool maintains a pointer to it. For accessing
 * volumes, this maintains a pointer to the relevant ChemCompt.
 */
class DiffPoolVec
{
public:
    DiffPoolVec();
    void process();
    void reinit();
    void advance( double dt );
    double getNinit( unsigned int vox ) const;
    void setNinit( unsigned int vox, double value );
    double getN( unsigned int vox ) const;
    void setN( unsigned int vox, double value );
    double getPrev( unsigned int vox ) const;

    double getDiffConst() const;
    void setDiffConst( double value );

    double getMotorConst() const;
    void setMotorConst( double value );

    void setNumVoxels( unsigned int num );
    unsigned int getNumVoxels() const;

    void setId( unsigned int id );
    unsigned int getId() const;

    /////////////////////////////////////////////////
    /// Used by parent solver to manipulate 'n'
    const vector< double >& getNvec() const;
    /// Used by parent solver to manipulate 'n'
    void setNvec( const vector< double >& n );
    void setNvec( unsigned int start, unsigned int num,
                  vector< double >::const_iterator q );
    void setPrevVec(); /// Assigns prev_ = n_
    void setOps( const vector< Triplet< double > >& ops_,
                 const vector< double >& diagVal_ ); /// Assign operations.

    // static const Cinfo* initCinfo();
private:
    unsigned int id_; /// Integer conversion of Id of pool handled.
    vector< double > n_; /// Number of molecules of pool in each voxel
    vector< double > prev_; /// # molecules of pool on previous timestep
    vector< double > nInit_; /// Boundary condition: Initial 'n'.
    double diffConst_; /// Diffusion const, assumed uniform
    double motorConst_; /// Motor const, ie, transport rate.
    vector< Triplet< double > > ops_;
    vector< double > diagVal_;
};

#endif // _DIFF_POOL_VEC_H
