# This file is part of libmumbl simulator: http://moose.ncbs.res.in.

# libmumbl is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# libmumbl is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with libmumbl.  If not, see <http://www.gnu.org/licenses/>.


"""helper.py: 

Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import debug 

def ifPathsAreValid(paths) :
    ''' Verify if path exists and are readable. '''
    if paths :
        for p in paths :
            if not paths[p] : continue
            for path in paths[p] :
                if not path : continue
                if os.path.isfile(path) : pass
                else :
                    debug.printDebug("ERROR"
                        , "Filepath {0} does not exists".format(path))
                    return False
            # check if file is readable
            if not os.access(path, os.R_OK) :
              debug.dump("ERROR", "File {0} is not readable".format(path))
              return False
    return True


