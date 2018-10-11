/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SHELL_H
#define _SHELL_H

#include <string>
using namespace std;

class DestFinfo;

enum AssignmentType { SINGLE, VECTOR, REPEAT };

// These Finfo objects are exposed to other classes for convenience in test cases and other functions.
// NOTE: These should never be re-registered in an initCinfo of another class.
//extern SrcFinfo4< Id, DataId, FuncId, unsigned int >* requestGet(); // Not available
extern DestFinfo* receiveGet();
//extern SrcFinfo2< unsigned int, unsigned int >* ack(); // Not currently used.

enum NodePolicy { MooseGlobal, MooseBlockBalance, MooseSingleNode };
class NodeBalance
{
public:
    NodeBalance( unsigned int nd, NodePolicy np, unsigned int node )
        : numData( nd ), policy( np ), preferredNode( node )
    {
        ;
    }

    unsigned int numData;
    NodePolicy policy;
    unsigned int preferredNode;
};

class Shell
{
public:
    Shell();
    ~Shell();

#ifdef  CYMOOSE

    /**
     * @brief Initialize shell.
     *
     * @return Pointer to shell.
     *
     * This function initialize shell and returns a pointer to it.
     * This function must create a fully functional shell which can
     * be used by cython interface.
     */
    Shell* initShell();


    /**
     * @brief A thin wrapper around doCreate function. Used in
     * cython interface.
     *
     * @param type Type of Moose-element to be created e.g. Table,
     * Compartment, etc.
     * @param parent Parent element under which this element is
     * being created.
     * @param name Name of the element. String.
     * @param numData
     * @param nodePolicy
     * @param preferredNode
     *
     * @return Id of the element.
     */
    Id create( string type, string name, unsigned int numData,
               NodePolicy nodePolicy = MooseBlockBalance,
               unsigned int preferredNode = 1 );


#endif     /* -----  CYMOOSE  ----- */
    ///////////////////////////////////////////////////////////
    // Field functions
    ///////////////////////////////////////////////////////////
    /**
     * Returns version number of the software.
     */
    string doVersion();

    /**
     * Assigns the current working Element of the Shell
     */
    void setCwe( ObjId cwe );

    /**
     * Returns the current working Element of the Shell
     */
    ObjId getCwe() const;

    /**
     * Returns flag to indicate whether simulation is still running
     */
    bool isRunning() const;

    ///////////////////////////////////////////////////////////
    // Parser functions
    ///////////////////////////////////////////////////////////

    /**
     * Create an Element. Returns its id.
     * type: Specifies classname of Objects in Element.
     * parent: Id of parent element
     * name: Name to be used for identifying Element.
     * numData: Size of array.
     */
    Id doCreate( string type, ObjId parent, string name,
                 unsigned int numData,
                 NodePolicy nodePolicy = MooseBlockBalance,
                 unsigned int preferredNode = 1 );

    /**
     * Delete specified Element and all its children and all
     * Msgs connected to it. This also works for Msgs, which are
     * also identified by an ObjId. Unlike regular objects, only
     * the one Msg entry specified by the DataIndex part of the ObjId
     * argument is deleted.
     */
    bool doDelete( ObjId oid );

    /**
     * Sets up a Message of specified type.
     * Later need to consider doing this through MsgSpecs only.
     * Here the 'args' vector handles whatever arguments we may need
     * to pass to the specified msgType.
     */
    ObjId doAddMsg( const string& msgType,
                    ObjId src, const string& srcField,
                    ObjId dest, const string& destField );

    /**
     * Cleanly quits simulation, wrapping up all nodes and threads.
     */
    void doQuit( );

    /**
     * Starts off simulation, to run for 'runtime' more than current
     * time. This version is blocking, and returns only when the
     * simulation is done. If `nofity = true' then also notify user
             * whenever 10\% of simulation is over.
     */
    void doStart( double runtime, bool notify = false );

    /**
     * Starts off simulation, to run for 'runtime' more than current
     * time. This version returns at once, and the parser can go
     * on to do other things. It has to check with the
     * Shell::isRunning function (accessible as a MOOSE field)
     * to find out if it is finished. Can call 'doStop', 'doTerminate'
     * or 'doReinit' at any time to stop the run with increasing
     * levels of prejudice.
     */
    void doNonBlockingStart( double runtime );

    /**
     * Reinitializes simulation: time goes to zero, all scheduled
     * objects are set to initial conditions. If simulation is
     * already running, first stops it.
     */
    void doReinit();

