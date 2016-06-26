'''
*******************************************************************
 * File:            writeSBML.py
 * Description:
 * Author:          HarshaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2016 Upinder S. Bhalla. and NCBS
Created : Friday May 27 12:19:00 2016(+0530)
Version 
Last-Updated:
		  By:
**********************************************************************/
/****************************

'''
from moose import *
from libsbml import *
import re
from collections import Counter
#from moose import wildcardFind, element, loadModel, ChemCompt, exists, Annotator, Pool, ZombiePool,PoolBase,CplxEnzBase,Function,ZombieFunction

#ToDo:
#	Table should be written
#	Group's should be added
#   x and y cordinates shd be added if exist

def mooseWriteSBML(modelpath,filename):
	sbmlDoc = SBMLDocument(3, 1)
	filepath,filenameExt = os.path.split(filename)
	if filenameExt.find('.') != -1:
		filename = filenameExt[:filenameExt.find('.')]
	else:
		filename = filenameExt
	
	#validatemodel
	sbmlOk = False
	global spe_constTrue
	spe_constTrue = []
	global nameList_
	nameList_ = []

	xmlns = XMLNamespaces()
	xmlns.add("http://www.sbml.org/sbml/level3/version1")
	xmlns.add("http://www.moose.ncbs.res.in","moose")
	xmlns.add("http://www.w3.org/1999/xhtml","xhtml")
	sbmlDoc.setNamespaces(xmlns)
	cremodel_ = sbmlDoc.createModel()
	cremodel_.setId(filename)
	cremodel_.setTimeUnits("second")
	cremodel_.setExtentUnits("substance")
	cremodel_.setSubstanceUnits("substance")
	
	writeUnits(cremodel_)
	modelAnno = writeSimulationAnnotation(modelpath)
	if modelAnno:
		cremodel_.setAnnotation(modelAnno)
	compartexist = writeCompt(modelpath,cremodel_)
	species = writeSpecies(modelpath,cremodel_,sbmlDoc)
	if species:
		writeFunc(modelpath,cremodel_)
	writeReac(modelpath,cremodel_)
	writeEnz(modelpath,cremodel_)

	consistencyMessages = ""
	SBMLok = validateModel( sbmlDoc )
	if ( SBMLok ):
		#filepath = '/home/harsha/Trash/python'
		#SBMLString = writeSBMLToString(sbmlDoc)
		writeTofile = filepath+"/"+filename+'.xml'
		writeSBMLToFile( sbmlDoc, writeTofile)
		return True,consistencyMessages

	if ( not SBMLok ):
		cerr << "Errors encountered " << endl;
		return -1,consistencyMessages

