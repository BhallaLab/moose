# -*- coding: utf-8 -*-
# This script can also be called directly to build and install the pymoose
# module.
#
# Alternatively, you can use cmake build system which provides finer control
# over the build. This script is called by cmake to install the python module. 
# 
# This script is compatible with python2.7 and python3+. Therefore use of
# super() is commented out.
#
# NOTES:
#  * Python2
#   - Update setuptools using `python2 -m pip install setuptools --upgrade --user'.

__author__     = "Dilawar Singh"

__copyright__  = "Copyright 2019-, Dilawar Singh"
__maintainer__ = "Dilawar Singh"
__email__      = "dilawar.s.rajput@gmail.com"

import os
import sys
import multiprocessing
import subprocess
import datetime

try:
    cmakeVersion = subprocess.call(["cmake", "--version"],
            stdout=subprocess.PIPE)
except Exception as e:
    print(e)
    print("[ERROR] cmake is not found. Please install cmake.")
    quit(-1)

# See https://docs.python.org/3/library/distutils.html
# setuptools is preferred over distutils. And we are supporting python3 only.
from setuptools import setup, Extension, Command
from setuptools.command.build_ext import build_ext  as _build_ext
import subprocess

# Global variables.
sdir_ = os.path.dirname(os.path.realpath(__file__))

stamp = datetime.datetime.now().strftime('%Y%m%d')
builddir_ = os.path.join(sdir_, '_temp__build')

if not os.path.exists(builddir_):
    os.makedirs(builddir_)

numCores_ = multiprocessing.cpu_count()


version_ = '3.2.1.dev%s' % stamp

# importlib is available only for python3. Since we build wheels, prefer .so
# extension. This way a wheel built by any python3.x will work with any python3.

class CMakeExtension(Extension):
    def __init__(self, name, **kwargs):
        # don't invoke the original build_ext for this special extension
        import tempfile
        # Create a temp file to create a dummy target. This build raises an
        # exception because sources are empty. With python3 we can fix it by
        # passing `optional=True` to the argument. With python2 there is no
        # getaway from it.
        f = tempfile.NamedTemporaryFile(suffix='.cpp', delete=False)
        f.write(b'int main() { return 1; }')
        Extension.__init__(self, name, sources=[f.name], **kwargs)
        f.close()

class TestCommand(Command):
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        print("[INFO ] Running tests... ")
        os.chdir(builddir_)
        self.spawn(["ctest", "--output-on-failure", '-j%d'%numCores_])
        os.chdir(sdir_)

class build_ext(_build_ext):
    user_options = [
            ('with-boost', None, 'Use Boost Libraries (OFF)')
            , ('with-gsl', None, 'Use Gnu Scienfific Library (ON)')
            , ('with-gsl-static', None, 'Use GNU Scientific Library (static library) (OFF)') 
            , ('debug', None, 'Build moose in debugging mode (OFF)')
            , ('no-build', None, 'DO NOT BUILD. (for debugging/development)')
            ] + _build_ext.user_options

    def initialize_options(self):
        # Initialize options.
        self.with_boost = 0
        self.with_gsl = 1
        self.with_gsl_static = 0
        self.debug = 0
        self.no_build = 0
        self.cmake_options = {}
        #  super().initialize_options()
        _build_ext.initialize_options(self)

    def finalize_options(self):
        # Finalize options.
        #  super().finalize_options()
        _build_ext.finalize_options(self)
        self.cmake_options['PYTHON_EXECUTABLE'] = os.path.realpath(sys.executable)
        self.cmake_options['VERSION_MOOSE'] = version_
        if self.with_boost:
            self.cmake_options['WITH_BOOST'] = 'ON'
            self.cmake_options['WITH_GSL'] = 'OFF'
        else:
            if self.with_gsl_static:
                self.cmake_options['GSL_USE_STATIC_LIBRARIES'] = 'ON'
        if self.debug:
            self.cmake_options['CMAKE_BUILD_TYPE'] = 'Debug'
        else:
            self.cmake_options['CMAKE_BUILD_TYPE'] = 'Release'

    def run(self):
        if self.no_build:
            return
        for ext in self.extensions:
            self.build_cmake(ext)
        #  super().run()
        _build_ext.run(self)

    def build_cmake(self, ext):
        global numCores_
        global sdir_
        print("\n==========================================================\n")
        print("[INFO ] Building pymoose in %s ..." % builddir_)
        cmake_args = []
        for k, v in self.cmake_options.items():
            cmake_args.append('-D%s=%s' % (k,v))
        os.chdir(str(builddir_))
        self.spawn(['cmake', str(sdir_)] + cmake_args)
        if not self.dry_run: 
            self.spawn(['make', '-j%d'%numCores_]) 
        os.chdir(str(sdir_))

with open(os.path.join(sdir_,  "README.md")) as f:
    readme = f.read()

setup(
    name="pymoose",
    version=version_,
    description= 'Python scripting interface of MOOSE Simulator (https://moose.ncbs.res.in)',
    long_description=readme,
    long_description_content_type='text/markdown',
    author='MOOSERes',
    author_email='bhalla@ncbs.res.in',
    maintainer='Dilawar Singh',
    maintainer_email='dilawars@ncbs.res.in',
    url='http://moose.ncbs.res.in',
    packages=[
        'rdesigneur', 'moose', 'moose.SBML', 'moose.genesis', 'moose.neuroml',
        'moose.neuroml2', 'moose.chemUtil', 'moose.chemMerge'
    ],
    package_dir={
        'rdesigneur': os.path.join(sdir_, 'python', 'rdesigneur'),
        'moose': os.path.join(sdir_, 'python', 'moose')
    },
    package_data={
        'moose': [
            '_moose.so'
            , os.path.join('neuroml2','schema','NeuroMLCoreDimensions.xml')
            , os.path.join('chemUtil', 'rainbow2.pkl')
        ]
    },
    # python2 specific version here as well.
    install_requires=['numpy', 'matplotlib'],
    extra_requires={'dev' : [ 'coverage', 'pytest', 'pytest-cov' ]},
    ext_modules=[CMakeExtension('dummy', optional=True)],
    cmdclass={'build_ext': build_ext, 'test': TestCommand},
)
