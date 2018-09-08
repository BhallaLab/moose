# -* coding: utf-8 -*-
# Utility functions for moose.

from __future__ import print_function, division, absolute_import
from __future__ import print_function, division
from __future__ import absolute_import

import types
import parser
import token
import symbol
import os
import math
import warnings
from datetime import datetime
from collections import defaultdict
import re

from moose.moose_constants import *
from moose.print_utils import *
# Print and Plot utilities.
try:
    from moose.plot_utils import *
except Exception as e:
    info( "Plot utilities are not loaded due to '%s'" % e )

def create_table_path(model, graph, element, field):

    field = field[0].upper() + field[1:]

    tablePathSuffix = element.path.partition(model.path)[-1]
    if tablePathSuffix.startswith('/'):
        tablePathSuffix = tablePathSuffix[1:]

    tablePathSuffix =  tablePathSuffix.replace('/', '_') + '.' + field
    tablePathSuffix = re.sub( '.'
                            , lambda m: { '[' : '_'
                                        , ']' : '_'
                                        }.get(m.group(), m.group())
                            ,tablePathSuffix
                            )

    if tablePathSuffix.startswith("_0__"):
        tablePathSuffix = tablePathSuffix[4:]

    #tablePath = dataroot + '/' +tablePath
    tablePath = graph.path + "/" + tablePathSuffix
    return tablePath


def create_table(tablePath, element, field,tableType):
    """Create table to record `field` from element `element`

    Tables are created under `dataRoot`, the names are generally
    created by removing `/model` in the beginning of `elementPath`
    and replacing `/` with `_`. If this conflicts with an existing
    table, the id value of the target element (elementPath) is
    appended to the name.
    """
    if moose.exists(tablePath):
        table = moose.element(tablePath)
    else:
        if tableType == "Table2":
            table = moose.Table2(tablePath)
        elif tableType == "Table":
            table = moose.Table(tablePath)
        moose.connect(table, 'requestOut', element, 'get%s' % (field))
    return table

def readtable(table, filename, separator=None):
    """Reads the file specified by filename to fill the MOOSE table object.

    The file can either have one float on each line, in that case the
    table will be filled with values sequentially.
    Or, the file can have
    index value
    on each line. If the separator between index and value is anything other than
    white space, you can specify it in the separator argument."""

    in_file = open(filename)
    ii = 0
    line_no = 0
    for line in in_file:
        line_no = line_no + 1
        tokens = split(line, separator)
        if len(token) is 0:
            continue
        elif len(token) == 1:
            table[ii] = float(token[0])
        elif len(token) == 2:
            table[int(token[0])] = float(token[1])
        else:
            print("pymoose.readTable(", table, ",", filename, ",", separator, ") - line#", line_no, " does not fit.")

def getfields(moose_object):
    """Returns a dictionary of the fields and values in this object."""
    field_names = moose_object.getFieldNames('valueFinfo')
    fields = {}
    for name in field_names:
        fields[name] = moose_object.getField(name)
    return fields

def findAllBut(moose_wildcard, stringToExclude):
    allValidObjects = moose.wildcardFind(moose_wildcard)
    refinedList = []
    for validObject in allValidObjects:
        if validObject.path.find(stringToExclude) == -1:
            refinedList.append(validObject)

    return refinedList

