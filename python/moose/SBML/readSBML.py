'''
*******************************************************************
 * File:            readSBML.py
 * Description:
 * Author:          HarshaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2016 Upinder S. Bhalla. and NCBS
Created : Thu May 12 10:19:00 2016(+0530)
Version 
Last-Updated:
		  By:
**********************************************************************/
/****************************

'''

import sys
import os.path
import collections
from moose import *
import libsbml

'''
   TODO in
    -Compartment
      --Need to add group
      --Need to deal with compartment outside
    -Molecule
      -- Need to add group 
      -- mathML only AssisgmentRule is taken partly I have checked addition and multiplication, 
       --, need to do for other calculation.
       -- In Assisgment rule one of the variable is a function, in moose since assignment is done using function,
          function can't get input from another function (model 000740 in l3v1)
    -Loading Model from SBML
      --Tested 1-30 testcase example model provided by l3v1 and l2v4 std.
        ---These are the models that worked (sbml testcase)1-6,10,14-15,17-21,23-25,34,35,58
	---Need to check
	 	----what to do when boundarycondition is true i.e.,
             differential equation derived from the reaction definitions
             should not be calculated for the species(7-9,11-13,16)
        	 ----kineticsLaw, Math fun has fraction,ceiling,reminder,power 28etc.
         	----Events to be added 26
	 	----initial Assisgment for compartment 27
        	 ----when stoichiometry is rational number 22
	 	---- For Michaelis Menten kinetics km is not defined which is most of the case need to calculate
'''

def mooseReadSBML(filepath,loadpath):
	print " filepath ",filepath
	try:
		filep = open(filepath, "r")
		document = libsbml.readSBML(filepath)
		num_errors = document.getNumErrors()
		if ( num_errors > 0 ):
			print("Encountered the following SBML errors:" );
			document.printErrors();
			return moose.element('/');
		else:
			level = document.getLevel();
			version = document.getVersion();
			print("\n" + "File: " + filepath + " (Level " + str(level) + ", version " + str(version) + ")" );
			model = document.getModel();
			if (model == None):
				print("No model present." );
				return moose.element('/');
			else:
				print " model ",model
				print("functionDefinitions: " + str(model.getNumFunctionDefinitions()) );
				print("    unitDefinitions: " + str(model.getNumUnitDefinitions()) );
				print("   compartmentTypes: " + str(model.getNumCompartmentTypes()) );
				print("        specieTypes: " + str(model.getNumSpeciesTypes()) );
				print("       compartments: " + str(model.getNumCompartments()) );
				print("            species: " + str(model.getNumSpecies()) );
				print("         parameters: " + str(model.getNumParameters()) );
				print(" initialAssignments: " + str(model.getNumInitialAssignments()) );
				print("              rules: " + str(model.getNumRules()) );
				print("        constraints: " + str(model.getNumConstraints()) );
				print("          reactions: " + str(model.getNumReactions()) );
				print("             events: " + str(model.getNumEvents()) );
				print("\n");

				if (model.getNumCompartments() == 0):
					return moose.element('/')
				else:
					baseId = moose.Neutral(loadpath)
					#All the model will be created under model as a thumbrule
					basePath = moose.Neutral(baseId.path+'/model')
					#Map Compartment's SBML id as key and value is list of[ Moose ID and SpatialDimensions ]
					comptSbmlidMooseIdMap = {}
					print ": ",basePath.path
					globparameterIdValue = {}
					modelAnnotaInfo = {}
					mapParameter(model,globparameterIdValue)
					errorFlag = createCompartment(basePath,model,comptSbmlidMooseIdMap)
					if errorFlag:
						specInfoMap = {}
						errorFlag = createSpecies(basePath,model,comptSbmlidMooseIdMap,specInfoMap)
						if errorFlag:
							errorFlag = createRules(model,specInfoMap,globparameterIdValue)
							if errorFlag:
								errorFlag = createReaction(model,specInfoMap,modelAnnotaInfo)
					if not errorFlag:
						print " errorFlag ",errorFlag
						#Any time in the middle if SBML does not read then I delete everything from model level
						#This is important as while reading in GUI the model will show up untill built which is not correct
						print "Deleted rest of the model"
						moose.delete(basePath)
				return baseId;


	except IOError:
		print "File " ,filepath ," does not exist."
		return moose.element('/')
