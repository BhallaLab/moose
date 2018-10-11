/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **   copyright (C) 2003-2011 Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#include "../basecode/header.h"
#include "../shell/Shell.h"
#include "Cell.h"

#include "HSolveStruct.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"
#include "HSolve.h"

map< string, Cell::MethodInfo > Cell::methodMap_;

const Cinfo* Cell::initCinfo()
{
    static DestFinfo process(
        "process",
        "Cell does not process at simulation time--"
        "it only sets up the solver at reset.",
        new ProcOpFunc< Cell >( &Cell::processDummy )
    );

    static DestFinfo reinit(
        "reinit",
        "Handles 'reinit' call: This triggers setting up the solver.",
        new ProcOpFunc< Cell >( &Cell::reinit )
        //~ new EpFunc0< Cell >( &Cell::reinit )
    );

    static Finfo* processShared[] =
    {
        &process,
        &reinit
    };

    static SharedFinfo proc(
        "proc",
        "This shared message exists only to receive a 'reinit' call, which is "
        "taken as a trigger to create and set up HSolve.",
        processShared,
        sizeof( processShared ) / sizeof( Finfo* )
    );

    static DestFinfo setup1(
        "setup1",
        "Setup.",
        new OpFunc1< Cell, Id >( &Cell::setupf )
    );

    static ValueFinfo< Cell, Id > setup2(
        "setupv",
        "Setupv.",
        &Cell::setupf,
        &Cell::getSetup
    );

    static ValueFinfo< Cell, string > method(
        "method",
        "Specifies the integration method to be used for the neuron managed "
        "by this Cell object.",
        &Cell::setMethod,
        &Cell::getMethod
    );

    static ValueFinfo< Cell, unsigned int > solverClock(
        "solverClock",
        "Specifies which clock to use for the HSolve, if it is used.",
        &Cell::setSolverClock,
        &Cell::getSolverClock
    );

    static ValueFinfo< Cell, string > solverName(
        "solverName",
        "Specifies name for the solver object.",
        &Cell::setSolverName,
        &Cell::getSolverName
    );

    static ReadOnlyValueFinfo< Cell, int > variableDt(
        "variableDt",
        "Read-only field which tells if the current method is a variable "
        "time-step method.",
        &Cell::getVariableDt
    );

    static ReadOnlyValueFinfo< Cell, int > implicit(
        "implicit",
        "Read-only field which tells if the current method is an implicit "
        "method.",
        &Cell::getImplicit
    );

    static ReadOnlyValueFinfo< Cell, string > description(
        "description",
        "Read-only field giving a short description of the currently selected "
        "integration method.",
        &Cell::getDescription
    );

    static Finfo* cellFinfos[] =
    {
        &method,            // Value
        &solverClock,       // Value
        &solverName,        // Value
        &setup2,
        &variableDt,        // ReadOnlyValue
        &implicit,          // ReadOnlyValue
        &description,       // ReadOnlyValue
        &setup1,
        &proc,              // Shared
    };

    static string doc[] =
    {
        "Name",             "Cell",
        "Author",           "Niraj Dudani, 2012, NCBS",
        "Description",      "Container for a neuron's components. "
        "Also manages automatic solver setup. "
        "In case of solver setup, assumes that all the "
        "compartments under this Cell belong to a single "
        "neuron. If more than 1 group of axially "
        "connected compartments are present, then only "
        "one of them will be taken over by the solver.",
    };

    static Cinfo cellCinfo(
        "Cell",
        Neutral::initCinfo(),
        cellFinfos,
        sizeof( cellFinfos ) / sizeof( Finfo* ),
        new Dinfo< Cell >()
    );

    Cell::addMethod( "ee",
                     "Exponential Euler.",
                     0, 0 );
    Cell::addMethod( "hsolve",
                     "Hines' algorithm.",
                     0, 1 );

    return &cellCinfo;
}

static const Cinfo* cellCinfo = Cell::initCinfo();

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

Cell::Cell()
    :
    solverClock_( 2 ),
    solverName_( "_integ" ),
    shell_( reinterpret_cast< Shell* >( Id().eref().data() ) )
{
    setMethod( "hsolve" );
}

