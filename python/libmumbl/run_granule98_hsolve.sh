#!/bin/bash
set +e

# Set pythonpath
# If first argument is not "d" then normal execution else run inside python
# debugger.

function runCode 
{
  $PYC main.py \
    --nml ./models/neuroml/v1.8/GranuleCell/GranuleCell.net.xml \
    --mumbl ./models/mumbl.xml \
    --config ./models/neuroml/v1.8/GranuleCell/config.xml
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

function testPythonCode 
{
    pf=$1
    echo "Checking .... $pf"
    pylint -E $pf
}

PYC=python2.7
if [ "$1" == "d" ]; then
  PYC=pydb
  runCode
elif [ "$1" == "c" ]; then 
    FILES=$(find . -name "*.py" -type f)
    for pf in $FILES; do
        testPythonCode $pf
    done
else
  PYC=python2.7
  runCode
fi