def setupEnzymaticReaction(enz,groupName,enzName,specInfoMap,modelAnnotaInfo):
	enzPool = (modelAnnotaInfo[groupName]["enzyme"])
	enzParent = specInfoMap[enzPool]["Mpath"]
	cplx = (modelAnnotaInfo[groupName]["complex"])
	complx = moose.element(specInfoMap[cplx]["Mpath"].path)
	
	enzyme_ = moose.Enz(enzParent.path+'/'+enzName)
	moose.move(complx,enzyme_)
	moose.connect(enzyme_,"cplx",complx,"reac")
	moose.connect(enzyme_,"enz",enzParent,"reac");

	sublist = (modelAnnotaInfo[groupName]["substrate"])
	prdlist = (modelAnnotaInfo[groupName]["product"])

	for si in range(0,len(sublist)):
		sl = sublist[si]
		mSId =specInfoMap[sl]["Mpath"]
		moose.connect(enzyme_,"sub",mSId,"reac")

	for pi in range(0,len(prdlist)):
		pl = prdlist[pi]
		mPId = specInfoMap[pl]["Mpath"]
		moose.connect(enzyme_,"prd",mPId,"reac")
	
	if (enz.isSetNotes):
		pullnotes(enz,enzyme_)

def addSubPrd(reac,reName,type,reactSBMLIdMooseId,specInfoMap):
	rctMapIter = {}

	if (type == "sub"):
		noplusStoichsub = 0
		addSubinfo = collections.OrderedDict()
		for rt in range(0,reac.getNumReactants()):
			rct = reac.getReactant(rt)
			sp = rct.getSpecies()
			rctMapIter[sp] = rct.getStoichiometry()
			noplusStoichsub = noplusStoichsub+rct.getStoichiometry()
		for key,value in rctMapIter.items():
			src = specInfoMap[key]["Mpath"]
			des = reactSBMLIdMooseId[reName]["MooseId"]
			for s in range(0,int(value)):
				moose.connect(des, 'sub', src, 'reac', 'OneToOne')
		addSubinfo = {"nSub" :noplusStoichsub}
		reactSBMLIdMooseId[reName].update(addSubinfo)

	else:
		noplusStoichprd = 0
		addPrdinfo = collections.OrderedDict()
		for rt in range(0,reac.getNumProducts()):
			rct = reac.getProduct(rt)
			sp = rct.getSpecies()
			rctMapIter[sp] = rct.getStoichiometry()
			noplusStoichprd = noplusStoichprd+rct.getStoichiometry()
		
		for key,values in rctMapIter.items():
			#src ReacBase
			src = reactSBMLIdMooseId[reName]["MooseId"]
			des = specInfoMap[key]["Mpath"]
			for i in range(0,int(values)):
				moose.connect(src, 'prd', des, 'reac', 'OneToOne')
		addPrdinfo = {"nPrd": noplusStoichprd}
		reactSBMLIdMooseId[reName].update(addPrdinfo)

def populatedict(annoDict,label,value):
	if annoDict.has_key(label):
		annoDict.setdefault(label,[])
		annoDict[label].update({value})
	else:
		annoDict[label]= {value}

