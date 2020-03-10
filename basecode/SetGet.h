/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SETGET_H
#define _SETGET_H

#ifndef NDEBUG
#include <typeinfo>
using namespace std;
#endif


// Forward declaration needed for localGet()
template< class T, class A > class GetOpFunc;

/**
 * Similar to Field< A >::fastGet(), except that an existing Msg is not needed.
 *
 * Instant-return call for a single value. Bypasses all the queueing stuff.
 * It is hardcoded so type safety will have to be coded in too:
 * the dynamic_cast will catch it only at runtime.
 *
 * Perhaps analogous localSet(), localLookupGet(), localGetVec(), etc. should
 * also be added.
 *
 * Also, will be nice to change this to Field< A >::localGet() to make things
 * more uniform.
 */
template< class T, class A >
A localGet( const Eref& er, string field )
{
    string fullFieldName = "get" + field;
    fullFieldName[3] = std::toupper( fullFieldName[3] );
    const Finfo* finfo = er.element()->cinfo()->findFinfo( fullFieldName );
    assert( finfo );

    const DestFinfo* dest = dynamic_cast< const DestFinfo* >( finfo );
    assert( dest );

    const OpFunc* op = dest->getOpFunc();
    assert( op );

    const GetOpFunc< T, A >* gop =
        dynamic_cast< const GetOpFunc< T, A >* >( op );
    assert( gop );

    return gop->reduceOp( er );
}

class SetGet
{
public:
    SetGet()
    {
        ;
    }

    virtual ~SetGet()
    {
        ;
    }

    /**
     * Utility function to check that the target field matches this
     * source type, to look up and pass back the fid, and to return
     * the number of targetEntries.
     * Tgt is passed in as the destination ObjId. May be changed inside,
     * if the function determines that it should be directed to a
     * child Element acting as a Value.
     * Checks arg # and types for a 'set' call. Can be zero to 3 args.
     * Returns # of tgts if good. This is 0 if bad.
     * Passes back found fid.
     */
    static const OpFunc* checkSet(
        const string& field, ObjId& tgt, FuncId& fid );

//////////////////////////////////////////////////////////////////////
    /**
     * Blocking 'get' call, returning into a string.
     * There is a matching 'get<T> call, returning appropriate type.
     */
    static bool strGet( const ObjId& tgt, const string& field, string& ret );

    /**
     * Blocking 'set' call, using automatic string conversion
     * There is a matching blocking set call with typed arguments.
     */
    static bool strSet( const ObjId& dest, const string& field, const string& val );

    /// Sends out request for data, and awaits its return.
    static const vector< double* >* dispatchGet(
        const ObjId& tgt, FuncId tgtFid,
        const double* arg, unsigned int size );

    virtual bool checkOpClass( const OpFunc* op ) const = 0;
};

