/*
 * =====================================================================================
 *
 *       Filename:  print_function.h
 *
 *    Description:  Some of basic print routines for c++
 *
 *        Version:  1.0
 *        Created:  07/18/2013 07:34:06 PM
 *       Revision:  none
 *       Compiler:  gcc/g++
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore.
 *
 * =====================================================================================
 */

#ifndef  print_function_INC
#define  print_function_INC

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cstring>

#define T_RESET       "\033[0m"
#define T_BLACK       "\033[30m"      /* Black */
#define T_RED         "\033[31m"      /* Red */
#define T_GREEN       "\033[32m"      /* Green */
#define T_YELLOW      "\033[33m"      /* Yellow */
#define T_BLUE        "\033[34m"      /* Blue */
#define T_MAGENTA     "\033[35m"      /* Magenta */
#define T_CYAN        "\033[36m"      /* Cyan */
#define T_WHITE       "\033[37m"      /* White */
#define T_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define T_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define T_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define T_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define T_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define T_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define T_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define T_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  Macros
 */
/* ----------------------------------------------------------------------------*/
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifndef NDEBUG
#define MOOSE_DEBUG( a ) { \
    stringstream ss; ss << a; \
    cout << "DEBUG: " << __FILENAME__ << ":" << __LINE__ << " " << ss.str() << std::endl; \
    }
#else
#define MOOSE_DEBUG( a ) {}
#endif

#define MOOSE_WARN( a ) { \
    stringstream ss; ss << __func__ << ": " << a; \
    moose::showWarn( ss.str() ); \
    }

namespace moose {

    /**
     * @brief Enumerate type for debug and log.
     */
    enum serverity_level_ {
        trace, debug, info , warning, fixme , error, fatal, failed
    };

    static string levels_[9] = {
        "TRACE", "DEBUG", "INFO", "WARNING", "FIXME" , "ERROR", "FATAL", "FAILED"
    };

    /*
     * ===  FUNCTION  ==============================================================
     *         Name:  mapToString
     *  Description:  GIven a map, return a string representation of it.
     *
     *  If the second argument is true then print the value with key. But default it
     *  is true.
     * ==============================================================================
     */

    template<typename A, typename B>
        string mapToString(const map<A, B>& m, bool value=true)
        {
            unsigned int width = 81;
            unsigned int size = 0;

            vector<string> row;

            /* Get the maximum size of any entry in map */
            stringstream ss;
            typename map<A, B>::const_iterator it;
            for(it = m.begin(); it != m.end(); it++)
            {
                ss.str("");
                ss << it->first;
                if(value)
                    ss << ": " << it->second;
                row.push_back(ss.str());
                if(ss.str().size() > size)
                    size = ss.str().size()+1;
            }

            unsigned int colums = width / size;
            ss.str("");

            size_t i = 0;
            for(unsigned int ii = 0; ii < row.size(); ii++)
            {
                if(i < colums)
                {
                    ss << setw(size+1) << row[ii];
                    i++;
                }
                else
                {
                    ss << endl;
                    i = 0;
                }
            }
            return ss.str();
        }

    inline string colored(string msg)
    {
        stringstream ss;
        ss << T_RED << msg << T_RESET;
        return ss.str();
    }

    inline string colored(string msg, string colorName)
    {
        stringstream ss;
        ss << colorName << msg << T_RESET;
        return ss.str();
    }

    // Not print it when built for release.
    inline string debugPrint(string msg, string prefix = "DEBUG"
            , string color=T_RESET, unsigned debugLevel = 0
            )
    {
        stringstream ss; ss.str("");
        if(debugLevel <= DEBUG_LEVEL)
        {
            ss << setw(debugLevel/2) << "[" << prefix << "] "
                << color << msg << T_RESET;
        }
        return ss.str();
    }

    /*-----------------------------------------------------------------------------
     *  This function __dump__ a message onto console. Fills appropriate colors as
     *  needed.
     *-----------------------------------------------------------------------------*/

    inline void __dump__(string msg, serverity_level_ type = debug, bool autoFormat = true)
    {
        stringstream ss;
        ss << "[" << levels_[type] << "] ";
        bool set = false;
        bool reset = true;
        string color = T_GREEN;
        if(type == warning || type == fixme )
            color = T_YELLOW;
        else if(type ==  debug )
            color = T_CYAN;
        else if(type == error || type == failed )
            color = T_RED;
        else if(type == info )
            color = T_MAGENTA;

        for(unsigned int i = 0; i < msg.size(); ++i)
        {
            if('`' == msg[i])
            {
                if(!set and reset)
                {
                    set = true;
                    reset = false;
                    ss << color;
                }
                else if(set && !reset)
                {
                    reset = true;
                    set = false;
                    ss << T_RESET;
                }
            }
            else if('\n' == msg[i])
                ss << "\n | ";
            else
                ss << msg[i];
        }

        /*  Be safe than sorry */
        if(!reset)
            ss << T_RESET;

        cout << ss.str() << endl;
    }

    /*
     * Wrapper function around __dump__
     */
    inline void showInfo( string msg )
    {
        moose::__dump__( msg, moose::info );
    }

    inline void showWarn( string msg )
    {
        moose::__dump__(msg, moose::warning );
    }

    inline void showDebug( const string msg )
    {
#ifdef DISABLE_DEBUG
#else
        moose::__dump__(msg, moose::debug );
#endif
    }

    inline void showError( string msg )
    {
        moose::__dump__( msg, moose::error );
    }

    /**
     * @brief This macro only expands when not compiling for release.
     *
     * @param a Stream to write to logger /console.
     * @param t Type of the stream.
     * @return  Nothing.
     */

#ifdef  NDEBUG
#define LOG(t, a ) ((void)0);
#else      /* -----  not NDEBUG  ----- */
#define LOG(t, a) { stringstream __ss__; \
    __ss__ << __func__ << ": " << a; moose::__dump__(__ss__.str(), t); \
}
#endif     /* -----  not NDEBUG  ----- */

    /*-----------------------------------------------------------------------------
     *  Log to a file, and also to console.
     *-----------------------------------------------------------------------------*/
    inline bool isBackTick(char a)
    {
        if('`' == a)
            return true;
        return false;
    }

    inline string formattedMsg(string& msg)
    {
        remove_if(msg.begin(), msg.end(), isBackTick);
        return msg;
    }

    /**
     * @brief Log to console (and to a log-file)
     *
     * @param msg String, message to be written.
     * @param type Type of the message.
     * @param redirectToConsole
     * @param removeTicks
     */
    inline void log(string msg, serverity_level_ type = debug
            , bool redirectToConsole = true
            , bool removeTicks = true
            )
    {

        if(redirectToConsole)
            __dump__(msg, type, true);

        /* remove any backtick from the string. */
        formattedMsg( msg );

        fstream logF;
        logF.open( "__moose__.log", ios::app);
        time_t rawtime; time(&rawtime);
        struct tm* timeinfo;
        timeinfo = localtime(&rawtime);

        logF << asctime(timeinfo) << ": " << msg;

        logF.close();
    }

    template<typename T>
    void print_array( T* a, size_t n, const string prefix = "" )
    {
        stringstream ss;
        ss << prefix;
        for (size_t i = 0; i < n; i++)
        {
            ss << a[i] << ' ';
            if( (i+1) % 20 == 0 )
                ss << endl;
        }
        ss << endl;
        cerr << ss.str();
    }

}
#endif   /* ----- #ifndef print_function_INC  ----- */
