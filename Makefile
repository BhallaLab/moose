#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2003- 2006 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
#######################################################################
# NOTE:
# This Makefile is compatible with _GNU Make_.
# This does not work with nmake or borland make.
# You may have to specify some variables when calling gnu make as
# described in the comments below. The defaults should work on most
# Unix clones.
########################################################################

# Linux compilation:
# We recommend two levels of compilation: either full debug, with gdb,
# unit tests and all the rest, or an optimized version for production
# simulations, without any unit tests or assertions. If you want some
# intermediate levels, edit the flags.
######################################################################
#
#     ADDITIONAL COMMANDLINE VARIABLES FOR MAKE
#
######################################################################
# The variable BUILD determines if it should be optimized (release)
# or a debug version (default).
# make can be run with a command line parameter like below:
# make clean BUILD=debug
# make BUILD=debug
# another option is to define BUILD as an environment variable:
# export BUILD=debug
# make clean
# make
#
# There are some more variables which just need to be defined for
# controlling the compilation and the value does not matter. These are:
#
# USE_GSL - use GNU Scientific Library for integration in kinetic simulations
#
# USE_READLINE - use the readline library which provides command history and
# 		better command line editing capabilities
#
# GENERATE_WRAPPERS - useful for python interface developers. The binary created
# 		with this option looks for a directory named 'generated' in the
# 		working directory and creates a wrapper class ( one .h file
# 		and a .cpp file ) and partial code for the swig interface file
# 		(pymoose.i). These files with some modification can be used for
# 		generating the python interface using swig.
#
# USE_MPI - compile with support for parallel computing through MPICH library
#
# USE_SBML (default value: 0) - compile with support for the Systems Biology
# 		Markup Language (SBML). This allows you to read and write chemical
# 		kinetic models in the simulator-indpendent SBML format.
#

# Default values for flags. The operator ?= assigns the given value only if the
# variable is not already defined.
#USE_SBML?=0
USE_HDF5?=1
USE_CUDA?=0
USE_NEUROKIT?=0
PYTHON?=2
# BUILD (= debug, release)
ifndef BUILD
BUILD=release
endif

#If using mac uncomment the following lines
# PLATFORM=mac
#export PLATFORM

# Get the processor architecture - i686 or x86_64
# All these should be taken care of in a script, not in the
# Makefile. But we are
ifndef MACHINE
MACHINE=i686
endif
# We are assuming all non-win32 systems to be POSIX compliant
# and thus have the command uname for getting Unix system name
ifneq ($(OSTYPE),win32)
MACHINE=$(shell uname -m)
PLATFORM := $(shell uname -s)
endif

USE_NUMPY=1
# Debug mode:

ifeq ($(BUILD),debug)
CXXFLAGS = -g -O0 -fpermissive -fno-strict-aliasing -fPIC -fno-inline-functions -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER
USE_GSL = true
endif
# Optimized mode:
ifeq ($(BUILD),release)
CXXFLAGS  = -O3 -fpermissive -fno-strict-aliasing -fPIC -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER
USE_GSL = true
endif
# Profiling mode:
ifeq ($(BUILD),profile)
CXXFLAGS  = -O3 -pg  -fpermissive -fno-strict-aliasing -fPIC -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER
USE_GSL = true
endif
# Profiling mode with gperftoools
ifeq ($(BUILD),gperf)
CXXFLAGS  = -O3 -fpermissive -fno-strict-aliasing -fPIC -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER
LDFLAGS += -lprofiler -ltcmalloc
USE_GSL = true
endif
# Threading mode:
ifeq ($(BUILD),thread)
CXXFLAGS  = -O3 -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER
USE_GSL = true
endif

# MPI mode:
ifeq ($(BUILD),mpi)
CXXFLAGS = -g -fpermissive -fno-strict-aliasing -fPIC -fno-inline-functions -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER
USE_MPI = 1
USE_GSL = true
endif

# optimized MPI mode:
ifeq ($(BUILD),ompi)
CXXFLAGS  = -O3 -fpermissive -fno-strict-aliasing -fPIC -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER
USE_MPI = 1
USE_GSL = true
endif

# optimised mode but with unit tests.
ifeq ($(BUILD),odebug)
CXXFLAGS = -O3 -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER
USE_GSL = true
endif

