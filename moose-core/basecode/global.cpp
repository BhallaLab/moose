/*
 * ==============================================================================
 *
 *       Filename:  global.cpp
 *
 *    Description:  Some global declarations.
 *
 *        Version:  1.0
 *        Created:  Tuesday 29 April 2014 10:18:35  IST
 *       Revision:  0.1
 *       Compiler:  gcc/g++
 *
 *         Author:  Dilawar Singh
 *   Organization:  Bhalla's lab, NCBS Bangalore
 *
 * ==============================================================================
 */

#include "global.h"
#include <numeric>
#include <sys/stat.h>
#include <sys/types.h>


/*-----------------------------------------------------------------------------
 *  This variable keep track of how many tests have been performed.
 *
 *  Check header.h for macros tbegin and tend which uses it.
 *-----------------------------------------------------------------------------*/
unsigned int totalTests = 0;

stringstream errorSS;

bool isRNGInitialized = false;

clock_t simClock = clock();

extern int checkPath( const string& path);
extern string joinPath( string pathA, string pathB);
extern string fixPath( string path);
extern string dumpStats( int  );

namespace moose {

    unsigned long __rng_seed__ = 0;

    map<string, valarray<double>> solverProfMap = { 
        { "Ksolve", {0.0, 0} }, 
        { "HSolve", {0.0, 0} }
    };

    moose::RNG rng;

    /* Check if path is OK */
    int checkPath( const string& path  )
    {
        if( path.size() < 1)
            return EMPTY_PATH;

        if( path.find_first_of( " \\!") != std::string::npos )
            return BAD_CHARACTER_IN_PATH;

        if ( path[path.size() - 1 ] != ']')
        {
            return MISSING_BRACKET_AT_END;
        }
        return 0;
    }

    /* Join paths */
    string joinPath( string pathA, string pathB )
    {
        pathA = moose::fixPath( pathA );
        string newPath = pathA + "/" + pathB;
        return moose::fixPath( newPath );
    }

    /* Fix given path */
    string fixPath(string path)
    {
        int pathOk = moose::checkPath( path );
        if( pathOk == 0)
            return path;
        else if( pathOk == MISSING_BRACKET_AT_END)
            return path + "[0]";
        return path;
    }

    /**
     * @brief Set the global seed or all rngs.
     *
     * @param x
     */
    void mtseed( unsigned int x )
    {
        moose::__rng_seed__ = x;
        moose::rng.setSeed( x );
        isRNGInitialized = true;
    }

    /*  Generate a random number */
    double mtrand( void )
    {
        return moose::rng.uniform( );
    }

    double mtrand( double a, double b )
    {
        return (b-a) * mtrand() + a; 
    }

    // MOOSE suffixes [0] to all elements to path. Remove [0] with null
    // character whenever possible. For n > 0, [n] should not be touched. Its
    // the user job to take the pain and write the correct path.
    string createMOOSEPath( const string& path )
    {
        string s = path;                        /* Local copy */
        // Remove [0] from paths. They will be annoying for normal users.
        std::string::size_type n = 0;
        string zeroIndex("[0]");
        while( (n = s.find( zeroIndex, n )) != std::string::npos )
            s.erase( n, zeroIndex.size() );
        return s;
    }

    /**
     * @brief Create directories recursively needed to open the given file p.
     *
     * @param path When successfully created, returns created path, else
     * convert path to a filename by replacing '/' by '_'.
     */
    bool createParentDirs( const string& path )
    {
        // Remove the filename from the given path so we only have the
        // directory.
        string p = path;
        bool failed = false;
        size_t pos = p.find_last_of( '/' );
        if( pos != std::string::npos )
            p = p.substr( 0, pos );
        else                                    /* no parent directory to create */
            return true;

        if( p.size() == 0 )
            return true;

        string command( "mkdir -p ");
        command += p;
        int ret = system( command.c_str() );
        struct stat info;
        if( stat( p.c_str(), &info ) != 0 )
        {
            LOG( moose::warning, "cannot access " << p );
            return false;
        }
        else if( info.st_mode & S_IFDIR )
        {
            LOG( moose::info, "Created directory " <<  p );
            return true;
        }
        else
        {
            LOG( moose::warning, p << " is no directory" );
            return false;
        }
        return true;
    }


    /*  Flatten a dir-name to return a filename which can be created in pwd . */
    string toFilename( const string& path )
    {
        string p = path;
        std::replace(p.begin(), p.end(), '/', '_' );
        std::replace(p.begin(), p.end(), '\\', '_' );
        return p;
    }

    /*  return extension of a filename */
    string getExtension(const string& path, bool without_dot )
    {
        size_t dotPos = path.find_last_of( '.' );
        if( dotPos == std::string::npos )
            return "";

        if( without_dot )
            return path.substr( dotPos + 1 );

        return path.substr( dotPos );
    }

    /*  returns `basename path`  */
    string pathToName( const string& path )
    {
        return path.substr( path.find_last_of( '/' ) );
    }

    /*  /a[0]/b[1]/c[0] -> /a/b/c  */
    string moosePathToUserPath( string path )
    {
        // Just write the moose path. Things becomes messy when indexing is
        // used.
        return createMOOSEPath( path );
    }

    /*  Return formatted string
     *  Precision is upto 17 decimal points.
     */
    string toString( double x )
    {
        char buffer[50];
        sprintf(buffer, "%.17g", x );
        return string( buffer );
    }

    int getGlobalSeed( )
    {
        return __rng_seed__;
    }

    void setGlobalSeed( int seed )
    {
        __rng_seed__ = seed;
    }

    void addSolverProf( const string& name, double time, size_t steps)
    {
        solverProfMap[ name ] = solverProfMap[name] + valarray<double>({ time, (double)steps });
    }

    void printSolverProfMap( )
    {
        for( auto &v : solverProfMap )
            cout <<  '\t' << v.first << ": " << v.second[0] << " sec (" << v.second[1] << ")" << endl;
    }

}
