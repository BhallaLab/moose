# -*- coding: utf-8 -*-
# Script to test all import modules.

import moose

try:
    import libsbml
except ImportError:
    pass

def library1():
    import moose.genesis 
    import moose.SBML 
    import moose.chemMerge
    import moose.utils 
    import moose.network_utils
    print('done')

    p1 = moose.le()
    a = moose.Pool('/a')
    for i in range(10):
        moose.Pool('/a/p%d'%i)
    p2 = moose.le()
    assert set(p2) - set(p1) == set(['/a'])
    aa = moose.le(a)
    assert len(aa) == 10

    try:
        moose.syncDataHandler('/a')
    except NotImplementedError:
        pass

    try:
        moose.showfield('/x')
    except ValueError:
        pass

    moose.showfield('/a')
    moose.showfields('/a')

def library_doc():
    print("=== Testing documents.")
    d1 = moose.getFieldDoc(('Pool', 'conc'))
    d2 = moose.getFieldDoc(('Ksolve', 'numThreads'))
    assert d1
    assert d1 != d2
    moose.doc(('Ksolve.numThreads'))
    moose.doc('Ksolve')


def test_library():
    library1()
    library_doc()
    

def main():
    test_library()

if __name__ == '__main__':
    main()
