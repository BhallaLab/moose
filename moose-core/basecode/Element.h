/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2013 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _ELEMENT_H
#define _ELEMENT_H

class SrcFinfo;
class FuncOrder;

/**
 * Base class for all object lookups.
 * Provides the MOOSE interface so that it handles messaging, class info,
 * and basic fields. Does not do data.
 */
class Element
{
public:
    /**
     * This is the main constructor, used by Shell::innerCreate
     * which makes most Elements. Also used to create base
     * Elements to init the simulator in main.cpp.
     * Id is the Id of the new Element
     * Cinfo is the class
     * name is its name
     * numData is the number of data entries, defaults to a singleton.
     * The isGlobal flag specifies whether the created objects should
     * be replicated on all nodes, or partitioned without replication.
     */
    Element( Id id, const Cinfo* c, const string& name );

    /**
     * Destructor
     */
    virtual ~Element();

    /**
     * Copier
     */
    virtual Element* copyElement( Id newParent, Id newId,
                                  unsigned int n, bool toGlobal ) const = 0;
    /////////////////////////////////////////////////////////////////
    // Information access fields
    /////////////////////////////////////////////////////////////////

    /**
     * Returns name of Element
     */
    const string& getName() const;

    /**
     * Changes name of Element
     */
    void setName( const string& val );

    /// Returns number of data entries across all nodes
    virtual unsigned int numData() const = 0;

    /// Returns number of local data entries on this node.
    virtual unsigned int numLocalData() const = 0;

    /// Returns index of first data entry on this node.
    virtual unsigned int localDataStart() const = 0;

    /// Returns number of field entries for specified data
    virtual unsigned int numField( unsigned int rawIndex ) const = 0;

    /**
     * Returns number of field entries on local node. If this is not
     * a FieldElement, returns numLocalData, assuming 1 field per data.
     */
    virtual unsigned int totNumLocalField() const = 0;

    /// Returns node number of specified dataIndex.
    virtual unsigned int getNode( unsigned int dataIndex ) const = 0;

    /// Returns start dataIndex on specified node
    virtual unsigned int startDataIndex( unsigned int nodeNum ) const = 0;

    /**
     * Converts dataIndex to index on current node. No error check.
     */
    virtual unsigned int rawIndex( unsigned int dataIndex ) const = 0;

    /**
     * Returns the Id on this Elm
     */
    Id id() const;

    /**
     * True if this is a FieldElement having an array of fields
     * on each data entry. Clearly not true for the base Element.
     */
    virtual bool hasFields() const = 0;

    /// True if there is a copy of every dataEntry on all nodes.
    virtual bool isGlobal() const = 0;

    /// Returns number of data (or, if relevant, field) entries on
    ///specified node.
    virtual unsigned int getNumOnNode( unsigned int node ) const = 0;

    /**
     * Returns Clock tick used by object. -1 means none.
     * -2 means none for now because I am a zombie, but if I should
     * be unzombified please put me back on my default clock tick.
     */
    int getTick() const;

    /**
     * Assigns clock tick to be used by object. -1 means none.
     * This function does substantial message manipulation to set up
     * the message from the Clock object to the current object to
     * define the Tick. Furthermore, if the object has both init and
     * proc methods, it will set them both up, with the init method
     * on 't-1' and the proc method on 't'
     */
    void setTick( int t );

    /**
     * Direct assignment of tick field, without doing any of the
     * message manipulation
     */
    void innerSetTick( unsigned int tick );

    /////////////////////////////////////////////////////////////////
    // data access stuff
    /////////////////////////////////////////////////////////////////

    /**
     * Looks up specified field data entry. On regular objects just
     * returns the data entry specified by the rawIndex.
     * On FieldElements like synapses, does a second lookup on the
     * field index.
     * Note that the index is NOT a
     * DataIndex: it is instead the raw index of the data on the current
     * node. Index is also NOT the character offset, but the index
     * to the data array in whatever type the data may be.
     *
     * The DataIndex has to be filtered through the nodeMap to
     * find a) if the entry is here, and b) what its raw index is.
     *
     * Returns 0 if either index is out of range.
     */
    virtual char* data( unsigned int rawIndex,
                        unsigned int fieldIndex = 0 ) const = 0;

    /**
     * Changes the number of entries in the data. Not permitted for
     * FieldElements since they are just fields on the data.
     */
    virtual void resize( unsigned int newNumData ) = 0;