void Cell::addMethod(
    const string& name,
    const string& description,
    int isVariableDt,
    int isImplicit )
{
    methodMap_[ name ] = MethodInfo( description, isVariableDt, isImplicit );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void Cell::processDummy( const Eref& cell, ProcPtr p )
{
    ;
}

void Cell::reinit( const Eref& cell, ProcPtr p )
//~ void Cell::reinit( const Eref& cell, const Qinfo* q )
{
    cout << ".. Cell::reinit()" << endl;
    //~ if ( q->protectedAddToStructuralQ() )
    //~ return;

    // Delete existing solver
    //~ string solverPath = cell.id().path() + "/" + solverName_;
    //~ Id solver( solverPath );
    //~ if ( solver.path() == solverPath )
    //~ solver.destroy();

    if ( method_ == "ee" )
        return;

    // Find any compartment that is a descendant of this cell
    Id seed = findCompt( cell.id() );
    if ( seed == Id() ) // No compartment found.
        return;

    setupSolver( cell.id(), seed );
}

Id Cell::getSetup() const
{
    return Id();
}
void Cell::setupf( Id cell )
{
    cout << "Cell::setup()" << endl;
    cout << ".... cell path: " << cell.path() << endl;
    //~ return;

    // Delete existing solver
    string solverPath = cell.path() + "/" + solverName_;
    Id solver( solverPath );
    if ( solver.path() == solverPath )
        solver.destroy();

    if ( method_ == "ee" )
        return;

    // Find any compartment that is a descendant of this cell
    Id seed = findCompt( cell );
    if ( seed == Id() ) // No compartment found.
        return;

    setupSolver( cell, seed );
}

vector< Id > Cell::children( Id obj )
{
    //~ return Field< vector< Id > >::get( obj, "children" );
    //~ return Field< vector< Id > >::fastGet( obj.eref(), "children" );
    //~ return localGet< Neutral, vector< Id > >( obj.eref(), "children" );

    vector< Id > c;
    Neutral::children( obj.eref(), c );
    return c;
}

/**
 * This function performs a depth-first search of the tree under the current
 * cell. First compartment found is returned as the seed.
 */
Id Cell::findCompt( Id cell )
{
    /* 'curr' is the current element under consideration. 'cstack' is a list
     * of all elements (and their immediate siblings) found on the path from
     * the root element (the Cell) to the current element.
     */
    vector< vector< Id > > cstack;
    Id seed;

    const Cinfo* compartmentCinfo = Cinfo::find( "Compartment" );

    cstack.push_back( children( cell ) );
    while ( !cstack.empty() )
    {
        const vector< Id >& child = cstack.back();

        if ( child.empty() )
        {
            cstack.pop_back();
            if ( !cstack.empty() )
                cstack.back().pop_back();
        }
        else
        {
            moose::showWarn( "TODO: Commented out code. ");
            Id curr = child.back();

#if  0     /* ----- #if 0 : If0Label_1 ----- */

            //~ string className = Field< string >::get( curr, "class" );
            if ( curr()->cinfo() == compartmentCinfo )
            {
                seed = curr;
                break;
            }
#endif     /* ----- #if 0 : If0Label_1 ----- */

            cstack.push_back( children( curr ) );
        }
    }

    return seed;
}

void Cell::setupSolver( Id cell, Id seed ) const
{
    Id solver = Id::nextId();
    moose::showWarn(
            "FIXME: Using 0 for parentMsgIndex in function call Shell::innerCreate"
         "0 in first and third argument to NodeBalance. "
         "I am not sure if I should be doing this here in this function."
        );
    NodeBalance nb(0, MooseBlockBalance, 0);
    shell_->innerCreate("HSolve", cell, solver, solverName_, nb, 0);
    HSolve* data = reinterpret_cast< HSolve* >( solver.eref().data() );
    data->setSeed( seed );
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void Cell::setMethod( string value )
{
    map< string, MethodInfo >::iterator i = methodMap_.find( value );

    if ( i != methodMap_.end() )
    {
        method_ = value;
    }
    else
    {
        method_ = "hsolve";
        cerr << "Warning: Cell::setMethod(): method '" << value
             << "' not known. Using '" << method_ << "'.\n";
        setMethod( method_ );
    }
}

string Cell::getMethod() const
{
    return method_;
}

void Cell::setSolverClock( unsigned int value )
{
    solverClock_ = value;
}

unsigned int Cell::getSolverClock() const
{
    return solverClock_;
}

void Cell::setSolverName( string value )
{
    solverName_ = value;
}

string Cell::getSolverName() const
{
    return solverName_;
}

int Cell::getVariableDt() const
{
    return methodMap_[ method_ ].isVariableDt;
}

int Cell::getImplicit() const
{
    return methodMap_[ method_ ].isImplicit;
}

string Cell::getDescription() const
{
    return methodMap_[ method_ ].description;
}
