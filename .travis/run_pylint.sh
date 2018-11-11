#!/usr/bin/env bash 
set -e
python3 -m pip install pylint --user 
PYLINT="python3 -m pylint -E \
    --disable=no-member --disable=no-name-in-module \
    --disable=invalid-unary-operand-type \
    --disable=import-error \
    "
function do_pylint() {
    echo "Checking $1"
    DIR=$(dirname $1)
    SNAME=$(basename $1)
    ( 
        cd $DIR
        $PYLINT $@ $SNAME
    )
}

FILES=$(find . -type f -name "*.py" | shuf)
N=4
i=0
for f in $FILES; do 
    #i=$((i+1))
    #if [ $i -eq $N ]; then
    #    i=0
    #    wait;
    #fi
    # do_pylint "$f"
    echo "Checking $f"
    DIR=$(dirname $f)
    SNAME=$(basename $f)
    ( 
        cd $DIR
        $PYLINT $@ $SNAME
    )
done
