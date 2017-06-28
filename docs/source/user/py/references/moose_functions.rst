MOOSE Functions
---------------

element
^^^^^^^
moose.element(arg) -> moose object

Convert a path or an object to the appropriate builtin moose class instance.

`arg` : str/vec/moose object
    path of the moose element to be converted or another element (possibly available as a superclass instance).

Returns - melement
    MOOSE element (object) corresponding to the `arg` converted to write subclass.

getFieldNames
^^^^^^^^^^^^^
moose.getFieldNames(className, finfoType='valueFinfo') -> tuple

Get a tuple containing the name of all the fields of `finfoType` kind.

`className` : string
    Name of the class to look up.
`finfoType` : string
    The kind of field -
    `valueFinfo` -
    `srcFinfo`   -
    `destFinfo`  -
    `lookupFinfo`-
    `fieldElementFinfo` -

Returns - tuple
    Names of the fields of type `finfoType` in class `className`.

copy
^^^^
moose.copy(src, dest, name, n, toGlobal, copyExtMsg) -> bool

Make copies of a moose object.

`src` : vec, element or str
    source object.
`dest` : vec, element or str
    Destination object to copy into.
`name` : str
    Name of the new object. If omitted, name of the original will be used.
`n` : int
    Number of copies to make.
`toGlobal` : int
    Relevant for parallel environments only. If false, the copies will
    reside on local node, otherwise all nodes get the copies.
`copyExtMsg` : int
    If true, messages to/from external objects are also copied.

Returns - vec
    newly copied vec

move
^^^^
moose.move(...)
    Move a vec object to a destination.

delete
^^^^^^
moose.delete(...)
    delete(obj)->None

Delete the underlying moose object. This does not delete any of the
Python objects referring to this vec but does invalidate them. Any
attempt to access them will raise a ValueError.

`id` : vec
    vec of the object to be deleted.

Returns - None

useClock
^^^^^^^^
moose.useClock(tick, path, fn)

schedule `fn` function of every object that matches `path` on tick no. `tick`.

Most commonly the function is 'process'.  NOTE: unlike earlier versions, now
autoschedule is not available. You have to call useClock for every element that
should be updated during the  simulation.

The sequence of clockticks with the same dt is according to their number.
This is utilized for controlling the order of updates in various objects where it matters. The following convention should be observed when assigning clockticks to various components of a model:

Clock ticks 0-3 are for electrical (biophysical) components, 4 and 5 are for chemical kinetics, 6 and 7 are for lookup tables and stimulus, 8 and 9 are for recording tables.

Generally, `process` is the method to be assigned a clock tick. Notable exception is `init` method of Compartment class which is assigned tick 0.

   - 0 : Compartment: `init`
   - 1 : Compartment: `process`
   - 2 : HHChannel and other channels: `process`
   - 3 : CaConc : `process`
   - 4,5 : Elements for chemical kinetics : `process`
   - 6,7 : Lookup (tables), stimulus : `process`
   - 8,9 : Tables for plotting : `process`

`tick` : int
        tick number on which the targets should be scheduled.
`path` : str
        path of the target element(s). This can be a wildcard also.
`fn` : str
        name of the function to be called on each tick. Commonly `process`.

Examples -

In multi-compartmental neuron model a compartment's membrane potential (Vm) is dependent on its neighbours' membrane potential. Thus it must get the neighbour's present Vm before computing its own Vm in next time step. This ordering is achieved by scheduling the `init` function, which communicates membrane potential, on tick 0 and `process` function on tick 1.::

      >>> moose.useClock(0, '/model/compartment_1', 'init')
      >>> moose.useClock(1, '/model/compartment_1', 'process')

setClock
^^^^^^^^
moose.setClock(tick, dt)

set the ticking interval of `tick` to `dt`.

A tick with interval `dt` will call the functions scheduled on that tick every `dt` timestep.

`tick` : int
    tick number
`dt` : double
    ticking interval

start
^^^^^
moose.start(time, notify = False) -> None

Run simulation for `t` time. Advances the simulator clock by `t`
time. If 'notify = True', a message is written to terminal whenever
10% of simulation time is over.

After setting up a simulation, YOU MUST CALL MOOSE.REINIT() before
CALLING MOOSE.START() TO EXECUTE THE SIMULATION. Otherwise, the
simulator behaviour will be undefined. Once moose.reinit() has been
called, you can call moose.start(t) as many time as you like. This
will continue the simulation from the last state for `t` time.

`t` : float
    duration of simulation.
`notify` : bool
    default False. If True, notify user whenever 10% of simultion
    is over.

Returns - None

reinit
^^^^^^
moose.reinit() -> None

Reinitialize simulation.

