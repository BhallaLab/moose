/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2004 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#ifndef  __MOOSE_GLOBAL_INC_
#define  __MOOSE_GLOBAL_INC_

#include <ctime>
#include <map>
#include <sstream>
#include <valarray>


#ifdef  USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#endif

#include "randnum/RNG.h"                        /* Use inbuilt rng */
#include "../utility/print_function.hpp"

using namespace std;

/**
 * @brief Global stringstream for message printing.
 */
extern stringstream errorSS;

/**
 * @brief Keep tracks of how many tests have been executed in moose.
 */
extern unsigned int totalTests;


/** @brief This macro prints the output of a test function onto console.  It
 * also keep track of index of the current test. The index of test is
 * automatically computed by increamenting the counter.
 */

#define TEST_BEGIN cout << endl << "Test(" << totalTests << "): " << SIMPLE_CURRENT_FUNCTION;
#define TEST_END totalTests++; \
    cout << std::right <<  setw(20) << "test of " << SIMPLE_CURRENT_FUNCTION << " finished.";

#define MISSING_BRACKET_AT_END                  -1
#define EMPTY_PATH                              -2
#define SPACES_AT_THE_BEGINING                  -3
#define SPACES_AT_THE_END                       -4
#define SPACES_IN_BETWEEN                       -5
#define BAD_CHARACTER_IN_PATH                   -6


/*-----------------------------------------------------------------------------
 *  Global functions in namespace moose
 *-----------------------------------------------------------------------------*/
namespace moose
{

    extern moose::RNG rng;

    extern map<string, valarray<double>> solverProfMap;

    /**
     * @brief A global seed for all RNGs in moose. When moose.seed( x ) is called,
     * this variable is set. Other's RNGs (except muparser) uses this seed to
     * initialize them. By default it is initialized by random_device (see
     * global.cpp).
     */
    extern unsigned long __rng_seed__;

    /**
     * @brief Fix a path. For testing purpose.
     *
     * @param path Path as string.
     *
     * @return  A fixed path.
     */
    string fixPath(string path);

    /**
     * @brief Checks if given path is correct.
     * If not, return false and error-code as well.
     *
     * @param path Path name.
     *
     * @return 0 if path is all-right. Negative number if path is not OK.
     */
    int checkPath( const string& path );

    /** @brief Append pathB to pathA and return the result.
     *
     * If pathA does not have [indexs] at the end, append "[0]" to pathA and
     * then add pathB to it.  This version does not care if the result has '[0]'
     * at its end.
     *
     * @param pathA First path.
     * @param pathB Second path.
     *
     * @return A string representing moose-path.
     */
    string joinPath(string pathA, string pathB);

    /**
     * @brief Seed seed for RNG.
     *
     * @param seed
     */
    void mtseed( unsigned int seed );

    /**
     * @brief Generate a random double between 0 and 1
     *
     * @return  A random number between 0 and 1.
     */
    double mtrand( void );

    /* --------------------------------------------------------------------------*/
    /**
     * @Synopsis  Overloaded function. Random number between a and b
     *
     * @Param a lower limit.
     * @Param b Upper limit.
     *
     * @Returns   
     */
    /* ----------------------------------------------------------------------------*/
    double mtrand( double a, double b );

    /**
     * @brief Create a POSIX compatible path from a given string.
     * Remove/replace bad characters.
     *
     * @param path Reutrn path is given path if creation was successful, else
     * directory is renamed to a filename.
     */
    string createMOOSEPath( const string& path );

    /**
     * @brief Convert a given value to string.
     *
     * @tparam T
     * @param x
     *
     * @return  String representation
     */
    string toString( double x );

    /**
     * @brief Create directory, recursively.
     */
    bool createParentDirs( const string& path );

    /**
     * @brief Replace all directory sepearator with _. This creates a filepath
     * which can be created in current directory without any need to create
     * parent directory.
     *
     * @param path string
     *
     * @return  filename without directory separator.
     */
    string toFilename( const string& path );

    /**
     * @brief Get the extension of a given filepath.
     *
     * @param path Given path.
     *
     * @return Extension (with or without preceeding '.' )
     */
    string getExtension( const string& path, bool without_dot = true );

    /**
     * @brief Return the name when path is given. Its behaviour is exactly the
     * same as of `basename`  command on unix system.
     *
     * @param path
     *
     * @return  name.
     */
    string pathToName( const string& path );

    /**
     * @brief When user gives a path /a/b/c, moose creates a path
     * /a[0]/b[0]/c[0]. This is helpful in cases where one needs to create more
     * than 1 element.
     *
     * @param path Removed '[0]' from path and return.
     *
     * @return
     */
    string moosePathToUserPath( string path );

    /* --------------------------------------------------------------------------*/
    /**
     * @Synopsis  Get the global seed set by call of moose.seed( X )
     *
     * @Returns  seed (int).
     */
    /* ----------------------------------------------------------------------------*/
    int getGlobalSeed( );

    /* --------------------------------------------------------------------------*/
    /**
     * @Synopsis  Set the seed for all random generator. When seed of a RNG is
     * not set, this seed it used. It is set to -1 by default.
     *
     * @Param seed
     */
    /* ----------------------------------------------------------------------------*/
    void setGlobalSeed( int seed );

    /* --------------------------------------------------------------------------*/
    /**
     * @Synopsis  Add solver performance into the global map.
     *
     * @Param name Name of the solver.
     * @Param time Time taken by the solver.
     * @Param steps Steps.
     */
    /* ----------------------------------------------------------------------------*/
    void addSolverProf( const string& name, double time, size_t steps = 1);
    void printSolverProfMap( );
}

#endif   /* ----- #ifndef __MOOSE_GLOBAL_INC_  ----- */