def writeEnz(modelpath,cremodel_):
	for enz in wildcardFind(modelpath+'/##[ISA=EnzBase]'):
		enzannoexist = False
		enzGpname = " "
		cleanEnzname = convertSpecialChar(enz.name) 
		enzSubt = ()        

		if moose.exists(enz.path+'/info'):
			Anno = moose.Annotator(enz.path+'/info')
			notesE = Anno.notes
			element = moose.element(enz)
			ele = getGroupinfo(element)
			if ele.className == "Neutral":
				enzGpname = "<moose:Group> "+ ele.name + " </moose:Group>\n"
				enzannoexist = True

		if (enz.className == "Enz" or enz.className == "ZombieEnz"):
			enzyme = cremodel_.createReaction()
			if notesE != "":
				cleanNotesE= convertNotesSpecialChar(notesE)
				notesStringE = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t"+ cleanNotesE + "\n\t </body>"
				enzyme.setNotes(notesStringE)
			enzyme.setId(str(idBeginWith(cleanEnzname+"_"+str(enz.getId().value)+"_"+str(enz.getDataIndex())+"_"+"Complex_formation_")))
			enzyme.setName(cleanEnzname)
			enzyme.setFast ( False )
			enzyme.setReversible( True)
			k1 = enz.k1
			k2 = enz.k2
			k3 = enz.k3

			enzAnno ="<moose:EnzymaticReaction>\n"
			if enzannoexist:
				enzAnno=enzAnno + enzGpname
			enzOut = enz.neighbors["enzOut"]
			
			if not enzOut:
				print " Enzyme parent missing for ",enz.name
			else:
				listofname(enzOut,True)
				enzSubt = enzOut
				for i in range(0,len(nameList_)):
					enzAnno=enzAnno+"<moose:enzyme>"+nameList_[i]+"</moose:enzyme>\n"
			#noofSub,sRateLaw = getSubprd(cremodel_,True,"sub",enzSub)
			#for i in range(0,len(nameList_)):
			#    enzAnno=enzAnno+"<moose:enzyme>"+nameList_[i]+"</moose:enzyme>\n"
			#rec_order  = noofSub
			#rate_law = "k1"+"*"+sRateLaw
			
			enzSub = enz.neighbors["sub"]
			if not enzSub:
				print "Enzyme \"",enz.name,"\" substrate missing"
			else:
				listofname(enzSub,True)
				enzSubt += enzSub
				for i in range(0,len(nameList_)):
					enzAnno= enzAnno+"<moose:substrates>"+nameList_[i]+"</moose:substrates>\n"
			if enzSubt:    
				rate_law = "k1"
				noofSub,sRateLaw = getSubprd(cremodel_,True,"sub",enzSubt)
				#rec_order = rec_order + noofSub
				rec_order = noofSub
				rate_law = rate_law +"*"+sRateLaw
				
		   

			enzPrd = enz.neighbors["cplxDest"]
			if not enzPrd:
				print "Enzyme \"",enz.name,"\"product missing"
			else:
				noofPrd,sRateLaw = getSubprd(cremodel_,True,"prd",enzPrd)
				for i in range(0,len(nameList_)):
					enzAnno= enzAnno+"<moose:product>"+nameList_[i]+"</moose:product>\n"
				rate_law = rate_law+ " - "+"k2"+'*'+sRateLaw 
			
			prd_order = noofPrd
			enzAnno = enzAnno + "<moose:groupName>" + cleanEnzname + "_" + str(enz.getId().value) + "_" + str(enz.getDataIndex()) + "_" + "</moose:groupName>\n"
			enzAnno = enzAnno+"<moose:stage>1</moose:stage>\n"
			enzAnno = enzAnno+ "</moose:EnzymaticReaction>"
			enzyme.setAnnotation(enzAnno)
			kl = enzyme.createKineticLaw()
			kl.setFormula( rate_law )
			punit = parmUnit( prd_order-1, cremodel_ )
			printParameters( kl,"k2",k2,punit ) 
			
			unit = parmUnit( rec_order-1, cremodel_)
			printParameters( kl,"k1",k1,unit ) 
			enzyme = cremodel_.createReaction()
			enzyme.setId(str(idBeginWith(cleanEnzname+"_"+str(enz.getId().value)+"_"+str(enz.getDataIndex())+"_"+"Product_formation_")))
			enzyme.setName(cleanEnzname)
			enzyme.setFast ( False )
			enzyme.setReversible( False )
			enzAnno2 = "<moose:EnzymaticReaction>"
			
			enzSub = enz.neighbors["cplxDest"]
			if not enzSub:
				print " complex missing from ",enz.name
			else:
				noofSub,sRateLaw = getSubprd(cremodel_,True,"sub",enzSub)
				for i in range(0,len(nameList_)):
					enzAnno2 = enzAnno2+"<moose:complex>"+nameList_[i]+"</moose:complex>\n"

			enzEnz = enz.neighbors["enzOut"]
			if not enzEnz:
				print "Enzyme parent missing for ",enz.name
			else:
				noofEnz,sRateLaw1 = getSubprd(cremodel_,True,"prd",enzEnz)
				for i in range(0,len(nameList_)):
					enzAnno2 = enzAnno2+"<moose:enzyme>"+nameList_[i]+"</moose:enzyme>\n"
			enzPrd = enz.neighbors["prd"]
			if enzPrd:
				noofprd,sRateLaw2 = getSubprd(cremodel_,True,"prd",enzPrd)
			else:
				print "Enzyme \"",enz.name, "\" product missing" 
			for i in range(0,len(nameList_)):
				enzAnno2 = enzAnno2+"<moose:product>"+nameList_[i]+"</moose:product>\n"
			enzAnno2 += "<moose:groupName>"+ cleanEnzname + "_" + str(enz.getId().value) + "_" + str(enz.getDataIndex())+"_" +"</moose:groupName>\n";
			enzAnno2 += "<moose:stage>2</moose:stage> \n";
			enzAnno2 += "</moose:EnzymaticReaction>";
			enzyme.setAnnotation( enzAnno2 );

			enzrate_law = "k3" + '*'+sRateLaw;
			kl = enzyme.createKineticLaw();
			kl.setFormula( enzrate_law );
			unit = parmUnit(noofPrd-1 ,cremodel_)
			printParameters( kl,"k3",k3,unit ); 
			
		elif(enz.className == "MMenz" or enz.className == "ZombieMMenz"):
			enzyme = cremodel_.createReaction()
			
			if notesE != "":
				cleanNotesE= convertNotesSpecialChar(notesE)
				notesStringE = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t"+ cleanNotesE + "\n\t </body>"
				enzyme.setNotes(notesStringE)
			enzyme.setId(str(idBeginWith(cleanEnzname+"_"+str(enz.getId().value)+"_"+str(enz.getDataIndex())+"_")))
			enzyme.setName(cleanEnzname)
			enzyme.setFast ( False )
			enzyme.setReversible( True)
			if enzannoexist:
				enzAnno = "<moose:EnzymaticReaction>\n" + enzGpname + "</moose:EnzymaticReaction>";
				enzyme.setAnnotation(enzAnno)
			Km = enz.numKm
			kcat = enz.kcat
			enzSub = enz.neighbors["sub"] 
			noofSub,sRateLawS = getSubprd(cremodel_,False,"sub",enzSub)
			#sRate_law << rate_law.str();
			#Modifier
			enzMod = enz.neighbors["enzDest"]
			noofMod,sRateLawM = getSubprd(cremodel_,False,"enz",enzMod)
			enzPrd = enz.neighbors["prd"]
			noofPrd,sRateLawP = getSubprd(cremodel_,False,"prd",enzPrd)
			kl = enzyme.createKineticLaw()
			fRate_law ="kcat *" + sRateLawS + "*" + sRateLawM + "/" + "(" + "Km" + "+" +sRateLawS +")"
			kl.setFormula(fRate_law)
			kl.setNotes("<body xmlns=\"http://www.w3.org/1999/xhtml\">\n\t\t" + fRate_law + "\n \t </body>")
			printParameters( kl,"Km",Km,"substance" )
			kcatUnit = parmUnit( 0,cremodel_ )
			printParameters( kl,"kcat",kcat,kcatUnit )

