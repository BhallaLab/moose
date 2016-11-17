# create_all_rstdoc.py --- 
# 
# Filename: create_all_rstdoc.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Mon Jun 30 21:35:07 2014 (+0530)
# Version: 
# Last-Updated: 
#           By: 
#     Update #: 0
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
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
"""Dump reStructuredText of moose builtin class docs as well as those
built into pymoose and moose python scripts."""

import sys
sys.path.append('../../../../moose-core/python')
import cStringIO
import re
import inspect
from datetime import datetime
import pydoc
import moose

# We assume any non-word-constituent character in the start of C++
# type name to be due to the name-mangling done by compiler on
# templated or user defined types.
type_mangling_regex = re.compile('^[^a-zA-Z]+')

finfotypes = dict(moose.finfotypes)

def extract_finfo_doc(cinfo, finfotype, docio, indent='   '):
    """Extract field documentation for all fields of type `finfotype`
    in class `cinfo` into `docio`.

    Parameters
    ----------
    cinfo: moose.Cinfo 
        class info object in MOOSE.
    
    ftype: str
        finfo type (valueFinfo/srcFinfo/destFinfo/lookupFinfo/sharedFinfo

    docio: StringIO
        IO object to write the documentation into
    """
    data = []
    try:
        finfo = moose.element('%s/%s' % (cinfo.path, finfotype)).vec
    except ValueError:
        return
    for field_element in finfo:
        dtype = type_mangling_regex.sub('', field_element.type)
        if len(dtype.strip()) == 0:
            dtype = 'void'
        if finfotype.startswith('dest'):
            name = '.. py:method:: {0}'.format(field_element.fieldName)
            dtype = ''
        else:
            name = '.. py:attribute:: {0}'.format(field_element.fieldName)
            dtype = '{0}'.format(dtype)
        doc = field_element.docs.replace('_', '\\_')
        
        docio.write('{0}{1}\n\n'.format(indent, name).replace('_', '\\_'))
        ftype = '{0} (*{1}*)\n\n'.format(dtype, finfotypes[finfotype]).strip()
        docio.write('{0}   {1}'.format(indent, ftype))
        for line in doc.split('\n'):
            docio.write('{0}   {1}\n'.format(indent, line))
        docio.write('\n\n')

def extract_class_doc(name, docio, indent='   '):
    """Extract documentation for Cinfo object at path
    
    Parameters
    ----------
    name: str
        path of the class.

    docio: StringIO
        output object to write the documentation into.
    """
    cinfo = moose.Cinfo('/classes/%s' % (name))
    docs = cinfo.docs
    docio.write('{0}.. py:class:: {1}\n\n'.format(indent, cinfo.name).replace('_', '\\_'))
    if docs:                    
        docs = docs.split('\n')
        # We need these checks to avoid mis-processing `:` within
        # description of the class
        name_done = False
        author_done = False
        descr_done = False
        for doc in docs:
            if not doc:
                continue
            field = None
            if not (name_done and author_done and descr_done):
                pos = doc.find(':')         
                field = doc[:pos]
            if field.lower() == 'name':
                name_done = True
                continue
            elif field.lower() == 'author':
                author_done = True
                continue          
            elif field.lower() == 'description':
                descr_done = True
                content = doc[pos+1:].strip()
            else:
                content = doc
            content = content.replace('_', '\\_')
            docio.write('{0}   {1}\n'.format(indent, content))
    docio.write('\n')
    for finfotype in finfotypes.keys():
	extract_finfo_doc(cinfo, finfotype, docio, indent + '   ')

def extract_all_class_doc(docio, indent='   '):
    for cinfo in moose.element('/classes').children:
	extract_class_doc(cinfo.name, docio, indent=indent)

def extract_all_func_doc(docio, indent='   '):
    for fname, fdef in (inspect.getmembers(moose, inspect.isbuiltin) +
                        inspect.getmembers(moose, inspect.isfunction)):
	docio.write('\n{}.. py:func:: {}\n'.format(indent, fname).replace('_', '\\_'))
	doc = inspect.getdoc(fdef)
	doc = doc.split('\n')
	drop = []
	for i in range(len(doc)):
	    di = doc[i]
	    doc[i] = di
	    hyphen_count = di.count('-')
	    if hyphen_count > 0 and hyphen_count == len(di) and i > 0:
		drop.append(i)
		doc[i-1] = indent + doc[i-1]
	for i in range(len(doc)):
	    if i not in drop:
		docio.write(doc[i].replace('_', '\\_') + '\n\n')


