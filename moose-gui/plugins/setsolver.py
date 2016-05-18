import moose

def deleteSolver(modelRoot):
	compts = moose.wildcardFind(modelRoot+'/##[ISA=ChemCompt]')
	for compt in compts:
		if moose.exists(compt.path+'/stoich'):
			st = moose.element(compt.path+'/stoich')
			st_ksolve = st.ksolve
			moose.delete(st)
			if moose.exists((st_ksolve).path):
				moose.delete(st_ksolve)
			
def addSolver(modelRoot,solver):
	compt = moose.wildcardFind(modelRoot+'/##[ISA=ChemCompt]')
	if compt:
		comptinfo = moose.Annotator(moose.element(compt[0]).path+'/info')
		previousSolver = comptinfo.solver
		currentSolver = previousSolver
		if solver == "Gillespie":
			currentSolver = "gssa"
		elif solver == "Runge Kutta":
			currentSolver = "gsl"
		elif solver == "Exponential Euler":
			currentSolver = "ee"

		if previousSolver != currentSolver:
			# if previousSolver != currentSolver
			comptinfo.solver = currentSolver
			if (moose.exists(compt[0].path+'/stoich')):
				# "A: and stoich exists then delete the stoich add solver"
				deleteSolver(modelRoot)
				setCompartmentSolver(modelRoot,currentSolver)
				return True
			else:
				# " B: stoich doesn't exists then addSolver, this is when object is deleted which delete's the solver "
				#  " and solver is also changed, then add addsolver "
				setCompartmentSolver(modelRoot,currentSolver)
				return True
		else:
			if moose.exists(compt[0].path+'/stoich'):
				# " stoich exist, doing nothing"
				return False
			else:
				# "but stoich doesn't exist,this is when object is deleted which deletes the solver
				# " but solver are not changed, then also call addSolver"
				setCompartmentSolver(modelRoot,currentSolver)
				return True
	return False

def setCompartmentSolver(modelRoot,solver):
	compts = moose.wildcardFind(modelRoot+'/##[ISA=ChemCompt]')
	for compt in compts:
		if ( solver == 'gsl' ) or (solver == 'Runge Kutta'):
			ksolve = moose.Ksolve( compt.path+'/ksolve' )
		if ( solver == 'gssa' ) or (solver == 'Gillespie'):
			ksolve = moose.Gsolve( compt.path+'/gsolve' )
		if ( solver != 'ee' ):
			stoich = moose.Stoich( compt.path+'/stoich' )
			stoich.compartment = compt
			stoich.ksolve = ksolve
			if moose.exists(compt.path):
				stoich.path = compt.path+"/##"
	stoichList = moose.wildcardFind(modelRoot+'/##[ISA=Stoich]')
	if len( stoichList ) == 2:
		stoichList[1].buildXreacs( stoichList[0] )
	if len( stoichList ) == 3:
		stoichList[1].buildXreacs (stoichList [0])
		stoichList[1].buildXreacs (stoichList [2])

	for i in stoichList:
		i.filterXreacs()
	
	for x in moose.wildcardFind( modelRoot+'/data/graph#/#' ):
		x.tick = 18