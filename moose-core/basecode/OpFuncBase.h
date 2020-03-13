/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _OPFUNCBASE_H
#define _OPFUNCBASE_H

#include "SrcFinfo.h"

extern const unsigned char MooseSendHop;
extern const unsigned char MooseSetHop;
extern const unsigned char MooseSetVecHop;
extern const unsigned char MooseGetHop;
extern const unsigned char MooseGetVecHop;
extern const unsigned char MooseReturnHop;
extern const unsigned char MooseTestHop;

class HopIndex
{
public:
    HopIndex( unsigned short bindIndex,
              unsigned char hopType = MooseSendHop)
        : bindIndex_( bindIndex ),
          hopType_( hopType )
    {;}

    unsigned short bindIndex() const
    {
        return bindIndex_;
    }

    unsigned char hopType() const
    {
        return hopType_;
    }
private:
    unsigned short bindIndex_;
    unsigned char hopType_;
};

class OpFunc
{
public:
    OpFunc();

    virtual ~OpFunc()
    {;}
    virtual bool checkFinfo( const Finfo* s) const = 0;

    virtual string rttiType() const = 0;

    virtual const OpFunc* makeHopFunc( HopIndex hopIndex) const =0;

    /// Executes the OpFunc by converting args.
    virtual void opBuffer( const Eref& e, double* buf ) const = 0;

    /// Executes the OpFunc for all data by converting a vector of args.
    virtual void opVecBuffer( const Eref& e, double* buf ) const
    {;}

    static const OpFunc* lookop( unsigned int opIndex );

    unsigned int opIndex() const
    {
        return opIndex_;
    }

    /// Used when rebuilding the Fid->OpFunc mapping.
    bool setIndex( unsigned int i );
    /// cleans out the entire Ops vector. Returns size of orig vector.
    static unsigned int rebuildOpIndex();
private:
    unsigned int opIndex_;
    static vector< OpFunc* >& ops();
};

class OpFunc0Base: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo0* >( s );
    }

    virtual void op( const Eref& e ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const;

    string rttiType() const
    {
        return "void";
    }
};

template< class A > class OpFunc1Base: public OpFunc
{
public:

    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo1< A >* >( s );
    }

    virtual void op( const Eref& e, A arg ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        op( e, Conv< A >::buf2val( &buf ) );
    }

    void opVecBuffer( const Eref& e, double* buf ) const
    {
        vector< A > temp = Conv< vector< A > >::buf2val( &buf );
        Element* elm = e.element();
        if ( elm->hasFields() )   // Assignment is to field array.
        {
            unsigned int di = e.dataIndex();
            unsigned int nf = elm->numField( di -
                                             elm->localDataStart() );
            for ( unsigned int i = 0; i < nf; ++i)
            {
                Eref er( elm, di, i );
                op( er, temp[ i % temp.size() ] );
            }
        }
        else     // Assignment is to data entries.
        {
            unsigned int k = 0;
            unsigned int start = elm->localDataStart();
            unsigned int end = start + elm->numLocalData();
            for ( unsigned int i = start; i < end; ++i)
            {
                Eref er( elm, i, 0 );
                op( er, temp[ k % temp.size() ] );
                ++k;
            }
        }
    }

    virtual void opVec( const Eref& e, const vector< A >& arg,
                        const OpFunc1Base< A >* op ) const
    { ; } // overridden in HopFuncs.

    string rttiType() const
    {
        return Conv< A >::rttiType();
    }
};

template< class A1, class A2 > class OpFunc2Base: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo2< A1, A2 >* >( s );
    }

    virtual void op( const Eref& e, A1 arg1, A2 arg2 )
    const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        const A1& arg1 = Conv< A1 >::buf2val( &buf );
        op( e,
            arg1, Conv< A2 >::buf2val( &buf ) );
    }

    void opVecBuffer( const Eref& e, double* buf ) const
    {
        vector< A1 > temp1 = Conv< vector< A1 > >::buf2val( &buf );
        vector< A2 > temp2 = Conv< vector< A2 > >::buf2val( &buf );
        Element* elm = e.element();
        assert( temp1.size() >= elm->numLocalData() );
        unsigned int k = 0;
        unsigned int start = elm->localDataStart();
        unsigned int end = start + elm->numLocalData();
        for ( unsigned int i = start; i < end; ++i)
        {
            unsigned int nf = elm->numField( i - start );
            for ( unsigned int j = 0; j < nf; ++j)
            {
                Eref er( elm, i, j );
                op( er, temp1[ k % temp1.size() ],
                    temp2[ k % temp2.size() ] );
                ++k;
            }
        }
    }

    virtual void opVec( const Eref& e,
                        const vector< A1 >& arg1,
                        const vector< A2 >& arg2,
                        const OpFunc2Base< A1, A2 >* op ) const
    { ; } // overridden in HopFuncs.

    string rttiType() const
    {
        return Conv< A1 >::rttiType() + "," + Conv< A2 >::rttiType();
    }
};