def printParameters( kl, k, kvalue, unit ):
	para = kl.createParameter()
	para.setId(str(idBeginWith( k )))
	para.setValue( kvalue )
	para.setUnits( unit )

def parmUnit( rct_order,cremodel_ ):
	order = rct_order
	if order == 0:
		unit_stream = "per_second"
	elif order == 1:
		unit_stream = "per_item_per_second"
	elif order == 2:
		unit_stream ="per_item_sq_per_second"
	else:
		unit_stream = "per_item_"+str(rct_order)+"_per_second";

	lud =cremodel_.getListOfUnitDefinitions();
	flag = False;
	for i in range( 0,len(lud)):
		ud = lud.get(i);
		if ( ud.getId() == unit_stream ):
			flag = True;
			break;
	if ( not flag ):
		unitdef = cremodel_.createUnitDefinition()
		unitdef.setId( unit_stream)
		#Create individual unit objects that will be put inside the UnitDefinition .
		if order != 0 :
			unit = unitdef.createUnit()
			unit.setKind( UNIT_KIND_ITEM )
			unit.setExponent( -order )
			unit.setMultiplier(1)
			unit.setScale( 0 )

		unit = unitdef.createUnit();
		unit.setKind( UNIT_KIND_SECOND );
		unit.setExponent( -1 );
		unit.setMultiplier( 1 );
		unit.setScale ( 0 );
	return unit_stream
	
