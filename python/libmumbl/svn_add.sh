#!/bin/bash
to="../../moose_svn1.7/moose/branches/buildQ/python/moose/multiscale/"
find . -type f -regex ".*\.\(sh\|py\|Make*\|xml\|xsd\|txt\|log\)" -print0 |\
     xargs -0 -I file cp --parents file $to
#cd $to && \
#    findAsciiTextFiles.sh -print0 | xargs -I file svn add file && svn ci
cd $to && \
    findAsciiTextFiles.sh -print0 | xargs -I file svn add file && svn ci
