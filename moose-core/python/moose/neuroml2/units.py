# -*- coding: utf-8 -*-
"""
Utility to create mapping from NeuroML2 unit strings (as specified
in NeuroMLCoreDimensions.xml) to SI

Author: 
    Subhasis Ray (Please check commit history for more details).
"""

from __future__ import print_function, division
import os
import re
from xml.etree import ElementTree

# The units and dimensions used in NeuroML2 are defined in this file:
# https://github.com/NeuroML/NeuroML2/blob/master/NeuroML2CoreTypes/NeuroMLCoreDimensions.xml
unitsdoc = ElementTree.parse(os.path.join(os.path.dirname(__file__),'schema', 'NeuroMLCoreDimensions.xml'))
dims = dict([(el.attrib['name'], el) for el in unitsdoc.getroot() if el.tag == '{http://www.neuroml.org/lems/0.7.4}Dimension'])
units = dict([(el.attrib['symbol'], el) for el in unitsdoc.getroot() if el.tag == '{http://www.neuroml.org/lems/0.7.4}Unit'])

# optional +/- followed by 0 or more digits followed by optional
# decimal point followed by optional digits followed by optional
# (exponent symbol followed by optional +/- followed by one or more
# digits.
magnitude_regex = re.compile(r'^[\+-]?\d*\.?\d*([eE][\+-]?\d+)?')

def SI(expression):
    try:
        return float(expression)
    except:
        pass
    expression=expression.replace(" ", "")
    match = magnitude_regex.match(expression)
    magnitude = float(match.group(0))
    unitstr = re.split(magnitude_regex, expression)[-1]
    
    #print("Converting %s: mag: %s, unitstr: %s"%(expression, magnitude, unitstr))
    try:
        unit = units[unitstr]
        #print('Unit: %s'%unit.attrib)
    except KeyError as ke:
        print("Error in converting %s: %s, using %s"%(expression, ke, magnitude))
        return magnitude

    power = int(unit.attrib['power']) if 'power' in unit.attrib else 0
    offset = float(unit.attrib['offset']) if 'offset' in unit.attrib else 0
    scale = float(unit.attrib['scale']) if 'scale' in unit.attrib else 1
    
    si = (magnitude + offset) * scale * 10**power 

    return si


if __name__ == "__main__":
    examples = ['-70mV','5 V','1',2,3.3, '330mM','15K','0 degC','-300degC','1min']
    for e in examples:
        print(SI(e))
#
# units.py ends here
