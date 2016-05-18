#!/usr/bin/env python

"""dynamic_vars.py: Class to create variables out of strings.

Last modified: Wed Dec 11, 2013  07:08PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"

__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


class DynamicObject:
    def __init__(self):
        pass

    def __init__(self, name):
        setattr(self, 'name', name)