def apply_to_tree(moose_wildcard, python_filter=None, value=None):
    """
    Select objects by a moose/genesis wildcard, apply a python filter on them and apply a value on them.

    moose_wildcard - this follows GENESIS convention.

    {path}/#[{condition}] returns all elements directly under {path} that satisfy condition. For example:

    '/mynetwork/mycell_0/#[TYPE=Compartment]'

    will return all Compartment objects directly under mycell_0 in mynetwork.

    '{path}/##[{condition}]' will recursively go through all the
    objects that are under {path} (i.e. children, grandchildren,
    great-grandchildren and so on up to the leaf level) and a list of
    the ones meet {condition} will be obtained.

    Thus, '/mynetwork/##[TYPE=Compartment]' will return all
    compartments under mynetwork or its children, or children thereof
    and so on.

    python_filter - if a single string, it will be taken as a
    fieldname, and value will be assigned to this field. It can also
    be a lambda function returning True or False which will be applied
    to each id in the id list returned by moose wildcard
    search. Remember, the argument to the lambda will be an Id, so it
    is up to you to wrap it into a moose object of appropriate type. An example is:

    lambda moose_id: Compartment(moose_id).diameter <  2e-6

    If your moose_wildcard selected objects of Compartment class, then
    this lambda function will select only those with diameter less
    than 2 um.

    value - can be a lambda function to apply arbitrary operations on
    the selected objects.

    If python_filter is a string it, the return
    value of applying the lambda for value() will assigned to the
    field specified by python_filter.

    But if it is value is a data object and {python_filter} is a
    string, then {value} will be assigned to the field named
    {python_filter}.


    If you want to assign Rm = 1e6 for each compartment in mycell
    whose name match 'axon_*':

    apply_to_tree('/mycell/##[Class=Compartment]',
            lambda x: 'axon_' in Neutral(x).name,
            lambda x: setattr(Compartment(x), 'Rm', 1e6))

    [you must use setattr to assign value to a field because lambda
    functions don't allow assignments].
    """
    if not isinstance(moose_wildcard, str):
        raise TypeError('moose_wildcard must be a string.')
    id_list = moose.getWildcardList(moose_wildcard, True)
    if isinstance(python_filter, types.LambdaType):
        id_list = [moose_id for moose_id in id_list if python_filter(moose_id)]
    elif isinstance(python_filter, str):
        id_list = [moose_id for moose_id in id_list if hasattr(eval('moose.%s(moose_id)' % (moose.Neutral(moose_id).className)), python_filter)]
    else:
        pass
    if isinstance(value, types.LambdaType):
        if isinstance(python_filter, str):
            for moose_id in id_list:
                moose_obj = eval('moose.%s(moose_id)' % (moose.Neutral(moose_id).className))
                setattr(moose_obj, python_filter, value(moose_id))
        else:
            for moose_id in id_list:
                value(moose_id)
    else:
        if isinstance(python_filter, str):
            for moose_id in id_list:
                moose_obj = eval('moose.%s(moose_id)' % (moose.Neutral(moose_id).className))
                setattr(moose_obj, python_filter, value)
        else:
            raise TypeError('Second argument must be a string specifying a field to assign to when third argument is a value')


def tweak_field(moose_wildcard, field, assignment_string):
    """Tweak a specified field of all objects that match the
    moose_wildcard using assignment string. All identifiers in
    assignment string must be fields of the target object.

    Example:

    tweak_field('/mycell/##[Class=Compartment]', 'Rm', '1.5 / (3.1416 * diameter * length')

    will assign Rm to every compartment in mycell such that the
    specific membrane resistance is 1.5 Ohm-m2.
    """
    if not isinstance(moose_wildcard, str):
        raise TypeError('moose_wildcard must be a string.')
    id_list = moose.getWildcardList(moose_wildcard, True)
    expression = parser.expr(assignment_string)
    expr_list = expression.tolist()
    # This is a hack: I just tried out some possible syntax trees and
    # hand coded the replacement such that any identifier is replaced
    # by moose_obj.identifier
    def replace_fields_with_value(x):
        if len(x)>1:
            if x[0] == symbol.power and x[1][0] == symbol.atom and x[1][1][0] == token.NAME:
                field = x[1][1][1]
                x[1] = [symbol.atom, [token.NAME, 'moose_obj']]
                x.append([symbol.trailer, [token.DOT, '.'], [token.NAME, field]])
            for item in x:
                if isinstance(item, list):
                    replace_fields_with_value(item)
        return x
    tmp = replace_fields_with_value(expr_list)
    new_expr = parser.sequence2st(tmp)
    code = new_expr.compile()
    for moose_id in id_list:
        moose_obj = eval('moose.%s(moose_id)' % (moose.Neutral(moose_id).className))
        value = eval(code)
        moose.setField(moose_id, field, str(value))

# 2012-01-11 19:20:39 (+0530) Subha: checked for compatibility with dh_branch
def printtree(root, vchar='|', hchar='__', vcount=1, depth=0, prefix='', is_last=False):
    """Pretty-print a MOOSE tree.

    root - the root element of the MOOSE tree, must be some derivatine of Neutral.

    vchar - the character printed to indicate vertical continuation of
    a parent child relationship.

    hchar - the character printed just before the node name

    vcount - determines how many lines will be printed between two
    successive nodes.

    depth - for internal use - should not be explicitly passed.

    prefix - for internal use - should not be explicitly passed.

    is_last - for internal use - should not be explicitly passed.

    """
    root = moose.element(root)
    # print('%s: "%s"' % (root, root.children))
    for i in range(vcount):
        print(prefix)

    if depth != 0:
        print(prefix + hchar, end=' ')
        if is_last:
            index = prefix.rfind(vchar)
            prefix = prefix[:index] + ' ' * (len(hchar) + len(vchar)) + vchar
        else:
            prefix = prefix + ' ' * len(hchar) + vchar
    else:
        prefix = prefix + vchar

    print(root.name)
    children = []
    for child_vec in root.children:
        try:
            child = moose.element(child_vec)
            children.append(child)
        except TypeError:
            pass
            # print 'TypeError:', child_vec, 'when converting to element.'
    for i in range(0, len(children) - 1):
        printtree(children[i],
                  vchar, hchar, vcount, depth + 1,
                  prefix, False)
    if len(children) > 0:
        printtree(children[-1], vchar, hchar, vcount, depth + 1, prefix, True)


