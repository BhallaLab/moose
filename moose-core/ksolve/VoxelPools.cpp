/***
 *    Description:  Class VoxelPool.
 *         Authors:  Upinder Bhalla <bhalla@ncbs.res.in>,
 *                  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *        License:  GNU GPL3
 */

#include <memory>

#include "../basecode/header.h"
#include "../basecode/SparseMatrix.h"

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#elif USE_BOOST_ODE
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric;
#endif

#include "OdeSystem.h"
#include "VoxelPoolsBase.h"
#include "VoxelPools.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "KinSparseMatrix.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Ksolve.h"
#include "Stoich.h"

//////////////////////////////////////////////////////////////
// Class definitions

VoxelPools::VoxelPools() : pLSODA(nullptr)
{
#ifdef USE_GSL
    driver_ = 0;
#endif
}

VoxelPools::~VoxelPools()
{
    for ( unsigned int i = 0; i < rates_.size(); ++i )
        delete( rates_[i] );
#ifdef USE_GSL
    if ( driver_ )
        gsl_odeiv2_driver_free( driver_ );
#endif
}

//////////////////////////////////////////////////////////////
// Solver ops
//////////////////////////////////////////////////////////////
void VoxelPools::reinit( double dt )
{
    VoxelPoolsBase::reinit();
#ifdef USE_GSL
    if ( !driver_ )
        return;
    gsl_odeiv2_driver_reset( driver_ );
    gsl_odeiv2_driver_reset_hstart( driver_, dt / 10.0 );
#endif

    // If method is LDODA create lSODA object and save the address of this as
    // param (void*).
    if( getMethod() == "lsoda" )
    {
        pLSODA.reset(new LSODA());
        pLSODA->param = (void *) this;
    }
}

void VoxelPools::setStoich( Stoich* s, const OdeSystem* ode )
{
    stoichPtr_ = s;
    if( ode )
    {
        epsAbs_ = ode->epsAbs;
        epsRel_ = ode->epsRel;
        method_ = ode->method;
    }

#ifdef USE_GSL
    if ( ode )
    {
        sys_ = ode->gslSys;
        if ( driver_ )
            gsl_odeiv2_driver_free( driver_ );

        driver_ = gsl_odeiv2_driver_alloc_y_new( &sys_, ode->gslStep
                  , ode->initStepSize, ode->epsAbs, ode->epsRel);
    }
#endif
    VoxelPoolsBase::reinit();
}

const string VoxelPools::getMethod( )
{
    Ksolve* k = reinterpret_cast<Ksolve*>( stoichPtr_->getKsolve().eref().data() );
    return k->getMethod( );
}

