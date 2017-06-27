# -*- coding: utf-8 -*-
# Author: Subhasis Ray
# Date: Mon Jun 10 16:23:41 IST 2013
import moose

import moose

def main():
    """
    This snippet shows various ways of displaying scheduling
    information of moose model components.

    The `/clock/tick` ematrix has 10 elements, any of which can be setup
    by using the `moose.setClock(tickNo, dt)` function. This sets the
    interval between the ticking events for it to `dt` time.

    Individual model components can be assigned ticks by
    `moose.useClock(tickNo, targetPath, targetFinfo)`. Commonly used
    target finfo is `process`, which causes the function of the same name
    in the ematrix at target path to be called at each ticking event of
    tick `tickNo`. Thus displaying the neighbors of `process` finfo of an
    element will show the tick assigned to it.

    On the other hand, the tick ematrix has 10 finfos, `proc0` ... `proc9`
    which connect to all the targets of the corresponding `tickNo`. You
    can display the neighbors of these finfos also to see what is
    scheduled on each tick.

    """
    comp = moose.Compartment('/comp')
    # Setup the ticks
    moose.setClock(0, 1e-6)
    moose.setClock(1, 1e-6)

    # Schedule the element.
    moose.useClock(0, '/##[ISA=Compartment]', 'init')
    moose.useClock(1, '/##[ISA=Compartment]', 'process')

    # List the ticks connected to an element.
    print(('Ticks connected to `process` method of', comp.path))
    for tick in comp.neighbors['process']:
        print((' ->',tick.path))

    # Different ticks can be connected to different fields.
    print(('Ticks connected to `init` method of', comp.path))
    for tick in comp.neighbors['init']:
        print((' ->',tick.path))

    # View the scheduled elements using the tick nos.
    t = moose.element('/clock')
    print('Elements on tick 0')
    for e in t.neighbors['proc0']:
        print((' ->', e.path))
    print('Elements on tick 1')
    for e in t.neighbors['proc1']:
        print((' ->', e.path))

    ch = moose.HHChannel('/comp/chan')
    moose.useClock(1, ch.path, 'process')
    print((ch.path, 'has been scheduled'))
    print('Elements on tick 1')
    for e in t.neighbors['proc1']:
        print((' ->', e.path))

    # Go through elements by wildcard search and list the ticks connected.
    # This can be slow when the model is too big.
    for el in moose.wildcardFind('/##[ISA=Compartment]'):
        print(('Ticks connected to `process` method of', el.path))
        for t in moose.element(el).neighbors['process']:
            print((' ->', t.path))

if __name__ == '__main__':
    main()
# <codecell>