def df_traverse(root, operation, *args):
    """Traverse the tree in a depth-first manner and apply the
    operation using *args. The first argument is the root object by
    default."""
    if hasattr(root, '_visited'):
        return
    operation(root, *args)
    for child in root.children:
        childNode = moose.Neutral(child)
        df_traverse(childNode, operation, *args)
    root._visited = True

def autoposition(root):
    """Automatically set the positions of the endpoints of all the
    compartments under `root`.

    This keeps x and y constant and extends the positions in
    z-direction only. This of course puts everything in a single line
    but suffices for keeping electrical properties intact.

    TODO: in future we may want to create the positions for nice
    visual layout as well. My last attempt resulted in some
    compartments overlapping in space.

    """
    compartments = moose.wildcardFind('%s/##[TYPE=Compartment]' % (root.path))
    stack = [compartment for compartment in map(moose.element, compartments)
              if len(compartment.neighbors['axial']) == 0]
    if len(stack) != 1:
        raise Exception('There must be one and only one top level compartment. Found %d' % (len(topcomp_list)))
    ret = stack[0]
    while len(stack) > 0:
        comp = stack.pop()
        parentlist = comp.neighbors['axial']
        parent = None
        if len(parentlist) > 0:
            parent = parentlist[0]
            comp.x0, comp.y0, comp.z0, = parent.x, parent.y, parent.z
        else:
            comp.x0, comp.y0, comp.z0, = (0, 0, 0)
        if comp.length > 0:
            comp.x, comp.y, comp.z, = comp.x0, comp.y0, comp.z0 + comp.length
        else:
            # for spherical compartments x0, y0, z0 are centre
            # position nad x,y,z are on the surface
            comp.x, comp.y, comp.z, = comp.x0, comp.y0, comp.z0 + comp.diameter/2.0
        # We take z == 0 as an indicator that this compartment has not
        # been processed before - saves against inadvertent loops.
        stack.extend([childcomp for childcomp in map(moose.element, comp.neighbors['raxial']) if childcomp.z == 0])
    return ret

def loadModel(filename, target,method='ee'):
    moose.loadModel(filename,target)
    moose.mooseaddChemSolver(target,method)
	
def readcell_scrambled(filename, target, method='ee'):
    """A special version for handling cases where a .p file has a line
    with specified parent yet to be defined.

    It creates a temporary file with a sorted version based on
    connectivity, so that parent is always defined before child."""
    pfile = open(filename, "r")
    tmpfilename = filename + ".tmp"
    graph = defaultdict(list)
    data = {}
    error = None
    root = None
    ccomment_started = False
    current_compt_params = []
    for line in pfile:
        tmpline = line.strip()
        if not tmpline or tmpline.startswith("//"):
            continue
        elif tmpline.startswith("/*"):
            ccomment_started = True
        if tmpline.endswith('*/'):
            ccomment_started = False
        if ccomment_started:
            continue
        if tmpline.startswith('*set_compt_param'):
            current_compt_params.append(tmpline)
            continue
        node, parent, rest, = tmpline.partition(' ')
        print('22222222', node, parent)
        if (parent == "none"):
            if (root is None):
                root = node
            else:
                raise ValueError("Duplicate root elements: ", root, node, "> Cannot process any further.")
                break
        graph[parent].append(node)
        data[node] = '\n'.join(current_compt_params)

    tmpfile = open(tmpfilename, "w")
    stack = [root]
    while stack:
        current = stack.pop()
        children = graph[current]
        stack.extend(children)
        print('#########"', current, '": ', data[current])
        tmpfile.write(data[current])
    tmpfile.close()
    ret = moose.loadModel(tmpfilename, target, method)
    return ret

## Subha: In many scenarios resetSim is too rigid and focussed on
## neuronal simulation.  The setDefaultDt and
## assignTicks/assignDefaultTicks keep the process of assigning dt to
## ticks and assigning ticks to objects separate. reinit() should be
## explicitly called by user just before running a simulation, and not
## when setting it up.
def updateTicks(tickDtMap):
    """Try to assign dt values to ticks.

    Parameters
    ----------
    tickDtMap: dict
    map from tick-no. to dt value. if it is empty, then default dt
    values are assigned to the ticks.

    """
    for tickNo, dt in list(tickDtMap.items()):
        if tickNo >= 0 and dt > 0.0:
            moose.setClock(tickNo, dt)
    if all([(v == 0) for v in list(tickDtMap.values())]):
        setDefaultDt()

