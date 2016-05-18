/**

\page Profiling Profiling

\section Introduction

It is possible to do profiling without altering any C++ implementation, and without writing any C++ testbed. Using Google's <a target="_blank" href="https://code.google.com/p/gperftools/">gperftools</a> combined with <a href="http://cython.org/">cython</a>, you can do C++ profiling by writing python script running the MOOSE functions in quetion.

First cython, gperftools, libc6-prof packages have to be installed. Secondly a cython wrapper should be made for three functions of gperftools. After that, moose may be recompiled with the 'profile' option. Lastly, the wrapper may be included into arbitrary python script, thus gperftools functions can be used.

\section PackageInstalltion Package Installation

<ul>
<li>Cython: \verbatim ~$ sudo apt-get install cython \endverbatim</li>

<li>gperftools: download it from <a target="_blank" href="https://code.google.com/p/gperftools/downloads/list">here</a>, then install it.</li>

<li>libc6-prof: \verbatim ~$ sudo apt-get install libc6-prof \endverbatim</li>

<li>kcachegrind (optional, for interpreting profiler output): \verbatim ~$ sudo apt-get install kcachegrind \endverbatim</li>

</ul>

\section CythonWrapper Cython gperftools wrapper

The simplest way to get the wrapper done is to write a cython script wrapping the gperftools functions and a python script that compiles the wrapped functions and link them to the gperftools library.

Let's call the cython script gperftools_wrapped.pyx:

\verbatim

cdef extern from "gperftools/profiler.h":
	int ProfilerStart(char* fname)
	void ProfilerStop()
	void ProfilerFlush()

def ProfStart(fname):
	return ProfilerStart(fname)

def ProfStop():
	ProfilerStop()

def ProfFlush():
	ProfilerFlush()

\endverbatim

Here we define a python function for each function of gperftools that we wrap. More functions can be wrapped for more custom profiling (see ProfilerStartWithOptions()).

The python compiler script may look something like this (setup.py):

\verbatim

from distutils.core import setup
from Cython.Build import cythonize

setup(
	name = 'gperftools_wrapped',
	ext_modules = cythonize("gperftools_wrapped.pyx"),
)

\endverbatim

Now the setup.py may be run with the following manner, adding the -lprofiler flag:
\verbatim ~$ python setup.py build_ext --inplace -lprofiler \endverbatim

If everything went right now you should have gperftools_wrapped.c, gperftools_wrapped.so, and a build directory as result of the compilation.

Put gperftools_wrapped.so nearby your python testbed and import as gperftools_wrapped, so you can profile python C extensions. But (!) first the C extensions may be compiled using the -lprofiler flag.

\section MooseRecomp Moose recompilation

To profile moose, it should be recompiled with altering the Makefile setting BUILD: \verbatim BUILD=profile \endverbatim

Essentially you should add the -lprofiler flag. So if the flags corresponding to the "profile" BUILD option does not include -lprofiler you should add it yourself (probably that is the case).

Flags to use for example: \verbatim CXXFLAGS  = -pg -lprofiler -fpermissive -fno-strict-aliasing -fPIC -Wall -Wno-long-long -pedantic -DUSE_GENESIS_PARSER \endverbatim

You may only add the -lprofiler flag to the Makefile which compiles the C++ code you are interested in profiling (not tested). Then recompile moose.

\section ProfilingInAction Profiling in action

Before profiling one should always set the PYTHONPATH to the directory from where python picks up moose functions. To get the function names in your profiling, this should be done, whether it is already set in e.g. your .bashrc script. Example:

\verbatim export PYTHONPATH=/path_to_moose/python/ \endverbatim

To test profiling let's use an existing demo to check the runtime of HSolve functions.

From the moose directory alter the script at Demos/traub_2005/py/test_hsolve_tcr.py. First import the wrapper we just made.

\verbatim from gperftools_wrapped import * \endverbatim

Then edit the testHSolve function, adding the wrapper functions:

\verbatim

    def testHSolve(self):
        ProfStart("hsolve.prof")
        self.schedule(simdt, plotdt, 'hsolve')
        self.runsim(simtime, pulsearray=pulsearray)
        self.savedata()
        ProfFlush()
        ProfStop()

    def testEE(self):
        pass
        #self.schedule(simdt, plotdt, 'ee')
        #self.runsim(simtime, pulsearray=pulsearray)
        #self.savedata()

\endverbatim

You can also comment out the testEE() function so the it will run faster.

After running the python script you should have a file named hsolve.prof. As you can see the string passed to ProfStart() determines the name of the profiler's output.

You can interpret the output using pprof, or if you installed kcachegrind. Note that for the 'program' parameter of pprof you should provide the _moose.so file inside /path_to_moose/python/moose/.

pprof text method:

\verbatim
~$ pprof --text /path_to_moose/python/moose/_moose.so hsolve.prof > log
~$ less log
\endverbatim

kcachegrind method:

\verbatim
~$ pprof --callgrind /path_to_moose/python/moose/_moose.so hsolve.prof > output.callgrind
~$ kcachegrind output.callgrind
\endverbatim

\author Viktor Tóth

*/
