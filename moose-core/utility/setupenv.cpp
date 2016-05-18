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

const map<string, string>& getArgMap()
{
    static map<string, string> argmap;
    if (argmap.empty()){
        char * verbosity = getenv("VERBOSITY");
        if (verbosity != NULL){
            argmap.insert(pair<string, string>("VERBOSITY", string(verbosity)));
        } else {
            argmap.insert(pair<string, string>("VERBOSITY", "0"));
        }
        char * isSingleThreaded = getenv("SINGLETHREADED");
        if (isSingleThreaded != NULL){
            argmap.insert(pair<string, string>("SINGLETHREADED", string(isSingleThreaded)));
        }
        else {
            argmap.insert(pair<string, string>("SINGLETHREADED", "0"));
        }
        char * isInfinite = getenv("INFINITE");
        if (isInfinite != NULL){
         argmap.insert(pair<string, string>("INFINITE", string(isInfinite)));
        }
        // else {
        //     argmap.insert(pair<string, string>("INFINITE", "0"));
        // }
        
        char * numCores = getenv("NUMCORES");
        if (numCores != NULL){
            argmap.insert(pair<string, string>("NUMCORES", string(numCores)));
        } else {
            unsigned int cores = getNumCores();
            stringstream s;
            s << cores;
            argmap.insert(pair<string, string>("NUMCORES", s.str()));        
        }
        char * numNodes = getenv("NUMNODES");
        if (numNodes != NULL){
            argmap.insert(pair<string, string>("NUMNODES", string(numNodes)));
        } // else {
        //     argmap.insert(pair<string, string>("NUMNODES", "1"));
        // }
        char * numProcessThreads = getenv("NUMPTHREADS");
        if (numProcessThreads != NULL){
            argmap.insert(pair<string, string>("NUMPTHREADS", string(numProcessThreads)));
        }
        char * doQuit = getenv("QUIT");
        if (doQuit != NULL){
            argmap.insert(pair<string, string>("QUIT", string(doQuit)));
        } // else {
        //     argmap.insert(pair<string, string>("QUIT", "0"));
        // }
        char * doUnitTests = getenv("DOUNITTESTS");
        if (doUnitTests != NULL){
            argmap.insert(pair<string, string>("DOUNITTESTS", string(doUnitTests)));
        } // else {
        //     argmap.insert(pair<string, string>("DOUNITTESTS", "0"));
        // }
        char * doRegressionTests = getenv("DOREGRESSIONTESTS");
        if (doRegressionTests != NULL){
            argmap.insert(pair<string, string>("DOREGRESSIONTESTS", string(doRegressionTests)));
        } // else {
        //     argmap.insert(pair<string, string>("DOREGRESSIONTESTS", "0"));
        // }
        
    }
    return argmap;
}



// 
// setupenv.cpp ends here