void VoxelPools::advance( const ProcInfo* p )
{
    double t = p->currTime - p->dt;
    Ksolve* k = reinterpret_cast<Ksolve*>( stoichPtr_->getKsolve().eref().data() );

    if( getMethod() == "lsoda" )
    {
        vector<double> yout(size()+1);
        pLSODA->lsoda_update( &VoxelPools::lsodaSys, size()
                , Svec(), yout , &t
                , p->currTime, &lsodaState, this
                );

        // Now update the y from yout. This is different thant normal GSL or
        // BOOST based approach.
        for (size_t i = 0; i < size(); i++)
            varS()[i] = yout[i+1];

        if( lsodaState == 0 )
        {
            cerr << "Error: VoxelPools::advance: LSODA integration error at time "
                 << t << "\n";
            assert(0);
        }
    }
    else
    {

#ifdef USE_GSL
        int status = gsl_odeiv2_driver_apply( driver_, &t, p->currTime, varS());
        if ( status != GSL_SUCCESS )
        {
            cerr << "Error: VoxelPools::advance: GSL integration error at time "
                << t << "\n";
            cerr << "Error info: " << status << ", " <<
                gsl_strerror( status ) << endl;
            if ( status == GSL_EMAXITER )
                cerr << "Max number of steps exceeded\n";
            else if ( status == GSL_ENOPROG )
                cerr << "Timestep has gotten too small\n";
            else if ( status == GSL_EBADFUNC )
                cerr << "Internal error\n";
            assert( 0 );
        }

#elif USE_BOOST_ODE
        // NOTE: Make sure to assing vp to BoostSys vp. In next call, it will be used by
        // updateRates func. Unlike gsl call, we can't pass extra void*  to gslFunc.
        VoxelPools* vp = reinterpret_cast< VoxelPools* >( this );

        /*-----------------------------------------------------------------------------
NOTE: 04/21/2016 11:31:42 AM

We need to call updateFuncs  here (unlike in GSL solver) because there
is no way we can update const vector_type_& y in evalRatesUsingBoost
function. In gsl implmentation one could do it, because const_cast can
take away the constantness of double*. This probably makes the call bit
cleaner.
         *-----------------------------------------------------------------------------*/
        stoichPtr_->updateFuncs( &Svec()[0], p->currTime );

        /*-----------------------------------------------------------------------------
         * Using integrate function works with with default stepper type.
         *
         *  NOTICE to developer:
         *  If you are planning your own custom typdedef of stepper_type_ (see
         *  file BoostSystem.h), the you may run into troble. Have a look at this
         *  http://boostw.boost.org/doc/libs/1_56_0/boost/numeric/odeint/integrate/integrate.hpp
         * 
         * To make numerical results comparable with "gsl" solvers,
         * by default, we pick adaptive step-size control. Use a suffix 'c' to
         * force a constant step size. The numerical accuracy of constant step
         * size solver is low.
         * More details can be found here:
         * https://www.boost.org/doc/libs/1_72_0/libs/numeric/odeint/doc/html/boost_numeric_odeint/odeint_in_detail/steppers.html
         */

        const double fixedDt = 0.1;

        if( method_ == "rk2" )
            odeint::integrate_const( rk_midpoint_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) {
                    VoxelPools::evalRates(this, dy, dydt ); 
                    }
                    , Svec()
                    , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                    );
        else if( method_ == "rk4c" )
            odeint::integrate_const( rk4_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                    );
        else if( method_ == "rk5c")
            odeint::integrate_const( rk_karp_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                    );
        else if( method_ == "rk5ck" )
            odeint::integrate_adaptive(
                    odeint::make_controlled<rk_karp_stepper_type_>( epsAbs_, epsRel_ )
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , p->dt
                    );
        else if ("rk54c" == method_ )
            odeint::integrate_const( rk_karp_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                    );
        else if ("rk54" == method_ )
            odeint::integrate_adaptive(
                    odeint::make_controlled<rk_karp_stepper_type_>( epsAbs_, epsRel_ )
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , p->dt
                    );
        else if ("rk5c" == method_ )
            odeint::integrate_const( rk_dopri_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , std::min( p->dt, fixedDt )
                    );
        else if ("rk5" == method_  || "gsl" == method_)
            odeint::integrate_adaptive(
                    odeint::make_controlled<rk_dopri_stepper_type_>( epsAbs_, epsRel_ )
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , p->dt
                    );
        else if( method_ == "rk8c" )
            odeint::integrate_const( rk_felhberg_stepper_type_()
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                    );
        else if( method_ == "rk8" )
            odeint::integrate_adaptive(
                    odeint::make_controlled<rk_felhberg_stepper_type_>( epsAbs_, epsRel_ )
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , p->dt
                    );
        else
            odeint::integrate_adaptive(
                    odeint::make_controlled<rk_karp_stepper_type_>( epsAbs_, epsRel_ )
                    , [this](const vector_type_& dy, vector_type_& dydt, const double t) { 
                    VoxelPools::evalRates(this, dy, dydt );
                    }
                    , Svec()
                    , p->currTime - p->dt
                    , p->currTime
                    , p->dt
                    );
#endif   // USE_GSL
    }

    if ( !stoichPtr_->getAllowNegative() )   // clean out negatives
    {
        unsigned int nv = stoichPtr_->getNumVarPools();
        double* vs = varS();
        for ( unsigned int i = 0; i < nv; ++i )
        {
            if ( std::signbit(vs[i]) )
                vs[i] = 0.0;
        }
    }
}

void VoxelPools::setInitDt( double dt )
{
#ifdef USE_GSL
    gsl_odeiv2_driver_reset_hstart( driver_, dt );
#endif
}

#ifdef USE_GSL
// static func. This is the function that goes into the Gsl solver.
int VoxelPools::gslFunc( double t, const double* y, double *dydt, void* params )
{
    VoxelPools* vp = reinterpret_cast< VoxelPools* >( params );
    double* q = const_cast< double* >( y ); // Assign the func portion.
    vp->stoichPtr_->updateFuncs( q, t );
    vp->updateRates( y, dydt );
    return GSL_SUCCESS;
}

#elif USE_BOOST_ODE   // NOT GSL

void VoxelPools::evalRates( VoxelPools* vp, const vector_type_& y,  vector_type_& dydt )
{
    vp->updateRates( &y[0], &dydt[0] );
}

#endif // USE_BOOST_ODE

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Function to pass LSODA::lsoda_update function. Since it is
 a * static function, we have to make sure void* param holds the value of
 pointer * to VoxelPools.  * * @Param t * @Param y * @Param dydt * @Param
 params Address of VoxelPools as void*.  */
