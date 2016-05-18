// utility.h --- 
// 
// Filename: utility.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Wed Mar 23 10:25:58 2011 (+0530)
// Version: 
// Last-Updated: Tue Jul 23 12:48:17 2013 (+0530)
//           By: subha
//     Update #: 19
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

#ifndef _UTILITY_H


char shortType(std::string type);
char innerType(char typecode);
char shortFinfo(std::string ftype);

#include "strutil.h"
const map<std::string, std::string>& getArgMap();

#endif // !_UTILITY_H


// 
// utility.h ends here