def assignTicks(tickTargetMap):
    """
    Assign ticks to target elements.

    Parameters
    ----------
    tickTargetMap:
    Map from tick no. to target path and method. The path can be wildcard expression also.
    """
    if len(tickTargetMap) == 0:
        assignDefaultTicks()
    for tickNo, target in list(tickTargetMap.items()):
        if not isinstance(target, str):
            if len(target) == 1:
                moose.useClock(tickNo, target[0], 'process')
            elif len(target) == 2:
                moose.useClock(tickNo, target[0], target[1])
        else:
            moose.useClock(tickNo, target, 'process')

    # # This is a hack, we need saner way of scheduling
    # ticks = moose.vec('/clock/tick')
    # valid = []
    # for ii in range(ticks[0].localNumField):
    #     if ticks[ii].dt > 0:
    #         valid.append(ii)
    # if len(valid) == 0:
    #     assignDefaultTicks()

def setDefaultDt(elecdt=1e-5, chemdt=0.01, tabdt=1e-5, plotdt1=1.0, plotdt2=0.25e-3):
    """Setup the ticks with dt values.

    Parameters
    ----------

    elecdt: dt for ticks used in computing electrical biophysics, like
    neuronal compartments, ion channels, synapses, etc.

    chemdt: dt for chemical computations like enzymatic reactions.

    tabdt: dt for lookup tables

    plotdt1: dt for chemical kinetics plotting

    plotdt2: dt for electrical simulations

    """
    moose.setClock(0, elecdt)
    moose.setClock(1, elecdt)
    moose.setClock(2, elecdt)
    moose.setClock(3, elecdt)
    moose.setClock(4, chemdt)
    moose.setClock(5, chemdt)
    moose.setClock(6, tabdt)
    moose.setClock(7, tabdt)
    moose.setClock(8, plotdt1) # kinetics sim
    moose.setClock(9, plotdt2) # electrical sim

def assignDefaultTicks(modelRoot='/model', dataRoot='/data', solver='hsolve'):
    if isinstance(modelRoot, moose.melement) or isinstance(modelRoot, moose.vec):
        modelRoot = modelRoot.path
    if isinstance(dataRoot, moose.melement) or isinstance(dataRoot, moose.vec):
        dataRoot = dataRoot.path
    if solver != 'hsolve' or len(moose.wildcardFind('%s/##[ISA=HSolve]' % (modelRoot))) == 0:
        moose.useClock(0, '%s/##[ISA=Compartment]' % (modelRoot), 'init')
        moose.useClock(1, '%s/##[ISA=Compartment]'  % (modelRoot), 'process')
        moose.useClock(2, '%s/##[ISA=HHChannel]'  % (modelRoot), 'process')
        # moose.useClock(2, '%s/##[ISA=ChanBase]'  % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=IzhikevichNrn]' % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=GapJunction]' % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=HSolve]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=LeakyIaF]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=IntFire]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=SpikeGen]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=PulseGen]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=StimulusTable]'  % (modelRoot), 'process')
    moose.useClock(1, '%s/##[ISA=TimeTable]'  % (modelRoot), 'process')
    moose.useClock(2, '%s/##[ISA=HHChannel2D]'  % (modelRoot), 'process')
    moose.useClock(2, '%s/##[ISA=SynChan]'  % (modelRoot), 'process')
    moose.useClock(2, '%s/##[ISA=MgBlock]'  % (modelRoot), 'process')
    moose.useClock(3, '%s/##[ISA=CaConc]'  % (modelRoot), 'process')
    moose.useClock(3, '%s/##[ISA=Func]' % (modelRoot), 'process')
    # The voltage clamp circuit depends critically on the dt used for
    # computing soma Vm and need to be on a clock with dt=elecdt.
    moose.useClock(0, '%s/##[ISA=DiffAmp]'  % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=VClamp]' % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=PIDController]' % (modelRoot), 'process')
    moose.useClock(0, '%s/##[ISA=RC]' % (modelRoot), 'process')
    # Special case for kinetics models
    kinetics = moose.wildcardFind('%s/##[FIELD(name)=kinetics]' % modelRoot)
    if len(kinetics) > 0:
        # Do nothing for kinetics models - until multiple scheduling issue is fixed.
        moose.useClock(4, '%s/##[ISA!=PoolBase]' % (kinetics[0].path), 'process')
        moose.useClock(5, '%s/##[ISA==PoolBase]' % (kinetics[0].path), 'process')
        moose.useClock(18, '%s/##[ISA=Table2]' % (dataRoot), 'process')
    else:
        # input() function is called in Table. process() which gets
        # called at each timestep. When a message is connected
        # explicitly to input() dest field, it is driven by the sender
        # and process() adds garbage value to the vector. Hence not to
        # be scheduled.
        for tab in moose.wildcardFind('%s/##[ISA=Table]' % (dataRoot)):
            if len(tab.neighbors['input']) == 0:
                moose.useClock(9, tab.path, 'process')

