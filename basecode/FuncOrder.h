/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
/**
 * Utility function for sorting by function pointer.
 * Used in Element::msgDigest
 */

class FuncOrder
{
public:
    FuncOrder()
        : func_( 0 ), index_( 0 )
    {;}

    const OpFunc* func() const
    {
        return func_;
    }
    unsigned int index() const
    {
        return index_;
    }

    void set( const OpFunc* func, unsigned int index )
    {
        func_ = func;
        index_ = index;
    }

    bool operator<( const FuncOrder& other ) const
    {
        return func_ < other.func_;
    }
private:
    const OpFunc* func_;
    unsigned int index_;
};
