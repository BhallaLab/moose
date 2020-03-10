/***
 *    Description:  benchamark ODE system.
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
#include <numeric>
#include <cmath>
#include <thread>

#include "LSODA.h"
#include "helper.h"

using namespace std;
using namespace std::chrono;

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
    ydot[1] = mu * (1- y[0]*y[0])*y[1] - y[0];
}

// This system is described here
// https://github.com/sdwfrost/liblsoda/issues/10
static void system_github_issue_10(double t, double* y, double* ydot, void* data)
{
    ydot[0]= 9*y[0] + 24*y[1] + 5*cos(t)-(1/3)*sin(t);
    ydot[1]= -24*y[0] -51*y[1] -95*cos(t) + (1/3)*sin(t);
}

double test_github_system( void )
{
    // cout << "Running test given https://github.com/sdwfrost/liblsoda/issues/10" << endl;
    double t = 0e0, tout = 0.5;

    vector<double> y = {4.0/3.0, 2.0/3.0};
    int istate = 1;

    LSODA lsoda;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    vector<double> yout;
    vector<double> res;
    for (size_t i = 0; i < 10; i++)
    {
        lsoda.lsoda_update( system_github_issue_10, 2, y, yout, &t, tout, &istate, nullptr );
        res.push_back( yout[1] );
        res.push_back( yout[2] );
        tout += 0.5;

        y[0] = yout[1];
        y[1] = yout[2];
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    if( !areEqual(-11.9400786, res[0])) cout << 'x';
    if( !areEqual( 3.8608262, res[1])) cout << 'x';
    double dt = duration_cast<chrono::microseconds>(end-begin).count();
    return dt;
}

double test_scipy_sys( void )
{
    // cout << "Running test scipy sys" << endl;
    double t, tout;
    t = 0e0; tout = 10;

    vector<double> y = {10,0};
    int istate = 1;

    LSODA lsoda;

    vector<double> yout;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    lsoda.lsoda_update( system_scipy, 2, y, yout, &t, tout, &istate, nullptr );
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    // printf(" at t= %12.4e y= %14.6e %14.6e\n", t, yout[1], yout[2]);

    if( ! areEqual(9.999899e+00, yout[1])) cout << 'x';
    if( !areEqual(-1.010111e-05, yout[2])) cout << 'x';

    if (istate <= 0)
    {
        cerr << "error istate = " <<  istate << endl;
        exit(0);
    }

    double dt = duration_cast<chrono::microseconds>(end-begin).count();
    return dt;
}

double test_fex(void)
{
    // cout << "Running test fex." << endl;
    int neq = 3;
    double t, tout;
    t = 0e0;
    tout = 0.4e0;

    vector<double> y = { 1e0,  0e0, 0.0 };
    int istate = 1;

    LSODA lsoda;
    setprecision( 12 );

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

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

    chrono::steady_clock::time_point end= chrono::steady_clock::now();
    double dt = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    // cout << "|| Time taken (us)= " << dt << endl;

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

    return dt;
}

int run_serial( )
{
    vector<double> t1s, t2s, t3s;
    size_t N = 10000;
    cout << "Running " << N << " iterations. " << endl;

    // Launch all three tests in parallel.
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++)
    {
        double t1 = test_scipy_sys();
        double t2 = test_fex();
        double t3 = test_github_system( );
        t1s.push_back( t1 );
        t2s.push_back( t2 );
        t3s.push_back( t3 );
    }

    cout << "Avg time for 1 " << std::accumulate(t1s.begin(), t1s.end(), 0)/N
        << " us per loop."<< endl;
    cout << "Avg time for 2 " << std::accumulate(t2s.begin(), t2s.end(), 0)/N
        << " us per loop."<< endl;
    cout << "Avg time for 3 " << std::accumulate(t3s.begin(), t3s.end(), 0)/N
        << " us per loop."<< endl;

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    double dt = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "Total time taken " << dt/N << " us. (per loop)" << endl;


    return 0;
}

int run_multithreaded( )
{
    vector<double> t1s, t2s, t3s;
    size_t N = 10000;
    cout << "Running " << N << " iterations. " << endl;

    // Launch all three tests in parallel.
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++)
    {
        std::thread th1(test_scipy_sys);
        std::thread th2(test_fex);
        std::thread th3(test_github_system);
        th1.join();
        th2.join();
        th3.join();
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    double dt = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "Total time taken " << dt / N << " us (per loop)." << endl;
    return 0;
}

int main(int argc, const char *argv[])
{
    cout <<"|| running serial version" << endl;
    run_serial();

    cout <<"|| running multithreaded version" << endl;
    run_multithreaded();
    return 0;
}
