#include "../basecode/header.h"
#include "Example.h"
#include "../basecode/ElementValueFinfo.h"
#include "../basecode/LookupElementValueFinfo.h"

static SrcFinfo1< double > *output() {
    static SrcFinfo1< double > output(
            "output",
            "Sends out the computed value"
            );
    return &output;
}

const Cinfo* Example::initCinfo(){

    //Value Field Definitions
    static ValueFinfo< Example, double > x(
        "x",
        "An example field of an example class",
        &Example::setX,
        &Example::getX
    );

    static ValueFinfo< Example, double > y(
        "y",
        "Another example field of an example class",
        &Example::setY,
        &Example::getY
    );

    //Destination Field Definitions
    static DestFinfo handleX( "handleX",
            "Saves arg value to x_",
            new OpFunc1< Example, double >( &Example::handleX )
    );
    static DestFinfo handleY( "handleY",
            "Saves arg value to y_",
            new OpFunc1< Example, double >( &Example::handleY )
    );

    static DestFinfo process( "process",
        "Handles process call",
        new ProcOpFunc< Example >( &Example::process )
    );
    static DestFinfo reinit( "reinit",
        "Handles reinit call",
        new ProcOpFunc< Example >( &Example::reinit )
    );


    static ReadOnlyLookupElementValueFinfo< Example, string, vector< Id > > fieldNeighbors(
		"fieldNeighbors",
		"Ids of Elements connected this Element on specified field.",
			&Example::getNeighbors );

    //////////////////////////////////////////////////////////////
    // SharedFinfo Definitions
    //////////////////////////////////////////////////////////////
    static Finfo* procShared[] = {
        &process, &reinit
    };
    static SharedFinfo proc( "proc",
        "Handles 'reinit' and 'process' calls from a clock.",
        procShared, sizeof( procShared ) / sizeof( const Finfo* )
    );


    static Finfo *exampleFinfos[] =
    {
        &x,         //Value
        &y,         //Value
        &handleX,   //DestFinfo
        &handleY,   //DestFinfo
        output(),   // SrcFinfo
        &proc,      //SharedFinfo
        &fieldNeighbors,		// ReadOnlyLookupValue
    };

    static Cinfo exampleCinfo(
        "Example",              // The name of the class in python
        Neutral::initCinfo(),   // TODO
        exampleFinfos,          // The array of Finfos created above
        sizeof( exampleFinfos ) / sizeof ( Finfo* ),                      // The number of Finfos
        new Dinfo< Example >()  // The class Example itself (FIXME ?)
    );
    return &exampleCinfo;
}

static const Cinfo* exampleCinfo = Example::initCinfo();

Example::Example()
	: x_( 0.0 )
        , y_( 0.0 )
	, output_( 0.0 )
{
	;
}

void Example::process( const Eref& e, ProcPtr p )
{
    output_ = x_ + y_;
    printf("%f\n", output_);
    output()->send( e, output_ );
}

void Example::reinit( const Eref& e, ProcPtr p )
{

}

void Example::handleX(double arg )
{
    x_ = arg;
}

void Example::handleY(double arg )
{
    y_ = arg;
}

double Example::getX() const
{
    return x_;
}

void Example::setX(double x)
{
    x_ = x;
}

double Example::getY() const
{
    return y_;
}

void Example::setY(double y)
{
    y_ = y;
}

vector< Id > Example::getNeighbors( const Eref& e, string field ) const
{
	vector< Id > ret;
	const Finfo* finfo = e.element()->cinfo()->findFinfo( field );
	if ( finfo )
		e.element()->getNeighbors( ret, finfo );
	else
		cout << "Warning: Example::getNeighbors: Id.Field '" <<
				e.id().path() << "." << field <<
				"' not found\n";
	return ret;
}
