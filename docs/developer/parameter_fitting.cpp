/**

\page ParamFitting Parameter fitting

\section Introduction

When you have experimental data on a phenomenon, and you intend to create a computational model of it, usually you need to apply parameter fitting/searching techniques. These methods help you determine those parameters of your model that you have no reference on. Parameter fitting using MOOSE models are accomplished through utilizing <a target="_blank" href="http://journal.frontiersin.org/Journal/10.3389/fninf.2014.00063/full">Optimizer</a> <a href="#cite1">[1]</a>, a parameter fitting tool developed for neural simulations.


\section Installation Installation

Installation of Optimizer and it's dependencies can be done by running through the <a target="_blank" href="http://optimizer.readthedocs.org/en/latest/install.html">Optimizer documentation</a>. If MOOSE is already installed, dependencies like inspyred, wxpython and pyelectro are needed to installed only besides Optimizer.

If you encounter errors while simulating, then probably the repository of Optimizer is not updated by some changes I made. Here is the repository of a definitely <a href="https://github.com/csiki/optimizer" target="_blank">working (but not necessarily the latest) version</a>.


\section Usage Usage

The <a target="_blank" href="http://optimizer.readthedocs.org/en/latest/tutorial.html">tutorial</a> of Optimizer can guide you through how to work with the GUI, though it's usage is quite obvious.

So the process of parameter fitting consists of the following steps:

<ol>
    <li>Provide experimental data, simulation script, fitting settings to Optimizer,</li>
    <li>Run parameter fitting,</li>
        <ol>
            <li>Optimizer provides parameters to the simulation through params.param file located next to the simulation script,</li>
            <li>Optimizer runs the simulation,</li>
                <ol>
                    <li>Simulation retrieves the parameters from params.param using OptimizerInterface,</li>
                    <li>Simulation runs with the given parameters,</li>
                    <li>Simulation saves the results to trace.dat, located next to the simulation script, using OptimizerInterface,</li>
                </ol>
            <li>Optimizer compares the results with the experimental data, finishes if the results ~fit the experimental data, otherwise goes to 2.1 to run the simulation again with other parameters.</li>
        </ol>
    <li>Optimizer shows parameter fitting results.</li>
</ol>

Let's see an example of a cooperation between Optimizer and MOOSE. First create a python script that is going to be the simulation file, in which your MOOSE code would be that runs the simulation. Let's call it opttest.py:

\verbatim

from moose import optimizer_interface
import math

# constants
time_range = 1000                       # time range (let's say in ms)
experimental_params = [2.1, 3.5, 8.1]   # these should be retrieved at the
                                        # end of the parameter fitting

def simulation(t, params):
    """
    Our simple, artificial 'neural simulation'.
    """
    return math.exp(-params[0]*t) * params[1] + params[2] * (t/5) * 0.1

def generateInput():
    """
    Generates the input file using the 'experimental parameters'.
    """
    with open('input.dat', 'w') as f:
        for t in range(time_range):
            f.write(str(simulation(t, experimental_params)) + '\n')

#~ generateInput()  # this should be uncommented at the first time
#~                  # to generate the experimental data of the simulation

# generally when you put up your MOOSE simulation, everything before this
# comment is NOT needed
# load params
oi = optimizer_interface.OptimizerInterface()   # load parameters from params.param
params = oi.getParams()     # stores the parameters as a list of floats

# simulate using simulation() function
results = []
for t in range(time_range):
    results.append(simulation(t, params))

# add & write traces
oi.addTrace(results)    # adds a trace as the result of the simulation
oi.writeTraces()        # writes the added traces to trace.dat so
                        # Optimizer can read and compare them to the
                        # experimental data

\endverbatim

Instead of having a real MOOSE simulation, there's just an artificial one (basically a function) implemented - it is faster to run, and fundamentally the same as if we had a real MOOSE simulation.

At first we have some global constants and two functions to simplify the code. The main part can be found after the OptimizerInterface object is initialised. We retrieve the parameters that Optimizer has suggested, then we run the 'simulation' with these parameters. Next we add a trace of the simulation's output that is going to be compared with the experimental data. Here you can either pass an iterable object (like a simple python list or numpy.array), or a moose.Table object. At the end we write the trace to trace.dat.

To use this script, first uncomment the call to generateInput() function so it can save the 'experimental data' into input.dat. This input may contain the times (not necessary) of the sampling in the first column and each trace in another following column. Run the script then comment generateInput() back - not necessary, but it would slow down the simulation. After that, open Optimizer GUI by running neuraloptimizer file inside optimizer_base_dir/optimizer/. Select input.dat as your input file, with the following parameters (and also uncheck 'Contains time'):
<ul>
    <li>number of traces: 1</li>
    <li>length of traces (ms): 1000</li>
    <li>sampling frequency (Hz): 1000</li>
</ul>

Then click the Load trace button and if everything goes well, you should see a plot of your input data (now a linear function). Click the arrow at the top!

On the second layer select 'external' where originally Neuron is selected. It tells Optimizer that we'd like to use a simulator apart from Neuron. Then in the 'Command to external simulator' box three elements should be given separated by space:
<ul>
    <li>the command to execute: python - this will run our python script (obviously) consisting the model definition and running of simulation (opttest.py)</li>
    <li>the model file: /absolute_path_to_the_model_file/opttest.py</li>
    <li>(some options passed to your simulation script as commandline arguments)</li>
    <li>number of parameters to optimize: 3</li>
</ul>
So the whole command should look somewhat like this:
\verbatim python /absolute_path_to_the_model_file/opttest.py 3 \endverbatim

On the next layer you can select the fitness function(s) of your choice. Let's select MSE, with a weight of 1.0.

On the 'Select Algorithm' layer choose Simulated Annealing (it's fast enough), then choose the boundaries and the starting points of your parameters (take into consideration the experimental parameters in opttest.py). After that, you can run the parameter fitting, leaving the rest settings as default.

When the parameter search is finished you can save the proper parameter values, see how the model fits the experimental data or check MSE values evolve simulation after simulation.

\author Viktor Tóth

<p><a name="cite1">[1] P. Friedrich, M. Vella, A. I. Gulyás, T. F. Freund, and S. Káli, “A flexible, interactive software tool for fitting the parameters of neuronal models,” Front. Neuroinform, vol. 8, p. 63, 2014.</a></p>

*/
