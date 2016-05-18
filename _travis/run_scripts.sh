#!/usr/bin/env bash

PWD=$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)

BLACKLISTED=$PWD/BLACKLISTED
SUCCEEDED=$PWD/SUCCEEDED
FAILED=$PWD/FAILED
TEMP=$PWD/__temp__

rm -f $BLACKLISTED $SUCCEEDED $FAILED $TEMP TORUN
$PWD/find_scripts_to_run.sh 

PYC=`which python`
MATPLOTRC=$PWD/matplotlibrc
if [ ! -f $MATPLOTRC ]; then
    echo "$MATPLOTRC not found"
    exit
fi

TIMEOUT=5m
for f in `cat ./TORUN`; do
    d=`dirname $f`
    fn=`basename $f`
    (
        cp $MATPLOTRC $d/
        cd $d
        echo "++ Executing script $f"
        # Do not run more than 2 minutes. 
        timeout $TIMEOUT $PYC $fn &> $TEMP
        status=$?
        if [ "$status" -eq "0" ]; then                   # success
            echo "|| Success. Written to $SUCCEEDED"
            echo "- [x] $f" >> $SUCCEEDED
        elif [ "$status" -gt "128" ]; then               # timeout
            # If there is timeout then add to BLACKLISTED
            echo "|| Killed by signal status: $status" 
            echo "- [ ] $f" >> $BLACKLISTED
            sed -i 's/^/\ \ /' $TEMP
            printf "\n\`i\`\`\n" >> $BLACKLISTED 
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
    )
done

echo "Following scripts were successful"
cat $SUCCEEDED

if [ -f $BLACKLISTED ]; then
    echo "Following scripts were blacklisted due to timeout or singal interrupt"
    cat $BLACKLISTED 
fi

if [ -f $FAILED ]; then 
    echo "=========================================="
    echo "Following scripts failed."
    cat $FAILED
    exit 1
fi

## If less than 84 files passed, raise and error.
