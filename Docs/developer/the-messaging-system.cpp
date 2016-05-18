/**
 * \page messagingSystem The Messaging System
 *
 * \section Intro Introduction
 *
 * The messaging system is central to the way moose works. Any understanding
 * of the internals of moose must start with the messaging framework.
 *
 * The framework essentially allows "moose objects" to "send" messages to or
 * "receive" messages from each other. The following sections expand on the
 * exact implementation of sending and receiving messages, both from a C++
 * programmer's perspective as well as from a python programmer's perspective.
 *
 * (TODO: add more messaging system philosophy)
 *
 * \section mooseObjects Moose Objects
 *
 * A moose object is an instance of a moose class. A moose class is a C++ class
 * that has a Cinfo object representing it. Cinfo objects are class
 * descriptors. They describe the fields that classes want to expose in the
 * python script in one way or another.
 *
 * The fields that go into python are of three main types:
 * - Value fields: the sort that are like a simple variable that can be
 *   changed as and when desired - a "public" data element on a python class
 * - Source fields: A source of messages. These fields can be used to send
 *   data to other moose objects
 * - Destination fields: A destination for messages. These are fields that act
 *   as recepients of messages sent by source fields.
 *
 * In an ordinary C++ class, there is no distinction between different class
 * members. In order to create the aforementioned classification of class
 * members into various field types, there is a need to use Finfo objects: the
 * so-called "field descriptors".
 *
 * Consider the example of the simple class, Example:
 *
 * \verbatim
    class Example {

    private:
        int x_;

    public:

        int getX() { return x_; }
        void setX( int x ) { x_ = x; }

        static const Cinfo *initCinfo() {
            static ValueFinfo< Example, int > x(
                "x",
                "An example field of an example class",
                &Example::getX,
                &Example::setX
            );
            static Finfo *exampleFinfos[] = { &x };
            static Cinfo exampleCinfo(
                "Example",              // The name of the class in python
                Neutral::initCinfo(),   // TODO
                exampleFinfos,          // The array of Finfos created above
                1,                      // The number of Finfos
                new Dinfo< Example >()  // The class Example itself (FIXME ?)
        }

    }; \endverbatim
 *
 * Example shows you how you can create a value field. The initCinfo function
 * here could have been called anything. It merely does the job of creating a
 * Cinfo object for the class. This is typically the case throughout moose. The
 * ValueFinfo object takes the class and the value's data type as template
 * arguments, as shown. The initialization parameters are the name of the
 * class member in python, the python docstring for the member and the
 * addresses of the set and get functions used to access and modify the said
 * value field.
 *
 * But this alone is not enough. We have not yet created a Cinfo *object*
 * corresponding to this class. The Cinfo object can be created in any of the
 * files in the project, but it is usually created below the respective
 * initCinfo function's definition. In this case, the object would be
 * instantiated in a manner such as:
 * \verbatim
    static const Cinfo *exampleCinfo = Example::Cinfo(); \endverbatim
 *
 * This creates a Cinfo object in the same file which is picked up by pymoose
 * during compilation. Example is then made into an actual python class,
 * accessible as moose.Example (provided that the directory under which these
 * files are located is included in the main moose Makefile for compilation).
 *
 * Note the importance of the "static" storage class specifier throughout this
 * example.
 *
 * Any class that has such a Cinfo object described after it is considered to
 * have been upgraded from a C++ class into a moose class.
 *
 * It helps to have moose classes rather than C++ classes, because they
 * provide a mechanism for introspection. For example, you can "ask" a moose
 * object what its fields are. In fact, you can be even more specific and ask
 * it to tell you only its value fields, source fields or destination fields.
 *
 * \section sendingAndReceiving Sending and receiving
 *
 * Sending and receiving messages in moose is accomplished through source and
 * destination fields respectively. Once again, in order to designate a field
 * as a source or destination field, it is necessary to use an Finfo object.
 *
 * The trials directory in the moose buildQ branch gives an excellent example
 * of how to define simple source and destination Finfos.
 *
 * \verbatim
    static SrcFinfo1< double > XOut( "XOut", "Value of random field X" );
    static DestFinfo handleX( "handleX",
            "Prints out X as and when it is received",
            new OpFunc1< Receiver, double >( &Receiver::handleX )
    ); \endverbatim
 *
 * The source Finfo is defined within a function that returns the address of
 * the Finfo. This is done because the same function is called in order to use
 * the send() method of the source Finfo that activates the sending of the
 * message.
 *
 * Notice that the source Finfo is defined using the class "SrcFinfo1". The 1
 * indicates the number of variables being sent across. It is also the number
 * of template arguments that have to be supplied and the number of extra
 * parameters that go into the send() call. Sender::process() calls
 * XOut()->send( e, pthreadIndexInGroup, X_ ). The X_ here is the variable
 * being sent across. There's only one variable being sent, which is why we
 * use an SrcFinfo1. For another example, one can take a look at
 * biophysics/Compartment.cpp. Here, we need to send out two variables, so we
 * use an SrcFinfo2 class. The sending function is defined as
 * raxialOut()->send( e, pthreadIndexInGroup, Ra_, Vm_ ) to send out Ra_ and
 * Vm_. In such a manner, upto six variables can be sent out in a single
 * message.
 *
 * The destination field is defined by a handler function which is held by the
 * OpFunc class. The handler should be able to take as many variables as the
 * source field sends out. So OpFunc can also take upto six template arguments.
 * The actual handler function (be it handleX or handleRaxial) takes these many
 * values as arguments (in the same order).
 *
 * More information regarding OpFunc-like classes can be found in the
 * Programmers Guide.
 *
 * \section creatingConnections Creating connections
 *
 * So far we have taken a look at how sources and destinations are made, but
 * not at how they are actually connected. There is as yet no information
 * designating which destinations a source field is supposed to send messages
 * to when their send() method is called.
 *
 * In order to find out more about how connections are made (and also about how
 * pymoose can be used) read the pymoose walkthrough in the user documentation.
 * In the trials example, we created the connection in test_trials.py with:
 * \verbatim
    conn = moose.connect(s, 'XOut', r, 'handleX') \endverbatim
 * In order to accomplish this in C++, one would do something like:
 * \verbatim
    MsgId mid = shell->doAddMsg("Single", srcId, "XOut", destId,
                                "handleX" ); \endverbatim
 * This requires the definition of a shell variable which handles the creation
 * of paths in the moose system. Read the Application Programming Interface
 * guide for more information on paths. For now, try to digest the fact that
 * the following lines create a shell object that can be used to make objects
 * on paths - a neutral object and a compartment object have been created as a
 * demonstration.
 * \verbatim
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    Id n = shell->doCreate( "Neutral", Id(), "n" );
    Id c = shell->doCreate( "Compartment", n, "c" ); \endverbatim
 * This creates a Neutral object at /n and a Compartment object at /n/c/.
 *
 */