    /**
     * Cleanly stops simulation, ready to take up again from where
     * the stop occurred. Waits till current operations are done.
     */
    void doStop();

    /**
     * Terminate ongoing simulation, with prejudice.
     * Uncleanly stops simulation. Things may be in a mess with
     * different objects at different times, but it stops at once.
     */
    void doTerminate();

    /**
     * shifts orig Element (including offspring) to newParent. All old
     * hierarchy, data, Msgs etc are preserved below the orig.
     */
    void doMove( Id orig, ObjId newParent );

    /**
     * Copies orig Element to newParent. n specifies how many copies
     * are made.
     * copyExtMsgs specifies whether to also copy messages from orig
     * to objects outside the copy tree. Usually we don't do this.
     */
    Id doCopy( Id orig, ObjId newParent, string newName,
               unsigned int n, bool toGlobal, bool copyExtMsgs );

    /**
     * Looks up the Id specified by the given path. May include
     * relative references and the internal cwe
     * (current working Element) on the shell
     */
    ObjId doFind( const string& path ) const;

    /**
     * Connects up process messages from the specified Tick to the
     * targets on the path. Does so for whole Elements, not individual
     * entries in the Element array.
     * The target on the path usually has the 'process' field but
     * other options are allowed, like 'init'
     */
    void doUseClock( string path, string field, unsigned int tick );

    /**
     * Loads in a model to a specified path.
     * Tries to figure out model type from fname or contents of file.
     * Currently knows about kkit, cspace.
     * Soon to learn .p, SBML, NeuroML.
     * Later to learn NineML
     */
    Id doLoadModel( const string& fname, const string& modelpath,
                    const string& solverClass = "" );

    /**
     * Saves specified model to specified file, using filetype
     * identified by filename extension. Currently known filetypes are:
     * .g: Kkit model
     *
     * Still to come:
     * .p: GENESIS neuron morphology and channel spec file
     * .sbml: SBML file
     * .nml: NeuroML file
     * .9ml: NineML file
     * .snml: SigNeurML
     */
    void doSaveModel( Id model, const string& fileName, bool qflag = 0 ) const;

    /**
     * This function synchronizes fieldDimension on the DataHandler
     * across nodes. Used after function calls that might alter the
     * number of Field entries in the table..
     * The tgt is the FieldElement whose fieldDimension needs updating.
     */
    void doSyncDataHandler( Id tgt );

    /**
     * This function builds a reac-diffusion mesh starting at the
     * specified ChemCompt, which houses MeshEntry FieldElements.
     * Assumes that the dimensions of the baseCompartment have just been
     * redefined, and we now need to go through and update the child
     * reaction system.
     */
    void doReacDiffMesh( Id baseCompartment );

    /**
     * This function is called by the parser to tell the ProcessLoop
     * to wait a bit between cycles. Used when we are waiting for user
     * input and there is no point in having the ProcessLoop go at
     * full speed. When flag is true, then the ProcessLoop will sleep
     * a bit, when false it will work at full speed.
     */
    void doSetParserIdleFlag( bool isParserIdle );

    /**
     * Works through internal queue of operations that modify the
     * structure of the simulation. These operations have to be
     * carefully separated from any other functions or messaging,
     * so this happens while all other threads are blocked.
     */
    static void clearRestructuringQ();
    ///////////////////////////////////////////////////////////
    // DestFinfo functions
    ///////////////////////////////////////////////////////////

    /**
     * Sets of a simulation for duration runTime. Handles
     * cases including single-thread, multithread, and multinode
     */
    void start( double runTime );

    /**
     * Initialize acks. This call should be done before the 'send' goes
     * out, because with the wonders of threading we might get a
     * response to the 'send' before this call is executed.
     * This MUST be followed by a waitForAck call.
     */
    void initAck();

    /**
     * test for completion of request. This MUST be preceded by an
     * initAck call.
     */
    void waitForAck();

    /**
     * Generic handler for ack msgs from various nodes. Keeps track of
     * which nodes have responded.
     */
    void handleAck( unsigned int ackNode, unsigned int status );

    /**
     * Test for receipt of acks from all nodes
     */
    bool isAckPending() const;

    /**
     * Wraps up operations. Doesn't quit instantly, completes the
     * current process cycle first.
     */
    void handleQuit();

    void handleCreate( const Eref& e,
                       string type, ObjId parent, Id newElm, string name,
                       NodeBalance nb, unsigned int parentMsgIndex );
    void destroy( const Eref& e, ObjId oid);

