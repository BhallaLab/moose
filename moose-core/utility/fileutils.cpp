/*
 *    Description:  Filesystem related utilities.
 *
 *         Author:  Dilawar Singh (), dilawars@ncbs.res.in
 *   Organization:  NCBS Bangalore
 */

#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#include "utility.h"

namespace moose {

bool filepath_exists( const string& path )
{
    struct stat buffer;   
    return (stat (path.c_str(), &buffer) == 0);
}

} //namespace moose.