    /**
     * Changes the number of fields on the specified data entry.
     * Doesn't do anything for the regular Element.
     */
    virtual void resizeField(
        unsigned int rawIndex, unsigned int newNumField ) = 0;

    /////////////////////////////////////////////////////////////////


    /**
     * Pushes the Msg mid onto the list.
     * The position on the list does not matter.
     *
     */
    void addMsg( ObjId mid );

    /**
     * Removes the specified msg from the list.
     */
    void dropMsg( ObjId mid );

    /**
     * Clears out all Msgs on specified BindIndex. Used in Shell::set
     */
    void clearBinding( BindIndex b );

    /**
     * Pushes back the specified Msg and Func pair into the properly
     * indexed place on the msgBinding_ vector.
     */
    void addMsgAndFunc( ObjId mid, FuncId fid, BindIndex bindIndex );

    /**
     * gets the Msg/Func binding information for specified bindIndex.
     * This is a vector.
     * Returns 0 on failure.
     */
    const vector< MsgFuncBinding >* getMsgAndFunc( BindIndex b ) const;

    /**
     * Returns true if there are one or more Msgs on the specified
     * BindIndex
     */
    bool hasMsgs( BindIndex b ) const;

    /**
     * Utility function for printing out all fields and their values
     */
    void showFields() const;

    /**
     * Utility function for traversing and displaying all messages
     */
    void showMsg() const;

    /**
     * Rebuild digested message array; traverse all messages to do so
     */
    void digestMessages();

    /**
     * Inner function that adds targets to a single function in the
     * MsgDigest
     */
    void putTargetsInDigest(
        unsigned int srcNum, const MsgFuncBinding& mfb,
        const FuncOrder& fo,
        vector< vector< bool > >& targetNodes
    );
    /**
     * Inner function that adds off-node targets to the MsgDigest
     */
    void putOffNodeTargetsInDigest(
        unsigned int srcNum, vector< vector< bool > >& targetNodes );

    /**
     * Gets the class information for this Element
     */
    const Cinfo* cinfo() const;

    /**
     * Destroys all Elements in tree, being efficient about not
     * trying to traverse through clearing messages to doomed Elements.
     * Assumes tree includes all child elements.
     * Typically the Neutral::destroy function builds up this tree
     * and then calls this function.
     */
    static void destroyElementTree( const vector< Id >& tree );

    /**
     * Set flag to state that Element is due for destruction. Useful to
     * avoid following messages that will soon be gone.
     */
    void markAsDoomed();
    bool isDoomed() const;

    /**
     * Set flag to state that the messages on this Element have
     * changed, and need to be re-digested.
     */
    void markRewired();

    /**
     * Utility function for debugging
     */
    void printMsgDigest( unsigned int srcIndex, unsigned int dataIndex ) const;

    /**
     * Drop all messages arriving from src onto current Element,
     * regardless of which field they come from.
     */
    void dropAllMsgsFromSrc( Id src );
    /////////////////////////////////////////////////////////////////////
    // Utility functions for message traversal
    /////////////////////////////////////////////////////////////////////

    /**
     * Raw lookup into MsgDigest vector. One for each MsgSrc X ObjEntry.
     * If the messages have been rewired, this call triggers the
     * re-parsing of all messages before returning the digested msgs.
     */
    const vector< MsgDigest >& msgDigest( unsigned int index );

    /**
     * Returns the binding index of the specified entry.
     * Returns ~0 on failure.
     */
    unsigned int findBinding( MsgFuncBinding b ) const;

    /**
     * Returns all incoming Msgs.
     */
    const vector< ObjId >& msgIn() const;

    /**
     * Returns the first Msg that calls the specified Fid,
     * on current Element.
     * Returns 0 on failure.
     */
    ObjId findCaller( FuncId fid ) const;

    /**
     * More general function. Fills up vector of ObjIds that call the
     * specified Fid on current Element. Returns # found
     */
    unsigned int getInputMsgs( vector< ObjId >& caller, FuncId fid)
    const;

    /**
     * Fills in vector of Ids connected to this Finfo on
     * this Element. Returns # found
     */
    unsigned int getNeighbors( vector< Id >& ret, const Finfo* finfo )
    const;