def getSubprd(cremodel_,mobjEnz,type,neighborslist):
	if type == "sub":
		reacSub = neighborslist
		reacSubCou = Counter(reacSub)

		#print " reacSubCou ",reacSubCou,"()",len(reacSubCou)
		noofSub = len(reacSubCou)
		rate_law = " "
		if reacSub:
			rate_law = processRateLaw(reacSubCou,cremodel_,noofSub,"sub",mobjEnz)
			return len(reacSub),rate_law
		else:
			print reac.className+ " has no substrate"
			return 0,rate_law
	elif type == "prd":
		reacPrd = neighborslist
		reacPrdCou = Counter(reacPrd)
		noofPrd = len(reacPrdCou)
		rate_law = " "
		if reacPrd:
			rate_law = processRateLaw(reacPrdCou,cremodel_,noofPrd,"prd",mobjEnz)
			return len(reacPrd),rate_law
	elif type == "enz":
		enzModifier = neighborslist
		enzModCou = Counter(enzModifier)
		noofMod = len(enzModCou)
		rate_law = " "
		if enzModifier:
			rate_law = processRateLaw(enzModCou,cremodel_,noofMod,"Modifier",mobjEnz)
			return len(enzModifier),rate_law
	

def processRateLaw(objectCount,cremodel,noofObj,type,mobjEnz):
	rate_law = ""
	nameList_[:] = []
	for value,count in objectCount.iteritems():
		value = moose.element(value)
		nameIndex = value.name+"_"+str(value.getId().value)+"_"+str(value.getDataIndex())+"_"
		clean_name = (str(idBeginWith(convertSpecialChar(nameIndex))))
		if mobjEnz == True:
			nameList_.append(clean_name)
		if type == "sub":
			sbmlRef = cremodel.createReactant()
		elif type == "prd":
			sbmlRef = cremodel.createProduct()
		elif type == "Modifier":
			sbmlRef = cremodel.createModifier()
			sbmlRef.setSpecies(clean_name)

		if type == "sub" or type == "prd":
			sbmlRef.setSpecies(clean_name)

			sbmlRef.setStoichiometry( count)
			if clean_name in spe_constTrue:
				sbmlRef.setConstant(True)
			else:
				sbmlRef.setConstant(False)
		if ( count == 1 ):
			if rate_law == "":
				rate_law = clean_name
			else:
				rate_law = rate_law+"*"+clean_name
		else:
			if rate_law == "":
				rate_law = clean_name+"^"+str(count)
			else:
				rate_law = rate_law+"*"+clean_name + "^" + str(count)
	return(rate_law)

def listofname(reacSub,mobjEnz):
	objectCount = Counter(reacSub)
	nameList_[:] = []
	for value,count in objectCount.iteritems():
		value = moose.element(value)
		nameIndex = value.name+"_"+str(value.getId().value)+"_"+str(value.getDataIndex())+"_"
		clean_name = convertSpecialChar(nameIndex)
		if mobjEnz == True:
			nameList_.append(clean_name)
