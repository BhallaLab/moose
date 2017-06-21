/*******************************************************************
 * File:            StringUtil.cpp
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-09-25 12:12:10
 ********************************************************************/
#include <string>
#include <iostream>
#include <vector>
#include "strutil.h"

using namespace std;

namespace moose {

// Adapted from code available on oopweb.com
void tokenize(
	const string& str,	
	const string& delimiters,
        vector< string >& tokens )
{
	// Token boundaries
	string::size_type begin = str.find_first_not_of( delimiters, 0 );
	string::size_type end = str.find_first_of( delimiters, begin );

	while ( string::npos != begin || string::npos != end )
	{
		// Found a token, add it to the vector.
		tokens.push_back( str.substr( begin, end - begin ) );
		
		// Update boundaries
		begin = str.find_first_not_of( delimiters, end );
		end = str.find_first_of( delimiters, begin );
	}
}

string& clean_type_name(string& arg)
{
    for (size_t pos = arg.find(' '); pos != string::npos; pos = arg.find(' ')){
        arg.replace(pos, 1, 1, '_');
    }
    for (size_t pos = arg.find('<'); pos != string::npos; pos = arg.find('<')){
        arg.replace(pos, 1, 1, '_');
    }
    for (size_t pos = arg.find('>'); pos != string::npos; pos = arg.find('>')){
        arg.replace(pos, 1, 1, '_');
    }
    return arg;
}

std::string trim(const std::string myString, const string& delimiters)
{
    if (myString.length() == 0 )
    {
        return myString;        
    }
    
    string::size_type  end = myString.find_last_not_of(delimiters);
    string::size_type begin = myString.find_first_not_of(delimiters);

    if(begin != string::npos)
    {
        return std::string(myString, begin, end-begin+1);
    }
    
    return "";    
}

std::string fix(const std::string userPath, const string& delimiters)
{
    string trimmedPath = trim(userPath, delimiters);

    string fixedPath;

    // In this loop, we check if there are more than one '/' together. If yes,
    // then accept only first one and reject other.
    for(unsigned int i = 0; i < trimmedPath.size(); ++i)
    {
        const char c = trimmedPath[i];
        if('/' == c)
        {
            if('/' != fixedPath[fixedPath.size()-1])
                fixedPath.push_back(c);
        }
        else
            fixedPath.push_back(c);

    }
    return fixedPath;
}

int testTrim()
{

    std::string testStrings [] = 
        {
            " space at beginning",
            "space at end ",
            " space at both sides ",
            "\ttab at beginning",
            "tab at end\t",
            "\ttab at both sides\t",
            "\nnewline at beginning",
            "newline at end\n",
            "\nnewline at both sides\n",
            "\n\tnewline and tab at beginning",
            "space and tab at end \t",
            "   \rtab and return at both sides \r"
        };
    
    std::string results[] = 
        {
            "space at beginning",
            "space at end",
            "space at both sides",
            "tab at beginning",
            "tab at end",
            "tab at both sides",
            "newline at beginning",
            "newline at end",
            "newline at both sides",
            "newline and tab at beginning",
            "space and tab at end",
            "tab and return at both sides"
        };
    
    bool success = true;
    
    for (unsigned int i = 0; i < sizeof(testStrings)/sizeof(*testStrings); ++i )
    {
        std::string trimmed = trim(testStrings[i]);
        success = (results[i].compare(trimmed)==0);
        
        cout << "'" << trimmed << "'" << (success ?" SUCCESS":"FAILED") << endl;
    }
    return success?1:0;    
}


bool endswith(const string & full, const string & ending)
{
    if (full.length() < ending.length()){
        return false;
    }
    return (0 == full.compare(full.length() - ending.length(), ending.length(), ending));
}

/* Compare two strings. */
int strncasecmp( const string& a, const string& b, size_t n)
{
    for( size_t i = 0; i < std::min(n, b.size()); ++i )
        if( tolower(a[i]) != tolower(b[i]) )
            return tolower(a[i]) - tolower(b[i]);

    if( b.size() < n )
        return a.size() - b.size();
    
    return 0;
}

}
