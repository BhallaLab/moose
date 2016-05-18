# pymoose.py --- 
# 
# Filename: pymoose.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Copyright (C) 2010 Subhasis Ray, all rights reserved.
# Created: Wed Apr 20 11:51:24 2011 (+0530)
# Version: 
# Last-Updated: Fri Apr 22 15:09:53 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 13
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This provides ports of the code in pymoose.py in earlier moose.
# 
# 

# Change log:
# 
# 
# 

# Code:

from moose import *
def listmsg(pymoose_object):
    """Print a list of the messages on this object.

    """
    print 'Incoming messages:'
    for msg in pymoose_object.inMessages():
        print msg
    print 'Outgoing messages:'
    for msg in pymoose_object.outMessages():
        print msg

# 
# pymoose.py ends here
