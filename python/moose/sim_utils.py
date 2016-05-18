#!/usr/bin/env python

"""sim_utils.py: 
    Helper function related with simulation.

Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

from . import _moose
from . import print_utils
from . import verification_utils 

def recordTarget(tablePath, target, field = 'vm', **kwargs):
    """Setup a table to record at given path.

    Make sure that all root paths in tablePath exists.

    Returns a table.
    """

    # If target is not an moose object but a string representing intended path
    # then we need to fetch the object first.

    if type( target) == str:
        if not _moose.exists(target):
            msg = "Given target `{}` does not exists. ".format( target )
            raise RuntimeError( msg )
        else:
            target = _moose.Neutral( target )

    assert target.path, "Target must have a valid moose path."

    table = _moose.Table( tablePath )
    assert table

    # Sanities field. 
    if field == "output":
        pass
    elif 'get' not in field:
        field = 'get'+field[0].upper()+field[1:]
    else:
        field = field[:2]+field[3].upper()+field[4:]
    try:
        print_utils.dump("TABLE"
                , "Connecting table {} to target {} field {}".format(
                    table.path
                    , target.path
                    , field
                    )
                )
        table.connect( 'requestOut', target, field )
    except Exception as e:
        debug.dump("ERROR"
                , [ "Failed to connect table to target"
                    , e
                    ]
                )
        raise e
    assert table, "Moose is not able to create a recording table"
    return table

 
def run(simTime, verify=False):
    if verify:
        verification_utils.verify()
    _moose.start(simTime)
