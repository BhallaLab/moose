# -*- coding: utf-8 -*-
"""count.py:

Counting related function goes here.

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2015, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


import numpy as np

def locate_spike_in_spike_train( vec, threshold ):
    """Return a list of instance where a spike has occured in vec """

    vec = np.array( vec )
    nSpikes = 0
    spikeBegin, spikeEnds = False, False
    spikeList = []
    for i, x in enumerate(vec):
        if x > threshold:
            if not spikeBegin:
                spikeBegin, spikeEnds = True, False
            else: pass
        else:
            if spikeBegin:
                spikeEnds, spikeBegin = True, False
                spikeList.append(i)
                nSpikes += 1
    return nSpikes, spikeList

def spike_train_simple_stat( vec, threshold = 0.0, **kwargs ):
    """Compute number of spikes, mean and variance of spike intervals in a given
    vec.

    """
    vec = np.array(vec)
    result = {}
    nSpikes, spikeList = locate_spike_in_spike_train(vec, threshold)
    if len(spikeList) > 1:
        diffIndex = np.diff(spikeList)
        meanSteps = np.mean(diffIndex)
        varSteps = np.var(diffIndex)
    else:
        meanSteps = -1
        varSteps = -1
    result['number of spikes'] = nSpikes
    result['mean spike-interval'] = meanSteps
    result['variance spike-interval'] = varSteps
    result['min'] = vec.min()
    result['max'] = vec.max()
    result['variance'] = np.var(vec)
    return result
