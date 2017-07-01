# -*- coding: utf-8 -*-
## Description: utility functions used while loading NeuroML L1,2,3 files.
## Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE
## Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, modified for parallel MOOSE
## Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, minor changes for parallel MOOSE

"""
Some useful constants like namespaces are defined.
And they can be set in ElementTree root element via set_neuroml_namespaces_attribs(neuromlroot).
Use tweak_model(root_element, params) to exclude certain populations and projections
while still including certain others.
indent(...) is an in-place prettyprint formatter copied from http://effbot.org/zone/element-lib.htm
"""

from __future__ import print_function
from xml.etree import cElementTree as ET
from xml.etree import ElementTree as slowET
import math
import os

neuroml_debug = False

neuroml_ns='http://morphml.org/neuroml/schema'
nml_ns='http://morphml.org/networkml/schema'
mml_ns='http://morphml.org/morphml/schema'
bio_ns='http://morphml.org/biophysics/schema'
cml_ns='http://morphml.org/channelml/schema'
meta_ns='http://morphml.org/metadata/schema'
xsi_ns='http://www.w3.org/2001/XMLSchema-instance'

### ElementTree parse works an order of magnitude or more faster than minidom
### BUT it doesn't keep the original namespaces,
## from http://effbot.org/zone/element-namespaces.htm , I got _namespace_map
## neuroml_ns, bio_ns, mml_ns, etc are defined above
slowET._namespace_map[neuroml_ns] = 'neuroml'
slowET._namespace_map[nml_ns] = 'nml'
slowET._namespace_map[mml_ns] = 'mml'
slowET._namespace_map[bio_ns] = 'bio'
slowET._namespace_map[cml_ns] = 'cml'
slowET._namespace_map[meta_ns] = 'meta'
slowET._namespace_map[xsi_ns] = 'xsi'

### cElementTree is much faster than ElementTree and is API compatible with the latter,
### but instead of _namespace_map above, use register_namespace below ...
### but this works only with python2.7 onwards, so stick to above,
### with import elementtree.ElementTree alongwith importing cElementTree as at
### http://dev.blogs.nuxeo.com/2006/02/elementtree-serialization-namespace-prefixes.html
#ET.register_namespace('neuroml',neuroml_ns)
#ET.register_namespace('nml',nml_ns)
#ET.register_namespace('mml',mml_ns)
#ET.register_namespace('bio',bio_ns)
#ET.register_namespace('cml',cml_ns)
#ET.register_namespace('meta',meta_ns)
#ET.register_namespace('xsi',xsi_ns)

CELSIUS_default = 32.0 # deg C # default temperature if meta:property tag for temperature is not present
ZeroCKelvin = 273.15 # zero dec C in Kelvin
VMIN = -0.1 # Volts
VMAX = 0.1 # Volts
NDIVS = 200 # number
dv = ( VMAX - VMIN ) / NDIVS # Volts

def set_neuroml_namespaces_attribs(neuromlroot):
    set_attrib_if_not_found(neuromlroot,"xmlns",neuroml_ns)
    set_attrib_if_not_found(neuromlroot,"xmlns:nml",nml_ns)
    set_attrib_if_not_found(neuromlroot,"xmlns:mml",mml_ns)
    set_attrib_if_not_found(neuromlroot,"xmlns:bio",bio_ns)
    set_attrib_if_not_found(neuromlroot,"xmlns:cml",cml_ns)
    set_attrib_if_not_found(neuromlroot,"xmlns:meta",meta_ns)
    ## later doc.write() assigns the xsi namespace a second time
    ## causing double definition and problem with xsltproc,
    ## hence commenting it out here.
    #set_attrib_if_not_found(neuromlroot,"xmlns:xsi",xsi_ns)

def set_attrib_if_not_found(elem, name, value):
    if elem.get(name) is None:
        elem.set(name,value)