def writeReac(modelpath,cremodel_):
	for reac in wildcardFind(modelpath+'/##[ISA=ReacBase]'):
		reaction = cremodel_.createReaction()
		reacannoexist = False
		reacGpname = " "
		cleanReacname = convertSpecialChar(reac.name) 
		reaction.setId(str(idBeginWith(cleanReacname+"_"+str(reac.getId().value)+"_"+str(reac.getDataIndex())+"_")))
		reaction.setName(cleanReacname)
		Kf = reac.numKf
		Kb = reac.numKb
		if Kb == 0.0:
			reaction.setReversible( False )
		else:
			reaction.setReversible( True )
		
		reaction.setFast( False )
		if moose.exists(reac.path+'/info'):
			Anno = moose.Annotator(reac.path+'/info')
			notesR = Anno.notes
			if notesR != "":
				cleanNotesR= convertNotesSpecialChar(notesR)
				notesStringR = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t"+ cleanNotesR + "\n\t </body>"
				reaction.setNotes(notesStringR)
			element = moose.element(reac)
			ele = getGroupinfo(element)
			if ele.className == "Neutral":
				reacGpname = "<moose:Group>"+ ele.name + "</moose:Group>\n"
				reacannoexist = True
			if reacannoexist :
				reacAnno = "<moose:ModelAnnotation>\n"
				if reacGpname:
					reacAnno = reacAnno + reacGpname
				reacAnno = reacAnno+ "</moose:ModelAnnotation>"
				#s1.appendAnnotation(XMLNode.convertStringToXMLNode(speciAnno))
				reaction.setAnnotation(reacAnno)
		
		kl_s = sRL = pRL = ""
		
		reacSub = reac.neighbors["sub"]
		reacPrd = reac.neighbors["prd"]
		if not reacSub and not reacPrd:
			print " Reaction ",reac.name, "missing substrate and product"
		else:
			kfl = reaction.createKineticLaw()
			if reacSub:
				noofSub,sRateLaw = getSubprd(cremodel_,False,"sub",reacSub)
				if noofSub:
					cleanReacname = cleanReacname+"_"+str(reac.getId().value)+"_"+str(reac.getDataIndex())+"_"
					kfparm = idBeginWith(cleanReacname)+"_"+"Kf"
					sRL = idBeginWith(cleanReacname) + "_Kf * " + sRateLaw
					unit = parmUnit( noofSub-1 ,cremodel_)
					printParameters( kfl,kfparm,Kf,unit ); 
					kl_s = sRL
				else:
					print reac.name + " has no substrate"
					return -2
			else:
				print " Substrate missing for reaction ",reac.name
				
			if reacPrd:
				noofPrd,pRateLaw = getSubprd(cremodel_,False,"prd",reacPrd)
				if  noofPrd:
					if Kb:
						kbparm = idBeginWith(cleanReacname)+"_"+"Kb"
						pRL = idBeginWith(cleanReacname) + "_Kb * " + pRateLaw
						unit = parmUnit( noofPrd-1 , cremodel_)
						printParameters( kfl,kbparm,Kb,unit );
						kl_s = kl_s+ "- "+pRL
				else:
					print reac.name + " has no product"
					return -2
			else:
				print " Product missing for reaction ",reac.name
		kfl.setFormula(kl_s)

def writeFunc(modelpath,cremodel_):
	funcs = wildcardFind(modelpath+'/##[ISA=Function]')
	#if func:
	for func in funcs:
		if func:
			fName = idBeginWith( convertSpecialChar(func.parent.name+"_"+str(func.parent.getId().value)+"_"+str(func.parent.getDataIndex())+"_"))
			item = func.path+'/x[0]'
			sumtot = moose.element(item).neighbors["input"]
			expr = moose.element(func).expr
			for i in range(0,len(sumtot)):
				v ="x"+str(i)
				if v in expr:
					z = str(convertSpecialChar(sumtot[i].name+"_"+str(moose.element(sumtot[i]).getId().value)+"_"+str(moose.element(sumtot[i]).getDataIndex()))+"_")
					expr = expr.replace(v,z)
			rule =  cremodel_.createAssignmentRule()
			rule.setVariable( fName )
			rule.setFormula( expr )
			
def convertNotesSpecialChar(str1):
	d = {"&":"_and","<":"_lessthan_",">":"_greaterthan_","BEL":"&#176"}
	for i,j in d.iteritems():
		str1 = str1.replace(i,j)
	#stripping \t \n \r and space from begining and end of string
	str1 = str1.strip(' \t\n\r')
	return str1
