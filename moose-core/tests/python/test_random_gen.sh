#!/usr/bin/env bash
PYTHON_EXECUTABLE=${1:-/usr/bin/python}
if [ $2 ]; then
    export PYTHONPATH="$2"
fi
FAILED=0
a=`${PYTHON_EXECUTABLE} -c 'import moose; moose.seed(1); print([moose.rand() for x in range(10)])'`
b=`${PYTHON_EXECUTABLE} -c 'import moose; moose.seed(2); print([moose.rand() for x in range(10)])'`
a=`printf "$a" | tail -n1`
b=`printf "$b" | tail -n1`
echo $a
echo $b

if [[ "$a" == "$b" ]]; then
    echo "Test 1 failed. Expecting not equal output. Got"
    printf "$a \n\t and,\n $b\n"
    FAILED=1
else
    echo "Test 1 passed"
fi

c=`${PYTHON_EXECUTABLE} -c 'import moose; moose.seed(10); print([moose.rand() for x in \
    range(10)])'`
d=`${PYTHON_EXECUTABLE} -c 'import moose; moose.seed(10); print([moose.rand() for x in \
    range(10)])'`
c=`printf "$c" | tail -n1`
d=`printf "$d" | tail -n1`
echo $c
echo $d

if [[ "$c" == "$d" ]]; then
    echo "Test 2 passed"
else
    echo "Test failed. Expecting equal output. Got"
    printf "$c \n\t and,\n$d\n"
    FAILED=1
fi

if [ $FAILED -eq 1 ]; then
    exit 1
else
    exit 0;
fi