def stepRun(simtime, steptime, verbose=True, logger=None):
    """Run the simulation in steps of `steptime` for `simtime`."""
    clock = moose.Clock('/clock')
    if verbose:
        msg = 'Starting simulation for %g' % (simtime)
        if logger is None:
            print(msg)
        else:
            logger.info(msg)
    ts = datetime.now()
    while clock.currentTime < simtime - steptime:
        ts1 = datetime.now()
        moose.start(steptime)
        te = datetime.now()
        td = te - ts1
        if verbose:
            msg = 'Simulated till %g. Left: %g. %g of simulation took: %g s' % (clock.currentTime, simtime - clock.currentTime, steptime, td.days * 86400 + td.seconds + 1e-6 * td.microseconds)
            if logger is None:
                print(msg)
            else:
                logger.info(msg)

    remaining = simtime - clock.currentTime
    if remaining > 0:
        if verbose:
            msg = 'Running the remaining %g.' % (remaining)
            if logger is None:
                print(msg)
            else:
                logger.info(msg)
        moose.start(remaining)
    te = datetime.now()
    td = te - ts
    dt = min([t for t in moose.element('/clock').dts if t > 0.0])
    if verbose:
        msg = 'Finished simulation of %g with minimum dt=%g in %g s' % (simtime, dt, td.days * 86400 + td.seconds + 1e-6 * td.microseconds)
        if logger is None:
            print(msg)
        else:
            logger.info(msg)



############# added by Aditya Gilra -- begin ################

def resetSim(simpaths, simdt, plotdt, simmethod='hsolve'):
    """ For each of the MOOSE paths in simpaths, this sets the clocks and finally resets MOOSE.
    If simmethod=='hsolve', it sets up hsolve-s for each Neuron under simpaths, and clocks for hsolve-s too. """
    print('Solver:', simmethod)
    moose.setClock(INITCLOCK, simdt)
    moose.setClock(ELECCLOCK, simdt) # The hsolve and ee methods use clock 1
    moose.setClock(CHANCLOCK, simdt) # hsolve uses clock 2 for mg_block, nmdachan and others.
    moose.setClock(POOLCLOCK, simdt) # Ca/ion pools & funcs use clock 3
    moose.setClock(STIMCLOCK, simdt) # Ca/ion pools & funcs use clock 3
    moose.setClock(PLOTCLOCK, plotdt) # for tables
    for simpath in simpaths:
        ## User can connect [qty]Out of an element to input of Table or
        ## requestOut of Table to get[qty] of the element.
        ## Scheduling the Table to a clock tick, will call process() of the Table
        ## which will send a requestOut and overwrite any value set by input(),
        ## thus adding garbage value to the vector. Hence schedule only if
        ## input message is not connected to the Table.
        for table in moose.wildcardFind(simpath+'/##[TYPE=Table]'):
            if len(table.neighbors['input']) == 0:
                moose.useClock(PLOTCLOCK, table.path, 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=PulseGen]', 'process')
        moose.useClock(STIMCLOCK, simpath+'/##[TYPE=DiffAmp]', 'process')
        moose.useClock(STIMCLOCK, simpath+'/##[TYPE=VClamp]', 'process')
        moose.useClock(STIMCLOCK, simpath+'/##[TYPE=PIDController]', 'process')
        moose.useClock(STIMCLOCK, simpath+'/##[TYPE=RC]', 'process')
        moose.useClock(STIMCLOCK, simpath+'/##[TYPE=TimeTable]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=LeakyIaF]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=IntFire]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=IzhikevichNrn]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=SpikeGen]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=Interpol]', 'process')
        moose.useClock(ELECCLOCK, simpath+'/##[TYPE=Interpol2D]', 'process')
        moose.useClock(CHANCLOCK, simpath+'/##[TYPE=HHChannel2D]', 'process')
        moose.useClock(CHANCLOCK, simpath+'/##[TYPE=SynChan]', 'process')
        ## If simmethod is not hsolve, set clocks for the biophysics,
        ## else just put a clock on the hsolve:
        ## hsolve takes care of the clocks for the biophysics
        if 'hsolve' not in simmethod.lower():
            print('Using exp euler')
            moose.useClock(INITCLOCK, simpath+'/##[TYPE=Compartment]', 'init')
            moose.useClock(ELECCLOCK, simpath+'/##[TYPE=Compartment]', 'process')
            moose.useClock(CHANCLOCK, simpath+'/##[TYPE=HHChannel]', 'process')
            moose.useClock(POOLCLOCK, simpath+'/##[TYPE=CaConc]', 'process')
            moose.useClock(POOLCLOCK, simpath+'/##[TYPE=Func]', 'process')
        else: # use hsolve, one hsolve for each Neuron
            print('Using hsolve')
            element = moose.Neutral(simpath)
            for childid in element.children:
                childobj = moose.Neutral(childid)
                classname = childobj.className
                if classname in ['Neuron']:
                    neuronpath = childobj.path
                    h = moose.HSolve( neuronpath+'/solve' )
                    h.dt = simdt
                    h.target = neuronpath
                    moose.useClock(INITCLOCK, h.path, 'process')
    moose.reinit()

