 /* ==============================================================================
 *
 *       Filename:  testing_macros.hpp
 *
 *    Description:  This file contains some macros useful in testing.
 *
 *        Version:  1.0
 *        Created:  Monday 19 May 2014 05:04:41  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:
 *
 * ==============================================================================
 */

#ifndef  TESTING_MACROS_INC
#define  TESTING_MACROS_INC


#include <sstream>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>

#include "current_function.hpp"
#include "print_function.hpp"

using namespace std;

inline bool doubleEq(double a, double b)
{
    return std::fabs(a-b) < 1e-7;
}

static ostringstream assertStream;

#define LOCATION(ss) \
    ss << "In function: " << SIMPLE_CURRENT_FUNCTION; \
    ss << " file: " << __FILE__ << ":" << __LINE__ << endl;

#define EXPECT_TRUE( condition, msg) \
    if( !(condition) ) {\
        assertStream.str(""); \
        LOCATION( assertStream ); \
        assertStream << msg << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_FALSE( condition, msg) \
    if( (condition) ) {\
        assertStream.str(""); \
        LOCATION( assertStream ); \
        assertStream << msg << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_EQ(a, b, token)  \
    if( (a) != (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream) \
        assertStream << "Expected " << b << ", received " << a ; \
        assertStream << token; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_NEQ(a, b, token)  \
    if( (a) == (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Not expected " << a << endl; \
        assertStream << token << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_GT(a, b, token)  \
    if( (a) <= (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Expected greater than " << a << ", received " << b << endl; \
        assertStream << token << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_GTE(a, b, token)  \
    if( (a) < (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Expected greater than or equal to " << a  \
            << ", received " << b << endl; \
        assertStream << token << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_LT(a, b, token)  \
    if( (a) >= (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Expected less than " << a << ", received " << b << endl; \
        assertStream << token << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define EXPECT_LTE(a, b, token)  \
    if( (a) < (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Expected less than or equal to " << a \
            << ", received " << b << endl; \
        assertStream << token << endl; \
        __dump__(assertStream.str(), "EXPECT_FAILURE"); \
    }

#define ASSERT_TRUE( condition, msg) \
    if( !(condition) ) {\
        assertStream.str(""); \
        assertStream << msg << endl;  \
        throw std::runtime_error( assertStream.str() );\
    }

#define ASSERT_FALSE( condition, msg) \
    if( (condition) ) {\
        assertStream.str(""); \
        assertStream.precision( 9 ); \
        assertStream << msg << endl; \
        throw std::runtime_error(assertStream.str()); \
    }

#define ASSERT_LT( a, b, msg) \
    EXPECT_LT(a, b, msg); \
    assertStream.str(""); \
    assertStream.precision( 9 ); \
    assertStream << msg; \
    throw std::runtime_error( assertStream.str() ); \

#define ASSERT_EQ(a, b, token)  \
    if( ! doubleEq((a), (b)) ) { \
        assertStream.str(""); \
        assertStream.precision( 9 ); \
        LOCATION(assertStream) \
        assertStream << "Expected " << a << ", received " << b  << endl; \
        assertStream << token << endl; \
        throw std::runtime_error(assertStream.str()); \
    }

#define ASSERT_DOUBLE_EQ(token, a, b)  \
    if(! doubleEq(a, b) ) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Expected " << b << ", received " << a  << endl; \
        assertStream << token; \
        moose::__dump__(assertStream.str(), moose::failed); \
        throw std::runtime_error( "float equality test failed" ); \
    }

#define ASSERT_NEQ(a, b, token)  \
    if( (a) == (b)) { \
        assertStream.str(""); \
        LOCATION(assertStream); \
        assertStream << "Not expected " << a << endl; \
        assertStream << token << endl; \
        throw std::runtime_error(assertStream.str()); \
    }


#endif   /* ----- #ifndef TESTING_MACROS_INC  ----- */
