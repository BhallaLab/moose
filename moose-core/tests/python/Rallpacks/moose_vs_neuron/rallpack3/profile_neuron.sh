#!/usr/bin/env bash
echo "Profiling neuron simulator of rallpack3"
ncomp=50
while [ $ncomp -lt 50000 ]; do
    echo "Cable with compartment $ncomp"
    python ./neuron_sim.py --ncomp $ncomp --data data/L$ncomp.out
    ncomp=$(echo $ncomp+50 | bc)
done
