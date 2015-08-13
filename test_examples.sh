#!/bin/bash
set -e
declare -i a
PYC=`which python`
RUNDIR=./_test_dir_moose-examples_
echo "Creating $RUNDIR"
mkdir -p $RUNDIR
MATPLOTRC=data/matplotlibrc
echo "copying examples out of source"
cp -r moose-examples/* $RUNDIR/
PYFILES=`find $RUNDIR -name "*.py"`
for pyf in $PYFILES; do
    let a=a+1
    dn=`dirname $pyf`
    fn=`basename $pyf`
    # copy matplotlibrc file to working directory
    cp $MATPLOTRC $dn/
    (
        cd $dn
        echo "Executing $fn in directory $dn"
        $PYC $fn
    )
done
echo "Total $a scripts run"