def getModelAnnotation(obj,modelAnnotaInfo):
	name = obj.getId()
	name = name.replace(" ","_space_")
	#modelAnnotaInfo= {}
	annotateMap = {}
	if (obj.getAnnotation() != None):
		annoNode = obj.getAnnotation()
		for ch in range(0,annoNode.getNumChildren()):
			childNode = annoNode.getChild(ch)
			if (childNode.getPrefix() == "moose" and childNode.getName() == "EnzymaticReaction"):
				sublist = []
				for gch in range(0,childNode.getNumChildren()):
					grandChildNode = childNode.getChild(gch)
					nodeName = grandChildNode.getName()
					nodeValue = ""
					if (grandChildNode.getNumChildren() == 1):
						nodeValue = grandChildNode.getChild(0).toXMLString()
					else:
						print "Error: expected exactly ONE child of ", nodeName
					
					if nodeName == "enzyme":
						populatedict(annotateMap,"enzyme",nodeValue)
	
					elif nodeName == "complex":
						populatedict(annotateMap,"complex" ,nodeValue)
					elif ( nodeName == "substrates"):
						populatedict(annotateMap,"substrates" ,nodeValue)
					elif ( nodeName == "product" ):
						populatedict(annotateMap,"product" ,nodeValue)
					elif ( nodeName == "groupName" ):
						populatedict(annotateMap,"grpName" ,nodeValue)
					elif ( nodeName == "stage" ):
						populatedict(annotateMap,"stage" ,nodeValue)
					elif ( nodeName == "Group" ):
						populatedict(annotateMap,"group" ,nodeValue)
					elif ( nodeName == "xCord" ):
						populatedict(annotateMap,"xCord" ,nodeValue)
					elif ( nodeName == "yCord" ):
						populatedict(annotateMap,"yCord" ,nodeValue)
	groupName = ""
	if annotateMap.has_key('grpName'):
		groupName = list(annotateMap["grpName"])[0]
		if list(annotateMap["stage"])[0] == '1':
			if modelAnnotaInfo.has_key(groupName):
				modelAnnotaInfo[groupName].update	(
					{"enzyme" : list(annotateMap["enzyme"])[0],
					"stage" : list(annotateMap["stage"])[0],
					"substrate" : list(annotateMap["substrates"])
					}
				)
			else:
				modelAnnotaInfo[groupName]= {
					"enzyme" : list(annotateMap["enzyme"])[0],
					"stage" : list(annotateMap["stage"])[0],
					"substrate" : list(annotateMap["substrates"])
					#"group" : list(annotateMap["Group"])[0],
					#"xCord" : list(annotateMap["xCord"])[0],
					#"yCord" : list(annotateMap["yCord"]) [0]
					}

		elif list(annotateMap["stage"])[0] == '2':
			if modelAnnotaInfo.has_key(groupName):
				stage = int(modelAnnotaInfo[groupName]["stage"])+int(list(annotateMap["stage"])[0])
				modelAnnotaInfo[groupName].update (
					{"complex" : list(annotateMap["complex"])[0],
					"product" : list(annotateMap["product"]),
					"stage" : [stage]
					}
				)
			else:
				modelAnnotaInfo[groupName]= {
					"complex" : list(annotateMap["complex"])[0],
					"product" : list(annotateMap["product"]),
					"stage" : [stage]
					}
	return(groupName)


def createReaction(model,specInfoMap,modelAnnotaInfo):
	# print " reaction "
	# Things done for reaction
	# --Reaction is not created, if substrate and product is missing
	# --Reaction is created under first substrate's compartment if substrate not found then product
	# --Reaction is created if substrate or product is missing, but while run time in GUI atleast I have stopped
	#ToDo
	# -- I need to check here if any substance/product is if ( constant == true && bcondition == false)
    # cout <<"The species "<< name << " should not appear in reactant or product as per sbml Rules"<< endl;

	errorFlag = True
	reactSBMLIdMooseId = {}

	for ritem in range(0,model.getNumReactions()):
		reactionCreated = False
		groupName = ""
		reac = model.getReaction( ritem )
		if ( reac.isSetId() ):
			rId = reac.getId()
		if ( reac.isSetName() ):
			rName = reac.getName()
			rName = rName.replace(" ","_space_")
		if not( rName ):
			rName = rId
		rev = reac.getReversible()
		fast = reac.getFast()
		if ( fast ):
			print " warning: for now fast attribute is not handled \"", rName,"\""
		if (reac.getAnnotation() != None):
			groupName = getModelAnnotation(reac,modelAnnotaInfo)
			
		if (groupName != "" and list(modelAnnotaInfo[groupName]["stage"])[0] == 3):
			setupEnzymaticReaction(reac,groupName,rName,specInfoMap,modelAnnotaInfo)

		elif(groupName == ""):
			numRcts = reac.getNumReactants()
			numPdts = reac.getNumProducts()
			nummodifiers = reac.getNumModifiers()
			
			if not (numRcts and numPdts):
				print rName," : Substrate and Product is missing, we will be skiping creating this reaction in MOOSE"
			
			elif (reac.getNumModifiers() > 0):
				reactionCreated = setupMMEnzymeReaction(reac,rName,specInfoMap,reactSBMLIdMooseId)
				print " reactionCreated after enz ",reactionCreated

			elif (numRcts):
				# In moose, reactions compartment are decided from first Substrate compartment info
				# substrate is missing then check for product
				if (reac.getNumReactants()):
					react = reac.getReactant(0)
					sp = react.getSpecies()
					speCompt = specInfoMap[sp]["comptId"].path
					reaction_ = moose.Reac(speCompt+'/'+rName)
					reactionCreated = True
					reactSBMLIdMooseId[rName] = {"MooseId" : reaction_ , "className ": "reaction"}
			elif (numPdts):
				# In moose, reactions compartment are decided from first Substrate compartment info
				# substrate is missing then check for product
				if (reac.getNumProducts()):
					react = reac.getProducts(0)
					sp = react.getSpecies()
					speCompt = specInfoMap[sp]["comptId"].path
					reaction_ = moose.Reac(speCompt+'/'+rName)
					reactionCreated = True
					reactSBMLIdMooseId[rName] = {"MooseId":reaction_}

			if reactionCreated:
				if (reac.isSetNotes):
					pullnotes(reac,reaction_)
				addSubPrd(reac,rName,"sub",reactSBMLIdMooseId,specInfoMap)
				addSubPrd(reac,rName,"prd",reactSBMLIdMooseId,specInfoMap)
	# print "react ",reactSBMLIdMooseId
	return errorFlag

