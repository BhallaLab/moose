#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""cmake_sanity_check.py: Check if Cmake files are ok.

Last modified: Sat Jan 18, 2014  05:01PM

NOTE: Run in this directory only.

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import re
from collections import defaultdict

makefiles = {}
cmakefiles = {}
makedirs = set()
cmakedirs = set()

def check(d):
    searchMakefiles(d)
    checkMissingCMake()
    checkSrcs()

def checkMissingCMake():
    if (makedirs - cmakedirs):
        print("[Failed] Test 1")
        print("Following directories have Makefile but not a CMakeFiles.txt file.")
        print("%s" % "\t\n".join(makedirs - cmakedirs))

def searchMakefiles(topdir):
    for d, subd, fs in os.walk(topdir):
        if "../_build" in d or ".." == d: continue
        if "CMakeLists.txt" in fs:
            cmakedirs.add(d)
            cmakefiles[d] = fs
        if "Makefile" in fs:
            if "_build" in d:
                continue
            else:
                makedirs.add(d)
                makefiles[d] = fs
        else: pass

def checkSrcs():
    objPat = re.compile(r"\w+\.o")
    srcPat = re.compile(r"\w+\.cpp")
    srcs = []
    csrcs = []
    for d in makefiles:
        with open(os.path.join(d, "Makefile"), "r") as f:
            txt = f.read()
            for i in txt.split("\n\n"):
                if "OBJ =" in i.upper():
                    for j in i.split():
                        if ".o" in j.strip():
                            srcs.append("%s"%(j.strip()))
        try:
            with open(os.path.join(d, "CMakeLists.txt"), "r") as f:
                txt = f.read()
                csrcs = srcPat.findall(txt)
        except:
            print("Dir {} does not have CMakeLists.txt".format(d))
            csrcs = []
        #print("[TEST 2] Checking if CMake is creating extra objects")
        for csr in csrcs:
            objName = csr.replace(".cpp", ".o")
            if objName in srcs:
                pass
            else:
                print(" Failed: In dir {}, CMake is creating extra object {}".format(d, objName))

                pass
        print("[TEST 3] Checking if CMake is missing some objects")
        for obj in srcs:
            srcName = obj.replace(".o", ".cpp")
            if srcName in csrcs: pass
            else:
                print(" Failed: In dir {}, CMake is missing object {}".format(d,
                    srcName))

def main():
    test_dir = sys.argv[1]
    check(test_dir)


if __name__ == '__main__':
    main()