if __name__ == '__main__':
    classes_doc = 'moose_classes.rst'
    builtins_doc = 'moose_builtins.rst'
    #overview_doc = 'moose_overview.rst'
    if len(sys.argv)  > 1:
        classes_doc = sys.argv[1]
    if len(sys.argv) > 2:
        builtins_doc = sys.argv[2]
    if len(sys.argv) > 3:
        overview_doc = sys.argv[3]
    ts = datetime.now()

    # # MOOSE overview - the module level doc - this is for extracting
    # # the moose docs into separate component files.
    # overview_docio = open(overview_doc, 'w')
    # overview_docio.write('.. MOOSE overview\n')
    # overview_docio.write('.. As visible in the Python module\n')
    # overview_docio.write(ts.strftime('.. Auto-generated on %B %d, %Y\n'))
    # overview_docio.write('\n'.join(pydoc.getdoc(moose).split('\n')).replace('_', '\\_'))
    # overview_docio.write('\n')        
        
    
    # if isinstance(overview_docio, cStringIO.OutputType):
    #     print overview_docio.getvalue()
    # else:
    #     overview_docio.close()
    
    ## Builtin docs - we are going to do something like what autodoc
    ## does for sphinx. Because we cannot afford to build moose on
    ## servers like readthedocs, we ourselvs ectract the docs into rst
    ## files.
#     builtins_docio = open(builtins_doc, 'w')
#     builtins_docio.write('.. Documentation for all MOOSE builtin functions\n')
#     builtins_docio.write('.. As visible in the Python module\n')
#     builtins_docio.write(ts.strftime('.. Auto-generated on %B %d, %Y\n'))
#     builtins_docio.write('''

# # MOOSE Builitin Classes and Functions
# # ====================================
# #     ''')
#     builtins_docio.write('\n.. py:module:: moose\n')    
#     indent = '   '
#     for item in ['vec', 'melement', 'LookupField', 'DestField', 'ElementField']:
#         builtins_docio.write('\n\n{0}.. py:class:: {1}\n'.format(indent, item).replace('_', '\\_'))        
#         class_obj = eval('moose.{0}'.format(item))
#         doc = pydoc.getdoc(class_obj).replace('_', '\\_')
#         for line in doc.split('\n'):
#             builtins_docio.write('\n{0}{0}{1}'.format(indent, line))
#         for name, member in inspect.getmembers(class_obj):
#             if name.startswith('__'):
#                 continue
#             if inspect.ismethod(member) or inspect.ismethoddescriptor(member):
#                 builtins_docio.write('\n\n{0}{0}.. py:method:: {1}\n'.format(indent, name).replace('_', '\\_'))
#             else:
#                 builtins_docio.write('\n\n{0}{0}.. py:attribute:: {1}\n'.format(indent, name).replace('_', '\\_'))
#             doc = inspect.getdoc(member).replace('_', '\\_')
#             for line in doc.split('\n'):
#                 builtins_docio.write('\n{0}{0}{0}{1}'.format(indent, line))
 
#     for item in ['pwe', 'le', 'ce', 'showfield', 'showmsg', 'doc', 'element',
#                  'getFieldNames', 'copy', 'move', 'delete',
#                  'useClock', 'setClock', 'start', 'reinit', 'stop', 'isRunning',
#                  'exists', 'writeSBML', 'readSBML', 'loadModel', 'saveModel',
#                  'connect', 'getCwe', 'setCwe', 'getFieldDict', 'getField',
#                  'seed', 'rand', 'wildcardFind', 'quit']:
#         builtins_docio.write('\n\n{0}.. py:function:: {1}\n'.format(indent, item).replace('_', '\\_'))        
#         doc = inspect.getdoc(eval('moose.{0}'.format(item))).replace('_', '\\_')
#         for line in doc.split('\n'):
#             builtins_docio.write('\n{0}{0}{1}'.format(indent, line))
#         builtins_docio.write('\n')                    
#     if isinstance(builtins_docio, cStringIO.OutputType):
# 	print builtins_docio.getvalue()
#     else:
# 	builtins_docio.close()
    # This is the primary purpos
    classes_docio = open(classes_doc, 'w')
    classes_docio.write('.. Documentation for all MOOSE classes and functions\n')
    classes_docio.write('.. As visible in the Python module\n')
    classes_docio.write(ts.strftime('.. Auto-generated on %B %d, %Y\n'))
    
    classes_docio.write('''

MOOSE Classes
==================
''')
    extract_all_class_doc(classes_docio, indent='')
#     classes_docio.write('''
# =================
# MOOSE Functions
# =================
# ''')
#     extract_all_func_doc(classes_docio, indent='')
    if isinstance(classes_docio, cStringIO.OutputType):
	print classes_docio.getvalue()
    else:
	classes_docio.close()
    
    
        
    



# 
# create_all_rstdoc.py ends here
