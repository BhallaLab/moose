/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _EREF_H
#define _EREF_H

class Eref
{
public:

    friend ostream& operator <<( ostream& s, const Eref& e );
    Eref();
    Eref( Element* e, unsigned int index, unsigned int field = 0 );

    /**
     * Returns data entry.
     */
    char* data() const;

    /**
     * Returns Element part
     */
    Element* element() const
    {
        return e_;
    }

    /**
     * Returns data index, for now typecast to unsigned int.
     */
    unsigned int dataIndex() const
    {
        return i_;
    }

    /**
     * Returns field index, for now typecast to unsigned int.
     * This is really an extra bit of baggage used here not for
     * immediate lookup, but to pass to EpFuncs that know
     * how to use this for lookup.
     */
    unsigned int fieldIndex() const
    {
        return f_;
    }

    /**
     * Returns the ObjId corresponding to the Eref. All info is kept.
     */
    ObjId objId() const;

    /**
     * Returns the Id corresponding to the Eref. Loses information.
     */
    Id id() const;

    /**
     * Returns the digested version of the specified msgsrc. If the
     * message has changed, this call triggers the digestion operation.
     */
    const vector< MsgDigest >& msgDigest(unsigned int bindIndex ) const;

    /**
     * True if the data are on the current node
     */
    bool isDataHere() const;

    /**
     * Returns node upon which specified data entry resides .
     * If this is a GlobalElement then it returns the local node.
     */
    unsigned int getNode() const;
private:
    Element* e_;
    unsigned int i_;
    unsigned int f_;
};

#endif // _EREF_H
