import moose
print( 'Using moose from %s' % moose.__file__)

modelRoot = moose.Neutral('/model')
compt = moose.CubeMesh('/model/kinetics')
p1 = moose.BufPool('/model/kinetics/Ca')
function = moose.Function('/model/kinetics/Ca/func')
#function out to pool
moose.connect( function, 'valueOut', p1 ,'setConc' )
#input pools to be connected to fuction
S1 = moose.BufPool( '/model/kinetics/s1' )
S2 = moose.BufPool( '/model/kinetics/s2' )

initiexpr = function.expr
#S1 connected to function
numVariables = function.numVars
expr = ""
expr = (function.expr+'+'+'x'+str(numVariables))
expr = expr.lstrip("0 +")
expr = expr.replace(" ","")
function.expr = expr
s1msg = moose.connect( S1, 'nOut', function.x[numVariables], 'input' )
#S2 connected to function
numVariables = function.numVars
expr = ""
expr = (function.expr+'+'+'x'+str(numVariables))
expr = expr.lstrip("0 +")
expr = expr.replace(" ","")
function.expr = expr
s2msg = moose.connect( S2, 'nOut', function.x[numVariables], 'input' )

compts = moose.wildcardFind(modelRoot.path+'/##[ISA=ChemCompt]')
solver = 'gsl'
moose.mooseAddChemSolver('/model',solver)
moose.mooseDeleteChemSolver('/model')

#moose.delete(s1msg)
#moose.delete(s2msg)
#deleting the function
#moose.delete(function) #deleting this will not cause seg fault
print(function.expr) #function's expr still has the expression which is causing the problem
