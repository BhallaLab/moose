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

#include "strutil.h"

// See types.cpp file for definitions.
namespace moose 
{

    char shortType(std::string type);
    char innerType(char typecode);
    char shortFinfo(std::string ftype);

    // In setuptevn.cpp 
    const map<std::string, std::string>& getArgMap();
    string getEnv( const string& env);

    // In fileutils.cpp
    bool filepath_exists( const string& path );

    /**
     * @brief Givem path of MOOSE element, return its name. It's behaviour is
     * like `basename` of unix command e.g. /a/b/c --> c
     *
     * @return
     */
    inline string basename( const string& path )
    {
        return path.substr( path.find_last_of('/') + 1 );
    }

}


#endif // !_UTILITY_H

//
// utility.h ends here
