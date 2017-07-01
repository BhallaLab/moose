# -*- coding: utf-8 -*-
# hdfutil.py ---
#
# Filename: hdfutil.py
# Description:
# Author:
# Maintainer:
# Created: Thu Aug 23 17:34:55 2012 (+0530)
# Version:
# Last-Updated: Mon Sep  3 17:55:03 2012 (+0530)
#           By: subha
#     Update #: 618
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Utility function to save data in hdf5 format.
#
# In this utility we are trying to address a serialization
# problem. The ultimate goal is to be able to save a snapshot of
# complete simulator state in a portable format so that it can be
# loaded later to reach that state and continue from there.
#
# TODO: random number generators: the full RNG state has to be
# saved. MOOSE does not provide access to this at user level.
#
# TODO: what about internal variables? they affect the state of the
# simulation yet MOOSE does not allow access to these variables. Do we
# need a change in the API to make all internal variables accessible
# in a generic manner?
#
# TODO: How do we translate MOOSE tree to HDF5? MOOSE has vec and
# elements. vec is a container and each element belongs to an
# vec.
#
#                    em-0
#              el-00 el-01 el-02
#               /      |       \
#              /       |        \
#             em-1     em-2      em-3
#           el-10    el-20     el-30 el-31 el-32 el-33
#                     /                 \
#                    /                   \
#                   em-4                 em-5
#                el-40 el-41             el-50
#
#
# Serializing MOOSE tree structure into an HDF5 tree structure has
# some issues to be resolved.  Each vec is saved as a HDF
# group. All the elements inside it as a HDF dataset.  But the problem
# is that HDF datasets cannot have children. But in MOOSE the
# parent-child relation is opposite, each element can have one or more
# ematrices as children.
#
# Serializing MOOSE tree structure into HDF5 tables for each class.
# This is the approach I took initially. This is possibly more space
# saving.


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
try:
    from future_builtins import zip
except ImportError:
    pass
from . import moose as moose__
import numpy as np
import h5py as h5
import time
from collections import defaultdict

size_step=256


# maps cpp data type names to numpy data types
cpptonp = {
    'int': 'i4',
    'long': 'i8',
    'bool': 'b',
    'unsigned int': 'u4',
    'unsigned long': 'u8',
    'float': 'f4',
    'double': 'f8',
    'string': 'S1024',
    'Id': 'S1024',
    'ObjId': 'S1024',
    }

dtype_table = {}
# dims allows only 3 dims for the time being
em_dtype = np.dtype([('path', 'S1024'), ('class', 'S64'), ('dims', 'i4', (3,))])

def get_rec_dtype(em):
    bad_fields = []
    # Check if already have data type information for this class.
    # Create it otherwise.
    if em.className in dtype_table:
        dtype = dtype_table[em.className]
    else:
        print('Creating entries for class:', obj.className)
        fielddict = moose__.getFieldDict(obj.className, 'valueFinfo')
        print(fielddict)

        # If we do not have the type of this field in cpp-np data
        # type map, skip it. We can handle vectors as nested
        # table, but the problem with that is numpy requires all
        # entries to have a fixed size for this table. Since
        # vectors can have arbitrary length, we cannot get such a
        # fixed size without checking the size of the vector field
        # in all elements.
        fields = [(fieldname, cpptonp[ftype]) # [('path', 'S1024')]
                  for fieldname, ftype in sorted(fielddict.items())
                  if ftype in cpptonp]
        dtype_table[obj.className] = np.dtype(fields)
        return dtype_table[obj.className]

def save_dataset(classname, rec, dtype, hdfnode):
    """Saves the data from rec into dataset"""
    # Check if there is a dataset for this class. Create it
    # otherwise.
    if len(rec) == 0:
        return
    if classname in hdfnode:
        ds = hdfnode[classname]
        oldlen = ds.len()
        newlen = oldlen + len(rec)
        ds.resize(newlen)
    else:
        ds = hdfnode.create_dataset(classname,
                                    shape=(len(rec),),
                                    dtype=dtype,
                                    compression='gzip',
                                    compression_opts=6)
    ds[oldlen:newlen] = np.rec.array(rec, dtype=dtype)