/*
----------------------------------------------------------------------------*/
void VoxelPools::lsodaSys( double t, double* y, double* dydt, void* param)
{
    VoxelPools* vp = reinterpret_cast< VoxelPools* >( param );
    // Fill in the values.
    vp->stoichPtr_->updateFuncs( y, t );
    vp->updateRates( y, dydt );
}

///////////////////////////////////////////////////////////////////////
// Here are the internal reaction rate calculation functions
///////////////////////////////////////////////////////////////////////

void VoxelPools::updateAllRateTerms( const vector< RateTerm* >& rates,
        unsigned int numCoreRates )
{
    // Clear out old rates if any
    for ( unsigned int i = 0; i < rates_.size(); ++i )
        delete( rates_[i] );

    rates_.resize(rates.size());

    for ( unsigned int i = 0; i < numCoreRates; ++i )
    {
        rates_[i] = rates[i]->copyWithVolScaling( getVolume(), 1, 1 );
    }

    for ( unsigned int i = numCoreRates; i < rates.size(); ++i )
    {
        rates_[i] = rates[i]->copyWithVolScaling(  getVolume(),
                getXreacScaleSubstrates(i-numCoreRates),
                getXreacScaleProducts(i-numCoreRates) 
                );
    }
}

void VoxelPools::updateRateTerms( const vector< RateTerm* >& rates,
                                  unsigned int numCoreRates, unsigned int index )
{
    // During setup or expansion of the reac system, it is possible to
    // call this function before the rates_ term is assigned. Disable.
    if ( index >= rates_.size() )
        return;
    delete( rates_[index] );
    if ( index >= numCoreRates )
    {
        rates_[index] = rates[index]->copyWithVolScaling(
                            getVolume(),
                            getXreacScaleSubstrates(index - numCoreRates),
                            getXreacScaleProducts(index - numCoreRates ) );
    }
    else
        rates_[index] = rates[index]->copyWithVolScaling(getVolume(), 1.0, 1.0);
}

void VoxelPools::updateRates( const double* s, double* yprime ) const
{
    const KinSparseMatrix& N = stoichPtr_->getStoichiometryMatrix();
    vector< double > v( N.nColumns(), 0.0 );
    vector< double >::iterator j = v.begin();
    // totVar should include proxyPools only if this voxel uses them
    unsigned int totVar = stoichPtr_->getNumVarPools() + stoichPtr_->getNumProxyPools();
    // totVar should include proxyPools if this voxel does not use them
    unsigned int totInvar = stoichPtr_->getNumBufPools();
    assert( N.nColumns() == 0 || N.nRows() == stoichPtr_->getNumAllPools() );
    assert( N.nColumns() == rates_.size() );

    for ( auto i = rates_.cbegin(); i != rates_.end(); i++)
        *j++ = (**i)( s );
    for (unsigned int i = 0; i < totVar; ++i)
    {
        auto rate = N.computeRowRate( i, v );
        assert(! std::isnan(rate));
        *yprime++ = rate;
    }
    for (unsigned int i = 0; i < totInvar ; ++i)
        *yprime++ = 0.0;
}

/**
 * updateReacVelocities computes the velocity *v* of each reaction.
 * This is a utility function for programs like SteadyState that need
 * to analyze velocity.
 */
void VoxelPools::updateReacVelocities(const double* s, vector< double >& v) const
{
    const KinSparseMatrix& N = stoichPtr_->getStoichiometryMatrix();
    assert( N.nColumns() == rates_.size() );

    v.clear();
    v.resize( rates_.size(), 0.0 );

    vector< RateTerm* >::const_iterator i;
    vector< double >::iterator j = v.begin();

    for ( i = rates_.begin(); i != rates_.end(); i++)
    {
        *j++ = (**i)( s );
        assert(! std::isnan(*(j-1)));
    }
}

/// For debugging: Print contents of voxel pool
void VoxelPools::print() const
{
    cout << "numAllRates = " << rates_.size() <<
         ", numLocalRates= " << stoichPtr_->getNumCoreRates() << endl;
    VoxelPoolsBase::print();
}

////////////////////////////////////////////////////////////
/**
 * Handle volume updates. Inherited Virtual func.
 */
void VoxelPools::setVolumeAndDependencies( double vol )
{
    VoxelPoolsBase::setVolumeAndDependencies(vol);
    updateAllRateTerms(stoichPtr_->getRateTerms(), stoichPtr_->getNumCoreRates());
}