def getMembers(node,ruleMemlist):
	if node.getType() == libsbml.AST_PLUS:
		if node.getNumChildren() == 0:
			print ("0")
			return
		getMembers(node.getChild(0),ruleMemlist)
		for i in range(1,node.getNumChildren()):
			# addition
			getMembers(node.getChild(i),ruleMemlist)
	elif node.getType() == libsbml.AST_REAL:
		#This will be constant
		pass
	elif node.getType() == libsbml.AST_NAME:
		#This will be the ci term"
		ruleMemlist.append(node.getName())

	elif node.getType() == libsbml.AST_TIMES:
		if node.getNumChildren() == 0:
			print ("0")
			return
		getMembers(node.getChild(0),ruleMemlist)
		for i in range(1,node.getNumChildren()):
			# Multiplication
			getMembers(node.getChild(i),ruleMemlist)
	else:
		print " this case need to be handled"

def createRules(model,specInfoMap,globparameterIdValue):
	for r in range(0,model.getNumRules()):
			rule = model.getRule(r)
			if (rule.isAssignment()):
				rule_variable = rule.getVariable();
				poolList = specInfoMap[rule_variable]["Mpath"].path
				funcId = moose.Function(poolList+'/func')
				moose.connect( funcId, 'valueOut', poolList ,'setN' )
				ruleMath = rule.getMath()
				ruleMemlist = []
				speFunXterm = {}
				getMembers(ruleMath,ruleMemlist)
				for i in ruleMemlist:
					if (specInfoMap.has_key(i)):
						specMapList = specInfoMap[i]["Mpath"]
						numVars = funcId.numVars
						x = funcId.path+'/x['+str(numVars)+']'
						speFunXterm[i] = 'x'+str(numVars)
						moose.connect(specMapList , 'nOut', x, 'input' )
						funcId.numVars = numVars +1
					elif not(globparameterIdValue.has_key(i)):
						print "check the variable type ",i

				exp = rule.getFormula()
				for mem in ruleMemlist:
					if ( specInfoMap.has_key(mem)):
						exp1 = exp.replace(mem,str(speFunXterm[mem]))
						exp = exp1
					elif( globparameterIdValue.has_key(mem)):
						exp1 = exp.replace(mem,str(globparameterIdValue[mem]))
						exp = exp1
					else:
						print "Math expression need to be checked"
				funcId.expr = exp.strip(" \t\n\r")
				return True

			elif( rule.isRate() ):
				print "Warning : For now this \"",rule.getVariable(), "\" rate Rule is not handled in moose "
				return False

			elif ( rule.isAlgebraic() ):
				print "Warning: For now this " ,rule.getVariable()," Algebraic Rule is not handled in moose"
				return False
	return True