class SetGet0: public SetGet
{
public:
    SetGet0()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field )
    {
        FuncId fid;
        ObjId tgt( dest ); // checkSet may change the tgt.
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc0Base* op =
            dynamic_cast< const OpFunc0Base* >( func );
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc0Base* hop =
                    dynamic_cast< const OpFunc0Base* >( op2 );
                assert( hop );
                hop->op( tgt.eref() );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref() );
                return true;
            }
            else
            {
                op->op( tgt.eref() );
                return true;
            }
        }
        return 0;
    }

    /**
     * Blocking call using string conversion
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        return set( dest, field );
    }

    bool checkOpClass( const OpFunc* op ) const
    {
        return dynamic_cast< const OpFunc0Base* >( op );
    }
};

template< class A > class SetGet1: public SetGet
{
public:
    SetGet1()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field, A arg )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc1Base< A >* op = dynamic_cast< const OpFunc1Base< A >* >( func );
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc1Base< A >* hop =
                    dynamic_cast< const OpFunc1Base< A >* >( op2 );
                hop->op( tgt.eref(), arg );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg );
                return true;
            }
        }
        return false;
    }

    /**
     * setVec assigns all the entries in the target Id to the
     * specified vector of values. If the target is a FieldElement
     * it assigns the entries on the specific DataIndex provided in the
     * ObjId argument.
     * The vector is used as a circular
     * buffer: if the number of targets exceeds the vector size, it
     * rolls over.
     */
    static bool setVec( ObjId destId, const string& field,
                        const vector< A >& arg )
    {
        if ( arg.size() == 0 ) return 0;

        ObjId tgt( destId );
        FuncId fid;

        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc1Base< A >* op = dynamic_cast< const OpFunc1Base< A >* >( func );
        if ( op )
        {
            const OpFunc* op2 = op->makeHopFunc(
                                    HopIndex( op->opIndex(), MooseSetVecHop ) );
            const OpFunc1Base< A >* hop =
                dynamic_cast< const OpFunc1Base< A >* >( op2 );
            hop->opVec( tgt.eref(), arg, op );
            delete op2;
            return true;
        }
        return false;
    }

    /**
     * Sets all target array values to the single value
     */
    static bool setRepeat( ObjId destId, const string& field,
                           const A& arg )
    {
        vector< A >temp ( 1, arg );
        return setVec( destId, field, temp );
    }

    /**
     * Blocking call using string conversion
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A arg;
        Conv< A >::str2val( arg, val );
        return set( dest, field, arg );
    }

    bool checkOpClass( const OpFunc* op ) const
    {
        return dynamic_cast< const OpFunc1Base< A > * >( op );
    }
};

template< class A > class Field: public SetGet1< A >
{
public:
    Field()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field, A arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet1< A >::set( dest, temp, arg );
    }

    static bool setVec( ObjId destId, const string& field,
                        const vector< A >& arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet1< A >::setVec( destId, temp, arg );
    }

    static bool setRepeat( ObjId destId, const string& field, A arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet1< A >::setRepeat( destId, temp, arg );
    }

    /**
     * Blocking call using string conversion
     */
    static bool innerStrSet( const ObjId& dest, const string& field, const string& arg )
    {
        A val;
        // Do NOT add 'set_' to the field name, as the 'set' func
        // does it anyway.
        Conv< A >::str2val( val, arg );
        return set( dest, field, val );
    }

    //////////////////////////////////////////////////////////////////
    // Returns a field value.
    static A get( const ObjId& dest, const string& field)
    {
        ObjId tgt( dest );
        FuncId fid;

        string fullFieldName = "get" + field;
        fullFieldName[3] = std::toupper( fullFieldName[3] );

        const OpFunc* func = SetGet::checkSet( fullFieldName, tgt, fid );
        const GetOpFuncBase< A >* gof = dynamic_cast< const GetOpFuncBase< A >* >( func );
        if ( gof )
        {
            if ( tgt.isDataHere() )
            {
                return gof->returnOp( tgt.eref() );
            }
            else
            {
                const OpFunc* op2 = gof->makeHopFunc(
                                        HopIndex( gof->opIndex(), MooseGetHop ) );
                const OpFunc1Base< A* >* hop =
                    dynamic_cast< const OpFunc1Base< A* >* >( op2 );
                assert( hop );
                // Blocking function.
                A ret;
                hop->op( tgt.eref(), &ret );
                delete op2;
                return ret;
            }
        }
        cout << "Warning: Field::Get conversion error for " <<
             dest.id.path() << "." << field << endl;
        return A();
    }

    /**
     * Returns a vector of values
     */
    static void getVec( ObjId dest, const string& field, vector< A >& vec)
    {

        vec.resize( 0 );
        ObjId tgt( dest );
        FuncId fid;
        string fullFieldName = "get" + field;
        fullFieldName[3] = std::toupper( fullFieldName[3] );
        const OpFunc* func = SetGet::checkSet( fullFieldName, tgt, fid );
        const GetOpFuncBase< A >* gof =
            dynamic_cast< const GetOpFuncBase< A >* >( func );
        if ( gof )
        {
            const OpFunc* op2 = gof->makeHopFunc(
                                    HopIndex( gof->opIndex(), MooseGetVecHop ) );
            const GetHopFunc< A >* hop =
                dynamic_cast< const GetHopFunc< A >* >( op2 );
            hop->opGetVec( tgt.eref(), vec, gof );
            delete op2;
            return;
        }
        cout << "Warning: Field::getVec conversion error for " <<
             dest.path() << endl;
    }

    /**
     * Blocking call for finding a value and returning in a
     * string.
     */
    static bool innerStrGet( const ObjId& dest, const string& field,
                             string& str )
    {
        Conv< A >::val2str( str, get( dest, field ) );
        return 1;
    }
};

