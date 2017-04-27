Clock
-----

.. py:class:: Clock

   Every object scheduled for operations in MOOSE is connected to oneof the 'Tick' entries on the Clock.
   The Clock manages 32 'Ticks', each of which has its own dt,which is an integral multiple of the clock baseDt\_. On every clock step the ticks are examined to see which of themis due for updating. When a tick is updated, the 'process' call of all the objects scheduled on that tick is called. Order of execution: If a subset of ticks are scheduled for execution at a given timestep, then they will be executed in numerical order, lowest tick first and highest last. There is no guarantee of execution order for objects within a clock tick.
   The clock provides default scheduling for all objects which can be accessed using Clock::lookupDefaultTick( className ). Specific items of note are that the output/file dump objects are second-last, and the postmaster is last on the order of Ticks. The clock also starts up with some default timesteps for each of these ticks, and this can be overridden using the shell command setClock, or by directly assigning tickStep values on the clock object.
   Which objects use which tick? As a rule of thumb, try this:
   Electrical/compartmental model calculations: Ticks 0-7
   Tables and output objects for electrical output: Tick 8
   Diffusion solver: Tick 10
   Chemical/compartmental model calculations: Ticks 11-17
   Tables and output objects for chemical output: Tick 18
   Unassigned: Ticks 20-29
   Special: 30-31
   Data output is a bit special, since you may want to store data at different rates for electrical and chemical processes in the same model. Here you will have to specifically assign distinct clock ticks for the tables/fileIO objects handling output at different time-resolutions. Typically one uses tick 8 and 18.
   Here are the detailed mappings of class to tick.
   |	Class				      Tick		dt
   |	DiffAmp				      0		50e-6
   |	Interpol			      0		50e-6
   |	PIDController	      0		50e-6
   |	PulseGen			      0		50e-6
   |	StimulusTable	      0		50e-6
   |	testSched			      0		50e-6
   |	VClamp				      0		50e-6
   |  SynHandlerBase			1		50e-6
   |	SimpleSynHandler		1		50e-6
   |  STDPSynHandler		  1		50e-6
    GraupnerBrunel2012CaPlasticitySynHandler    1		50e-6
    SeqSynHandler		1		50e-6
   	CaConc				1		50e-6
   	CaConcBase			1		50e-6
   	DifShell			1		50e-6
   	MgBlock				1		50e-6
   	Nernst				1		50e-6
   	RandSpike			1		50e-6
   	ChanBase			2		50e-6
   	IntFire				2		50e-6
   	IntFireBase			2		50e-6
   	LIF				2		50e-6
   	QIF				2		50e-6
   	ExIF				2		50e-6
   	AdExIF				2		50e-6
   	AdThreshIF				2		50e-6
   	IzhIF				2		50e-6
   	IzhikevichNrn			2		50e-6
   	SynChan				2		50e-6
   	NMDAChan				2		50e-6
   	GapJunction			2		50e-6
   	HHChannel			2		50e-6
   	HHChannel2D			2		50e-6
   	Leakage				2		50e-6
   	MarkovChannel			2		50e-6
   	MarkovGslSolver			2		50e-6
   	MarkovRateTable			2		50e-6
   	MarkovSolver			2		50e-6
   	MarkovSolverBase		2		50e-6
   	RC				2		50e-6
   	Compartment (init)		3		50e-6
   	CompartmentBase (init )		3		50e-6
   	SymCompartment	(init)		3		50e-6
   	Compartment 			4		50e-6
   	CompartmentBase			4		50e-6
   	SymCompartment			4		50e-6
   	SpikeGen			5		50e-6
   	HSolve				6		50e-6
   	SpikeStats			7		50e-6
   	Table				8		0.1e-3
   	TimeTable			8		0.1e-3
   	Dsolve				10		0.01
   	Adaptor				11		0.1
   	Func				12		0.1
   	Function			12		0.1
   	Arith				12		0.1
   	BufPool				13		0.1
   	Pool				13		0.1
   	PoolBase			13		0.1
   	CplxEnzBase			14		0.1
   	Enz				14		0.1
   	EnzBase				14		0.1
   	MMenz				14		0.1
   	Reac				14		0.1
   	ReacBase			14		0.1
   	Gsolve	(init)			15		0.1
   	Ksolve	(init)			15		0.1
   	Gsolve				16		0.1
   	Ksolve				16		0.1
   	Stats				17		0.1
   	Table2				18		1
   	Streamer			19		10
   	HDF5DataWriter			30		1
   	HDF5WriterBase			30		1
   	NSDFWriter			30		1
          PyRun                           30              1
   	PostMaster			31		0.01

   	Note that the other classes are not scheduled at all.

   .. py:attribute:: clockControl

      void (*shared message field*)      Controls all scheduling aspects of Clock, usually from Shell


   .. py:attribute:: proc0

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc1

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc2

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc3

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc4

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc5

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc6

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc7

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc8

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc9

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc10

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc11

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc12

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc13

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc14

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc15

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc16

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc17

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc18

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc19

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc20

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc21

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc22

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc23

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc24

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc25

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc26

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc27

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc28

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc29

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc30

      void (*shared message field*)      Shared process/reinit message


   .. py:attribute:: proc31

      void (*shared message field*)      Shared process/reinit message


   .. py:method:: setBaseDt

      (*destination message field*)      Assigns field value.


   .. py:method:: getBaseDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getRunTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCurrentTime

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNsteps

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getNumTicks

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getStride

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getCurrentStep

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDts

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getIsRunning

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTickStep

      (*destination message field*)      Assigns field value.


   .. py:method:: getTickStep

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: setTickDt

      (*destination message field*)      Assigns field value.


   .. py:method:: getTickDt

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: getDefaultTick

      (*destination message field*)      Requests field value. The requesting Element must provide a handler for the returned value.


   .. py:method:: start

      (*destination message field*)      Sets off the simulation for the specified duration


   .. py:method:: step

      (*destination message field*)      Sets off the simulation for the specified # of steps. Here each step advances the simulation by the timestep of the smallest tick that is actually in use.


   .. py:method:: stop

      (*destination message field*)      Halts the simulation, with option to restart seamlessly


   .. py:method:: reinit

      (*destination message field*)      Zeroes out all ticks, starts at t = 0


   .. py:attribute:: finished

      void (*source message field*)      Signal for completion of run


   .. py:attribute:: process0

      PK8ProcInfo (*source message field*)      process for Tick 0


   .. py:attribute:: reinit0

      PK8ProcInfo (*source message field*)      reinit for Tick 0


   .. py:attribute:: process1

      PK8ProcInfo (*source message field*)      process for Tick 1


   .. py:attribute:: reinit1

      PK8ProcInfo (*source message field*)      reinit for Tick 1


   .. py:attribute:: process2

      PK8ProcInfo (*source message field*)      process for Tick 2


   .. py:attribute:: reinit2

      PK8ProcInfo (*source message field*)      reinit for Tick 2


   .. py:attribute:: process3

      PK8ProcInfo (*source message field*)      process for Tick 3


   .. py:attribute:: reinit3

      PK8ProcInfo (*source message field*)      reinit for Tick 3


   .. py:attribute:: process4

      PK8ProcInfo (*source message field*)      process for Tick 4


   .. py:attribute:: reinit4

      PK8ProcInfo (*source message field*)      reinit for Tick 4


   .. py:attribute:: process5

      PK8ProcInfo (*source message field*)      process for Tick 5


   .. py:attribute:: reinit5

      PK8ProcInfo (*source message field*)      reinit for Tick 5


   .. py:attribute:: process6

      PK8ProcInfo (*source message field*)      process for Tick 6


   .. py:attribute:: reinit6

      PK8ProcInfo (*source message field*)      reinit for Tick 6


   .. py:attribute:: process7

      PK8ProcInfo (*source message field*)      process for Tick 7


   .. py:attribute:: reinit7

      PK8ProcInfo (*source message field*)      reinit for Tick 7


   .. py:attribute:: process8

      PK8ProcInfo (*source message field*)      process for Tick 8


   .. py:attribute:: reinit8

      PK8ProcInfo (*source message field*)      reinit for Tick 8


   .. py:attribute:: process9

      PK8ProcInfo (*source message field*)      process for Tick 9


   .. py:attribute:: reinit9

      PK8ProcInfo (*source message field*)      reinit for Tick 9


   .. py:attribute:: process10

      PK8ProcInfo (*source message field*)      process for Tick 10


   .. py:attribute:: reinit10

      PK8ProcInfo (*source message field*)      reinit for Tick 10


   .. py:attribute:: process11

      PK8ProcInfo (*source message field*)      process for Tick 11


   .. py:attribute:: reinit11

      PK8ProcInfo (*source message field*)      reinit for Tick 11


   .. py:attribute:: process12

      PK8ProcInfo (*source message field*)      process for Tick 12


   .. py:attribute:: reinit12

      PK8ProcInfo (*source message field*)      reinit for Tick 12


   .. py:attribute:: process13

      PK8ProcInfo (*source message field*)      process for Tick 13


   .. py:attribute:: reinit13

      PK8ProcInfo (*source message field*)      reinit for Tick 13


   .. py:attribute:: process14

      PK8ProcInfo (*source message field*)      process for Tick 14


   .. py:attribute:: reinit14

      PK8ProcInfo (*source message field*)      reinit for Tick 14


   .. py:attribute:: process15

      PK8ProcInfo (*source message field*)      process for Tick 15


   .. py:attribute:: reinit15

      PK8ProcInfo (*source message field*)      reinit for Tick 15


   .. py:attribute:: process16

      PK8ProcInfo (*source message field*)      process for Tick 16


   .. py:attribute:: reinit16

      PK8ProcInfo (*source message field*)      reinit for Tick 16


   .. py:attribute:: process17

      PK8ProcInfo (*source message field*)      process for Tick 17


   .. py:attribute:: reinit17

      PK8ProcInfo (*source message field*)      reinit for Tick 17


   .. py:attribute:: process18

      PK8ProcInfo (*source message field*)      process for Tick 18


   .. py:attribute:: reinit18

      PK8ProcInfo (*source message field*)      reinit for Tick 18


   .. py:attribute:: process19

      PK8ProcInfo (*source message field*)      process for Tick 19


   .. py:attribute:: reinit19

      PK8ProcInfo (*source message field*)      reinit for Tick 19


   .. py:attribute:: process20

      PK8ProcInfo (*source message field*)      process for Tick 20


   .. py:attribute:: reinit20

      PK8ProcInfo (*source message field*)      reinit for Tick 20


   .. py:attribute:: process21

      PK8ProcInfo (*source message field*)      process for Tick 21


   .. py:attribute:: reinit21

      PK8ProcInfo (*source message field*)      reinit for Tick 21


   .. py:attribute:: process22

      PK8ProcInfo (*source message field*)      process for Tick 22


   .. py:attribute:: reinit22

      PK8ProcInfo (*source message field*)      reinit for Tick 22


   .. py:attribute:: process23

      PK8ProcInfo (*source message field*)      process for Tick 23


   .. py:attribute:: reinit23

      PK8ProcInfo (*source message field*)      reinit for Tick 23


   .. py:attribute:: process24

      PK8ProcInfo (*source message field*)      process for Tick 24


   .. py:attribute:: reinit24

      PK8ProcInfo (*source message field*)      reinit for Tick 24


   .. py:attribute:: process25

      PK8ProcInfo (*source message field*)      process for Tick 25


   .. py:attribute:: reinit25

      PK8ProcInfo (*source message field*)      reinit for Tick 25


   .. py:attribute:: process26

      PK8ProcInfo (*source message field*)      process for Tick 26


   .. py:attribute:: reinit26

      PK8ProcInfo (*source message field*)      reinit for Tick 26


   .. py:attribute:: process27

      PK8ProcInfo (*source message field*)      process for Tick 27


   .. py:attribute:: reinit27

      PK8ProcInfo (*source message field*)      reinit for Tick 27


   .. py:attribute:: process28

      PK8ProcInfo (*source message field*)      process for Tick 28


   .. py:attribute:: reinit28

      PK8ProcInfo (*source message field*)      reinit for Tick 28


   .. py:attribute:: process29

      PK8ProcInfo (*source message field*)      process for Tick 29


   .. py:attribute:: reinit29

      PK8ProcInfo (*source message field*)      reinit for Tick 29


   .. py:attribute:: process30

      PK8ProcInfo (*source message field*)      process for Tick 30


   .. py:attribute:: reinit30

      PK8ProcInfo (*source message field*)      reinit for Tick 30


   .. py:attribute:: process31

      PK8ProcInfo (*source message field*)      process for Tick 31


   .. py:attribute:: reinit31

      PK8ProcInfo (*source message field*)      reinit for Tick 31


   .. py:attribute:: baseDt

      double (*value field*)      Base timestep for simulation. This is the smallest dt out of all the clock ticks. By definition all other timesteps are integral multiples of this, and are rounded to ensure that this is the case .


   .. py:attribute:: runTime

      double (*value field*)      Duration to run the simulation


   .. py:attribute:: currentTime

      double (*value field*)      Current simulation time


   .. py:attribute:: nsteps

      unsigned long (*value field*)      Number of steps to advance the simulation, in units of the smallest timestep on the clock ticks


   .. py:attribute:: numTicks

      unsigned int (*value field*)      Number of clock ticks


   .. py:attribute:: stride

      unsigned int (*value field*)      Number by which the simulation advances the current step on each cycle. stride = smallest active timestep/smallest defined timestep.


   .. py:attribute:: currentStep

      unsigned long (*value field*)      Current simulation step


   .. py:attribute:: dts

      vector<double> (*value field*)      Utility function returning the dt (timestep) of all ticks.


   .. py:attribute:: isRunning

      bool (*value field*)      Utility function to report if simulation is in progress.


   .. py:attribute:: tickStep

      unsigned int,unsigned int (*lookup field*)      Step size of specified Tick, as integral multiple of dt\_ A zero step size means that the Tick is inactive


   .. py:attribute:: tickDt

      unsigned int,double (*lookup field*)      Timestep dt of specified Tick. Always integral multiple of dt\_. If you assign a non-integer multiple it will round off.  A zero timestep means that the Tick is inactive


   .. py:attribute:: defaultTick

      string,unsigned int (*lookup field*)      Looks up the default Tick to use for the specified class. If no tick is assigned, as for classes without a process operation or zombie classes, the tick is ~0U. If nothing can be found returns 0 and emits a warning.
