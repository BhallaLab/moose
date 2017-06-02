The source code of documentation is ./source directory. 

This directory contains MOOSE user documentation in reST format that can be 
compiled into various formats by sphinx. To build the documentation in HTML,
format  enter the command:

make html

in this directory. Then you can open _build/html/index.html in browser to 
browse the generated documentation.

Every MOOSE class has builtin documentation. This can be extracted into a
reST file by running the script:

	    python create_all_rst_doc.py

This process must be carried out before making the docs after any
change in the built-in documentation is compiled into MOOSE.

Other files 
    - conf.py: the Sphinx configuration file.
    - index.rst: This is the index file for use when building the Python
            docs using sphinx.
    - moose_builtins.rst: This is for sphinx to process the pymoose builtin 
            doc strings (using autodoc extension).
    - moose_classes.rst: The Python docs extracted above using 
	    create_all_rst_doc.py

This directory contains MOOSE user documentation in reST format that can be 
compiled into various formats by sphinx. To build the documentation in HTML,
format  enter the command:

make html

in this directory. Then you can open _build/html/index.html in browser to 
browse the generated documentation.

files 
    - conf.py: the Sphinx configuration file.
    - index.rst: This is the index file for use when building the Python
            docs using sphinx.
    - moose_cookbook.rst: Recipes for specific tasks in moose.


This directory contains MOOSE user documentation in reST format that can be 
compiled into various formats by sphinx. To build the documentation in HTML,
format  enter the command:

make html

in this directory. Then you can open _build/html/index.html in browser to 
browse the generated documentation.

files 
    - conf.py: the Sphinx configuration file.
    - index.rst: This is the index file for use when building the Python
            docs using sphinx.
    - moose_quickstart.rst: Recipes for specific tasks in moose.