/**
 * SetGet2 handles 2-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2 > class SetGet2: public SetGet
{
public:
    SetGet2()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field,
                     A1 arg1, A2 arg2 )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc2Base< A1, A2 >* op =
            dynamic_cast< const OpFunc2Base< A1, A2 >* >( func );
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc2Base< A1, A2 >* hop =
                    dynamic_cast< const OpFunc2Base< A1, A2 >* >( op2 );
                hop->op( tgt.eref(), arg1, arg2 );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg1, arg2 );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg1, arg2 );
                return true;
            }
        }
        return false;
    }

    /**
     * Assign a vector of targets, using matching vectors of arguments
     * arg1 and arg2. Specifically, index i on the target receives
     * arguments arg1[i], arg2[i].
     * Note that there is no requirement for the size of the
     * argument vectors to be equal to the size of the target array
     * of objects. If there are fewer arguments then the index cycles
     * back, so as to tile the target array with as many arguments as
     * we have.
     *
     * Not yet implemented correct handling for FieldElements.
     */
    static bool setVec( Id destId, const string& field,
                        const vector< A1 >& arg1, const vector< A2 >& arg2 )
    {
        ObjId tgt( destId, 0 );
        FuncId fid;
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc2Base< A1, A2 >* op =
            dynamic_cast< const OpFunc2Base< A1, A2 >* >( func );
        if ( op )
        {
            /*
            unsigned int size = tgt.element()->numData();
            // total # of entries on element and maybe to include fields
            for ( unsigned int i = 0; i < size; ++i ) {
            	Eref er( tgt.element(), i );
            	op->op( er, arg1[ i % arg1.size() ],
            					arg2[ i % arg2.size() ] );
            }
            return true;
            */
            const OpFunc* op2 = op->makeHopFunc(
                                    HopIndex( op->opIndex(), MooseSetVecHop ) );
            const OpFunc2Base< A1, A2 >* hop =
                dynamic_cast< const OpFunc2Base< A1, A2 >* >( op2 );
            hop->opVec( tgt.eref(), arg1, arg2, op );
            delete op2;
            return true;
        }
        return false;
    }

    /**
     * Blocking call using string conversion.
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A1 arg1;
        A2 arg2;
        string::size_type pos = val.find_first_of( "," );
        Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
        Conv< A2 >::str2val( arg2, val.substr( pos + 1 ) );
        return set( dest, field, arg1, arg2 );
    }
};

/**
 * LookupField handles fields that have an index arguments. Examples include
 * arrays and maps.
 * The first argument in the 'Set' is the index, the second the value.
 * The first and only argument in the 'get' is the index.
 * Here A is the type of the value, and L the lookup index.
 *
 */