# including SMOLDYN
ifdef USE_SMOLDYN
CXXFLAGS = -g -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER
USE_GSL = true
endif

# Use a strict compilation
ifeq ($(BUILD),developer)
    CXXFLAGS=-g \
	     -Wall -Werror -Wno-unused-variable -Wno-unused-function \
	     -DDO_UNIT_TESTS -DDEVELOPER -DDEBUG
    USE_GSL = true
endif
##########################################################################
#
# MAC OS X compilation, Debug mode:
ifeq ($(PLATFORM),Darwin)
CXXFLAGS += -DMACOSX # GCC compiler also sets __APPLE__ for Mac OS X which can be used instead
CXXFLAGS += -Wno-deprecated -force_cpusubtype_ALL -mmacosx-version-min=10.4
endif
# Use the options below for compiling on GCC4.1
# GNU C++ 4.1 and newer might need -ffriend-injection
#
#CXXFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -ffriend-injection -DUSE_GENESIS_PARSER

# Insert the svn revision no. into the code as a preprocessor macro.
# Only for release versions we want to pass SVN=0 to make.
# No more SVN so pass zero
SVN=0
ifndef SVN
SVN?=1
endif
empty :=
space := $(empty) $(empty)
ifneq ($(SVN),0)
# Some versions of svnrevision return "Unversioned directory" which causes confusion to gcc
SVN_REVISION := $(shell svnversion)
SVN_REVISION := $(subst $(space),_,$(SVN_REVISION))
# SVN_REVISION := $(subst :,_,$(SVN_REVISION))
ifneq ($(SVN_REVISION),export)
CXXFLAGS+=-DSVN_REVISION=\"$(SVN_REVISION)\"
endif
endif


# Libraries are defined below.
SUBLIBS =
# Notice that pthread is no more included
LIBS =	-L/usr/lib -L/usr/local/lib

#LIBS = 	-lm

# For 64 bit Linux systems add paths to 64 bit libraries
ifeq ($(PLATFORM),Linux)
CXXFLAGS += -DLINUX
ifeq ($(MACHINE),x86_64)
LIBS+= -L/lib64 -L/usr/lib64
endif
endif

##########################################################################
#
# Developer options (Don't try these unless you are writing new code!)
##########################################################################
# For parallel (MPI) version:
ifdef USE_MUSIC
USE_MPI = 1		# Automatically enable MPI if USE_MUSIC is on
CXXFLAGS += -DUSE_MUSIC
LIBS += -lmusic
endif

# The -DMPICH_IGNORE_CXX_SEEK flag is because of a bug in the
# MPI-2 standard. Enabled by default because it use crops up
# often enough. You won't need if if you are not using MPICH, or
# if your version of MPICH has fixed the issue.
ifdef USE_MPI
# CXXFLAGS += -DUSE_MPI
CXXFLAGS += -DUSE_MPI -DMPICH_IGNORE_CXX_SEEK
endif

#use this for readline library
#CXXFLAGS = -g -Wall -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_READLINE


# To use GSL, pass USE_GSL=true ( anything on the right will do) in make command line
ifdef USE_GSL
LIBS+= $(shell gsl-config --libs)
#LIBS+= -L/usr/lib -Wl,--no-as-needed -lgsl -lgslcblas -lm
#LIBS+= -L/usr/lib -lgsl -lgslcblas -lm
CXXFLAGS+= -DUSE_GSL
else
LIBS+= -lm
endif

#Saeed
# To use CUDA, pass USE_CUDA=1 in make command line
ifeq ($(USE_CUDA),1)
LIBS+= -L/usr/local/cuda/lib64 -LhsolveCuda/cudaLibrary  -lcuda -lcudart -lm -lmooseCudaLibrary
HCUDA_DIR = hsolveCuda
HCUDA_LIB = hsolveCuda/_hsolveCuda.o
endif

# To disable numpy pass USE_NUMPY=0
ifeq ($(USE_NUMPY),1)
CXXFLAGS+=-DUSE_NUMPY
endif

# To compile examples, pass EXAMPLES=true ( anything on the right will do) in make command line
ifdef EXAMPLES
EXAMPLES_DIR = examples
EXAMPLES_LIB = examples/_trials.o
endif

