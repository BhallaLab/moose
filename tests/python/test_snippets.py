"""test_snippets.py: 

    This script tests all the snippets.

"""
from __future__ import print_function
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2015, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import tempfile
import subprocess
import re

results_ = {}
pat_ = re.compile(r'show\(\)|draw\(\)')
max_run_ = 2

def executeCode(filepath):
    print("Executing %s" % filepath)
    code = []
    with open(filepath, "r") as f:
        for l in f.readlines():
            if not pat_.search(l):
                code.append(l)
    newfile = tempfile.NamedTemporaryFile(delete=False)
    newfile.write("".join(code))

    ret = subprocess.call(["python", newfile.name], shell=False)
    if ret:
        results_[filepath] = 'Passed'
    else:
        results_[filepath] = 'Failed'

    #p = subprocess.Popen(["python", newfile.name]
            #, stderr = subprocess.PIPE
            #, stdout = subprocess.PIPE
            #)
    #output, err = p.communicate()
    #print output, err

def main():
    snippetPath = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../Demos/snippets')
    print("Testing snippets in %s" % snippetPath)
    print("[INFO] Disabling all pylab/matplotlib show() functions")
    files = []
    for d, subd, fs in os.walk(snippetPath):
        for f in fs:
            if ".py" in f:
                files.append(os.path.join(d, f))
    [ executeCode(f) for f in files[0:] ]
    for k in results_:
        print("{0} {1}".format(results_[k], k))

if __name__ == '__main__':
    main()
