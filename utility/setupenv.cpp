// setupenv.cpp ---
//
// Filename: setupenv.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Sat Mar 26 22:36:10 2011 (+0530)
// Version:
// Last-Updated: Fri Aug 10 17:15:17 2012 (+0530)
//           By: subha
//     Update #: 31
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
//
//
//

// Change log:
//
//
//

// Code:

#include <map>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

extern unsigned getNumCores();

namespace moose
{

const map<string, string>& getArgMap()
{
    static map<string, string> argmap;
    if (argmap.empty())
    {
        char * verbosity = getenv("VERBOSITY");
        if (verbosity != NULL)
        {
            argmap.insert(pair<string, string>("VERBOSITY", string(verbosity)));
        }
        else
        {
            argmap.insert(pair<string, string>("VERBOSITY", "0"));
        }
        char * isInfinite = getenv("INFINITE");
        if (isInfinite != NULL)
        {
            argmap.insert(pair<string, string>("INFINITE", string(isInfinite)));
        }

        char * numNodes = getenv("NUMNODES");
        if (numNodes != NULL)
        {
            argmap.insert(pair<string, string>("NUMNODES", string(numNodes)));
        } 
        char * doQuit = getenv("QUIT");
        if (doQuit != NULL)
        {
            argmap.insert(pair<string, string>("QUIT", string(doQuit)));
        } 
        char * doUnitTests = getenv("DOUNITTESTS");
        if (doUnitTests != NULL)
        {
            argmap.insert(pair<string, string>("DOUNITTESTS", string(doUnitTests)));
        } 
    }
    return argmap;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Get environment valus.
 *
 * @Param env Name of the environment variable.
 *
 * @Returns  value of environment if set, empty string otherwise.
 */
/* ----------------------------------------------------------------------------*/
string getEnv( const string& env )
{
    const char* pEnv = std::getenv( env.c_str() );
    if( pEnv )
        return string(pEnv);
    return "";
}

int getEnvInt(const string& env, const int defaultVal=1) 
{
    string e = getEnv(env);
    if(e.empty())
        return defaultVal;
    try 
    {
        return std::stoi(e);
    }
    catch(std::exception& exc)
    {
        return defaultVal;
    }
}

}


//
// setupenv.cpp ends here
