# -*- coding: utf-8 -*-
# mtypes.py ---
#
# Filename: mtypes.py
# Description:
# Author:
# Maintainer:
# Created: Fri Feb  8 11:29:36 2013 (+0530)
# Version:
# Last-Updated: Tue Mar  1 02:52:35 2016 (-0500)
#           By: subha
#     Update #: 182
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Utility to detect the model type in a file
#
#

# Change log:
#
#
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:

from __future__ import print_function
import re
import moose

##!!!!!!!!!!!!!!!!!!!!!!!!!!!!
## !! This was stolen from
## http://eli.thegreenplace.net/2011/10/19/perls-guess-if-file-is-text-or-binary-implemented-in-python/#id2

import sys
PY3 = sys.version_info[0] >= 3

# A function that takes an integer in the 8-bit range and returns
# a single-character byte object in py3 / a single-character string
# in py2.
#
int2byte = (lambda x: bytes((x,))) if PY3 else chr

_text_characters = (
        b''.join(int2byte(i) for i in range(32, 127)) +
        b'\n\r\t\f\b')

def istextfile(fileobj, blocksize=512):
    """ Uses heuristics to guess whether the given file is text or binary,
        by reading a single block of bytes from the file.
        If more than 30% of the chars in the block are non-text, or there
        are NUL ('\x00') bytes in the block, assume this is a binary file.

        - Originally written by Eli Bendersky from the algorithm used
          in Perl:
          http://eli.thegreenplace.net/2011/10/19/perls-guess-if-file-is-text-or-binary-implemented-in-python/#id2
    """
    block = fileobj.read(blocksize)
    if b'\x00' in block:
        # Files with null bytes are binary
        return False
    elif not block:
        # An empty file is considered a valid text file
        return True

    # Use translate's 'deletechars' argument to efficiently remove all
    # occurrences of _text_characters from the block
    nontext = block.translate(None, _text_characters)
    return float(len(nontext)) / len(block) <= 0.30

## Eli Bendersky till here !!
##!!!!!!!!!!!!!!!!!!!!!!!!!!!!


def getType(filename, mode='t'):
    """Returns the type of the model in file `filename`. Returns None
    if type is not known.

    mode: 'b' for binary, 't' for text. Not used currently.

    """
    mtype = None
    msubtype = None
    if mode == 't':
        for typename, typefunc in list(typeChecks.items()):
            if typefunc(filename):
                return typename
    return None

def getSubtype(filename, typename):
    """Returns what subtype of the specified `typename` is the model file.
    None if could not resolve the subtype.
    """
    for subtype in subtypes[typename]:
        subtypeFunc = subtypeChecks['%s/%s' % (typename, subtype)]
        if subtypeFunc(filename):
            return subtype
    return ''

# Dictionary of model description types and functions to detect them.
#
# Fri Feb 8 11:07:41 IST 2013 - as of now we only recognize GENESIS .g
# XML .xml extensions
#
# Fri Feb 22 16:29:43 IST 2013 - added .cspace



isCSPACE = lambda x: x.lower().endswith('.cspace')
isGENESIS = lambda x: x.lower().endswith('.g') or x.endswith('.p')
isXML = lambda x: x.lower().endswith('.xml')

isProto = lambda x: x.lower().endswith('.p')

import xml.dom.minidom as md

def isNeuroML(filename):
    """Check if a model is in neuroml format. An xml document is
    considered a neuroml if the top level element is either
    'networkml', morphml', 'channelml' or 'neuroml'.

    """
    doc = md.parse(filename)
    for child in doc.childNodes:
        print(child.nodeName, child.nodeType == child.ELEMENT_NODE)
        if child.nodeType == child.ELEMENT_NODE and \
                (child.nodeName == 'networkml' or \
                     child.nodeName == 'morphml' or \
                     child.nodeName == 'channelml'or \
                     child.nodeName == 'neuroml'):
            return True
    return  False

def isSBML(filename):
    """Check model in `filename` is in SBML format."""
    doc = md.parse(filename)
    for child in doc.childNodes:
        if child.nodeType == child.ELEMENT_NODE and \
                child.nodeName == 'sbml':
            return True
    return  False

def isKKIT(filename):
    """Check if `filename` is a GENESIS/KINETIKIT file.

    """
    pattern = re.compile('include\s+kkit') # KKIT files must have "include kkit" statement somewhere
    with open(filename, 'r') as infile:
        while True:
            sentence = ''
            contd = False # Flags if we are inside a multi-line entry
            line = infile.readline()
            if not line: # End of file
                return False
            line = line.strip()
            # print 'read:', line
            if line.find('//') == 0: # skip c++ style comment lines
                # print 'c++ comment'
                continue
            # Skip C style multi-line comments
            comment_start = line.find('/*')
            if comment_start >= 0:
                sentence = line[:comment_start]
                # print 'cstart', comment_start, sentence
            while comment_start >= 0 and line:
                # print '#', line
                comment_end = line.find('*/')
                if comment_end >= 0:
                    comment_start = -1;
                    line = line[comment_end+2:] # keep the rest of the line
                    break
                line = infile.readline()
                if line:
                    line = line.strip()
            # Keep extending the sentence with next line if current
            # line ends with continuation token '\'
            if line:
                contd = line.endswith('\\')
            while line and contd:
                sentence += ' ' + line[:-1]
                line = infile.readline()
                if line:
                    line = line.strip()
                    contd = line.endswith('\\')
            # if contd turned false, the last line came out of the
            # while loop unprocessed
            if line:
                sentence += ' ' + line
            # print '>', sentence
            if re.search(pattern, sentence):
                return True
    return False

# Mapping model types to functions to check them.  These functions
# should take the filename as the single argument and return True or
# False. Define new functions for new model type checks and add them
# here.
typeChecks = {
    'cspace': isCSPACE,
    'genesis': isGENESIS,
    'xml': isXML,
}

# These are "type/subtype": function maps for checking model subtype
# New model types with subtypes should be added here with
# corresponding checker function.
subtypeChecks = {
    'genesis/kkit': isKKIT,
    'genesis/proto': isProto,
    'xml/neuroml': isNeuroML,
    'xml/sbml': isSBML,
}

# Mapping types to list of subtypes.
subtypes = {
    'cspace': [],
    'genesis': ['kkit', 'proto'],
    'xml': ['neuroml', 'sbml'],
}



#
# mtypes.py ends here