This function (re)initializes moose simulation. It must be called
before you start the simulation (see moose.start). If you want to
continue simulation after you have called moose.reinit() and
moose.start(), you must NOT call moose.reinit() again. Calling
moose.reinit() again will take the system back to initial setting
(like clear out all data recording tables, set state variables to
their initial values, etc.

stop
^^^^
moose.stop(...)
    Stop simulation

isRunning
^^^^^^^^^
moose.isRunning(...)
    True if the simulation is currently running.

exists
^^^^^^
moose.exists(...)
    True if there is an object with specified path.

loadModel
^^^^^^^^^
moose.loadModel(...)
    loadModel(filename, modelpath, solverclass) -> vec

    Load model from a file to a specified path.

`filename` : str
    model description file.
`modelpath` : str
    moose path for the top level element of the model to be created.
`solverclass` : str, optional
    solver type to be used for simulating the model.

Returns - vec
    loaded model container vec.

connect
^^^^^^^
moose.connect(src, srcfield, destobj, destfield[,msgtype]) -> bool

    Create a message between `src_field` on `src` object to `dest_field` on `dest` object.
    This function is used mainly, to say, connect two entities, and to denote what kind of
    give-and-take relationship they share.It enables the 'destfield' (of
    the 'destobj') to acquire the data, from 'srcfield'(of the 'src').

`src` : element/vec/string
    the source object (or its path)
    (the one that provides information)
`srcfield` : str
    source field on self.(type of the information)
`destobj` : element
    Destination object to connect to.
    (The one that need to get information)
`destfield` : str
    field to connect to on `destobj`.
`msgtype` : str
    type of the message. Can be
    `Single` -
    `OneToAll` -
    `AllToOne` -
    `OneToOne` -
    `Reduce` -
    `Sparse` -
    Default: `Single`.

Returns - `msgmanager` : melement
    message-manager for the newly created message.

Examples -
Connect the output of a pulse generator to the input of a spike generator::

    >>> pulsegen = moose.PulseGen('pulsegen')
    >>> spikegen = moose.SpikeGen('spikegen')
    >>> pulsegen.connect('output', spikegen, 'Vm')

getCwe
^^^^^^
moose.getCwe(...)
    Get the current working element. 'pwe' is an alias of this function.

setCwe
^^^^^^
moose.setCwe(...)
    Set the current working element. 'ce' is an alias of this function

getFieldDict
^^^^^^^^^^^^
moose.getFieldDict(className, finfoType) -> dict

    Get dictionary of field names and types for specified class.

`className` : str
    MOOSE class to find the fields of.
`finfoType` : str (optional)
    Finfo type of the fields to find. If empty or not specified, all
    fields will be retrieved.

Returns - dict
    field names and their types.

Notes -
    This behaviour is different from `getFieldNames` where only
    `valueFinfo`s are returned when `finfoType` remains unspecified.

Examples -
    List all the source fields on class Neutral::

    >>> moose.getFieldDict('Neutral', 'srcFinfo')
    >>> {'childMsg': 'int'}

getField
^^^^^^^^
moose.getField(...)
    getField(element, field, fieldtype) -- Get specified field of specified type from object vec.

seed
^^^^
moose.seed(...)
    moose.seed(seedvalue) -> seed

    Reseed MOOSE random number generator.

`seed` : int
    Value to use for seeding.
    All RNGs in moose except rand functions in moose.Function
    expression use this seed.
    By default (when this function is not called) seed is initializecd
    to some random value using system random device (if available).


    default: random number generated using system random device

    Returns - None

rand
^^^^
moose.rand(...)
    moose.rand() -> [0,1)

    Returns - float in [0, 1) real interval generated by MT19937.

    Notes -
    MOOSE does not automatically seed the random number generator. You
    must explicitly call moose.seed() to create a new sequence of random
    numbers each time.

wildcardFind
^^^^^^^^^^^^
moose.wildcardFind(expression) -> tuple of melements.

    Find an object by wildcard.

`expression` : str
    MOOSE allows wildcard expressions of the form::

          {PATH}/{WILDCARD}[{CONDITION}]

    where {PATH} is valid path in the element tree.
    `{WILDCARD}` can be `#` or `##`.

    `#` causes the search to be restricted to the children of the
    element specified by {PATH}.

    `##` makes the search to recursively go through all the descendants
    of the {PATH} element.
    {CONDITION} can be::

        TYPE={CLASSNAME} : an element satisfies this condition if it is of
        class {CLASSNAME}.
        ISA={CLASSNAME} : alias for TYPE={CLASSNAME}
        CLASS={CLASSNAME} : alias for TYPE={CLASSNAME}
        FIELD({FIELDNAME}){OPERATOR}{VALUE} : compare field {FIELDNAME} with
        {VALUE} by {OPERATOR} where {OPERATOR} is a comparison operator (=,
        !=, >, <, >=, <=).

    For example, /mymodel/##[FIELD(Vm)>=-65] will return a list of all
    the objects under /mymodel whose Vm field is >= -65.

    Returns - tuple
      all elements that match the wildcard.

quit
^^^^
    Finalize MOOSE threads and quit MOOSE. This is made available for
    debugging purpose only. It will automatically get called when moose
    module is unloaded. End user should not use this function.

moose.quit(...)
    Finalize MOOSE threads and quit MOOSE. This is made available for debugging purpose only. It will automatically get called when moose module is unloaded. End user should not use this function.