def pullnotes(sbmlId,mooseId):
	if sbmlId.getNotes() != None:
		tnodec = ((sbmlId.getNotes()).getChild(0)).getChild(0)
		notes = tnodec.getCharacters()
		notes = notes.strip(' \t\n\r')
		objPath = mooseId.path+"/info"
		if not moose.exists(objPath):
			objInfo = moose.Annotator(mooseId.path+'/info')
		else:
			objInfo = moose.element(mooseId.path+'/info')
		objInfo.notes = notes

def createSpecies(basePath,model,comptSbmlidMooseIdMap,specInfoMap):
	# ToDo:
	# - Need to add group name if exist in pool
	# - Notes
	# print "species "
	if not 	(model.getNumSpecies()):
		return False
	else:
		for sindex in range(0,model.getNumSpecies()):
			spe = model.getSpecies(sindex)
			sName = None
			sId = spe.getId()

			if spe.isSetName():
				sName = spe.getName()
				sName = sName.replace(" ","_space_")

			if spe.isSetCompartment():
				comptId = spe.getCompartment()

			if not( sName ):
				sName = sId

			constant = spe.getConstant()
			boundaryCondition = spe.getBoundaryCondition()
			comptEl = comptSbmlidMooseIdMap[comptId]["MooseId"].path
			hasonlySubUnit = spe.getHasOnlySubstanceUnits();
			# "false": is {unit of amount}/{unit of size} (i.e., concentration or density). 
			# "true": then the value is interpreted as having a unit of amount only.

			if (boundaryCondition):
				poolId = moose.BufPool(comptEl+'/'+sName)
			else:
				poolId = moose.Pool(comptEl+'/'+sName)
			
			if (spe.isSetNotes):
				pullnotes(spe,poolId)
					
			specInfoMap[sId] = {"Mpath" : poolId, "const" : constant, "bcondition" : boundaryCondition, "hassubunit" : hasonlySubUnit, "comptId" : comptSbmlidMooseIdMap[comptId]["MooseId"]}
			initvalue = 0.0
			unitfactor,unitset,unittype = transformUnit(spe,hasonlySubUnit)
			if hasonlySubUnit == True:
				if spe.isSetInitialAmount():
					initvalue = spe.getInitialAmount()
					# populating nInit, will automatically calculate the concInit.
					if not (unitset):
						# if unit is not set,
						# default unit is assumed as Mole in SBML
						unitfactor = pow(6.0221409e23,1)
						unittype = "Mole"

					initvalue = initvalue * unitfactor
				elif spe.isSetInitialConcentration():
					initvalue = spe.getInitialConcentration()
					print " Since hasonlySubUnit is true and concentration is set units are not checked"
				poolId.nInit = initvalue

			elif hasonlySubUnit == False:
				#ToDo : check 00976
				if spe.isSetInitialAmount():
					initvalue = spe.getInitialAmount()
					#initAmount is set we need to convert to concentration
					initvalue = initvalue / comptSbmlidMooseIdMap[comptId]["size"]

				elif spe.isSetInitialConcentration():
					initvalue = spe.getInitialConcentration()
				if not unitset:
					#print " unit is not set"
					unitfactor  = power(10,-3)

				initvalue = initvalue * unitfactor
				poolId.concInit = initvalue
			else:
				nr = model.getNumRules()
				found = False
				for nrItem in range(0,nr):
					rule = model.getRule(nrItem)
					assignRule = rule.isAssignment()
					if ( assignRule ):
						rule_variable = rule.getVariable()
						if (rule_variable == sId):
							found = True
							break
				if not (found):
					print "Invalid SBML: Either initialConcentration or initialAmount must be set or it should be found in assignmentRule but non happening for ",sName
					return False	
	return True

