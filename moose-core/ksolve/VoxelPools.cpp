/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#elif USE_BOOST
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric;
#endif

#include "OdeSystem.h"
#include "VoxelPoolsBase.h"
#include "VoxelPools.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "../mesh/VoxelJunction.h"
#include "XferInfo.h"
#include "ZombiePoolInterface.h"
#include "Stoich.h"

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////

VoxelPools::VoxelPools()
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
}

void VoxelPools::setStoich( Stoich* s, const OdeSystem* ode )
{
    stoichPtr_ = s;
#ifdef USE_GSL
    if ( ode ) {
        sys_ = ode->gslSys;
        if ( driver_ )
            gsl_odeiv2_driver_free( driver_ );
        driver_ = gsl_odeiv2_driver_alloc_y_new(
                &sys_, ode->gslStep, ode->initStepSize,
                ode->epsAbs, ode->epsRel );
    }
#elif USE_BOOST
    if( ode )
        sys_ = ode->boostSys;
#endif
    VoxelPoolsBase::reinit();
}

void VoxelPools::advance( const ProcInfo* p )
{
    double t = p->currTime - p->dt;
#ifdef USE_GSL
    int status = gsl_odeiv2_driver_apply( driver_, &t, p->currTime, varS());
    if ( status != GSL_SUCCESS ) {
        cout << "Error: VoxelPools::advance: GSL integration error at time "
            << t << "\n";
        cout << "Error info: " << status << ", " <<
            gsl_strerror( status ) << endl;
        if ( status == GSL_EMAXITER )
            cout << "Max number of steps exceeded\n";
        else if ( status == GSL_ENOPROG )
            cout << "Timestep has gotten too small\n";
        else if ( status == GSL_EBADFUNC )
            cout << "Internal error\n";
        assert( 0 );
    }

#elif USE_BOOST


    // NOTE: Make sure to assing vp to BoostSys vp. In next call, it will be used by
    // updateRates func. Unlike gsl call, we can't pass extra void*  to gslFunc.
    VoxelPools* vp = reinterpret_cast< VoxelPools* >( sys_.params );
    sys_.vp = vp;
    /*-----------------------------------------------------------------------------
    NOTE: 04/21/2016 11:31:42 AM

    We need to call updateFuncs  here (unlike in GSL solver) because there
    is no way we can update const vector_type_& y in evalRatesUsingBoost
    function. In gsl implmentation one could do it, because const_cast can
    take away the constantness of double*. This probably makes the call bit
    cleaner.
     *-----------------------------------------------------------------------------*/
    vp->stoichPtr_->updateFuncs( &Svec()[0], p->currTime );

    /*-----------------------------------------------------------------------------
     * Using integrate function works with with default stepper type.
     *
     *  NOTICE to developer:
     *  If you are planning your own custom typdedef of stepper_type_ (see
     *  file BoostSystem.h), the you may run into troble. Have a look at this
     *  http://boostw.boost.org/doc/libs/1_56_0/boost/numeric/odeint/integrate/integrate.hpp
     *-----------------------------------------------------------------------------
     */

    double absTol = sys_.epsAbs;
    double relTol = sys_.epsRel;


    /**
     * @brief Default step size for fixed size iterator.
     * FIXME/TODO: I am not sure if this is a right value to pick by default. May be
     * user should provide the stepping size when using fixed dt. This feature
     * can be incredibly useful on large system.
     */
    const double fixedDt = 0.1;

    if( sys_.method == "rk2" )
        odeint::integrate_const( rk_midpoint_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if( sys_.method == "rk4" )
        odeint::integrate_const( rk4_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if( sys_.method == "rk5")
        odeint::integrate_const( rk_karp_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if( sys_.method == "rk5a")
        odeint::integrate_adaptive(
                odeint::make_controlled<rk_karp_stepper_type_>( absTol, relTol)
                , sys_
                , Svec()
                , p->currTime - p->dt
                , p->currTime
                , p->dt
                );
    else if ("rk54" == sys_.method )
        odeint::integrate_const( rk_karp_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if ("rk54a" == sys_.method )
        odeint::integrate_adaptive(
                odeint::make_controlled<rk_karp_stepper_type_>( absTol, relTol )
                , sys_, Svec()
                , p->currTime - p->dt
                , p->currTime
                , p->dt
                );
    else if ("rk5" == sys_.method )
        odeint::integrate_const( rk_dopri_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if ("rk5a" == sys_.method )
        odeint::integrate_adaptive(
                odeint::make_controlled<rk_dopri_stepper_type_>( absTol, relTol )
                , sys_, Svec()
                , p->currTime - p->dt
                , p->currTime
                , p->dt
                );
    else if( sys_.method == "rk8" )
        odeint::integrate_const( rk_felhberg_stepper_type_()
                , sys_ , Svec()
                , p->currTime - p->dt, p->currTime, std::min( p->dt, fixedDt )
                );
    else if( sys_.method == "rk8a" )
        odeint::integrate_adaptive(
                odeint::make_controlled<rk_felhberg_stepper_type_>( absTol, relTol )
                , sys_, Svec()
                , p->currTime - p->dt
                , p->currTime
                , p->dt
                );

    else
        odeint::integrate_adaptive(
                odeint::make_controlled<rk_karp_stepper_type_>( absTol, relTol )
                , sys_, Svec()
                , p->currTime - p->dt
                , p->currTime
                , p->dt
                );
#endif
    if ( !stoichPtr_->getAllowNegative() ) { // clean out negatives
		unsigned int nv = stoichPtr_->getNumVarPools();
		double* vs = varS();
		for ( unsigned int i = 0; i < nv; ++i ) {
			if ( signbit(vs[i]) )
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
int VoxelPools::gslFunc( double t, const double* y, double *dydt,
						void* params )
{
	VoxelPools* vp = reinterpret_cast< VoxelPools* >( params );
	// Stoich* s = reinterpret_cast< Stoich* >( params );
	double* q = const_cast< double* >( y ); // Assign the func portion.

	// Assign the buffered pools
	// Not possible because this is a static function
	// Not needed because dydt = 0;
	/*
	double* b = q + s->getNumVarPools();
	vector< double >::const_iterator sinit = Sinit_.begin() + s->getNumVarPools();
	for ( unsigned int i = 0; i < s->getNumBufPools(); ++i )
		*b++ = *sinit++;
		*/

	vp->stoichPtr_->updateFuncs( q, t );
	vp->updateRates( y, dydt );
#ifdef USE_GSL
	return GSL_SUCCESS;
#else
	return 0;
#endif
}

#elif USE_BOOST
void VoxelPools::evalRates(
    const vector_type_& y,  vector_type_& dydt,  const double t, VoxelPools* vp
    )
{
    vp->updateRates( &y[0], &dydt[0] );
}
#endif

///////////////////////////////////////////////////////////////////////
// Here are the internal reaction rate calculation functions
///////////////////////////////////////////////////////////////////////

void VoxelPools::updateAllRateTerms( const vector< RateTerm* >& rates,
			   unsigned int numCoreRates )
{
	// Clear out old rates if any
	for ( unsigned int i = 0; i < rates_.size(); ++i )
		delete( rates_[i] );

	rates_.resize( rates.size() );
	for ( unsigned int i = 0; i < numCoreRates; ++i )
		rates_[i] = rates[i]->copyWithVolScaling( getVolume(), 1, 1 );
	for ( unsigned int i = numCoreRates; i < rates.size(); ++i ) {
		rates_[i] = rates[i]->copyWithVolScaling(  getVolume(),
				getXreacScaleSubstrates(i - numCoreRates),
				getXreacScaleProducts(i - numCoreRates ) );
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
		rates_[index] = rates[index]->copyWithVolScaling(
				getVolume(),
				getXreacScaleSubstrates(index - numCoreRates),
				getXreacScaleProducts(index - numCoreRates ) );
	else
		rates_[index] = rates[index]->copyWithVolScaling(
				getVolume(), 1.0, 1.0 );
}

void VoxelPools::updateRates( const double* s, double* yprime ) const
{
	const KinSparseMatrix& N = stoichPtr_->getStoichiometryMatrix();
	vector< double > v( N.nColumns(), 0.0 );
	vector< double >::iterator j = v.begin();
	// totVar should include proxyPools only if this voxel uses them
	unsigned int totVar = stoichPtr_->getNumVarPools() +
			stoichPtr_->getNumProxyPools();
	// totVar should include proxyPools if this voxel does not use them
	unsigned int totInvar = stoichPtr_->getNumBufPools();
	assert( N.nColumns() == 0 ||
			N.nRows() == stoichPtr_->getNumAllPools() );
	assert( N.nColumns() == rates_.size() );

	for ( vector< RateTerm* >::const_iterator
		i = rates_.begin(); i != rates_.end(); i++) {
		*j++ = (**i)( s );
		assert( !std::isnan( *( j-1 ) ) );
	}

	for (unsigned int i = 0; i < totVar; ++i)
		*yprime++ = N.computeRowRate( i , v );
	for (unsigned int i = 0; i < totInvar ; ++i)
		*yprime++ = 0.0;
}

/**
 * updateReacVelocities computes the velocity *v* of each reaction.
 * This is a utility function for programs like SteadyState that need
 * to analyze velocity.
 */
void VoxelPools::updateReacVelocities(
			const double* s, vector< double >& v ) const
{
	const KinSparseMatrix& N = stoichPtr_->getStoichiometryMatrix();
	assert( N.nColumns() == rates_.size() );

	vector< RateTerm* >::const_iterator i;
	v.clear();
	v.resize( rates_.size(), 0.0 );
	vector< double >::iterator j = v.begin();

	for ( i = rates_.begin(); i != rates_.end(); i++) {
		*j++ = (**i)( s );
		assert( !std::isnan( *( j-1 ) ) );
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
	VoxelPoolsBase::setVolumeAndDependencies( vol );
	updateAllRateTerms( stoichPtr_->getRateTerms(),
		stoichPtr_->getNumCoreRates() );
}

