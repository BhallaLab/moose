#!/bin/bash

test_scripts="./test_function.py
    test_function.py
    test_moose_paths.py
    test_mumbl.py
    test_pymoose.py
    test_synchan.py
    test_vec.py
    test_difshells.py"

for testFile in $test_scripts; do
    echo "Executing $testFile"
    python $testFile | tee test_all.sh.log
done
