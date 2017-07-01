# -*- coding: utf-8 -*-

"""setup.py: This
scripts prepare MOOSE for PyPI.

Last modified: Mon Jul 28, 2014  12:52AM

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import os
import sys
import shutil

from setuptools import setup
from distutils.core import Command, Extension
from distutils.command.install import install as _install
from distutils.command.build import build as _build
from distutils.command.build_py import build_py as _build_py


import distutils.spawn as ds

build_dir = 'buildMooseUsingCmake'
if not os.path.isdir(build_dir):
    os.makedirs(build_dir)

class BuildCommand(_build):
    """This command builds """
    user_options = _build.user_options + []

    def initialize_options(self):
        self.cwd = os.getcwd()
        self.build_base = '/tmp'
        self.build_temp = '/tmp'
        self.build_lib = '/tmp'
        self.new_dir = os.path.join(os.path.split(__file__)[0], build_dir)

    def finalize_options(self):
        pass

    def get_source_files(self):
        return []

    def run(self):
        print("++ Building MOOSE")
        os.chdir(self.new_dir)
        try:
            ds.spawn(['cmake',  '..' ])
            ds.spawn(['make', '_moose'])
        except ds.DistutilsExecError as e:
            print("Can't build MOOSE")
            print(e)
            os.chdir(self.cwd)
            sys.exit(-1)
        os.chdir(self.cwd)

class InstallCommand(_install):
    user_options = _install.user_options + [
            ('single-version-externally-managed', None, '')
            ]

    def initialize_options(self):
        _install.initialize_options(self)
        self.cwd = os.getcwd()
        self.single_version_externally_managed = False
        self.record = None
        self.build_lib = None

    def finalize_options(self):
        _install.finalize_options(self)

    def run(self):
        self.new_dir = os.path.join(os.path.split(__file__)[0], build_dir)
        os.chdir(self.new_dir)
        try:
            ds.spawn(['cmake',  '..' ])
            ds.spawn(['make', '_moose'])
        except ds.DistutilsExecError as e:
            print("Can't build MOOSE")
            print(e)
            os.chdir(self.cwd)
            sys.exit(-1)
        os.chdir(self.cwd)

        print("++ Installing PyMOOSE")
        self.new_dir = os.path.join(os.path.split(__file__)[0], 'python')
        os.chdir(self.new_dir)
        try:
            ds.spawn(["python", "setup.py", "install"])
        except ds.DistutilsExecError as e:
            print("Can't install PyMOOSE")
            print(e)
            os.chdir(self.cwd)
            sys.exit(-1)
        os.chdir(self.cwd)

class BuildPyCommand(_build_py):
    """Build PyMoose for distribution"""
    user_options =  _build_py.user_options + [
            ( 'build_lib', None, 'Build library' )
            ]

    def initialize_options(self):
        self.data_files = []
        self.build_lib = '/tmp'
        self.cwd = os.getcwd()
        self.compiler = None
        self.new_dir = os.path.join(os.path.split(__file__)[0], 'python')

    def finalize_options(self):
        pass

    def run(self):
        pass

##
# @brief FUnction to read a file.
#
# @param fname Name of the file.
#
# @return  A string content of the file.
def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

name          = 'moose'
version       = '3.0'
description   = (
        'MOOSE is the Multiscale Object-Oriented Simulation Environment. '
        'It is the base and numerical core for large, detailed simulations '
        'including Computational Neuroscience and Systems Biology.' )
url           = 'http://moose.ncbs.res.in/'


setup(
        name = name
        , version = version
        , author = "Upinder Bhalla et. al."
        , author_email = "bhalla@ncbs.res.in"
        , maintainer = 'Dilawar Singh'
        , maintainer_email = 'dilawars@ncbs.res.in'
        , description = description
        , license = "LGPL"
        , url = url
        , long_description = read('README')
        , ext_modules = [
            Extension('_moose', [ '*' ])
            ]
        , cmdclass = {
             'install' : InstallCommand
            , 'build_py' : BuildPyCommand
            , 'build_ext' : BuildCommand
            }
        , require = [ 'python-qt4' ]
        , keywords = "neural simulation"
        , classifiers=[
            'Intended Audience :: Science/Research',
            'Operating System :: Linux',
            'Programming Language :: Python',
            'Programming Language :: C++',
            ]
        )
