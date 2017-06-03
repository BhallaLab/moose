The source code of documentation is /source directory.

This directory contains MOOSE user documentation in reST format that can be
compiled into various formats by sphinx. To build the documentation in HTML,
format  enter the command:

	make html

in this directory. Then you can open build/html/index.html in browser to
browse the generated documentation.

Other files in /source folder
    - conf.py: the Sphinx configuration file.
    - index.rst: This is the index file for use when building the Python
            docs using sphinx.
