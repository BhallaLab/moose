/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * This class sets up fields that should be accessed as independent
 * FieldElements. These fields are typically complex ones with several
 * sub-fields, and are also typically arranged in an array.
 * Examples are Synapses on an IntFire Neuron.
 *
 * The creation of this one Finfo sets up three things:
 * First, the information for the FieldElement that will handle the class.
 * Second, the set and get functions for the size of the array of Fields.
 * Third, an automatic creation of the FieldElement whenever an Element
 * of the Parent class is made. This defaults to a child, but can be moved.
 */

#ifndef _FIELD_ELEMENT_FINFO_H
#define _FIELD_ELEMENT_FINFO_H

class FieldElementFinfoBase: public Finfo
{
public:
    FieldElementFinfoBase(
        const string& name,
        const string& doc,
        const Cinfo* fieldCinfo,
        bool deferCreate
    )
        : 	Finfo( name, doc),
           setNum_( 0 ),
           getNum_( 0 ),
           fieldCinfo_( fieldCinfo ),
           deferCreate_( deferCreate )
    {
        ;
    }

    virtual ~FieldElementFinfoBase()
    {
        if ( setNum_ )
            delete setNum_;
        if ( getNum_ )
            delete getNum_;
    }

    void registerFinfo( Cinfo* c )
    {
        c->registerFinfo( setNum_ );
        c->registerFinfo( getNum_ );
        c->registerPostCreationFinfo( this );
    }

    bool strSet( const Eref& tgt, const string& field,
                 const string& arg ) const
    {
        return 0; // always fails
    }

    bool strGet( const Eref& tgt, const string& field,
                 string& returnValue ) const
    {
        return 0; // always fails
    }

    /**
     * Virtual function, defined in Finfo.h and overridden here.
     */
    void postCreationFunc( Id parent, Element* parentElm ) const;

    /**
     * virtual function to look up pointer for Field object.
     * This casts everything down to chars so that the FieldElement
     * can use it.
     */
    virtual char* lookupField( char* parent,
                               unsigned int fieldIndex ) const = 0;

    /// virtual func to resize Field array, using char* for parent
    virtual void setNumField( char* parent, unsigned int num )
    const = 0;

    /// virtual func to get size of Field array, using char* for parent
    virtual unsigned int getNumField( const char* parent )
    const = 0;

    // Virtual function to look up type of FieldElementFinfo, not
    // defined here.
    // virtual string rttiType() const = 0;

protected:
    DestFinfo* setNum_;
    DestFinfo* getNum_;
    const Cinfo* fieldCinfo_;
    bool deferCreate_;
};

template < class T, class F >
class FieldElementFinfo: public FieldElementFinfoBase
{
public:
    FieldElementFinfo(
        const string& name,
        const string& doc,
        const Cinfo* fieldCinfo,
        F* ( T::*lookupField )( unsigned int ),
        void( T::*setNumField )( unsigned int num ),
        unsigned int ( T::*getNumField )() const,
        bool deferCreate = 0
    )
        : FieldElementFinfoBase( name, doc, fieldCinfo, deferCreate ),
          lookupField_( lookupField ),
          setNumField_( setNumField ),
          getNumField_( getNumField )
    {
        string setname = "setNum" + name;
        setname[6] = std::toupper( setname[6] );
        // setNumField is a tricky operation, because it may require
        // cross-node rescaling of the
        // FieldDataHandler::fieldDimension. To acheive this we
        // wrap the setNumField in something more interesting
        // than a simple OpFunc.
        setNum_ = new DestFinfo(
            setname,
            "Assigns number of field entries in field array.",
            new OpFunc1< T, unsigned int >( setNumField ) );

        string getname = "getNum" + name;
        getname[6] = std::toupper( getname[6] );
        getNum_ = new DestFinfo(
            getname,
            "Requests number of field entries in field array."
            "The requesting Element must "
            "provide a handler for the returned value.",
            new GetOpFunc< T, unsigned int >( getNumField ) );
    }

    /**
     * virtual function to look up pointer for Field object.
     * This casts everything down to chars so that the FieldElement
     * can use it.
     */
    char* lookupField( char* parent, unsigned int fieldIndex ) const
    {
        T* pa = reinterpret_cast< T* >( parent );
        if ( fieldIndex < ( pa->*getNumField_ )() )
        {
            F* self = ( pa->*lookupField_ )( fieldIndex );
            return reinterpret_cast< char* >( self );
        }
        return 0;
    }

    /// virtual func to resize Field array, using char* for parent
    void setNumField( char* parent, unsigned int num ) const
    {
        T* pa = reinterpret_cast< T* >( parent );
        ( pa->*setNumField_ )( num );
    }

    /// virtual func to get size of Field array, using char* for parent
    unsigned int getNumField( const char* parent ) const
    {
        const T* pa = reinterpret_cast< const T* >( parent );
        return ( pa->*getNumField_ )();
    }



    /// Virtual function to look up type of FieldElementFinfo
    string rttiType() const
    {
        return Conv<F>::rttiType();
    }

private:
    F* ( T::*lookupField_ )( unsigned int );
    void( T::*setNumField_ )( unsigned int num );
    unsigned int ( T::*getNumField_ )() const;
};

#endif // _FIELD_ELEMENT_FINFO_H
