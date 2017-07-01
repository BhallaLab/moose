# -*- coding: utf-8 -*-
"""
Created on Sun Jun  1 15:35:13 2014

@author: subhasis ray
"""

import moose

def make_nmda(path):
    nmda = moose.MarkovChannel(path)
    return nmda