def getGroupinfo(element):
	#   Note: At this time I am assuming that if group exist (incase of Genesis)
	#   1. for 'pool' its between compartment and pool, /modelpath/Compartment/Group/pool 
	#   2. for 'enzComplx' in case of ExpilcityEnz its would be, /modelpath/Compartment/Group/Pool/Enz/Pool_cplx 
	#   For these cases I have checked, but subgroup may exist then this bit of code need to cleanup further down
	#   if /modelpath/Compartment/Group/Group1/Pool, then I check and get Group1
	#   And /modelpath is also a NeutralObject,I stop till I find Compartment

	while not mooseIsInstance(element, ["Neutral"]) and not mooseIsInstance(element,["CubeMesh","CyclMesh"]):
		element = element.parent
	return element
	
def mooseIsInstance(element, classNames):
	return moose.element(element).__class__.__name__ in classNames

def findCompartment(element):
	while not mooseIsInstance(element,["CubeMesh","CyclMesh"]):
		element = element.parent
	return element

def idBeginWith( name ):
	changedName = name;
	if name[0].isdigit() :
		changedName = "_"+name
	return changedName;
	
def convertSpecialChar(str1):
	d = {"&":"_and","<":"_lessthan_",">":"_greaterthan_","BEL":"&#176","-":"_minus_","'":"_prime_",
		 "+": "_plus_","*":"_star_","/":"_slash_","(":"_bo_",")":"_bc_",
		 "[":"_sbo_","]":"_sbc_",".":"_dot_"," ":"_"
		}
	for i,j in d.iteritems():
		str1 = str1.replace(i,j)
	return str1
	
def writeSpecies(modelpath,cremodel_,sbmlDoc):
	#getting all the species 
	for spe in wildcardFind(modelpath+'/##[ISA=PoolBase]'):
		sName = convertSpecialChar(spe.name)
		comptVec = findCompartment(spe)
		speciannoexist = False;
		speciGpname = ""

		if not isinstance(moose.element(comptVec),moose.ChemCompt):
			return -2
		else:
			compt = comptVec.name+"_"+str(comptVec.getId().value)+"_"+str(comptVec.getDataIndex())+"_"
			s1 = cremodel_.createSpecies()
			spename = sName+"_"+str(spe.getId().value)+"_"+str(spe.getDataIndex())+"_"
			spename = str(idBeginWith(spename))
			s1.setId(spename)
			
			if spename.find("cplx") != -1 and isinstance(moose.element(spe.parent),moose.EnzBase):
				enz = spe.parent
				if (moose.element(enz.parent),moose.PoolBase):
					#print " found a cplx name ",spe.parent, moose.element(spe.parent).parent
					enzname = enz.name
					enzPool = (enz.parent).name
					sName = convertSpecialChar(enzPool+"_"+enzname+"_"+sName)

			
			s1.setName(sName)
			s1.setInitialAmount(spe.nInit)
			s1.setCompartment(compt)
			#  Setting BoundaryCondition and constant as per this rule for BufPool
			#  -constanst  -boundaryCondition  -has assignment/rate Rule  -can be part of sub/prd
			#   false           true              yes                       yes   
			#   true            true               no                       yes
			if spe.className == "BufPool" or spe.className == "ZombieBufPool" :
				#BoundaryCondition is made for buff pool
				s1.setBoundaryCondition(True);

				if moose.exists(spe.path+'/func'):
					bpf = moose.element(spe.path)
					for fp in bpf.children:
						if fp.className =="Function" or fp.className == "ZombieFunction":
							if len(moose.element(fp.path+'/x').neighbors["input"]) > 0:
								s1.setConstant(False)
							else:
								#if function exist but sumtotal object doesn't exist
								spe_constTrue.append(spename)
								s1.setConstant(True)
				else:
					spe_constTrue.append(spename)
					s1.setConstant(True)
			else:
				#if not bufpool then Pool, then 
				s1.setBoundaryCondition(False)
				s1.setConstant(False)
			s1.setUnits("substance")
			s1.setHasOnlySubstanceUnits( True )
			if moose.exists(spe.path+'/info'):
				Anno = moose.Annotator(spe.path+'/info')
				notesS = Anno.notes
				if notesS != "":
					cleanNotesS= convertNotesSpecialChar(notesS)
					notesStringS = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t"+ cleanNotesS + "\n\t </body>"
					s1.setNotes(notesStringS)
			#FindGroupName
			element = moose.element(spe)
			ele = getGroupinfo(element)
			if ele.className == "Neutral":
				speciGpname = "<moose:Group>"+ ele.name + "</moose:Group>\n"
				speciannoexist = True
			if speciannoexist :
				speciAnno = "<moose:ModelAnnotation>\n"
				if speciGpname:
					speciAnno = speciAnno + speciGpname
				speciAnno = speciAnno+ "</moose:ModelAnnotation>"
	return True