    /**
     * Fills in vector, each entry of which identifies the src and
     * dest fields respectively.
     * Src field is local and identified by BindIndex
     * Dest field is a FuncId on the remote Element.
     */
    unsigned int getFieldsOfOutgoingMsg(
        ObjId mid, vector< pair< BindIndex, FuncId > >& ret ) const;

    /**
     * Fills in matching vectors of destination ObjIds and the
     * destination function, for the specified source Finfo and
     * dataId on this Element.
     * Used in Neutral::msgDests and Neutral::msgDestFunctions.
     * Returns number of dests found.
     */
    unsigned int getMsgTargetAndFunctions( DataId srcDataId,
                                           const SrcFinfo* finfo ,
                                           vector< ObjId >& tgt,
                                           vector< string >& func
                                         ) const;

    /**
     * Fills in matching vectors of destination ObjIds and the
     * destination function, for the specified source Finfo and
     * dataId on this Element.
     * Used in Neutral::msgDests and Neutral::msgDestFunctions.
     * Returns number of dests found.
     */
    unsigned int getMsgSourceAndSender( FuncId fid,
                                        vector< ObjId >& src,
                                        vector< string >& sender
                                      ) const;

    /**
     * Enumerates msg targets. Will get confused if some
     * are across nodes.
     */
    vector< ObjId > getMsgTargets( DataId srcDataId,
                                   const SrcFinfo* finfo  ) const;

    /** Used upon ending of MOOSE session, to rapidly clear out
     * messages, secure in the knowledge that the data structures
     * will be destroyed separately.
     */
    void clearAllMsgs();

    /**
     * zombieSwap: replaces the Cinfo and data of the zombie.
     * Allocates a new data block using zCinfo,
     * that matches the number of entries of the old data block.
     * Deletes old data.
     * The base version calls the Clock object to assign
     * a suitable default clock.
     */
    virtual void zombieSwap( const Cinfo* zCinfo );

    /// Support function for zombieSwap, replaces Cinfo.
    void replaceCinfo( const Cinfo* newCinfo );

    /**
     * Returns vectors of sources of messages to a field or data
     * element. To go with each entry, also return the field or data
     * index of _target_.
     */
    unsigned int getInputsWithTgtIndex(
        vector< pair< Id, unsigned int> >& ret, const DestFinfo* finfo)
    const;

private:
    /**
     * Fills in vector of Ids receiving messages from this SrcFinfo.
     * Returns # found
     */
    unsigned int getOutputs( vector< Id >& ret, const SrcFinfo* finfo )
    const;

    /**
     * Fills in vector of Ids sending messeges to this DestFinfo on
     * this Element. Returns # found
     */
    unsigned int getInputs( vector< Id >& ret, const DestFinfo* finfo )
    const;


    string name_; /// Name of the Element.

    Id id_; /// Stores the unique identifier for Element.

    /**
     * Class information
     */
    const Cinfo* cinfo_;

    /**
     * Message vector. This is the low-level messaging information.
     * Contains info about incoming as well as outgoing Msgs.
     */
    vector< ObjId > m_;

    /**
     * Binds an outgoing message to its function.
     * Each index (BindIndex) gives a vector of MsgFuncBindings,
     * which are just pairs of ObjId, FuncId.
     * SrcFinfo keeps track of the BindIndex to look things up.
     * Note that a single BindIndex may refer to multiple Msg/Func
     * pairs. This means that a single MsgSrc may dispatch data
     * through multiple msgs using a single 'send' call.
     */
    vector< vector < MsgFuncBinding > > msgBinding_;

    /**
     * Digested vector of message traversal sets. Each set has a
     * Func and element to lead off, followed by a list of target
     * indices and fields.
     * The indexing is like this:
     * msgDigest_[ numSrcMsgs * dataIndex + srcMsgIndex ][ func# ]
     * So we look up a vector of MsgDigests, each with a unique func,
     * based on both the dataIndex and the message number. This is
     * designed
     * so that if we expand the number of data entries we don't have
     * to redo the ordering.
     */
    vector< vector < MsgDigest > > msgDigest_;

    /// Returns tick on which element is scheduled. -1 for disabled.
    int tick_;

    /// True if messages have been changed and need to digestMessages.
    bool isRewired_;

    /// True if the element is marked for destruction.
    bool isDoomed_;
};

#endif // _ELEMENT_H