# To use Smoldyn, pass USE_SMOLDYN=true ( anything on the right will do) in make command line
ifdef USE_SMOLDYN
#LIBS+= -L/usr/local/lib -lsmoldyn
CXXFLAGS+= -DUSE_SMOLDYN
SMOLDYN_DIR = smol
SMOLDYN_LIB = smol/_smol.o /usr/local/lib/libsmoldyn.a
LIBS += -lsmoldyn
endif

# To compile with readline support pass USE_READLINE=true in make command line
ifdef USE_READLINE
LIBS+= -lreadline
CXXFLAGS+= -DUSE_READLINE
endif

# To compile with curses support (terminal aware printing) pass USE_CURSES=true in make command line
ifdef USE_CURSES
LIBS += -lcurses
CXXFLAGS+= -DUSE_CURSES
endif

ifdef USE_MUSIC
	MUSIC_DIR = music
	MUSIC_LIB = music/music.o
endif

# Here we automagically change compilers to deal with MPI.
ifdef USE_MPI
	CXX = mpicxx
#	CXX = /usr/local/mvapich2/bin/mpicxx
#	PARALLEL_DIR = parallel
#	PARALLEL_LIB = parallel/parallel.o
else
	CXX = g++
#	CXX = CC	# Choose between Solaris CC and g++ on a Solaris machine
endif

ifeq ($(USE_HDF5),1)
	CXXFLAGS+= -DUSE_HDF5  -DH5_NO_DEPRECATED_SYMBOLS -I/usr/local/hdf5/include -I/usr/include/hdf5/serial
	LIBS+= -lhdf5 -lhdf5_hl -L/usr/local/hdf5/lib -L/usr/lib/x86_64-linux-gnu/hdf5/serial/
endif

LD = ld

SUBDIR = \
	basecode \
	msg \
	shell \
	randnum\
	scheduling\
	mpi \
	builtins\
	utility \
	external/muparser \
	biophysics \
	synapse \
	intfire \
	kinetics \
	ksolve \
	mesh \
	hsolve \
	diffusion \
	device \
	signeur \
	benchmarks \
	$(SMOLDYN_DIR) \
	$(SBML_DIR) \
	$(HCUDA_DIR) \
	$(EXAMPLES_DIR) \

# Used for 'make clean'
CLEANSUBDIR = $(SUBDIR) $(PARALLEL_DIR) pymoose

OBJLIBS =	\
	basecode/_basecode.o \
	msg/_msg.o \
	shell/_shell.o \
	randnum/_randnum.o \
	scheduling/_scheduling.o \
	mpi/_mpi.o \
	builtins/_builtins.o \
	utility/_utility.o \
	external/muparser/_muparser.o \
	biophysics/_biophysics.o \
	synapse/_synapse.o \
	intfire/_intfire.o \
	kinetics/_kinetics.o \
	ksolve/_ksolve.o \
	hsolve/_hsolve.o \
	mesh/_mesh.o \
	diffusion/_diffusion.o \
	device/_device.o \
	signeur/_signeur.o \
	benchmarks/_benchmarks.o \
	$(SMOLDYN_LIB) \
	$(SBML_LIB) \
	$(HCUDA_LIB) \
	$(EXAMPLES_LIB) \


