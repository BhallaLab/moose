#!/usr/bin/env bash
set -e 
set -x
valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./test_lsoda
gprof2dot -f callgrind  ./callgrind.out > callgrind.prof.dot 
dot -Tpng ./callgrind.prof.dot > lsoda_prof.png