def tweak_model(root_element, params):
    if 'excludePopulations' in params: # if params has key 'excludePopulations'
        ## both excludePopulations and excludeProjections must be present together
        pruneExcludes(root_element,params['excludePopulations'],params['excludeProjections'])
    if 'onlyInclude' in params: # if params has key 'onlyInclude'
        keepOnlyInclude(root_element,params['onlyInclude'])

def pruneExcludes(network, excludepops, excludeprojs):
    """
    remove the populations in the excludepops list
    remove the projections in the excludeprojs list
    """
    populations = network.find(".//{"+nml_ns+"}populations")
    pop_remove_list = []
    for population in populations.findall(".//{"+nml_ns+"}population"):
        populationname = population.attrib["name"]
        ## if any of the name-s in exclude_list are a SUBSTRING
        ## of the name of the neuroml population, mark it for removal
        for name in excludepops:
            if name in populationname: # substring
                pop_remove_list.append(population)
    ## remove only unique names,
    ## else you could end up trying to remove same population twice
    for population in set(pop_remove_list):
        populations.remove(population)

    projections = network.find(".//{"+nml_ns+"}projections")
    proj_remove_list = []
    for projection in projections.findall(".//{"+nml_ns+"}projection"):
        projectionname = projection.attrib["name"]
        ## if any of the name-s in exclude_list are a SUBSTRING
        ## of the name of the neuroml projection, mark it for removal
        for name in excludeprojs:
            if name in projectionname: # substring
                proj_remove_list.append(projection)
    ## remove only unique names,
    ## else you could end up trying to remove same projection twice
    for projection in set(proj_remove_list):
        projections.remove(projection)