ifeq ($(USE_NEUROKIT),1)
	NEUROKIT_COMMAND = cd ./python/moogli; python setup.py build;  cd ../../; cp ./python/moogli/build/*/_moogli.so ./python/moogli/;
# else
# 	NEUROKIT_COMMAND = ""
endif


export CXX
export CXXFLAGS
export LD
export LIBS
export USE_GSL
#export USE_SBML

all: moose pymoose

neurokit: ./python/moogli/setup.py
	$(NEUROKIT_COMMAND)

moose: libs $(OBJLIBS) $(PARALLEL_LIB)
	$(CXX) $(CXXFLAGS) $(OBJLIBS) $(PARALLEL_LIB) $(LIBS) -o moose
	@echo "Moose compilation finished"

libmoose.so: libs
	$(CXX) -G $(LIBS) -o libmoose.so
	@echo "Created dynamic library"

# Get the python version
ifneq ($(OSTYPE),win32)
ifeq ($(PYTHON),3)
PYTHON_VERSION := $(subst ., ,$(lastword $(shell python3 --version 2>&1)))
PYTHON_CFLAGS := $(shell python3-config --cflags)
PYTHON_LDFLAGS := $(shell python3-config --ldflags)
else # Python 2.x
PYTHON_VERSION := $(subst ., ,$(lastword $(shell python --version 2>&1)))
ifneq ($(BUILD),debug)
PYTHON_CFLAGS := $(shell python-config --cflags)
PYTHON_LDFLAGS := $(shell python-config --ldflags)
else
PYTHON_CFLAGS := $(shell python-config --includes) \
    -fno-strict-aliasing -fwrapv \
    -Wstrict-prototypes  \
    -Wformat -Wformat-security -Werror=format-security \
    -fstack-protector --param=ssp-buffer-size=4

PYTHON_LDFLAGS := -L/usr/lib/$(INSTALLED_PYTHON) \
    -Xlinker -export-dynamic -Wl,-O0 -Wl,-Bsymbolic-functions
endif # ifneq ($(BUILD),debug)
endif # ifeq ($(PYTHON),3)
PYTHON_VERSION_MAJOR := $(word 1,${PYTHON_VERSION})
PYTHON_VERSION_MINOR := $(word 2,${PYTHON_VERSION})
INSTALLED_PYTHON := python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
endif

# For developer build and strict checking
ifeq ($(BUILD),developer)
    PYTHON_CFLAGS := $(PYTHON_CFLAGS) -Wall -Werror -Wno-unused-function -Wno-unused-variable
endif  # ifeq(${BUILD},developer)

ifndef PYTHON_CFLAGS
PYTHON_CFLAGS := -I/usr/include/$(INSTALLED_PYTHON) -fno-strict-aliasing \
    -fwrapv \
     -fstack-protector \
    #-Wstrict-prototypes \ # This option is obsolete for g++-4.8
    --param=ssp-buffer-size=4 \
    -Wformat -Wformat-security -Werror=format-security

PYTHON_LDFLAGS := -L/usr/lib/$(INSTALLED_PYTHON)  -Xlinker -export-dynamic -Wl,-O0 -Wl,-Bsymbolic-functions
endif
# There are some unix/gcc specific paths here. Should be cleaned up in future.
pymoose: python/moose/_moose.so
pymoose: CXXFLAGS += -DPYMOOSE $(PYTHON_CFLAGS)
# fix: add include dir for numpy headers required by pymoose/moosemodule.cpp
pymoose: CXXFLAGS += -I$(shell /usr/bin/python -c 'from numpy import get_include; print get_include()')
pymoose: OBJLIBS += pymoose/_pymoose.o
pymoose: LDFLAGS += $(PYTHON_LDFLAGS)
export CXXFLAGS
python/moose/_moose.so: libs $(OBJLIBS)
	$(MAKE) -C pymoose
	$(CXX) -shared $(LDFLAGS) $(CXXFLAGS) -o $@ $(OBJLIBS) $(LIBS)
	@echo "pymoose module built."

# This will generate an object file without main
libs:
	@for i in $(SUBDIR) $(PARALLEL_DIR); do \
	    $(MAKE) -C $$i || exit $$?; \
	done
	@echo "All Libs compiled"

clean:
	@(for i in $(CLEANSUBDIR) ; do $(MAKE) -C $$i clean;  done)
	-rm -rf moose  core.* DOCS/html python/moose/*.so python/moose/*.pyc

############ INSTALL (works for sudo make install and deb packaging using dpkg-buildpackage)
## get the default python module install location
## no need to know this, the default for public modules is /usr/share/pyshared
pydir_cmd := python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
pydistpkg_dir := $(shell $(pydir_cmd))
## /usr/share/pyshared/ is a directory which is independent of python version,
## but modules need to be registered with update-python-modules, giving a list of files
## debhelper (dpkg-buildpackage) will also copy to this location and do the registration.
## For make install, I stick to the default python .../dist-packages directory.
pydistpkg_dirB := /usr/share/pyshared/

username=$(shell basename $(HOME))

install_prefix=/usr
## Note that $(DESTDIR) is provided by dpkg-buildpackage to specify the local install for deb packaging
## if doing sudo make install, $(DESTDIR) will be undefined and will cause no trouble
install:
	## discard symbols from object files
	strip python/moose/_moose.so

	## delete older .../share/moose, - before rm means ignore errors (eg not found)
	-rm -rf $(DESTDIR)$(install_prefix)/share/moose
	## -p creates parent directories also if they don't exist
	mkdir -p $(DESTDIR)$(install_prefix)/share/moose
	## copy filtering out the .svn (hidden) files
	rsync -r --exclude=.svn Demos/* $(DESTDIR)$(install_prefix)/share/moose/Demos
	rsync -r --exclude=.svn gui/* $(DESTDIR)$(install_prefix)/share/moose/gui
	test -d $(DESTDIR)$(install_prefix)/share/doc || mkdir -p $(DESTDIR)$(install_prefix)/share/doc
	rsync -r --exclude=.svn Docs/* $(DESTDIR)$(install_prefix)/share/doc/moose
	# rsync -r --exclude=.svn Demos/* $(DESTDIR)$(install_prefix)/share/moose/
	## pymoose module goes to python's dist-packages
	## delete older .../dist-packages/moose, - before rm means ignore errors (eg not found)
	-rm -rf $(DESTDIR)$(pydistpkg_dir)/moose
	-rm -rf $(DESTDIR)$(pydistpkg_dirB)/moose
	## make directory in case non-existent (needed for deb pkg building)
	mkdir -p $(DESTDIR)$(pydistpkg_dir)
	rsync -r --exclude=.svn --exclude=moose_py3k.py python/* $(DESTDIR)$(pydistpkg_dir)/

	## shell command moosegui for the moose GUI.
	chmod a+x $(DESTDIR)$(install_prefix)/share/moose/gui/MooseGUI.py
	## -rm instructs make to ignore errors from make eg. file not found
	-rm -f $(DESTDIR)$(install_prefix)/bin/moosegui
	mkdir -p $(DESTDIR)$(install_prefix)/bin
	ln -s $(DESTDIR)$(install_prefix)/share/moose/gui/MooseGUI.py $(DESTDIR)$(install_prefix)/bin/moosegui

	## byte compile the module, gui, Demos (since, later running as user cannot create .pyc in root-owned dirs)
	python -c "import compileall; compileall.compile_dir('$(DESTDIR)$(pydistpkg_dir)/moose',force=1)"
	python -c "import compileall; compileall.compile_dir('$(DESTDIR)$(install_prefix)/share/moose/gui',force=1)"
	python -c "import compileall; compileall.compile_dir('$(DESTDIR)$(install_prefix)/share/moose/Demos',force=1)"

        ## see standards.freedesktop.org for specifications for where to put menu entries and icons
        ## copy the .desktop files to /usr/share/applications for link to show up in main menu
	mkdir -p $(DESTDIR)$(install_prefix)/share/applications
	cp gui/MooseGUI.desktop $(DESTDIR)$(install_prefix)/share/applications/
	cp gui/MooseSquidAxon.desktop $(DESTDIR)$(install_prefix)/share/applications/
        ## copy the .desktop files to the desktop too to get icons
	cp gui/MooseGUI.desktop $$HOME/Desktop/
	chmod a+x $$HOME/Desktop/MooseGUI.desktop
	chown $(username) $(HOME)/Desktop/MooseGUI.desktop
	chgrp $(username) $(HOME)/Desktop/MooseGUI.desktop
	cp gui/MooseSquidAxon.desktop $$HOME/Desktop/
	chmod a+x $$HOME/Desktop/MooseSquidAxon.desktop
	chgrp $(username) $(HOME)/Desktop/MooseSquidAxon.desktop
	chown $(username) $(HOME)/Desktop/MooseSquidAxon.desktop
        ## copy icon to /usr/share/icons/hicolor/<size>/apps (hicolor is the fallback theme)
	mkdir -p $(DESTDIR)$(install_prefix)/share/icons/hicolor/scalable/apps
	cp gui/icons/moose_icon.png $(DESTDIR)$(install_prefix)/share/icons/hicolor/scalable/apps/
	cp gui/icons/squid.png $(DESTDIR)$(install_prefix)/share/icons/hicolor/scalable/apps/
        ## need to update the icon cache to show the icon
	update-icon-caches $(DESTDIR)$(install_prefix)/share/icons/hicolor/

.PHONY: install
