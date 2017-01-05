from __future__ import print_function 

#*******************************************************************
# * File:            merge.py
# * Description:
# * Author:          HarshaRani
# * E-mail:          hrani@ncbs.res.in
# ********************************************************************/
# **********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2003-2016 Upinder S. Bhalla. and NCBS
#Created : Friday June 13 12:19:00 2016(+0530)
#Version 
#Last-Updated:Tuesday June 21 17.48.37
#		  By: Harsha
#**********************************************************************/

# This program is used to merge models
# -- Model B is merged to A
# -- Rules are 
#    ## check for compartment in B exist in A
#	    *** If compartment from model B doesn't exist in model A, then copy entire compartment from B to A
#	    *** If compartment from model B exist in model A
#			^^^^ check the volume of compartment of B as compared to A
#				!!!!! If same, then copy all the moose object which doesn't exist in A and reference to both mooseId
#					which is used for connecting objects.
#					
#			^^^^ If volume of compartment of model B is different then change the volume of compartment of model B same as A
#				!!!! If same, then copy all the moose object which doesn't exist in A and reference to both mooseId
#					which is used for connecting objects

#					&&&&&& copying pools from compartment from B to A is easy 
#					&&&&& but while copying reaction and enzyme check if the same reaction exist
#						-- if same reaction name exists
#							-- Yes 
#								1) check if substrate and product are same as compartment from model B to compartment modelA
#									--yes do nothing
#									--No then duplicate the reaction in model A and connect the message of approraite sub/prd
#									and warn the user the same reaction name with different sub or product
#							-- No
#								copy the reaction

import sys
from . import _moose as moose

def merge(A,B):
	#load models into moose and solver's are deleted
	apath = loadModels(A)
	bpath = loadModels(B)

	comptAdict = comptList(apath)
	comptBdict = comptList(bpath)

	for key in list(comptBdict.keys()):

		if key not in comptAdict:
			# comptBdict[key] - compartment from model B which does not exist in model A
			moose.copy(comptBdict[key],moose.element(apath))
		else:			
			war_msg = ""
			copied , duplicated = [],[]
			
			if not (comptAdict[key].volume == comptBdict[key].volume):
				#change volume of ModelB same as ModelA
				volumeA = comptAdict[key].volume
				comptBdict[key].volume = volumeA
		
			#Merging Pool
			poolName_a = []
			poolListina = moose.wildcardFind(comptAdict[key].path+'/##[ISA=PoolBase]') 
			if poolListina:
				neutral_compartment = findgroup_compartment(poolListina[0])
			for pl in poolListina:
				poolName_a.append(pl.name)
			
			for pool in moose.wildcardFind(comptBdict[key].path+'/##[ISA=PoolBase]'):
				if not pool.name in poolName_a :
					#pool has compartment(assume its direct pool ),so copy under kinetics
					if ((pool.parent).className == "CubeMesh" or (pool.parent).className == "Neutral"):
						c = moose.copy(pool,neutral_compartment)
						copied.append(c)
					elif (pool.parent).className == "ZombieEnz" or (pool.parent).className == "Enz":
						print(" Pool not in modelA but enz parent",pool.name)
						pass
					else:
						print(" Check this pool, parent which doesn't have ChemCompt or Enz", end=' ')
			
			#Mergering StimulusTable
			stimName_a = []
			stimListina = moose.wildcardFind(comptAdict[key].path+'/##[ISA=StimulusTable]') 
			if stimListina:
				neutral_compartment = findgroup_compartment(stimListina[0])
			for st in stimListina:
				stimName_a.append(st.name)
			for stb in moose.wildcardFind(comptBdict[key].path+'/##[ISA=StimulusTable]'):
				if stb.name in stimName_a:
					sA = comptAdict[key].path+'/'+stb.name
					sB = comptBdict[key].path+'/'+stb.name
					stAOutput = subprdList(sA,"output")
					stBOutput = subprdList(sB,"output")
					sas = set(stAOutput)
					sbs = set(stBOutput)
					uniq = (sas.union(sbs) - sas.intersection(sbs))
					for u in uniq:
						if u not in stAOutput:
							src = moose.element(sA)
							des = moose.element(comptAdict[key].path+'/'+u)
							moose.connect(src,'output',des,'setConcInit')
				else:
					st1 = moose.StimulusTable(comptAdict[key].path+'/'+stb.name)
					for sb in sbs:
						des = moose.element(comptAdict[key].path+'/'+sb)
						moose.connect(st1,'output',des,'setConcInit')	
			#Mergering Reaction
			reacName_a = []
			reacListina = moose.wildcardFind(comptAdict[key].path+'/##[ISA=ReacBase]') 
			if reacListina:
				neutral_compartment = findgroup_compartment(poolListina[0])
			
			for re in reacListina:
				reacName_a.append(re.name)
			for reac in moose.wildcardFind(comptBdict[key].path+'/##[ISA=ReacBase]'):
				if reac.name in reacName_a:
					
					rA = comptAdict[key].path+'/'+reac.name
					rB = comptBdict[key].path+'/'+reac.name
						
					rAsubname,rBsubname,rAprdname,rBprdname = [], [], [], []
					rAsubname = subprdList(rA,"sub")
					rBsubname = subprdList(rB,"sub")
					rAprdname = subprdList(rA,"prd")
					rBprdname = subprdList(rB,"prd")
					
					aS = set(rAsubname)
					bS = set(rBsubname)
					aP = set(rAprdname)
					bP = set(rBprdname)
					
					hasSameSub,hasSamePrd = True,True
					hassameSlen,hassamePlen = False,False
					
					hasSameSub = not (len (aS.union(bS) - aS.intersection(bS)))
					hasSamePrd = not (len(aP.union(bP) - aP.intersection(bP)))
					
					hassameSlen = ( len(rAsubname) == len(rBsubname))
					hassamePlen = ( len(rAprdname) == len(rBprdname))
					
					
					if not all((hasSameSub,hasSamePrd,hassameSlen,hassamePlen)):
						war_msg = war_msg +"Reaction \""+reac.name+ "\" also contains in modelA but with different"
						if not all((hasSameSub,hassameSlen)):
							war_msg = war_msg+ " substrate "

						if not all((hasSamePrd, hassamePlen)):
							war_msg = war_msg+ " product"	
						war_msg = war_msg +", reaction is duplicated in modelA. \nModeler should decide to keep or remove this reaction"
						
						reac = moose.Reac(comptAdict[key].path+'/'+reac.name+"_duplicated")
						mooseConnect(comptAdict[key].path,reac,rBsubname,"sub")
						mooseConnect(comptAdict[key].path,reac,rBprdname,"prd")
						
						duplicated.append(reac)
						
				elif not reac.name in reacName_a :
					#reac has compartment(assume its direct reac ),so copy under kinetics
					if ((reac.parent).className == "CubeMesh" or (reac.parent).className == "Neutral"):
						c = moose.copy(reac,neutral_compartment)
						copied.append(c)

						rBsubname, rBprdname = [],[]
						rBsubname = subprdList(reac,"sub")
						rBprdname = subprdList(reac,"prd")
						mooseConnect(comptAdict[key].path,reac,rBsubname,"sub")
						mooseConnect(comptAdict[key].path,reac,rBprdname,"prd")
	print("\ncopied: ", copied, \
		 "\n\nDuplicated: ",duplicated, \
		  "\n\nwarning: ",war_msg)
	return copied,duplicated,war_msg