def writeCompt(modelpath,cremodel_):
	#getting all the compartments
	for compt in wildcardFind(modelpath+'/##[ISA=ChemCompt]'):
		comptName = convertSpecialChar(compt.name)
		#converting m3 to litre
		size =compt.volume*pow(10,3)
		ndim = compt.numDimensions
		c1 = cremodel_.createCompartment()
		c1.setId(str(idBeginWith(comptName+"_"+str(compt.getId().value)+"_"+str(compt.getDataIndex())+"_")))
		c1.setName(comptName)                     
		c1.setConstant(True)               
		c1.setSize(size)          
		c1.setSpatialDimensions(ndim)
		c1.setUnits('volume')

#write Simulation runtime,simdt,plotdt 
def writeSimulationAnnotation(modelpath):
	modelAnno = ""
	if moose.exists(modelpath+'/info'):
		mooseclock = moose.Clock('/clock')
		modelAnno ="<moose:ModelAnnotation>\n"
		modelAnnotation = moose.element(modelpath+'/info')
		modelAnno = modelAnno+"<moose:ModelTime> "+str(modelAnnotation.runtime)+" </moose:ModelTime>\n"
		modelAnno = modelAnno+"<moose:ModelSolver> "+modelAnnotation.solver+" </moose:ModelSolver>\n"
		modelAnno = modelAnno+"<moose:simdt>"+ str(mooseclock.dts[11]) + " </moose:simdt>\n";
		modelAnno = modelAnno+"<moose:plotdt> " + str(mooseclock.dts[18]) +" </moose:plotdt>\n";
		plots = "";
		graphs = moose.wildcardFind(modelpath+"/##[TYPE=Table2]")
		for gphs in range(0,len(graphs)):
			gpath = graphs[gphs].neighbors['requestOut']
			if len(gpath) != 0:
				q = moose.element(gpath[0])
				ori = q.path
				graphSpefound = False
				while not(isinstance(moose.element(q),moose.CubeMesh)):
					q = q.parent
					graphSpefound = True
				if graphSpefound:
					if not plots:
						plots = ori[ori.find(q.name)-1:len(ori)]
					else:
						plots = plots + "; "+ori[ori.find(q.name)-1:len(ori)]
		if plots != " ":
			modelAnno = modelAnno+ "<moose:plots> "+ plots+ "</moose:plots>\n";
		modelAnno = modelAnno+"</moose:ModelAnnotation>"
	return modelAnno

def writeUnits(cremodel_):
	unitVol = cremodel_.createUnitDefinition()
	unitVol.setId( "volume")
	unit = unitVol.createUnit()
	unit.setKind(UNIT_KIND_LITRE)
	unit.setMultiplier(1.0)
	unit.setExponent(1.0)
	unit.setScale(0)

	unitSub = cremodel_.createUnitDefinition()
	unitSub.setId("substance")
	unit = unitSub.createUnit()
	unit.setKind( UNIT_KIND_ITEM )
	unit.setMultiplier(1)
	unit.setExponent(1.0)
	unit.setScale(0)
	

