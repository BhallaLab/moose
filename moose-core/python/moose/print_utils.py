"""print_utils.py: A library with some print functions. Very useful during
development.

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

import inspect
import sys
from . import _moose as moose

HEADER = '\033[95m'
OKBLUE = '\033[94m'
OKGREEN = '\033[92m'
WARNING = '\033[90m'
ERR = '\033[31m'
ENDC = '\033[0m'
RED = ERR
WARN = WARNING
INFO = OKGREEN
TODO = OKBLUE
DEBUG = HEADER
ERROR = ERR

prefixDict = dict(
    ERR = ERR
    , ERROR = ERR
    , WARN = WARN
    , FATAL = ERR
    , INFO = INFO
    , TODO = TODO
    , NOTE = HEADER
    , DEBUG = DEBUG
    , STEP = INFO
    , FAIL = ERR
    , FAILED = ERR
    )

def colored(msg, label="INFO") :
    """
    Return a colored string. Formatting is optional.

    At each ` we toggle the color.
    
    """
    global prefixDict
    if label in prefixDict :
        color = prefixDict[label]
    else :
        color = ""
    txt = ''
    newMsg = msg.split('`')
    i = 0
    for m in newMsg:
        if i % 2 == 0:
            txt += color + m
        else:
            txt += ENDC + m
        i += 1
    return "{0} {1}".format(txt, ENDC)

def cl(msg, label="INFO"):
    return colored(msg, label)

def dump(label, msg, frame=None, exception=None):
    ''' If msg is a list then first msg in list is the main message. Rest are
    sub message which should be printed prefixed by \n\t.
    '''

    prefix = '[{0}] '.format(label)

    ''' Enable it if you want indented messages 
    stackLength = len(inspect.stack()) - 1
    if stackLength == 1:
        prefix = '\n[{}] '.format(label)
    else:
        prefix = ' '.join(['' for x in range(stackLength)])
    '''

    if type(msg) == list:
        if len(msg) > 1:
            msg = [msg[0]] + ["`|- {0}`".format(x) for x in msg[1:]] 
        msg ="\n\t".join(msg)


    if not frame :
        print((prefix+"{0}".format(colored(msg,label))))
    else :
        filename = frame.f_code.co_filename
        filename = "/".join(filename.split("/")[-2:])
        print((prefix+"@{0}:{1} {2}".format(filename, frame.f_lineno, colored(msg, label))))
    if exception:
        print((" [Expcetion] {0}".format(exception)))

def info(msg): dump("INFO", msg)
def warn(msg): dump("WARN", msg)

def error(msg): 
    dump("ERROR", msg)

def fatal(msg):
    dump("FATAL", msg)
    raise Exception(msg)

def debug(msg): dump("DEBUG", msg)

def log(label, msg): dump(label, msg)

## Following prints the elements in model
def modelInfo(path = '/##', **kwargs):
    """Generate the list of all available moose-elements in model
    """
    info("Couting elements in model under %s" % path)
    msg = []
    types = [ "Table", "Table2", "Compartment", "Pool", "BufPool", "Enz", "Reac" ]
    for t in types:
        paths = moose.wildcardFind("{}[TYPE={}]".format(path, t))
        if len(paths) > 0:
            msg.append("{:>20} : {}".format(t, len(paths)))
    return "\n".join(msg)
