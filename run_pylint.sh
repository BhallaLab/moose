#!/usr/bin/env bash
set -e
PYLINT="python3 -m pylint -E --disable=no-member --disable=no-name-in-module \
    --disable=invalid-unary-operand-type \
    --disable=import-error \
    "
FILES=$(find . -type f -name "*.py" | shuf)
i=0
N=3
for f in $FILES; do 
    i=$((i+1))
    if [ $i -eq $N ]; then
        wait
        i=0
    fi
    ( 
        echo "Checking $f"
        DIR=$(dirname $f)
        SNAME=$(basename $f)
        cd $DIR
        $PYLINT $@ $SNAME
    ) &
done
