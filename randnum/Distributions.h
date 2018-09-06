/***
 *       Filename:  Distributions.h
 *
 *    Description:  All distributions.
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

#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include "NormalDistribution.hpp"
#include <random>

namespace moose {

    template<typename T=double>
        using MOOSE_UNIFORM_DISTRIBUTION = std::uniform_real_distribution<T>;

    template<typename T=double>
        using MOOSE_NORMAL_DISTRIBUTION = moose::normal_distribution<T>;

}


#endif /* end of include guard: DISTRIBUTIONS_H */
