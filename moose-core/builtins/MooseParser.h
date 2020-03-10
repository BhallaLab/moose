/***
 *    Description:  MooseParser class. 
 *
 *        Created:  2019-05-30

 *         Author:  Dilawar Singh <dilawars@ncbs.res.in>
 *   Organization:  NCBS Bangalore
 *        License:  MIT License
 */

#ifndef MOOSEPARSER_H
#define MOOSEPARSER_H

#include <map>
using namespace std;

#include "../external/muparser/include/muParser.h"

namespace moose {
    namespace Parser {

        struct ParserException : public std::exception
        {
            ParserException( const std::string msg ) : msg_(msg) { ; }

            string GetMsg()
            {
                return msg_;
            }

            string msg_;
        };

        typedef ParserException exception_type;


        // These should be same for both muparser and EXPRTK parser.
        typedef std::map<string, double> valmap_type;
        typedef std::map<string, double*> varmap_type;
        typedef double value_type;

    } // namespace Parser

} // namespace moose

class MooseParser : public mu::Parser
{
    public:
        MooseParser();
        ~MooseParser();

    private:
        /* data */
};

#endif /* end of include guard: MOOSEPARSER_H */
