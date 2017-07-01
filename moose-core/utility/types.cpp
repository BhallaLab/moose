// types.cpp ---
//
// Filename: types.cpp
// Description:
// Author: Subhasis Ray
// Maintainer:
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Wed Mar 23 10:10:45 2011 (+0530)
// Version:
// Last-Updated: Tue Jul 23 12:47:59 2013 (+0530)
//           By: subha
//     Update #: 93
// URL:
// Keywords:
// Compatibility:
//
//

// Commentary:
//
// Some utility functions to give short character representations for
// particular type names.
//
//

// Change log:
//
//
//

// Code:
#include <string>
#include <map>

using namespace std;

/**
   returns a single character code for a given string representation of data type.
   Mostly follows Python/C API's PyArg_ParseTuple convention.
*/
char shortType(string name)
{
    static map<string, char> typemap;
    if (typemap.empty()){
        typemap.insert(pair<string, char>("bool", 'b'));
        typemap.insert(pair<string, char>("char", 'c'));
        typemap.insert(pair<string, char>("int", 'i')); // python
        typemap.insert(pair<string, char>("short", 'h')); // python
        typemap.insert(pair<string, char>("unsigned short", 'H')); // python
        typemap.insert(pair<string, char>("long", 'l')); // python
        typemap.insert(pair<string, char>("long long", 'L')); // python
        typemap.insert(pair<string, char>("unsigned int", 'I')); // python
        typemap.insert(pair<string, char>("unsigned long", 'k')); // python
        typemap.insert(pair<string, char>("unsigned long long", 'K')); // python
        typemap.insert(pair<string, char>("float", 'f')); // python
        typemap.insert(pair<string, char>("double", 'd')); // python
        typemap.insert(pair<string, char>("string", 's')); // python
        typemap.insert(pair<string, char>("Id", 'x')); // moose
        typemap.insert(pair<string, char>("ObjId", 'y')); // moose
        typemap.insert(pair<string, char>("DataId", 'z')); // moose
        typemap.insert(pair<string, char>("vector<char>", 'C')); // moose
        typemap.insert(pair<string, char>("vector<int>", 'v')); // moose
        typemap.insert(pair<string, char>("vector<short>", 'w')); // moose
        typemap.insert(pair<string, char>("vector<long>", 'M')); // moose
        typemap.insert(pair<string, char>("vector<long long>", 'A')); // moose
        typemap.insert(pair<string, char>("vector<unsigned long long>", 'B')); // moose
        typemap.insert(pair<string, char>("vector<unsigned int>", 'N')); // moose
        typemap.insert(pair<string, char>("vector<unsigned long>", 'P')); // moose
        typemap.insert(pair<string, char>("vector<float>", 'F')); // moose
        typemap.insert(pair<string, char>("vector<double>", 'D')); // moose
        typemap.insert(pair<string, char>("vector<string>", 'S')); // moose
        typemap.insert(pair<string, char>("vector<Id>", 'X')); // moose
        typemap.insert(pair<string, char>("vector<ObjId>", 'Y')); // moose
        typemap.insert(pair<string, char>("vector<DataId", 'Z')); //moose
        typemap.insert(pair<string, char>("void", '_')); // moose
        typemap.insert(pair<string, char>("vector< vector<unsigned int> >", 'T'));
        typemap.insert(pair<string, char>("vector< vector<int> >", 'Q'));
        typemap.insert(pair<string, char>("vector< vector<double> >", 'R'));
    }
    map<string, char>::iterator iter = typemap.find(name);
    if (iter == typemap.end()){
        return 0;
    }
    return iter->second;
}

char shortFinfo(string finfoType)
{
    static map<string, char> finfomap;
    if (finfomap.empty()){
        finfomap.insert(pair<string, char>("srcFinfo", 's'));
        finfomap.insert(pair<string, char>("destFinfo", 'd'));
        finfomap.insert(pair<string, char>("sharedFinfo", 'x'));
        finfomap.insert(pair<string, char>("valueFinfo", 'v'));
        finfomap.insert(pair<string, char>("lookupFinfo", 'l'));
    }
    map <string, char>::iterator iter = finfomap.find(finfoType);
    if (iter == finfomap.end()){
        return 0;
    }
    return iter->second;
}

char innerType(char typecode){
    static map<char, char> innerTypeMap;
    if (innerTypeMap.empty()){
        innerTypeMap.insert(pair<char, char>('D', 'd')); // vector<double>
        innerTypeMap.insert(pair<char, char>('v', 'i')); // vector<int>
        innerTypeMap.insert(pair<char, char>('M', 'l')); // vector<long>
        innerTypeMap.insert(pair<char, char>('X', 'x')); // vector<Id>
        innerTypeMap.insert(pair<char, char>('Y', 'y')); // vector<ObjId>
        innerTypeMap.insert(pair<char, char>('C', 'c')); // vector<char>
        innerTypeMap.insert(pair<char, char>('w', 'h')); // vector<short>
        innerTypeMap.insert(pair<char, char>('N', 'I')); // vector<unsigned int>
        innerTypeMap.insert(pair<char, char>('P', 'k')); // vector<unsigned long>
        innerTypeMap.insert(pair<char, char>('A', 'L')); // vector<long long>
        innerTypeMap.insert(pair<char, char>('B', 'K')); // vector<unsigned long long>
        innerTypeMap.insert(pair<char, char>('F', 'f')); // vector<float>
        innerTypeMap.insert(pair<char, char>('S', 's')); // vector<string>
        innerTypeMap.insert(pair<char, char>('T', 'N')); // vector<vector<unsigned>>
        innerTypeMap.insert(pair<char, char>('Q', 'v')); // vector<vector<int>>
        innerTypeMap.insert(pair<char, char>('R', 'D')); // vector<vector<double>>
    }
    map<char, char>::iterator iter = innerTypeMap.find(typecode);
    if (iter == innerTypeMap.end()){
        return 0;
    }
    return iter->second;
}


//
// types.cpp ends here