def setupTable(name, obj, qtyname, tables_path=None, threshold=None, spikegen=None):
    """ Sets up a table with 'name' which stores 'qtyname' field from 'obj'.
    The table is created under tables_path if not None, else under obj.path . """
    if tables_path is None:
        tables_path = obj.path+'/data'
    ## in case tables_path does not exist, below wrapper will create it
    tables_path_obj = moose.Neutral(tables_path)
    qtyTable = moose.Table(tables_path_obj.path+'/'+name)
    ## stepMode no longer supported, connect to 'input'/'spike' message dest to record Vm/spiktimes
    # qtyTable.stepMode = TAB_BUF
    if spikegen is None:
        if threshold is None:
            ## below is wrong! reads qty twice every clock tick!
            #moose.connect( obj, qtyname+'Out', qtyTable, "input")
            ## this is the correct method
            moose.connect( qtyTable, "requestOut", obj, 'get'+qtyname)
        else:
            ## create new spikegen
            spikegen = moose.SpikeGen(tables_path_obj.path+'/'+name+'_spikegen')
            ## connect the compartment Vm to the spikegen
            moose.connect(obj,"VmOut",spikegen,"Vm")
            ## spikegens for different synapse_types can have different thresholds
            spikegen.threshold = threshold
            spikegen.edgeTriggered = 1 # This ensures that spike is generated only on leading edge.
    else:
        moose.connect(spikegen,'spikeOut',qtyTable,'input') ## spikeGen gives spiketimes
    return qtyTable

def connectSynapse(compartment, synname, gbar_factor):
    """
    Creates a synname synapse under compartment, sets Gbar*gbar_factor, and attaches to compartment.
    synname must be a synapse in /library of MOOSE.
    """
    synapseid = moose.copy(moose.SynChan('/library/'+synname),compartment,synname)
    synapse = moose.SynChan(synapseid)
    synapse.Gbar = synapse.Gbar*gbar_factor
    synapse_mgblock = moose.Mstring(synapse.path+'/mgblockStr')
    if synapse_mgblock.value=='True': # If NMDA synapse based on mgblock, connect to mgblock
        mgblock = moose.Mg_block(synapse.path+'/mgblock')
        compartment.connect("channel", mgblock, "channel")
    else:
        compartment.connect("channel", synapse, "channel")
    return synapse

def printNetTree():
    """ Prints all the cells under /, and recursive prints the cell tree for each cell. """
    root = moose.Neutral('/')
    for id in root.children: # all subelements of 'root'
        if moose.Neutral(id).className == 'Cell':
            cell = moose.Cell(id)
            print("-------------------- CELL : ",cell.name," ---------------------------")
            printCellTree(cell)

def printCellTree(cell):
    """
    Prints the tree under MOOSE object 'cell'.
    Assumes cells have all their compartments one level below,
    also there should be nothing other than compartments on level below.
    Apart from compartment properties and messages,
    it displays the same for subelements of compartments only one level below the compartments.
    Thus NMDA synapses' mgblock-s will be left out.

    FIXME: no lenght cound on compartment.
    """
    for compartmentid in cell.children: # compartments
        comp = moose.Compartment(compartmentid)
        print("  |-",comp.path, 'l=',comp.length, 'd=',comp.diameter, 'Rm=',comp.Rm, 'Ra=',comp.Ra, 'Cm=',comp.Cm, 'EM=',comp.Em)
        #for inmsg in comp.inMessages():
        #    print "    |---", inmsg
        #for outmsg in comp.outMessages():
        #    print "    |---", outmsg
        printRecursiveTree(compartmentid, level=2) # for channels and synapses and recursively lower levels

