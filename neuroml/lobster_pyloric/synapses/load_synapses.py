
# -*- coding: utf-8 -*-

from GluSyn_STG import GluSyn_STG
from AchSyn_STG import AchSyn_STG

def load_synapses():
    GluSyn_STG("/library/DoubExpSyn_Glu")
    AchSyn_STG("/library/DoubExpSyn_Ach")
