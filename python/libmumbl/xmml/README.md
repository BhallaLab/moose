The xMML format, version 0.4
==============================

The xMML format is documented in xmml.xsd, and a brief introducion
was presented at eScience 2011 [1]. An older documentation is
available of xmml.dtd. Two examples implementing the specification
are given: isr\_xmml.xml and canal.xml.

With the scripts in the bin directory certain tasks are eased.
* bin/validate: call to validate an xmml file, which must be in the
  same directory as xmml.dtd. With two arguments, it performes
  XInclude and saves the result in the second file.
* bin/mkrelease: call with a version number to have a release of that
  version put in the release directory
* bin/dtd2xsd.pl: call to convert xmml.dtd to xmml.xsd
* bin/xsd2dtd.xsl: XML Transformation to convert xmml.xsd to xmml.dtd

[1] J. Borgdorff, J.-L. Falcone, E. Lorenz, B. Chopard, A.G. Hoekstra, "A principled approach to Distributed Multiscale Computing, from formalization to execution," In Proceedings of The Seventh IEEE International Conference on e-Science Workshops, Stockholm, Sweden, December 5-8, pp 97-104, 2011 [doi:10.1109/eScienceW.2011.9](http://dx.doi.org/10.1109/eScienceW.2011.9).
