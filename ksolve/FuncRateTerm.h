/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 3 or later.
**********************************************************************/

/**
 * This FuncRate manages a one-way reaction whose rate is
 * determined by a Function. It has no substrates, just controls the
 * rate of change of a target molecule.
 *
 * dtarget/dt = func( x0, x1, x2..., t )
 *
 * The values x0, x1, x2.. are numbers at this point. So is the rate
 * output of this function. There may be a problem
 * if there is a volume change.
 */
class FuncRate: public ExternReac
{
    public:
        FuncRate( double k, unsigned int targetPoolIndex ) :
            k_( k )
            , func_(shared_ptr<FuncTerm>(new FuncTerm()))
            , funcVolPower_( 0.0 )
    {
        func_->setTarget( targetPoolIndex );
    }

        double operator() ( const double* S ) const {
            double t = Field< double >::get( Id(1), "currentTime" );
            auto v = (*func_)( S, t ); // get rate from func calculation.
            assert(! std::isnan(v));
            return v;
        }

        unsigned int getReactants( vector< unsigned int >& molIndex ) const{
            molIndex.resize( 1 );
            molIndex[0] = func_->getTarget();

            // This is the number of substrates to the reac. It is zero.
            return 0;
            // The target molecule is handled as a product.
        }

        void setReactants( const vector< unsigned int >& molIndex ) {
            assert( molIndex.size() > 0 );
            func_->setTarget( molIndex[0] );
        }

        const vector< unsigned int >& getFuncArgIndex()
        {
            return func_->getReactantIndex();
        }

        void setFuncArgIndex( const vector< unsigned int >& mol ) {
            func_->setReactantIndex( mol );
        }

        void setExpr( const string& s ) {
            func_->setExpr( s );
        }
        const string& getExpr() const {
            return func_->getExpr();
        }

        RateTerm* copyWithVolScaling(double vol, double sub, double prd ) const
        {
            assert(! std::isnan(vol));
            double ratio = sub * pow( NA * vol, funcVolPower_ );
            FuncRate* ret = new FuncRate( k_ / ratio, func_->getTarget() );
            ret->funcVolPower_ = funcVolPower_;
            ret->func_ = func_;
            return ret;
        }

    protected:
        double k_;
        shared_ptr<FuncTerm> func_;
        double funcVolPower_;
};


/**
 * This FuncReac manages a one-way NOrder reaction whose rate is determined
 * by a Function, but which also has regular substrates and products.
 *
 *
 * dproduct/dt = func( x0, x1, x2..., t ) * [sub0] * [sub1] * ....
 *
 * The values x0, x1, x2 are expected to be concentrations so that they
 * do not depend on volume.
 * The substrates sub0, sub1, ... are # of molecules.
 * The term k_ is scaled so that it is unity at vol = 1/NA m^3.
 * k_ = (NA * vol)^(numSub-1)
 * The copyWithVolScaling operation scales it up and down from there.
 */
class FuncReac: public FuncRate
{
    public:
        FuncReac( double k, vector< unsigned int > v )
            : FuncRate( k, 0 ),
            v_( v )
        {;}

        double operator() ( const double* S ) const
        {
            // double ret = k_ * func_( S, 0.0 ); // get rate from func calculation.
            double ret = (*func_)( S, 0.0 ); // get rate from func calculation.
            vector< unsigned int >::const_iterator i;
            for ( i = v_.begin(); i != v_.end(); i++) {
                assert( !std::isnan( S[ *i ] ) );
                ret *= S[ *i ];
            }
            return ret;
        }

        unsigned int getReactants( vector< unsigned int >& molIndex ) const
        {
            molIndex = v_;
            return numSubstrates_;
        }

        void setReactants( const vector< unsigned int >& molIndex ) 
        {
            v_ = molIndex;
        }

        void rescaleVolume( short comptIndex,
            const vector< short >& compartmentLookup, double ratio )
        {
            for ( unsigned int i = 1; i < v_.size(); ++i ) {
                if ( comptIndex == compartmentLookup[ v_[i] ] )
                    k_ /= ratio;
            }
        }


        RateTerm* copyWithVolScaling(
                double vol, double sub, double prd ) const
        {
            assert( v_.size() > 0 );
            double ratio = sub * pow( NA * vol,
                            funcVolPower_ + (int)( v_.size() ) - 1 );
            FuncReac* ret = new FuncReac( k_ / ratio, v_ );
            ret->func_ = func_;
            ret->funcVolPower_ = funcVolPower_;
            return ret;
        }

    private:
        vector< unsigned int > v_;
        unsigned int numSubstrates_;
};