    /**
     * Function that does the actual work of creating a new Element.
     * The Class of the Moose objects formed is specified by type.
     * The NodeBalance specifies how many entries and how they are
     * distributed across nodes.
     * The parentMsgIndex specifies the index for the
     * parent-child message.
     */
    void innerCreate( string type, ObjId parent, Id newElm, string name,
                      const NodeBalance& nb, unsigned int parentMsgIndex );

    /// Does actual work of copying. Returns true on success.
    bool innerCopy( const vector< ObjId >& args, const string& newName,
                    unsigned int n, bool toGlobal, bool copyExtMsgs );

    /**
     * Connects src to dest on appropriate fields, with specified
     * msgType.
     * This inner function does NOT send an ack. Returns true on
     * success
     */
    const Msg* innerAddMsg( string msgType, ObjId src, string srcField,
                            ObjId dest, string destField, unsigned int msgIndex );

    /**
     * Connects src to dest on appropriate fields, with specified
     * msgType.
     * This wrapper function sends the ack back to the master node.
     */
    void handleAddMsg( const Eref& e,
                       string msgType,
                       ObjId src, string srcField,
                       ObjId dest, string destField,
                       unsigned int msgIndex );

    /**
     * Moves Element orig onto the newParent.
     */
    bool innerMove( Id orig, ObjId newParent );

    /**
     * Handler to move Element orig onto the newParent.
     */
    void handleMove( const Eref& e,
                     Id orig, ObjId newParent );

    /**
     * Handles sync of DataHandler indexing across nodes
     */
    void handleSync( const Eref& e, Id elm, FuncId fid);

    /**
     * Deep copy of source element to target, renaming it to newName.
     * The Args are orig, newParent, newElm
     * where the newElm is the Id passed in for the root of the copy.
     * All subsequent created Elements should have successive Ids.
     * The copy may generate an array with n entries.
     * Normally only copies msgs within the tree, but if the flag
     * copyExtMsgs is true then it copies external Msgs too.
     */
    void handleCopy( const Eref& e,
                     vector< ObjId > args, string newName, unsigned int n,
                     bool toGlobal, bool copyExtMsgs );

    /**
     * Sets up scheduling for elements on the path.
     */
    bool innerUseClock( string path, string field,
                        unsigned int tick, unsigned int msgIndex);

    void handleUseClock( const Eref& e,
                         string path, string field, unsigned int tick,
                         unsigned int msgIndex );

    /**
     * Utility function to set up messages to schedule a list of Ids
     * using the specified field and tick
     */
    void addClockMsgs( const vector< ObjId >& list,
                       const string& field, unsigned int tick, unsigned int msgIndex );

    /**
     * Utility function to unschedule the specified elist operating
     * on the specified field, typically 'process'
     */
    static void dropClockMsgs(
        const vector< ObjId >& list, const string& field );

    ////////////////////////////////////////////////////////////////
    // Thread and MPI handling functions
    ////////////////////////////////////////////////////////////////

    /**
     * Assigns the hardware availability. Assumes that each node will
     * have the same number of cores available.
     */
    static void setHardware(
        unsigned int numCores, unsigned int numNodes,
        unsigned int myNode );

    static unsigned int myNode();
    static unsigned int numNodes();
    static unsigned int numCores();
    static unsigned int numProcessThreads();

    /**
     * Stub for eventual function to handle load balancing. This must
     * be called to set up default groups.
     */
    static void loadBalance();

    static void launchParser();

    /**
     * True when the parser is in a call which is being blocked because
     * it requires the event loop to complete some actions.
     */
    static bool inBlockingParserCall();

    /**
     * True in single-threaded mode. This is a special mode of
     * the system in which it does not start up the event loop
     * at all, and the whole thing operates on one thread, which
     * is ultimately under the control of the parser.
     * Note that this is distinct from running on one core. It is
     * possible, and even recommended, to run in multithread mode
     * even when the system has just one core to run it on.
     */
    static bool isSingleThreaded();

    /**
     * True as long as the main process loop is looping
     */
    static bool keepLooping();

    /**
     * Flag to indicate if the parser is idle. If so, the main
     * ProcessLoop should also slow down to avoid pounding on the CPUs
     */
    static bool isParserIdle();

    /**
     * This function sets up the threading for the entire system.
     * It creates all the worker threads and the threads for
     * handling MPI and handling shell requests.
     */
    static void launchThreads();

    /**
     * Checks for highest 'val' on all nodes
     */
    static unsigned int reduceInt( unsigned int val );

    ////////////////////////////////////////////////////////////////
    // Sets up clock ticks. Essentially is a call into the
    // Clock::setupTick function, but may be needed to be called from
    // the parser so it is a Shell function too.
    void doSetClock( unsigned int tickNum, double dt );

