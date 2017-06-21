#!/bin/bash
if [[ $# < 3 ]]; then
    echo "Usage: $0 scriptpath loopcount outfile"
    echo "Run script specified by `scriptpath` for `loopcount` repeats and time it. The output is written in "
    exit 1
fi
scriptfile=$1
loopcount=$2
outfile=$3

echo "Benchmarking Python script: $1 with $2 repeats"
echo -e "Real\tUser\tSystem\tMem(K)" > "$outfile"
for ((ii=1; ii <= loopcount; ++ii)); do
    /usr/bin/time  -a -o "$outfile" -f "%E\t%U\t%S\t%K" python "$scriptfile"
done
# Command to extract the timings from the log files:
# grep 'Simulation time with solver' /data/`date '%Y_%m_%d'`/*.log | sed 's/^.*Simulation time with solver [a-z:]+//'
