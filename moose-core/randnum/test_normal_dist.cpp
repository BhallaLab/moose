/***
 *       Filename:  test_normal_dist.cpp
 *
 *    Description:  test script.
 *
 *        Version:  0.0.1
 *        Created:  2018-08-04

 *       Revision:  none
 *
 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *
 *        License:  GNU GPL2
 */


#include <iostream>
#include <map>
#include <iomanip>

#include "Distributions.h"
#include "../utility/testing_macros.hpp"


int test_normal_dist( )
{

    moose::MOOSE_RANDOM_DEVICE rd;
    moose::MOOSE_RNG_DEFAULT_ENGINE gen;
    gen.seed( 10 );

    moose::MOOSE_NORMAL_DISTRIBUTION<double> d(0, 1);

    std::map<int, int> hist{};
    std::vector<double> dist;
    for(int n=0; n<100000; ++n)
    {
        double x = d(gen);
        dist.push_back( x );
        ++hist[ std::round(x) ];
    }

    for(auto p : hist) {
        std::cout << std::setw(2)
                  << p.first << ' ' << std::string(p.second/200, '*') << '\n';
    }

    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[-3], 542 );
    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[-2], 5987 );
    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[-1], 24262 );
    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[0],  38236 );
    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[1],  24386 );
    ASSERT_DOUBLE_EQ( "NORMALDIST", hist[2],  6071 );

    return 0;
}

int main(int argc, const char *argv[])
{
   test_normal_dist();
    return 0;
}