def loadModels(filename):
	apath = '/'

	apath = filename[filename.rfind('/'): filename.rfind('.')]
	
	moose.loadModel(filename,apath)
	
	#Solvers are removed
	deleteSolver(apath)	
	return apath

def comptList(modelpath):
	comptdict = {}
	for ca in moose.wildcardFind(modelpath+'/##[ISA=ChemCompt]'):
		comptdict[ca.name] = ca
	return comptdict
	
def mooseConnect(modelpath,reac,spList,sptype):
	for spl in spList:
		spl_id = moose.element(modelpath+'/'+spl)
		reac_id = moose.element(modelpath+'/'+reac.name)
		if sptype == "sub":
			m = moose.connect( reac_id, "sub", spl_id, 'reac' )
		else:
			moose.connect( reac_id, "prd", spl_id, 'reac' )

def deleteSolver(modelRoot):
	compts = moose.wildcardFind(modelRoot+'/##[ISA=ChemCompt]')
	for compt in compts:
		if moose.exists(compt.path+'/stoich'):
			st = moose.element(compt.path+'/stoich')
			st_ksolve = st.ksolve
			moose.delete(st)
			if moose.exists((st_ksolve).path):
				moose.delete(st_ksolve)

def subprdList(reac,subprd):
	rtype = moose.element(reac).neighbors[subprd]
	rname = []
	for rs in rtype:
		rname.append(rs.name)
	return rname

def findCompartment(element):
	while not mooseIsInstance(element,["CubeMesh","CyclMesh"]):
		element = element.parent
	return element
def mooseIsInstance(element, classNames):
	#print classNames
	#print moose.element(element).__class__.__name__ in classNames
	return moose.element(element).__class__.__name__ in classNames

def findgroup_compartment(element):
	#Try finding Group which is Neutral but it should exist before Compartment, if Compartment exist then stop at Comparment
	while not mooseIsInstance(element,"Neutral"):
		if mooseIsInstance(element,["CubeMesh","CyclMesh"]):
			return element
		element = element.parent
	return element
if __name__ == "__main__":
	
	model1 = '/home/harsha/genesis_files/gfile/acc12.g'
	model2 = '/home/harsha/Trash/acc12_withadditionPool.g'
	#model1 = '/home/harsha/Trash/modelA.g'
	#model2 = '/home/harsha/Trash/modelB.g'
	model1 = '/home/harsha/genesis_files/gfile/acc44.g'
	model2 = '/home/harsha/genesis_files/gfile/acc45.g'
	mergered = merge(model1,model2)
