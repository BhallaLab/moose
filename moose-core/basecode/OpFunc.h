/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _OPFUNC_H
#define _OPFUNC_H

template< class T > class OpFunc0: public OpFunc0Base
{
public:
    OpFunc0( void ( T::*func )( ) )
        : func_( func )
    {;}

    void op( const Eref& e ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)();
    }
private:
    void ( T::*func_ )( );
};

template< class T, class A > class OpFunc1: public OpFunc1Base<A>
{
public:
    OpFunc1( void ( T::*func )( A ) )
        : func_( func )
    {;}
    void op( const Eref& e, A arg ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)( arg );
    }
private:
    void ( T::*func_ )( A );
};

template< class T, class A1, class A2 > class OpFunc2:
    public OpFunc2Base< A1, A2 >
{
public:
    OpFunc2( void ( T::*func )( A1, A2 ) )
        : func_( func )
    {;}

    void op( const Eref& e, A1 arg1, A2 arg2 ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)( arg1, arg2 );
    }

private:
    void ( T::*func_ )( A1, A2 );
};

template< class T, class A1, class A2, class A3 > class OpFunc3:
    public OpFunc3Base< A1, A2, A3 >
{
public:
    OpFunc3( void ( T::*func )( A1, A2, A3 ) )
        : func_( func )
    {;}
    void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3 ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)( arg1, arg2, arg3);
    }
private:
    void ( T::*func_ )( A1, A2, A3 );
};

template< class T, class A1, class A2, class A3, class A4 > class OpFunc4:
    public OpFunc4Base< A1, A2, A3, A4 >
{
public:
    OpFunc4( void ( T::*func )( A1, A2, A3, A4 ) )
        : func_( func )
    {;}

    void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3, A4 arg4 ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)(
            arg1, arg2, arg3, arg4 );
    }

private:
    void ( T::*func_ )( A1, A2, A3, A4 );
};

template< class T, class A1, class A2, class A3, class A4, class A5 >
class OpFunc5: public OpFunc5Base< A1, A2, A3, A4, A5 >
{
public:
    OpFunc5( void ( T::*func )( A1, A2, A3, A4, A5 ) )
        : func_( func )
    {;}

    void op( const Eref& e,
             A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5 ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)(
            arg1, arg2, arg3, arg4, arg5 );
    }

private:
    void ( T::*func_ )( A1, A2, A3, A4, A5 );
};


template< class T,
          class A1, class A2, class A3, class A4, class A5, class A6 >
class OpFunc6: public OpFunc6Base< A1, A2, A3, A4, A5, A6 >
{
public:
    OpFunc6( void ( T::*func )( A1, A2, A3, A4, A5, A6 ) )
        : func_( func )
    {;}

    void op( const Eref& e, A1 arg1, A2 arg2, A3 arg3, A4 arg4,
             A5 arg5, A6 arg6 ) const
    {
        (reinterpret_cast< T* >( e.data() )->*func_)(
            arg1, arg2, arg3, arg4, arg5, arg6 );
    }

private:
    void ( T::*func_ )( A1, A2, A3, A4, A5, A6 );
};


/**
 * This specialized OpFunc is for returning a single field value
 * It generates an opFunc that takes a single argument, a reference
 * to a vector into which the return value should be appended.
 * It manages the 'get' function that retrieves the return value.
 * Note that the 'get' function must be blocking in case the request
 * goes off node.
 */
template< class T, class A > class GetOpFunc: public GetOpFuncBase< A >
{
public:
    GetOpFunc( A ( T::*func )() const ) : func_( func )
    {;}

    void op( const Eref& e, vector< A >* ret ) const
    {
        ret->push_back( returnOp( e ) );
    }

    A returnOp( const Eref& e ) const
    {
        return ( reinterpret_cast< T* >( e.data() )->*func_)();
    }

private:
    A ( T::*func_ )() const;
};

/**
 * This specialized OpFunc is for looking up a single field value
 * using a single argument.
 * It generates an opFunc that takes two arguments:
 * 1. FuncId of the function on the object that requested the value.
 * 2. Index or other identifier to do the look up.
 * The OpFunc then sends back a message with the info.
 * Here T is the class that owns the function.
 * A is the return type
 * L is the lookup index.
 */
template< class T, class L, class A > class GetOpFunc1:
    public LookupGetOpFuncBase< L, A >
{
public:
    GetOpFunc1( A ( T::*func )( L ) const )
        : func_( func )
    {;}

    /**
     * The buf just contains the funcid on the src element that is
     * ready to receive the returned data.
     * Also we are returning the data along the Msg that brought in
     * the request, so we don't need to scan through all Msgs in
     * the Element to find the right one.
     * So we bypass the usual SrcFinfo::sendTo, and instead go
     * right to the Qinfo::addToQ to send off data.
     * Finally, the data is copied back-and-forth about 3 times.
     * Wasteful, but the 'get' function is not to be heavily used.
     */
    void op( const Eref& e, L index, ObjId recipient, FuncId fid )
    const
    {
        const OpFunc *f = recipient.element()->cinfo()->getOpFunc( fid);
        const OpFunc1Base< A >* recvOpFunc =
            dynamic_cast< const OpFunc1Base< A >* >( f );
        assert( recvOpFunc );
        recvOpFunc->op( recipient.eref(), returnOp( e, index ) );
    }

    /// Distinct reduceOp with the index.
    A returnOp( const Eref& e, const L& index ) const
    {
        return ( reinterpret_cast< T* >( e.data() )->*func_)( index );
    }

private:
    A ( T::*func_ )( L ) const;
};

#endif // _OPFUNC_H