def transformUnit(unitForObject,hasonlySubUnit=False):
	#print "unit ",UnitDefinition.printUnits(unitForObject.getDerivedUnitDefinition())
	unitset = False
	unittype = None
	if (unitForObject.getDerivedUnitDefinition()):
		unit = (unitForObject.getDerivedUnitDefinition())
		unitnumber = int(unit.getNumUnits())
		if unitnumber > 0:
			for ui in range(0,unit.getNumUnits()):
				lvalue = 1.0			
				unitType =  unit.getUnit(ui)
				if( unitType.isLitre()):
					exponent = unitType.getExponent()
					multiplier = unitType.getMultiplier()
					scale = unitType.getScale()
					offset = unitType.getOffset()
					#units for compartment is Litre but MOOSE compartment is m3
					scale = scale-3
					lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
					unitset = True
					unittype = "Litre"

				elif( unitType.isMole()):
					exponent = unitType.getExponent()
					multiplier = unitType.getMultiplier()
					scale = unitType.getScale()
					offset = unitType.getOffset()
					#if hasOnlySubstanceUnit = True, then assuming Amount
					if hasonlySubUnit == True:
						lvalue *= pow(multiplier * pow(10.0,scale),exponent) + offset
						#If SBML units are in mole then convert to number by multiplying with avogadro's number
						lvalue = lvalue * pow(6.0221409e23,1)

					elif hasonlySubUnit == False: 
						#Pool units are in mM, so to scale adding +3 to convert to m
						lvalue *= pow( multiplier * pow(10.0,scale+3), exponent ) + offset;
					unitset = True
					unittype = "Mole"
		
				elif( unitType.isItem()):
					exponent = unitType.getExponent()
					multiplier = unitType.getMultiplier()
					scale = unitType.getScale()
					offset = unitType.getOffset()
					#if hasOnlySubstanceUnit = True, then assuming Amount
					if hasonlySubUnit == True:
						#If SBML units are in Item then amount is populate as its
						lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
					if hasonlySubUnit == False:
						# hasonlySubUnit is False, which is assumed concentration, 
						# Here Item is converted to mole by dividing by avogadro and at initiavalue divided by volume"
						lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
						lvalue = lvalue/pow(6.0221409e23,1)
					unitset = True
					unittype = "Item"
		else:
			lvalue = 1.0
		print " end of the func lvaue ",lvalue
	return (lvalue,unitset,unittype)
def createCompartment(basePath,model,comptSbmlidMooseIdMap):
	#ToDoList : Check what should be done for the spaitialdimension is 2 or 1, area or length
	if not(model.getNumCompartments()):
		return False
	else:
		for c in range(0,model.getNumCompartments()):
			compt = model.getCompartment(c)
			# print("Compartment " + str(c) + ": "+ UnitDefinition.printUnits(compt.getDerivedUnitDefinition()))
			msize = 0.0
			unitfactor = 1.0
			sbmlCmptId = None
			name = None

			if ( compt.isSetId() ):
				sbmlCmptId = compt.getId()
				
			if ( compt.isSetName() ):
				name = compt.getName()
				name = name.replace(" ","_space")
					
			if ( compt.isSetOutside() ):
				outside = compt.getOutside()
					
			if ( compt.isSetSize() ):
				msize = compt.getSize()
				if msize == 1:
					print "Compartment size is 1"

			dimension = compt.getSpatialDimensions();
			if dimension == 3:
				unitfactor,unitset, unittype = transformUnit(compt)
				
			else:
				print " Currently we don't deal with spatial Dimension less than 3 and unit's area or length"
				return False

			if not( name ):
				name = sbmlCmptId
			
			mooseCmptId = moose.CubeMesh(basePath.path+'/'+name)
			mooseCmptId.volume = (msize*unitfactor)
			comptSbmlidMooseIdMap[sbmlCmptId]={"MooseId": mooseCmptId, "spatialDim":dimension, "size" : msize}
	return True
def mapParameter(model,globparameterIdValue):
	for pm in range(0,model.getNumParameters()):
		prm = model.getParameter( pm );
		if ( prm.isSetId() ):
			parid = prm.getId()
		value = 0.0;
		if ( prm.isSetValue() ):
			value = prm.getValue()
		globparameterIdValue[parid] = value

if __name__ == "__main__":
	
	filepath = sys.argv[1]
	path = sys.argv[2]
	
	f = open(filepath, 'r')
	
	if path == '':
		loadpath = filepath[filepath.rfind('/'):filepath.find('.')]
	else:
		loadpath = path
	
	read = mooseReadSBML(filepath,loadpath)
	if read:
		print " Read to path",loadpath
	else:
		print " could not read  SBML to MOOSE"