template< class L, class A > class LookupField: public SetGet2< L, A >
{
public:
    LookupField( const ObjId& dest )
        : SetGet2< L, A >( dest )
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call. Identical to SetGet2::set.
     */
    static bool set( const ObjId& dest, const string& field,
                     L index, A arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet2< L, A >::set( dest, temp, index, arg );
    }

    /**
     * This setVec assigns goes through each object entry in the
     * destId, and assigns the corresponding index and argument to it.
     */
    static bool setVec( Id destId, const string& field,
                        const vector< L >& index, const vector< A >& arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet2< L, A >::setVec( destId, temp, index, arg );
    }

    /**
     * This setVec takes a specific object entry, presumably one with
     * an array of values within it. The it goes through each specified
     * index and assigns the corresponding argument.
     * This is a brute-force assignment.
     */
    static bool setVec( ObjId dest, const string& field,
                        const vector< L >& index, const vector< A >& arg )
    {
        string temp = "set" + field;
        temp[3] = std::toupper( temp[3] );
        return SetGet2< L, A >::setVec( dest, temp, index, arg );
    }

    /**
     * Faking setRepeat too. Just plugs into setVec.
     */
    static bool setRepeat( Id destId, const string& field,
                           const vector< L >& index, A arg )
    {
        vector< A > avec( index.size(), arg );
        return setVec( destId, field, index, avec );
    }

    /**
     * Blocking call using string conversion
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& indexStr, const string& val )
    {
        L index;
        Conv< L >::str2val( index, indexStr );

        A arg;
        // Do NOT add 'set_' to the field name, as the 'set' func
        // does it anyway.
        Conv< A >::str2val( arg, val );
        return set( dest, field, index, arg );
    }

    /**
     * Gets a value on a specific object, looking it up using the
     * provided index.
     */
    // Returns a field value.
    static A get( const ObjId& dest, const string& field, L index )
    {
        ObjId tgt( dest );
        FuncId fid;
        string fullFieldName = "get" + field;
        fullFieldName[3] = std::toupper( fullFieldName[3] );
        const OpFunc* func = SetGet::checkSet( fullFieldName, tgt, fid);
        const LookupGetOpFuncBase< L, A >* gof =
            dynamic_cast< const LookupGetOpFuncBase< L, A >* >( func );
        if ( gof )
        {
            if ( tgt.isDataHere() )
            {
                return gof->returnOp( tgt.eref(), index );
            }
            else
            {
                /*
                const OpFunc* op2 = gof->makeHopFunc(
                HopIndex( gof->opIndex(), MooseGetHop ), index );
                const OpFunc1Base< A* >* hop =
                dynamic_cast< const OpFunc1Base< A* >* >( op2 );
                */
                cout << "Warning: LookupField::get: cannot cross nodes yet\n";
                return A();
                /*
                assert( hop );
                // Blocking function.
                hop->op( tgt.eref(), &ret );
                delete op2;
                return ret;
                */
            }
        }
        cout << "LookupField::get: Warning: Field::Get conversion error for " <<
             dest.id.path() << "." << field << endl;
        return A();
    }

    /**
     * Blocking call that returns a vector of values in vec.
     * This variant goes through each target object entry on dest,
     * and passes in the same lookup index to each one. The results
     * are put together in the vector vec.
     */
    static void getVec( Id dest, const string& field,
                        vector< L >& index, vector< A >& vec )
    {
        vec.resize( 0 );
        ObjId tgt( dest );
        FuncId fid;
        string fullFieldName = "get" + field;
        fullFieldName[3] = std::toupper( fullFieldName[3] );
        const OpFunc* func = SetGet::checkSet( fullFieldName, tgt, fid );
        const LookupGetOpFuncBase< L, A >* gof =
            dynamic_cast< const LookupGetOpFuncBase< L, A >* >( func );
        if ( gof )
        {
            Element* elm = dest.element();
            unsigned int size = vec.size(); // temporary. See SetVec.
            vec.resize( size );
            for ( unsigned int i = 0; i < size; ++i )
            {
                Eref e( elm, i );
                vec[i] = gof->returnOp( e, index );
            }
            return;
        }
        cout << "Warning: Field::getVec conversion error for " <<
             dest.path() << endl;
    }

    /**
     * Blocking virtual call for finding a value and returning in a
     * string.
     */
    static bool innerStrGet( const ObjId& dest, const string& field,
                             const string& indexStr, string& str )
    {
        L index;
        Conv< L >::str2val( index, indexStr );

        A ret = get( dest, field, index );
        Conv<A>::val2str( str, ret );
        return 1;
    }
};