def savetree(moosenode, hdfnode):
    """Dump the MOOSE element tree rooted at moosenode as datasets
    under hdfnode."""
    # Keep track of total number of ematrices seen in each class.
    obj_count = defaultdict(int)
    # Buffer the last `size_step` object records for each class in
    # this array.
    obj_rec = defaultdict(list)
    em_rec = []
    hdfnode.attr['path'] = moosenode.path
    elements = hdfnode.create_group('elements')
    for em in moose.wildcardFind(moosenode.path+'/##'):
        em_rec.append((em.path, em.className, em.shape))
        dtype = get_rec_dtype(em)
        for obj in em:
            fields = []
            for fname in dtype.names:
                f = obj.getField(fname)
                if isinstance(f, moose.vec) or isinstance(f, moose.element):
                    fields.append(f.path)
                else:
                    fields.append(f)
            obj_rec[em.className].append(fields)
            obj_count[em.className] += 1
            if obj_count[em.className] % size_step == 0:
                save_dataset(em.className, obj_rec[em.className], dtype, elements)
                obj_rec[em.className][:] = [] # clear the records after saving
    # now save the remaining records (length < size_step)
    for classname, rec in list(obj_rec.items()):
        save_dataset(classname, rec, dtype_table[classname], hdfnode)
    vec = hdfnode.create_dataset('vec', shape=(len(em_rec),), dtype=em_dtype)
    vec[:] = em_rec

def loadtree(hdfnode, moosenode):
    """Load the element tree saved under the group `hdfnode` into `moosenode`"""
    pathclass = {}
    basepath = hdfnode.attr['path']
    if basepath != '/':
        basepath = basepath + '/'
    emdata = hdfnode['vec'][:]
    sorted_paths = sorted(emdata['path'], key=lambda x: x.count('/'))
    dims = dict(emdata['path', 'dims'])
    classes = dict(emdata['path', 'class'])
    current = moose.getCwe()
    moose.setCwe(moosenode)
    # First create all the ematrices
    for path in sorted_paths:
        rpath = path[len(basepath):]
        classname = classes[path]
        shape = dims[path]
        em = moose.vec(rpath, shape, classname)
    wfields = {}
    for cinfo in moose__.element('/classes').children:
        cname = cinfo[0].name
        wfields[cname] = [f[len('set_'):] for f in moose__.getFieldNames(cname, 'destFinfo')
                          if f.startswith('set_') and f not in ['set_this', 'set_name', 'set_lastDimension', 'set_runTime'] and not f.startswith('set_num_')]
        for key in hdfnode['/elements']:
            dset = hdfnode['/elements/'][key][:]
            fieldnames = dset.dtype.names
            for ii in range(len(dset)):
                obj = moose__.element(dset['path'][ii][len(basepath):])
                for f in wfields[obj.className]:
                    obj.setField(f, dset[f][ii])