## Use printCellTree which calls this
def printRecursiveTree(elementid, level):
    """ Recursive helper function for printCellTree,
    specify depth/'level' to recurse and print subelements under MOOSE 'elementid'. """
    spacefill = '  '*level
    element = moose.Neutral(elementid)
    for childid in element.children:
        childobj = moose.Neutral(childid)
        classname = childobj.className
        if classname in ['SynChan','KinSynChan']:
            childobj = moose.SynChan(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'Gbar=',childobj.Gbar, 'numSynapses=', childobj.numSynapses)
            return # Have yet to figure out the children of SynChan, currently not going deeper
        elif classname in ['HHChannel', 'HHChannel2D']:
            childobj = moose.HHChannel(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'Gbar=',childobj.Gbar, 'Ek=',childobj.Ek)
        elif classname in ['CaConc']:
            childobj = moose.CaConc(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'thick=',childobj.thick, 'B=',childobj.B)
        elif classname in ['Mg_block']:
            childobj = moose.Mg_block(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'CMg',childobj.CMg, 'KMg_A',childobj.KMg_A, 'KMg_B',childobj.KMg_B)
        elif classname in ['SpikeGen']:
            childobj = moose.SpikeGen(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'threshold',childobj.threshold)
        elif classname in ['Func']:
            childobj = moose.Func(childid)
            print(spacefill+"|--", childobj.name, childobj.className, 'expr',childobj.expr)
        elif classname in ['Table']: # Table gives segfault if printRecursiveTree is called on it
            return # so go no deeper
        #for inmsg in childobj.inMessages():
        #    print spacefill+"  |---", inmsg
        #for outmsg in childobj.outMessages():
        #    print spacefill+"  |---", outmsg
        if len(childobj.children)>0:
            printRecursiveTree(childid, level+1)

def setup_vclamp(compartment, name, delay1, width1, level1, gain=0.5e-5):
    """
    Sets up a voltage clamp with 'name' on MOOSE 'compartment' object:
    adapted from squid.g in DEMOS (moose/genesis)
    Specify the 'delay1', 'width1' and 'level1' of the voltage to be applied to the compartment.
    Typically you need to adjust the PID 'gain'
    For perhaps the Davison 4-compartment mitral or the Davison granule:
    0.5e-5 optimal gain - too high 0.5e-4 drives it to oscillate at high frequency,
    too low 0.5e-6 makes it have an initial overshoot (due to Na channels?)
    Returns a MOOSE table with the PID output.
    """
    ## If /elec doesn't exists it creates /elec and returns a reference to it.
    ## If it does, it just returns its reference.
    moose.Neutral('/elec')
    pulsegen = moose.PulseGen('/elec/pulsegen'+name)
    vclamp = moose.DiffAmp('/elec/vclamp'+name)
    vclamp.saturation = 999.0
    vclamp.gain = 1.0
    lowpass = moose.RC('/elec/lowpass'+name)
    lowpass.R = 1.0
    lowpass.C = 50e-6 # 50 microseconds tau
    PID = moose.PIDController('/elec/PID'+name)
    PID.gain = gain
    PID.tau_i = 20e-6
    PID.tau_d = 5e-6
    PID.saturation = 999.0
    # All connections should be written as source.connect('',destination,'')
    pulsegen.connect('outputSrc',lowpass,'injectMsg')
    lowpass.connect('outputSrc',vclamp,'plusDest')
    vclamp.connect('outputSrc',PID,'commandDest')
    PID.connect('outputSrc',compartment,'injectMsg')
    compartment.connect('VmSrc',PID,'sensedDest')

    pulsegen.trigMode = 0 # free run
    pulsegen.baseLevel = -70e-3
    pulsegen.firstDelay = delay1
    pulsegen.firstWidth = width1
    pulsegen.firstLevel = level1
    pulsegen.secondDelay = 1e6
    pulsegen.secondLevel = -70e-3
    pulsegen.secondWidth = 0.0

    vclamp_I = moose.Table("/elec/vClampITable"+name)
    vclamp_I.stepMode = TAB_BUF #TAB_BUF: table acts as a buffer.
    vclamp_I.connect("inputRequest", PID, "output")
    vclamp_I.useClock(PLOTCLOCK)

    return vclamp_I

def setup_iclamp(compartment, name, delay1, width1, level1):
    """
    Sets up a current clamp with 'name' on MOOSE 'compartment' object:
    Specify the 'delay1', 'width1' and 'level1' of the current pulse to be applied to the compartment.
    Returns the MOOSE pulsegen that sends the current pulse.
    """
    ## If /elec doesn't exists it creates /elec and returns a reference to it.
    ## If it does, it just returns its reference.
    moose.Neutral('/elec')
    pulsegen = moose.PulseGen('/elec/pulsegen'+name)
    iclamp = moose.DiffAmp('/elec/iclamp'+name)
    iclamp.saturation = 1e6
    iclamp.gain = 1.0
    pulsegen.trigMode = 0 # free run
    pulsegen.baseLevel = 0.0
    pulsegen.firstDelay = delay1
    pulsegen.firstWidth = width1
    pulsegen.firstLevel = level1
    pulsegen.secondDelay = 1e6 # to avoid repeat
    pulsegen.secondLevel = 0.0
    pulsegen.secondWidth = 0.0
    pulsegen.connect('output',iclamp,'plusIn')
    iclamp.connect('output',compartment,'injectMsg')
    return pulsegen

