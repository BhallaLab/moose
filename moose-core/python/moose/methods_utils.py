# -*- coding: utf-8 -*-

"""methods_utils.py:

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


import re

objPathPat = re.compile(r'(\/\w+\[\d+\])+?$')

def idPathToObjPath( idPath ):
    """ Append a [0] if missing from idPath.

    Id-paths do not have [0] at their end. This does not allow one to do
    algebra properly.
    """
    m = objPathPat.match( idPath )
    if m: return idPath
    else:
        return '{}[0]'.format(idPath)


def main():
    p1 = '/cable[0]/comp_[1]/a'
    p2 = '/cab[1]/comp/com'
    p3 = '/cab[1]/p[2]/c[3]'
    p4 = '/ca__b[1]/_p[2]/c[122]'
    for p in [p1, p2, p3, p4]:
        m = objPathPat.match(p)
        if m:
            print(m.group(0))
        else:
            print(("{} is invalid Obj path in moose".format( p )))

if __name__ == '__main__':
    main()
