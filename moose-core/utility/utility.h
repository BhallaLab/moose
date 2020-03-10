// utility.h ---
// Description: Some utility function.
// Author: Subhasis Ray
// Maintainer:
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Wed Mar 23 10:25:58 2011 (+0530)
// Version:
// Last-Updated: Tue Jul 23 12:48:17 2013 (+0530)
//           By: subha
//     Update #: 19

#ifndef _UTILITY_H

#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <cassert>
#include <string>

using namespace std;

// See types.cpp file for definitions.
namespace moose 
{

    char shortType(std::string type);
    char innerType(char typecode);
    char shortFinfo(std::string ftype);

    // In setuptevn.cpp 
    const map<std::string, std::string>& getArgMap();

    string getEnv(const string& env);
    int getEnvInt(const string& env, const int defaultVal=1);

    // In fileutils.cpp
    bool filepath_exists( const string& path );

    /**
     * @brief Givem path of MOOSE element, return its name. It's behaviour is
     * like `basename` of unix command e.g. /a/b/c --> c
     *
     * @return
     */
    inline std::string basename( const std::string& path )
    {
        return path.substr( path.find_last_of('/') + 1 );
    }

    /* --------------------------------------------------------------------------*/
    /**
     * @Synopsis  Split a interval (0, max) in n parts which are almost equal
     * to each other.
     *
     * @Param max
     * @Param n
     * @Param result. A vector of interval [start, end) (as std::pair).
     */
    /* ----------------------------------------------------------------------------*/
    void splitIntervalInNParts(size_t max, size_t n, std::vector<std::pair<size_t, size_t>>& result);
}


#endif // !_UTILITY_H

//
// utility.h ends here
