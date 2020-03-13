# -*- coding: utf-8 -*-
import numpy
import sys
import os
import moose

scriptdir = os.path.dirname(os.path.realpath(__file__))
print('Script dir %s' % scriptdir)

def test_kkit():
    """This example illustrates loading, running, and saving a kinetic model
        defined in kkit format. It uses a default kkit model but you can
        specify another using the command line ``python filename runtime
        solver``. We use the gsl solver here. The model already defines a
        couple of plots and sets the runtime to 20 seconds.
        """
    solver = "gsl"  # Pick any of gsl, gssa, ee..
    mfile = os.path.join(scriptdir, '..', 'data', 'kkit_objects_example.g')
    runtime = 20.0
    modelId = moose.loadModel(mfile, 'model')
    moose.mooseAddChemSolver('model', solver)
    moose.reinit()
    moose.start(runtime)

    for x in moose.wildcardFind('/model/#graphs/conc#/#'):
        t = numpy.arange(0, x.vector.size, 1) * x.dt

    vals = x.vector
    stats = [vals.min(), vals.max(), vals.mean(), vals.std()]
    expected = [0.0, 0.00040464, 0.0001444, 0.00013177]
    assert numpy.allclose(stats, expected,
                          rtol=1e-4), 'Got %s expected %s' % (stats, expected)

def main():
    test_kkit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
