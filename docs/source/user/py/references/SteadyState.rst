SteadyState
-----------

.. py:class:: SteadyState

   SteadyState: works out a steady-state value for a reaction system. This class uses the GSL multidimensional root finder algorithms to find the fixed points closest to the current molecular concentrations. When it finds the fixed points, it figures out eigenvalues of the solution, as a way to help classify the fixed points. Note that the method finds unstable as well as stable fixed points.
    The SteadyState class also provides a utility function *randomInit()*	to randomly initialize the concentrations, within the constraints of stoichiometry. This is useful if you are trying to find the major fixed points of the system. Note that this is probabilistic. If a fixed point is in a very narrow range of state space the probability of finding it is small and you will have to run many iterations with different initial conditions to find it.
    The numerical calculations used by the SteadyState solver are prone to failing on individual calculations. All is not lost, because the system reports the solutionStatus. It is recommended that you test this field after every calculation, so you can simply ignore cases where it failed and try again with different starting conditions.
    Another rule of thumb is that the SteadyState object is more likely to succeed in finding solutions from a new starting point if you numerically integrate the chemical system for a short time (typically under 1 second) before asking it to find the fixed point.

   .. py:method:: setStoich

      (*destination message field*)      Assigns field value.


   .. py:method:: getStoich

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getBadStoichiometry

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIsInitialized

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNIter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStatus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setMaxIter

      (*destination message field*)      Assigns field value.


   .. py:method:: getMaxIter

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setConvergenceCriterion

      (*destination message field*)      Assigns field value.


   .. py:method:: getConvergenceCriterion

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumVarPools

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getRank

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStateType

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNNegEigenvalues

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNPosEigenvalues

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getSolutionStatus

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTotal

      (*destination message field*)      Assigns field value.


   .. py:method:: getTotal

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getEigenvalues

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setupMatrix

      (*destination message field*)      This function initializes and rebuilds the matrices used in the calculation.


   .. py:method:: settle

      (*destination message field*)      Finds the nearest steady state to the current initial conditions. This function rebuilds the entire calculation only if the object has not yet been initialized.


   .. py:method:: resettle

      (*destination message field*)      Finds the nearest steady state to the current initial conditions. This function rebuilds the entire calculation


   .. py:method:: showMatrices

      (*destination message field*)      Utility function to show the matrices derived for the calculations on the reaction system. Shows the Nr, gamma, and total matrices


   .. py:method:: randomInit

      (*destination message field*)      Generate random initial conditions consistent with the massconservation rules. Typically invoked in order to scanstates


   .. py:attribute:: stoich

      Id (*value field*)      Specify the Id of the stoichiometry system to use


   .. py:attribute:: badStoichiometry

      bool (*value field*)      Bool: True if there is a problem with the stoichiometry


   .. py:attribute:: isInitialized

      bool (*value field*)      True if the model has been initialized successfully


   .. py:attribute:: nIter

      unsigned int (*value field*)      Number of iterations done by steady state solver


   .. py:attribute:: status

      string (*value field*)      Status of solver


   .. py:attribute:: maxIter

      unsigned int (*value field*)      Max permissible number of iterations to try before giving up


   .. py:attribute:: convergenceCriterion

      double (*value field*)      Fractional accuracy required to accept convergence


   .. py:attribute:: numVarPools

      unsigned int (*value field*)      Number of variable molecules in reaction system.


   .. py:attribute:: rank

      unsigned int (*value field*)      Number of independent molecules in reaction system


   .. py:attribute:: stateType

      unsigned int (*value field*)      0: stable; 1: unstable; 2: saddle; 3: osc?; 4: one near-zero eigenvalue; 5: other


   .. py:attribute:: nNegEigenvalues

      unsigned int (*value field*)      Number of negative eigenvalues: indicates type of solution


   .. py:attribute:: nPosEigenvalues

      unsigned int (*value field*)      Number of positive eigenvalues: indicates type of solution


   .. py:attribute:: solutionStatus

      unsigned int (*value field*)      0: Good; 1: Failed to find steady states; 2: Failed to find eigenvalues


   .. py:attribute:: total

      unsigned int,double (*lookup field*)      Totals table for conservation laws. The exact mapping ofthis to various sums of molecules is given by the conservation matrix, and is currently a bit opaque.The value of 'total' is set to initial conditions whenthe 'SteadyState::settle' function is called.Assigning values to the total is a special operation:it rescales the concentrations of all the affectedmolecules so that they are at the specified total.This happens the next time 'settle' is called.


   .. py:attribute:: eigenvalues

      unsigned int,double (*lookup field*)      Eigenvalues computed for steady state