def savestate(filename=None):
    """Dump the state of MOOSE in an hdf5 file.

    The file will have a data set for each class.
    Each such dataset will be a column of field values.

    This function needs more work on moose serialization.
    """
    if filename is None:
        filename = 'moose_session_' + time.strftime('%Y%m%d_%H%M%S') + '.hdf5'
    with h5.File(filename, 'w') as fd:
        root = fd.create_group('/elements')
        meta = fd.create_group('/metadata')
        typeinfo_dataset = meta.create_dataset('typeinfo', shape=(size_step,), dtype=[('path', 'S1024'), ('class', 'S64'), ('dims', 'S64'), ('parent', 'S1024')], compression='gzip', compression_opts=6)
        typeinfo = []
        class_dataset_dict = {}
        class_count_dict = {}
        class_array_dict = {}
        objcount = 0
        for obj in moose__.wildcardFind("/##"):
            if obj.path.startswith('/Msg') or obj.path.startswith('/class') or obj.className == 'Table' or obj.className == 'TableEntry':
                continue
            print('Processing:', obj.path, obj.className)
            typeinfo.append((obj.path, obj.className, str(obj.shape), obj[0].parent.path))
            objcount += 1
            if len(typeinfo) == size_step:
                typeinfo_dataset.resize(objcount)
                typeinfo_dataset[objcount - size_step: objcount] = np.rec.array(typeinfo, typeinfo_dataset.dtype)
                typeinfo = []
            # If we do not yet have dataset for this class, create one and keep it in dict
            if obj.className not in class_dataset_dict:
                print('Creating entries for class:', obj.className)
                fielddict = moose__.getFieldDict(obj.className, 'valueFinfo')
                print(fielddict)
                keys = sorted(fielddict)
                fields = [] # [('path', 'S1024')]
                for fieldname in keys:
                    ftype = fielddict[fieldname]
                    if ftype in cpptonp:
                        fields.append((fieldname, cpptonp[ftype]))
                    elif ftype == 'Id' or ftype == 'ObjId':
                        fields.append((fieldname, 'S1024'))
                # print fields
                ds = root.create_dataset(obj.className, shape=(size_step,), dtype=fields, compression='gzip', compression_opts=6)
                class_dataset_dict[obj.className] = ds
                class_array_dict[obj.className] = []
                class_count_dict[obj.className] = 0
            # Lookup the dataset for the class this object belongs to
            ds = class_dataset_dict[obj.className]
            for entry in obj:
                fields = []
                print(entry.path, end=' ')
                for f in ds.dtype.names:
                    print('getting field:', f)
                    entry.getField(f)
                fields = [f.path if isinstance(f, moose__.vec) or isinstance(f, moose__.element) else f for f in fields]
                class_array_dict[obj.className].append(fields)
                # print 'fields:'
                # print fields
                # print 'length:', len(class_array_dict[obj.className])
                class_count_dict[obj.className] += 1
                if class_count_dict[obj.className] == size_step:
                    oldlen = ds.len()
                    if oldlen <= class_count_dict[obj.className]:
                        ds.resize((class_count_dict[obj.className]))
                    ds[oldlen: class_count_dict[obj.className]] = np.rec.array(class_array_dict[obj.className], dtype=ds.dtype)
                    class_array_dict[obj.className] = []
        for classname in class_array_dict:
            ds = class_dataset_dict[classname]
            ds.resize((class_count_dict[classname], ))
            if len(class_array_dict[classname]) > 0:
                start = class_count_dict[classname] - len(class_array_dict[classname])
                ds[start:] = np.rec.array(class_array_dict[classname], dtype=ds.dtype)

        if len(typeinfo) > 0:
            typeinfo_dataset.resize((objcount,))
            typeinfo_dataset[objcount-len(typeinfo): objcount] = np.rec.array(typeinfo, dtype=typeinfo_dataset.dtype)

def restorestate(filename):
    wfields = {}
    for cinfo in moose__.element('/classes').children:
        cname = cinfo[0].name
        wfields[cname] = [f[len('set_'):] for f in moose__.getFieldNames(cname, 'destFinfo')
                          if f.startswith('set_') and f not in ['set_this', 'set_name', 'set_lastDimension', 'set_runTime'] and not f.startswith('set_num_')]
    with h5.File(filename, 'r') as fd:
        typeinfo = fd['/metadata/typeinfo'][:]
        classdict = {}
        dimsdict = dict(zip(typeinfo['path'], typeinfo['dims']))
        classdict = dict(zip(typeinfo['path'], typeinfo['class']))
        parentdict = dict(zip(typeinfo['path'], typeinfo['parent']))
        sorted_paths = sorted(typeinfo['path'], key=lambda x: x.count('/'))
        for path in sorted_paths:
            name = path.rpartition('/')[-1].partition('[')[0]
            moose__.vec(parentdict[path] + '/' + name, eval(dimsdict[path]), classdict[path])
        for key in fd['/elements']:
            dset = fd['/elements/'][key][:]
            fieldnames = dset.dtype.names
            for ii in range(len(dset)):
                obj = moose__.element(dset['path'][ii])
                for f in wfields[obj.className]:
                    obj.setField(f, dset[f][ii])

#
# hdfutil.py ends here
