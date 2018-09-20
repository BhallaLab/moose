try:
    from neuron import h, rxd
except ImportError as e:
    print( "[INFO ] Failed to import neuron. Quitting ..." )
    quit()
    
import numpy
from matplotlib import pyplot
import time

# needed for standard run system
h.load_file('stdrun.hoc')

dend = h.Section()
dend.nseg = 501

# WHERE the dynamics will take place
where = rxd.Region(h.allsec())

# WHO the actors are
u = rxd.Species(where, d=1, initial=0)

# HOW they act
bistable_reaction = rxd.Rate(u, -u * (1 - u) * (0.3 - u))

# initial conditions
h.finitialize()
for node in u.nodes:
    if node.x < .2: node.concentration = 1

def plot_it(color='k'):
    y = u.nodes.concentration
    x = u.nodes.x

    # convert x from normalized position to microns
    x = dend.L * numpy.array(x)

    pyplot.plot(x, y, color)

plot_it('r')
interval = 50

t1 = time.time()
for i in range(1, 5):
    h.continuerun(i * interval)
    plot_it()
print(("Time = ", time.time() - t1))

pyplot.show()