def get_matching_children(parent, names):
    """ Returns non-recursive children of 'parent' MOOSE object
    with their names containing any of the strings in list 'names'. """
    matchlist = []
    for childID in parent.children:
        child = moose.Neutral(childID)
        for name in names:
            if name in child.name:
                matchlist.append(childID)
    return matchlist

def underscorize(path):
    """ Returns: / replaced by underscores in 'path'.
    But async13 branch has indices in the path like [0],
    so just replacing / by _ is not enough,
    should replace [ and ] also by _ """
    return path.replace('/','_').replace('[','-').replace(']','-')

def blockChannels(cell, channel_list):
    """
    Sets gmax to zero for channels of the 'cell' specified in 'channel_list'
    Substring matches in channel_list are allowed
    e.g. 'K' should block all K channels (ensure that you don't use capital K elsewhere in your channel name!)
    """
    for compartmentid in cell.children: # compartments
        comp = moose.Compartment(compartmentid)
        for childid in comp.children:
            child = moose.Neutral(childid)
            if child.className in ['HHChannel', 'HHChannel2D']:
                chan = moose.HHChannel(childid)
                for channame in channel_list:
                    if channame in chan.name:
                        chan.Gbar = 0.0

def get_child_Mstring(mooseobject,mstring):
    for child in mooseobject.children:
        if child.className=='Mstring' and child.name==mstring:
            child = moose.Mstring(child)
            return child
    return None

def connect_CaConc(compartment_list, temperature=None):
    """ Connect the Ca pools and channels within each of the compartments in compartment_list
     Ca channels should have a child Mstring named 'ion' with value set in MOOSE.
     Ca dependent channels like KCa should have a child Mstring called 'ionDependency' with value set in MOOSE.
     Call this only after instantiating cell so that all channels and pools have been created. """
    for compartment in compartment_list:
        caconc = None
        for child in compartment.children:
            neutralwrap = moose.Neutral(child)
            if neutralwrap.className == 'CaConc':
                caconc = moose.CaConc(child)
                break
        if caconc is not None:
            child = get_child_Mstring(caconc,'phi')
            if child is not None:
                caconc.B = float(child.value) # B = phi by definition -- see neuroml 1.8.1 defn
            else:
                ## B has to be set for caconc based on thickness of Ca shell and compartment l and dia,
                ## OR based on the Mstring phi under CaConc path.
                ## I am using a translation from Neuron for mitral cell, hence this method.
                ## In Genesis, gmax / (surfacearea*thick) is set as value of B!
                caconc.B = 1 / (2*FARADAY) / \
                    (math.pi*compartment.diameter*compartment.length * caconc.thick)
            for child in compartment.children:
                neutralwrap = moose.Neutral(child)
                if neutralwrap.className == 'HHChannel':
                    channel = moose.HHChannel(child)
                    ## If child Mstring 'ion' is present and is Ca, connect channel current to caconc
                    for childid in channel.children:
                        # in async13, gates which have not been created still 'exist'
                        # i.e. show up as a child, but cannot be wrapped.
                        try:
                            child = moose.element(childid)
                            if child.className=='Mstring':
                                child = moose.Mstring(child)
                                if child.name=='ion':
                                    if child.value in ['Ca','ca']:
                                        moose.connect(channel,'IkOut',caconc,'current')
                                        #print 'Connected IkOut of',channel.path,'to current of',caconc.path
                                ## temperature is used only by Nernst part here...
                                if child.name=='nernst_str':
                                    nernst = moose.Nernst(channel.path+'/nernst')
                                    nernst_params = child.value.split(',')
                                    nernst.Cout = float(nernst_params[0])
                                    nernst.valence = float(nernst_params[1])
                                    nernst.Temperature = temperature
                                    moose.connect(nernst,'Eout',channel,'setEk')
                                    moose.connect(caconc,'concOut',nernst,'ci')
                                    #print 'Connected Nernst',nernst.path
                        except TypeError:
                            pass

                if neutralwrap.className == 'HHChannel2D':
                    channel = moose.HHChannel2D(child)
                    ## If child Mstring 'ionDependency' is present, connect caconc Ca conc to channel
                    for childid in channel.children:
                        # in async13, gates which have not been created still 'exist'
                        # i.e. show up as a child, but cannot be wrapped.
                        try:
                            child = moose.element(childid)
                            if child.className=='Mstring' and child.name=='ionDependency':
                                child = moose.Mstring(child)
                                if child.value in ['Ca','ca']:
                                    moose.connect(caconc,'concOut',channel,'concen')
                                    #print 'Connected concOut of',caconc.path,'to concen of',channel.path
                        except TypeError:
                            pass