/**
 * SetGet3 handles 3-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3 > class SetGet3: public SetGet
{
public:
    SetGet3()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field,
                     A1 arg1, A2 arg2, A3 arg3 )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc3Base< A1, A2, A3 >* op =
            dynamic_cast< const OpFunc3Base< A1, A2, A3 >* >( func);
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc3Base< A1, A2, A3 >* hop =
                    dynamic_cast< const OpFunc3Base< A1, A2, A3 >* >(
                        op2 );
                hop->op( tgt.eref(), arg1, arg2, arg3 );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg1, arg2, arg3 );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg1, arg2, arg3 );
                return true;
            }
        }
        return 0;
    }

    /**
     * Blocking call using string conversion.
     * As yet we don't have 2 arg conversion from a single string.
     * So this is a dummy
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A1 arg1;
        A2 arg2;
        A3 arg3;
        string::size_type pos = val.find_first_of( "," );
        Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
        string temp = val.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A2 >::str2val( arg2, temp.substr( 0,pos ) );
        Conv< A3 >::str2val( arg3, temp.substr( pos + 1 ) );
        return set( dest, field, arg1, arg2, arg3 );
    }
};

/**
 * SetGet4 handles 4-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4 > class SetGet4: public SetGet
{
public:
    SetGet4()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field,
                     A1 arg1, A2 arg2, A3 arg3, A4 arg4 )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc4Base< A1, A2, A3, A4 >* op =
            dynamic_cast< const OpFunc4Base< A1, A2, A3, A4 >* >( func);
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc4Base< A1, A2, A3, A4 >* hop =
                    dynamic_cast< const OpFunc4Base< A1, A2, A3, A4 >* >( op2 );
                hop->op( tgt.eref(), arg1, arg2, arg3, arg4 );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg1, arg2, arg3, arg4 );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg1, arg2, arg3, arg4 );
                return true;
            }
        }
        return 0;
    }

    /**
     * Blocking call using string conversion.
     * As yet we don't have 2 arg conversion from a single string.
     * So this is a dummy
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A1 arg1;
        A2 arg2;
        A3 arg3;
        A4 arg4;
        string::size_type pos = val.find_first_of( "," );
        Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
        string temp = val.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
        Conv< A4 >::str2val( arg4, temp.substr( pos + 1 ) );
        return set( dest, field, arg1, arg2, arg3, arg4 );
    }
};

/**
 * SetGet5 handles 5-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4, class A5 > class SetGet5:
    public SetGet
{
public:
    SetGet5()
    {
        ;
    }

    static bool set( const ObjId& dest, const string& field,
                     A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5 )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc5Base< A1, A2, A3, A4, A5 >* op =
            dynamic_cast< const OpFunc5Base< A1, A2, A3, A4, A5 >* >( func);
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc5Base< A1, A2, A3, A4, A5 >* hop =
                    dynamic_cast< const OpFunc5Base< A1, A2, A3, A4, A5 >* >( op2 );
                hop->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5 );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5 );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5 );
                return true;
            }
        }
        return 0;
    }

    /**
     * Blocking call using string conversion.
     * As yet we don't have 2 arg conversion from a single string.
     * So this is a dummy
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A1 arg1;
        A2 arg2;
        A3 arg3;
        A4 arg4;
        A5 arg5;
        string::size_type pos = val.find_first_of( "," );
        Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
        string temp = val.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A4 >::str2val( arg4, temp.substr( 0, pos ) );
        Conv< A5 >::str2val( arg5, temp.substr( pos + 1 ) );
        return set( dest, field, arg1, arg2, arg3, arg4, arg5 );
    }
};

/**
 * SetGet6 handles 6-argument Sets. It does not deal with Gets.
 */
template< class A1, class A2, class A3, class A4, class A5, class A6 > class SetGet6:
    public SetGet
{
public:
    SetGet6()
    {
        ;
    }

    /**
     * Blocking, typed 'Set' call
     */
    static bool set( const ObjId& dest, const string& field,
                     A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6 )
    {
        FuncId fid;
        ObjId tgt( dest );
        const OpFunc* func = checkSet( field, tgt, fid );
        const OpFunc6Base< A1, A2, A3, A4, A5, A6 >* op =
            dynamic_cast< const OpFunc6Base< A1, A2, A3, A4, A5, A6 >* >( func);
        if ( op )
        {
            if ( tgt.isOffNode() )
            {
                const OpFunc* op2 = op->makeHopFunc(
                                        HopIndex( op->opIndex(), MooseSetHop ) );
                const OpFunc6Base< A1, A2, A3, A4, A5, A6 >* hop =
                    dynamic_cast< const OpFunc6Base< A1, A2, A3, A4, A5, A6 >* >( op2 );
                hop->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5, arg6 );
                delete op2;
                if ( tgt.isGlobal() )
                    op->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5, arg6 );
                return true;
            }
            else
            {
                op->op( tgt.eref(), arg1, arg2, arg3, arg4, arg5, arg6);
                return true;
            }
        }
        return 0;
    }

    /**
     * Blocking call using string conversion.
     */
    static bool innerStrSet( const ObjId& dest, const string& field,
                             const string& val )
    {
        A1 arg1;
        A2 arg2;
        A3 arg3;
        A4 arg4;
        A5 arg5;
        A6 arg6;
        string::size_type pos = val.find_first_of( "," );
        Conv< A1 >::str2val( arg1, val.substr( 0, pos ) );
        string temp = val.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A2 >::str2val( arg2, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A3 >::str2val( arg3, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A4 >::str2val( arg4, temp.substr( 0, pos ) );
        temp = temp.substr( pos + 1 );
        pos = temp.find_first_of( "," );
        Conv< A5 >::str2val( arg5, temp.substr( 0, pos ) );
        Conv< A6 >::str2val( arg6, temp.substr( pos + 1 ) );
        return set( dest, field, arg1, arg2, arg3, arg4, arg5, arg6 );
    }
};

#endif // _SETGET_H
