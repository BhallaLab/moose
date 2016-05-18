#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"
__date__    = "Mon 21 Oct 2013 11:37:01 PM IST"
__version__ = "1.0"
__credits__ = "NCBS"

__log__     = """


"""
# Basic imports
import os 
import sys 
import logging 
import debug 

logger = logging.getLogger('multiscale')
try:
    import cElementTree as etree
    debug.printDebug("DEBUG", "running with lxml.etree")
except ImportError:
    try:
        # Python 2.5
        import xml.etree.cElementTree as etree
        debug.printDebug("DEBUG", "running with cElementTree")
    except ImportError:
        try:
            # Python 2.5
            import xml.etree.cElementTree as etree
            debug.printDebug("DEBUG", "running with ElementTree")
        except ImportError:
            try:
              # normal cElementTree install
              import cElementTree as etree
              debug.printDebug("DEBUG", "running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree
                    debug.printDebug("DEBUG", "running with ElementTree")
                except ImportError:
                    try : 
                      import lxml.etree as etree
                    except ImportError :
                        debug.prefix("FATAL", "Failed to import ElementTree")
                        os._exit(1)

def ifPathsAreValid(paths) :
  ''' Verify if path exists and are readable. '''
  if paths.nml :
    if os.path.isfile(paths.nml) : pass
    else :
      debug.printDebug("ERROR", "Filepath {0} is not valid".format(paths.nml))
      return False
  if paths.sbml :
    if os.path.isfile(paths.sbml) : pass 
    else :
      debug.printDebug("ERROR", "Filepath {0} is not valid".format(paths.sbml))
      return False
  return True