def validateModel( sbmlDoc ):
	#print " sbmlDoc ",sbmlDoc.toSBML()
	if ( not sbmlDoc ):
		print "validateModel: given a null SBML Document"
		return False
	consistencyMessages    = ""
	validationMessages     = ""
	noProblems             = True
	numCheckFailures       = 0
	numConsistencyErrors   = 0
	numConsistencyWarnings = 0
	numValidationErrors    = 0
	numValidationWarnings  = 0
	#Once the whole model is done and before it gets written out, 
	#it's important to check that the whole model is in fact complete, consistent and valid.
	numCheckFailures = sbmlDoc.checkInternalConsistency()
	if ( numCheckFailures > 0 ):
		noProblems = False
		for i in range(0,numCheckFailures ):
			sbmlErr = sbmlDoc.getError(i);
			if ( sbmlErr.isFatal() or sbmlErr.isError() ):
				++numConsistencyErrors;
			else:
				++numConsistencyWarnings
		constStr = sbmlDoc.printErrors()
		consistencyMessages = constStr
	  
	#If the internal checks fail, it makes little sense to attempt
	#further validation, because the model may be too compromised to
	#be properly interpreted.
	if ( numConsistencyErrors > 0 ):
		consistencyMessages += "Further validation aborted.";
	else:
		numCheckFailures = sbmlDoc.checkConsistency()
		#numCheckFailures = sbmlDoc.checkL3v1Compatibility() 
		if ( numCheckFailures > 0 ):
			noProblems = False;
			for i in range(0, (numCheckFailures ) ):
				consistencyMessages = sbmlDoc.getErrorLog().toString()
				sbmlErr = sbmlDoc.getError(i);
				if ( sbmlErr.isFatal() or sbmlErr.isError() ):
					++numValidationErrors;
				else:
					++numValidationWarnings;
		warning = sbmlDoc.getErrorLog().toString()
		oss = sbmlDoc.printErrors()
		validationMessages = oss
	if ( noProblems ):
		return True
	else:
		if consistencyMessages != "":
			print " consistency Warning: "+consistencyMessages
		
		if ( numConsistencyErrors > 0 ):
			if numConsistencyErrors == 1: t = "" 
			else: t="s"
			print "ERROR: encountered " + numConsistencyErrors + " consistency error" +t+ " in model '" + sbmlDoc.getModel().getId() + "'."
	if ( numConsistencyWarnings > 0 ):
		if numConsistencyWarnings == 1:
			t1 = "" 
		else: t1 ="s"
		print "Notice: encountered " + numConsistencyWarnings +" consistency warning" + t + " in model '" + sbmlDoc.getModel().getId() + "'."
	  	
	if ( numValidationErrors > 0 ):
		if numValidationErrors == 1:
			t2 = "" 
		else: t2 ="s" 
		print "ERROR: encountered " + numValidationErrors  + " validation error" + t2 + " in model '" + sbmlDoc.getModel().getId() + "'."
		if ( numValidationWarnings > 0 ):
			if numValidationWarnings == 1:
				t3 = "" 
			else: t3 = "s"

			print "Notice: encountered " + numValidationWarnings + " validation warning" + t3 + " in model '" + sbmlDoc.getModel().getId() + "'." 
		
		print validationMessages;
	return ( numConsistencyErrors == 0 and numValidationErrors == 0)
	#return ( numConsistencyErrors == 0 and numValidationErrors == 0, consistencyMessages)

if __name__ == "__main__":

	filepath = sys.argv[1]
	path = sys.argv[2]

	f = open(filepath, 'r')
	
	if path == '':
		loadpath = filepath[filepath.rfind('/'):filepath.find('.')]
	else:
		loadpath = path
	
	moose.loadModel(filepath,loadpath,"gsl")
	
	written = mooseWriteSBML(loadpath,filepath)
	if written:
		print " File written to ",written
	else:
		print " could not write model to SBML file"
	