def keepOnlyInclude(network, onlyInclude):
    """
    Keep only the cells that are in onlyInclude['includePopulation']
    and also keep cells that are connected to cells in onlyInclude['includePopulation']
    and keep connections to any of the cells in onlyInclude['includePopulation'].
    Prune the extraneous connections
    but keep those connections in onlyInclude['includeProjections']
    on cells connected to those in onlyInclude['includePopulation']
    """
    ### Remove the connections that do not connect to cells in onlyInclude.
    ### Simultaneously build up a list of cells 'includeCellsDict' that connect to cells in onlyInclude.
    ### Of course this includeCellDict must have the originally included cells!
    ### At the end of this pruning, even if some population-s / projection-s have no elements,
    ### it doesn't matter, as this findall() returns an empty list and not None - so no error.
    ### Further I am not changing the 'size' attrib in <instances> and <connections>,
    ### as it's not used by this reader and I'm not saving the network after pruning.
    ### Do not prune 'includeProjections' immediately;
    ### prune them later avoiding second order cells in includeCellsDict.
    includepopname = onlyInclude['includePopulation'][0]
    includecellids = onlyInclude['includePopulation'][1]
    ## first of all, include those primary cells that the user instructs.
    includeCellsDict = {includepopname:includecellids}
    ## projections 'includeProjs' will be pruned later, keeping connections to second order cells.
    includeProjs = []
    print("removing obviously extra connections in ... ")
    for projection in network.findall(".//{"+nml_ns+"}projection"):
        projname = projection.attrib['name']
        includeProj = False
        ## check if any of the given includeprojname is a substring of this projname
        for includeprojname in onlyInclude['includeProjections']:
            if includeprojname in projname:
                includeProj = True
        ## if it is a substring, add this projection
        ## to the list of projections to be pruned later
        if includeProj:
            includeProjs.append(projection)
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        print(projname, source, target)
        connections = projection.find(".//{"+nml_ns+"}connections")
        if connections is not None:
            for connection in connections.findall(".//{"+nml_ns+"}connection"):
                pre_cell_id = connection.attrib['pre_cell_id']
                ## is the user-included cell a source cell of the connection?
                includecellinsource = (pre_cell_id in includecellids and includepopname==source)
                post_cell_id = connection.attrib['post_cell_id']
                ## is the user-included cell a target cell of the connection?
                includecellintarget = (post_cell_id in includecellids and includepopname==target)
                ## the second-order cell connected to the user-included cell must also be kept
                if includecellinsource:
                    ## since source is included, include the target also
                    ## there can be self connections between the same population i.e. same source and target
                    try:
                        includeCellsDict[target].append(post_cell_id)
                    except KeyError: # create this population entry in the dictionary if not present
                        includeCellsDict[target] = [post_cell_id]
                elif includecellintarget:
                    ## since target is included, include the source also, except if source is a file
                    if 'file' not in source:
                        try:
                            includeCellsDict[source].append(pre_cell_id)
                        except KeyError: # create this population entry in the dictionary if not present
                            includeCellsDict[source] = [pre_cell_id]
                else:
                    ## this connection is extraneous
                    ## but remove this connection only if
                    ## it is not part of the projections to be pruned later
                    if not includeProj:
                        connections.remove(connection)

    ## convert includeCellsDict elements to set-s rather than lists
    ## to have only unique cell_ids and save time below.
    for key in includeCellsDict:
        includeCellsDict[key] = set(includeCellsDict[key])

    print("removing extra cells ... ")
    ### remove the cells that are not in includeCellsDict
    populations = network.find(".//{"+nml_ns+"}populations")
    for population in network.findall(".//{"+nml_ns+"}population"):
        popname = population.attrib["name"]
        if popname in includeCellsDict:
            includecellids = includeCellsDict[popname]
            instances = population.find(".//{"+nml_ns+"}instances")
            for instance in instances.findall(".//{"+nml_ns+"}instance"):
                ## not a connected cell, so remove
                if instance.attrib['id'] not in includecellids:
                    instances.remove(instance)
        else: ## this whole population is not required!
            populations.remove(population)

    ### Prune the 'includeProjections' that we skipped pruning before,
    ### while keeping connections to second order cells!
    for projection in includeProjs:
        print("removing projection",projection.attrib['name'],\
            "keeping second-order connections.")
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        ## boolean: True if includeCellsDict has key source
        source_in_includeCellsDict = source in includeCellsDict
        ## boolean: True if the word 'file' occurs in str source
        file_in_source = 'file' in source
        ## boolean: True if includeCellsDict has key target
        target_in_includeCellsDict = target in includeCellsDict
        connections = projection.find(".//{"+nml_ns+"}connections")
        for connection in connections.findall(".//{"+nml_ns+"}connection"):
            ## is the included cell a source cell of the connection?
            ## keep 'file' as source also.
            if file_in_source:
                includecellinsource = True
            elif source_in_includeCellsDict and \
                    connection.attrib['pre_cell_id'] in includeCellsDict[source]:
                includecellinsource = True
            else: includecellinsource = False
            ## is the included cell a target cell of the connection?
            if target_in_includeCellsDict and \
                    connection.attrib['post_cell_id'] in includeCellsDict[target]:
                includecellintarget = True
            else: includecellintarget= False
            ## this connection is extraneous
            ## if either sourcecell or targetcell is not included.
            if not includecellinsource or not includecellintarget:
                ## remove is a very slow operation!
                connections.remove(connection)

def indent(elem, level=0):
    """ in-place prettyprint formatter copied from http://effbot.org/zone/element-lib.htm
    first call indent(root, level=0), and then doc.write(filename) ."""
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

## make a list of safe functions possible to be used safely in eval()
safe_list = ('acos', 'asin', 'atan', 'atan2', 'ceil',
    'cos', 'cosh', 'degrees', 'e', 'exp', 'fabs', 'floor',
    'fmod', 'frexp', 'hypot', 'ldexp', 'log', 'log10', 'modf',
    'pi', 'pow', 'radians', 'sin', 'sinh', 'sqrt', 'tan', 'tanh')
## use the list to filter the local namespace
safe_dict = {k:getattr(math, k) for k in safe_list}
## add any needed builtins back in.
safe_dict['abs'] = abs

def find_first_file(name, path):
    """ Finds and returns the first occurence of the filename in the directory tree under a given path.
    If nothing is returned, return value defaults to None.  """
    for root, dirs, files in os.walk(path):
        if name in files:
            return os.path.join(root, name)
