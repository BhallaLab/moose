/***
 *    Description:  test libsoda.
 *
 *        Created:  2018-08-14

 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *        License:  MIT License
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include <chrono>
#include <cmath>
#include <stdexcept>

#include "LSODA.h"
#include "helper.h"

using namespace std;

// Describe the system.
static void fex(double t, double *y, double *ydot, void *data)
{
    ydot[0] = 1.0E4 * y[1] * y[2] - .04E0 * y[0];
    // Don't swap ydot[1] and ydot[2]. The order will change and test will fail.
    ydot[2] = 3.0E7 * y[1] * y[1];
    ydot[1] = -1.0 * (ydot[0] + ydot[2]);
}

static void system_scipy( double t, double* y, double* ydot, void* data)
{
    double mu = 1E4;
    ydot[0] = y[1];
    ydot[1] = mu * (1- y[0]*y[0]) * y[1] - y[0];
}

// This system is described here
// https://github.com/sdwfrost/liblsoda/issues/10
static void system_github_issue_10(double t, double* y, double* ydot, void* data)
{
    ydot[0]= 9*y[0] + 24*y[1] + 5*cos(t)-(1/3)*sin(t);
    ydot[1]= -24*y[0] -51*y[1] -95*cos(t) + (1/3)*sin(t);
}

int test_github_system( void )
{
    // cout << "Running test given https://github.com/sdwfrost/liblsoda/issues/10" << endl;
    double t = 0e0, tout = 0.5;

    vector<double> y = {4.0/3.0, 2.0/3.0};
    int istate = 1;

    LSODA lsoda;

    vector<double> yout;
    vector<double> res;

    for (size_t i = 0; i < 10; i++)
    {
        lsoda.lsoda_update( system_github_issue_10, 2, y, yout, &t, tout, &istate, nullptr, 1e-6, 1e-6 );
        res.push_back( yout[1] );
        res.push_back( yout[2] );
        tout += 0.5;

        y[0] = yout[1];
        y[1] = yout[2];

        cout << t << ' ' << setprecision(8) << y[0] << ' ' << y[1] << endl;
    }

    cout << res[0] << ' ' << res[1] << endl;
    assert( areEqual(-11.94045, res[0]) );
    assert( areEqual( 3.8610102, res[1]) );

    if (istate <= 0)
    {
        cerr << "error istate = " <<  istate << endl;
        throw runtime_error( "Failed to compute the solution." );
    }

    return 0;
}

int test_scipy_sys( void )
{
    cout << "Running test scipy sys" << endl;
    double t, tout;
    t = 0e0; tout = 10;

    vector<double> y = {10,0};
    int istate = 1;

    LSODA lsoda;

    // Create vector to store results. NOTE THAT yout[0] will be ignored.
    vector<double> yout;
    lsoda.lsoda_update( system_scipy, 2, y, yout, &t, tout, &istate, nullptr );

    areEqual( 9.999899e+00, yout[1]);
    areEqual( -1.010111e-05, yout[2] );

    if (istate <= 0)
    {
        cerr << "error istate = " <<  istate << endl;
        exit(0);
    }
    return 0;
}

int test_fex(void)
{
    // cout << "Running test fex." << endl;
    int neq = 3;
    double t, tout;
    t = 0e0;
    tout = 0.4e0;
    vector<double> y = { 1e0, 0e0, 0.0 };
    int istate = 1;

    LSODA lsoda;
    setprecision( 12 );

    vector<double> res;

    vector<double> yout;
    for (size_t iout = 1; iout <= 12; iout++)
    {
        lsoda.lsoda_update( fex, neq, y, yout, &t, tout, &istate, nullptr, 1e-4, 1e-8 );
        // cerr << " at t " << t << " y= " << yout[1] << ' ' << yout[2] << ' ' << yout[3] << endl;
        // Update the y for next iteration.
        y[0] = yout[1];
        y[1] = yout[2];
        y[2] = yout[3];

        res.push_back( y[0] );
        res.push_back( y[1] );
        res.push_back( y[2] );

        if (istate <= 0)
        {
            cerr << "error istate = " <<  istate << endl;
            exit(0);
        }
        tout = tout * 10.0E0;
    }


    vector<double> expected = { 0.985172    ,3.3864e-05 , 0.0147939
            , 0.905514    ,2.24042e-05,  0.0944634
            , 0.715803    ,9.18446e-06,  0.284188
            , 0.450479    ,3.22234e-06,  0.549517
            , 0.183171    ,8.94046e-07,  0.816828
            , 0.0389738   ,1.62135e-07,  0.961026
            , 0.00493686  ,1.98442e-08,  0.995063
            , 0.00051665  ,2.06765e-09,  0.999483
            , 5.20075e-05 ,2.08041e-10,  0.999948
            , 5.20168e-06 ,2.08068e-11,  0.999995
            , 5.19547e-07 ,2.07819e-12,  0.999999
    };

    // Assert here.
    for (size_t i = 0; i < expected.size(); i++)
    {
        double err = abs( expected[i] - res[i] );
        if( err > 1e-6 )
        {
            cerr << "FAILED: Expected " << expected[i] << ". Got " << res[i] << endl;
            assert( false );
        }
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    test_scipy_sys();
    chrono::steady_clock::time_point end= chrono::steady_clock::now();
    cout << "|| Time taken (us)= " << chrono::duration_cast<chrono::microseconds>(end - begin).count() <<endl;

    begin = chrono::steady_clock::now();
    test_fex();
    end = chrono::steady_clock::now();
    cout << "|| Time taken (us)=" <<
        chrono::duration_cast<chrono::nanoseconds> (end - begin).count()/1000 << endl;

    begin = chrono::steady_clock::now();
    test_github_system();
    end = chrono::steady_clock::now();
    cout << "|| Time taken (us)="
        << chrono::duration_cast<chrono::nanoseconds> (end - begin).count()/1000
        <<endl;

    return 0;
}
