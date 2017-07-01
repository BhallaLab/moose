#!/usr/bin/env bash

trap ctrl_c INT 

function ctrl_c( )
{
    echo "CTRL+C pressed. Quitting..."
    exit;
}

PWD=$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)

BLACKLISTED=$PWD/BLACKLISTED
SUCCEEDED=$PWD/SUCCEEDED
FAILED=$PWD/FAILED
TEMP=$PWD/__temp__

rm -f $BLACKLISTED $SUCCEEDED $FAILED $TEMP TORUN
$PWD/find_scripts_to_run.sh 

PYC=`which python2`
PYC=/usr/bin/python                 # Force PYTHONPATH.
MATPLOTRC=$PWD/matplotlibrc
if [ ! -f $MATPLOTRC ]; then
    echo "$MATPLOTRC not found"
    exit
fi

TIMEOUT=${1:-60}     # default timeout is 60 secs.
NTHREADS=4
for f in `cat ./TORUN`; do
    d=`dirname $f`
    fn=`basename $f`
    # Wait of NTHREADS to join
    ((i=i%NTHREADS)); ((i++==0)) && wait
    (
        cp $MATPLOTRC $d/
        cd $d
        echo "++ Executing script $f"
        # Do not run more than $TIMEOUT
        timeout $TIMEOUT $PYC $fn &> $TEMP
        status=$?
        if [ "$status" -eq "0" ]; then                   # success
            echo "|| Success. Written to $SUCCEEDED"
            echo "- [x] $f" >> $SUCCEEDED
            # If there is timeout then add to BLACKLISTED
            # For return status See 
            # http://git.savannah.gnu.org/cgit/coreutils.git/tree/src/timeout.c
        elif [ "$status" -eq "124" ]; then               # timeout. 
            echo "|| Timed-out status: $status" 
            echo "- [ ] $f" >> $BLACKLISTED
            sed -i 's/^/\ \ /' $TEMP
            printf "\n\`\`\`\n" >> $BLACKLISTED 
            cat $TEMP >> $BLACKLISTED 
            printf "\`\`\`\n" >> $BLACKLISTED 
        else                                    # Failed
            echo "|| Failed with status "$status" "
            echo "- [ ] $f" >> $FAILED
            sed -i 's/^/\ \ /' $TEMP
            printf "\n\`\`\`\n" >> $FAILED 
            cat $TEMP >> $FAILED 
            printf "\`\`\`\n" >> $FAILED 
            cat $TEMP
            echo "|| Failed. Error written to $FAILED"
        fi
    ) & 
done

# Auto deploy to README.md file
python ./deploy_gh_pages.py 

echo "Following scripts were successful"
cat $SUCCEEDED

if [ -f $BLACKLISTED ]; then
    echo "Following scripts were blacklisted due to timeout or singal interrupt"
    cat $BLACKLISTED 
    echo "# Blacklisted " >> ../README.md
fi


if [ -f $FAILED ]; then 
    echo "=========================================="
    echo "Following scripts failed."
    cat $FAILED
    exit 1
fi
