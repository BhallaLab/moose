#!/bin/bash
#set -x
set -e
source ./colors.sh

# matplotlibrc file.
MATPLOTRC=./matplotlibrc

PWD=$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)

# All scripts are here.
SRCDIR=./..

MAINLESS=$PWD/MAINLESS
BLACKLISTED=$PWD/BLACKLISTED
GUI=$PWD/GUISCRIPTS
INTERACTIVE=$PWD/INTERACTIVE
TORUN=$PWD/TORUN
BROKEN=$PWD/BROKEN

for f in $MAINLESS $BLACKLISTED $GUI $INTERACTIVE $TORUN; do
    # Remove already existing files
    if [ -f $f ]; then
        rm -f $f
    fi
done

declare -i a
PYC=`which python`

function check_file
{
    filepath="$1"

    # These files were not tested on travis. Check issue #2
    if [[ $filepath == *"Fig2A.py" ]]; then
        echo "$filepath is disabled by developer"
        return
    elif [[ $filepath == *"nsdf_vec.py" ]]; then
        echo "$filepath is disabled by developer"
        return
    fi

    if grep -q "__BROKEN__" $filepath 
    then 
        coloredPrint "INFO" "This script is marked as broken by developer"
        echo $filepath >> $BROKEN
    elif grep -q "input(" $filepath
    then
        coloredPrint WARN "File contains input() call. Interactive script. \
            WONT RUN"
        echo $filepath >> $INTERACTIVE
    elif grep -q "QtGui" $filepath
    then
        coloredPrint WARN "File seems to be a GUI. WONT RUN"
        echo $filepath >> $GUI
    elif grep -q "__main__\|main(" $filepath
    then
        coloredPrint "INFO" "Script with main() or __main__"
        echo $filepath >> $TORUN
    fi
}

function find_files 
{
    echo "|| searching for files in $1"
    PYFILES=`find "$1" \( -name "*.py" ! -iname "test_all*" \)`
    for pyf in $PYFILES; do
        let a=a+1
        dn=`dirname $pyf`
        fn=`basename $pyf`
        # copy matplotlibrc file to working directory
        check_file $pyf
    done
}

# Search files to run. One can ignore directories which has .dont_run_on_travis
# file in them.

DIRS_TO_SEARCH=""
while read -r dir; do
    echo $dir
    if [ -f $dir/.ignore_on_travis ]; then
        echo "Ignoring this directory on travis"
    else
        echo "Find script from this directory to run"
        find_files $dir
    fi
done < <(find $SRCDIR -mindepth 1 -maxdepth 1 -type d)