template< class A1, class A2, class A3 > class OpFunc3Base:
    public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo3< A1, A2, A3 >* >( s );
    }

    virtual void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3 )
    const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        const A1& arg1 = Conv< A1 >::buf2val( &buf );
        const A2& arg2 = Conv< A2 >::buf2val( &buf );
        op( e,
            arg1, arg2, Conv< A3 >::buf2val( &buf ) );
    }

    string rttiType() const
    {
        return Conv< A1 >::rttiType() + "," + Conv< A2 >::rttiType() +
               "," + Conv< A3 >::rttiType();
    }
};

template< class A1, class A2, class A3, class A4 >
class OpFunc4Base: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo4< A1, A2, A3, A4 >* >( s );
    }

    virtual void op( const Eref& e,
                     A1 arg1, A2 arg2, A3 arg3, A4 arg4 ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        const A1& arg1 = Conv< A1 >::buf2val( &buf );
        const A2& arg2 = Conv< A2 >::buf2val( &buf );
        const A3& arg3 = Conv< A3 >::buf2val( &buf );
        op( e,
            arg1, arg2, arg3, Conv< A4 >::buf2val( &buf ) );
    }

    string rttiType() const
    {
        return Conv< A1 >::rttiType() + "," + Conv< A2 >::rttiType() +
               "," + Conv<A3>::rttiType() + "," + Conv<A4>::rttiType();
    }
};

template< class A1, class A2, class A3, class A4, class A5 >
class OpFunc5Base: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo5< A1, A2, A3, A4, A5 >* >( s );
    }

    virtual void op( const Eref& e,
                     A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5 ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        const A1& arg1 = Conv< A1 >::buf2val( &buf );
        const A2& arg2 = Conv< A2 >::buf2val( &buf );
        const A3& arg3 = Conv< A3 >::buf2val( &buf );
        const A4& arg4 = Conv< A4 >::buf2val( &buf );
        op( e,
            arg1, arg2, arg3, arg4, Conv< A5 >::buf2val( &buf ) );
    }

    string rttiType() const
    {
        return Conv< A1 >::rttiType() + "," + Conv< A2 >::rttiType() +
               "," + Conv<A3>::rttiType() + "," + Conv<A4>::rttiType() +
               "," + Conv<A5>::rttiType();
    }
};

template< class A1, class A2, class A3, class A4, class A5, class A6 >
class OpFunc6Base: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return dynamic_cast< const SrcFinfo6< A1, A2, A3, A4, A5, A6 >* >( s );
    }

    virtual void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3, A4 arg4,
                     A5 arg5, A6 arg6 ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    void opBuffer( const Eref& e, double* buf ) const
    {
        const A1& arg1 = Conv< A1 >::buf2val( &buf );
        const A2& arg2 = Conv< A2 >::buf2val( &buf );
        const A3& arg3 = Conv< A3 >::buf2val( &buf );
        const A4& arg4 = Conv< A4 >::buf2val( &buf );
        const A5& arg5 = Conv< A5 >::buf2val( &buf );
        op( e,
            arg1, arg2, arg3, arg4, arg5, Conv< A6 >::buf2val( &buf ) );
    }

    string rttiType() const
    {
        return Conv< A1 >::rttiType() + "," + Conv< A2 >::rttiType() +
               "," + Conv<A3>::rttiType() + "," + Conv<A4>::rttiType() +
               "," + Conv<A5>::rttiType() + "," + Conv<A6>::rttiType();
    }
};

/**
 * This is the base class for all GetOpFuncs.
 */
template< class A > 
class GetOpFuncBase: public OpFunc1Base< vector< A >* >
{
public:
    virtual A returnOp( const Eref& e ) const = 0;

    // This returns an OpFunc1< A* > so we can pass back the arg A
    const OpFunc* makeHopFunc( HopIndex hopIndex) const;

    // This is called on the target node when a remoteGet happens.
    // It needs to do the 'get' function and stuff the data into the
    // buffer for sending back.
    void opBuffer( const Eref& e, double* buf ) const
    {
        A ret = returnOp( e );
        buf[0] = Conv<A>::size( ret );
        buf++;
        Conv< A >::val2buf( ret, &buf );
    }

};

/**
 * This is the base class for all LookupGetOpFuncs.
 */
template< class L, class A > class LookupGetOpFuncBase: public OpFunc
{
public:
    bool checkFinfo( const Finfo* s ) const
    {
        return ( dynamic_cast< const SrcFinfo1< A >* >( s )
                 || dynamic_cast< const SrcFinfo2< FuncId, L >* >( s ) );
    }

    virtual void op( const Eref& e, L index,
                     ObjId recipient, FuncId fid ) const = 0;

    virtual A returnOp( const Eref& e, const L& index ) const = 0;

    const OpFunc* makeHopFunc( HopIndex hopIndex) const
    {
        // Perhaps later we can put in something for x-node gets.
        return 0;
    }

    const OpFunc* makeHopFunc( HopIndex hopIndex, const L& index ) const
    {
        // We need to convert the index and pass it into the HopFunc
        // to pass on to target node.
        return 0;
    }

    void opBuffer( const Eref& e, double* buf ) const
    {
        // Later figure out how to handle.
    }

    string rttiType() const
    {
        return Conv< A >::rttiType();
    }
};

#endif // _OPFUNCBASE_H