    // Should set these up as streams so that we can build error
    // messages similar to cout.
    void warning( const string& text );
    void error( const string& text );

    static const Cinfo* initCinfo();

    ////////////////////////////////////////////////////////////////
    // Utility functions
    ////////////////////////////////////////////////////////////////
    static bool adopt( ObjId parent, Id child, unsigned int msgIndex );
    static bool adopt( Id parent, Id child, unsigned int msgIndex );

    static const unsigned int OkStatus;
    static const unsigned int ErrorStatus;

    // Initialization function, used only in main.cpp:init()
    void setShellElement( Element* shelle );

    /// Static func for returning the ProcInfo of the shell.
    static const ProcInfo* procInfo();

    const ProcInfo* getProcInfo( unsigned int index ) const;

    /**
     * Chops up the names in the string into the vector of strings,
     * using the specified separator.
     * Returns true if it is an absolute path, that is, starts with the
     * separator.
     */
    static bool chopString( const string& path, vector< string >& ret,
                            char separator = '/' );

    /**
     * Checks that the provided name is valid for an object.
     * This returns false if it finds the reserved path chars /#[]
     */
    static bool isNameValid( const string& name );

    /**
     * Chop up the path into a vector of Element names, and
     * also fills out a matching vector of indices. If at any level of
     * the path there are no indices or the index is zero, the
     * index entry * remains empty. Otherwise the entry contains a
     * vector with index values for this level of the path.
     * The zeroth position of this index vector is the slowest
     * varying, i.e., most significant.
     * Returns true if it starts at '/'.
     *
     * Example: /foo/bar[10]/zod[3][4][5] would return:
     * ret: {"foo", "bar", "zod" }
     * index: { {}, {10}, {3,4,5} }
     */
    static bool chopPath( const string& path, vector< string >& ret,
                          vector< unsigned int >& index );

    // static void wildcard( const string& path, vector< Id >& list );

    /**
     * Cleans up all Elements except /root itself, /clock, /classes,
     * and /Msgs.
     * In due course will also do suitable reinitialization of
     * tick and other values.
     */
    static void cleanSimulation();

    /**
     * set the gettingVector_ flag
     */
    void expectVector( bool flag );

private:
    Element* shelle_; // It is useful for the Shell to have this.

    /**
     * Buffer into which return values from the 'get' command are placed
     * Only index 0 is used for any single-value 'get' call.
     * If it was the 'getVec' command then the array is filled up
     */
    vector< double* > getBuf_;

    /**
     * Flag, used by the 'get' subsystem which maintains a buffer for
     * returned value. True when the returned value is a vector.
     */
    bool gettingVector_;

    /**
     * Counter, used by the 'get' subsystem in order to keep track of
     * number of returned values, especially for getVec.
     */
    unsigned int numGetVecReturns_;

    /**
     * Flag: True when the parser thread is blocked waiting for
     * some system call to be handled by the threading and the
     * MPI connected nodes.
     */
    static bool isBlockedOnParser_;

    /**
     * Flag: Tells the ProcessLoop to keep on going.
     * Should only be altered during a barrier.
     */
    static bool keepLooping_;

    /**
     * Number of CPU cores in system.
     */
    static unsigned int numCores_;

    /**
     * Number of threads dedicated to the ProcessLoop.
     * The parser thread is the master thread.
     * Additional threads may be created for graphics, but not now.
     */
    static unsigned int numProcessThreads_;

    /**
     * Number of nodes in MPI-based system. Each node may have many
     * threads.
     */
    static unsigned int numNodes_;

    /**
     * Identifier for current node
     */
    static unsigned int myNode_;

    /**
     * Shell owns its own ProcInfo, has global thread/node info.
     * Used to talk to parser and for thread specification in
     * setup operations.
     */
    static ProcInfo p_;

    static vector< ProcInfo > threadProcs_;

    /**
     * Array of threads, initialized in launchThreads.
     */

    static unsigned int numAcks_;
    static vector< unsigned int > acked_;


    /**
     * Flag to tell system to reinitialize. We use this to defer the
     * actual operation to the 'process' call for a clean reinit.
     */
    static bool doReinit_;

    /**
     * Simulation run time
     */
    static double runtime_;

    static bool isParserIdle_;

    /// Current working Element
    ObjId cwe_;
};

/*
extern bool set( Eref& dest, const string& destField, const string& val );

extern bool get( const Eref& dest, const string& destField );
*/

#endif // _SHELL_H
