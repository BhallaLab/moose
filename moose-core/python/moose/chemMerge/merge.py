# -*- coding: utf-8 -*-
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
#**           copyright (C) 2003-2017 Upinder S. Bhalla. and NCBS
#Created : Friday Dec 16 23:19:00 2016(+0530)
#Version
#Last-Updated: Wed May 21 11:52:33 2018(+0530)
#         By: Harsha
#**********************************************************************/

# This program is used to merge chem models from src to destination
#Rules are :
#   -- If Compartment from the src model doesn't exist in destination model,
#       then entire compartment and its children are copied over including groups
#   -- Models are mergered at group level (if exists)
#       (Group is Neutral object in moose, which may represent pathway in network model)
#   -- Pool's are copied from source to destination if it doesn't exist, if exist nothing is done
#   -- Reaction (Reac), Enzyme (Enz) are copied
#       --- if any danglling Reac or Enz exist then that is not copied
#
#       --- if Reac Name's is different for a given path (group level)
#            then copy the entire Reac along with substrate/product
#       --- if same Reac Name and same sub and prd then nothing is copied
#       --- if same Reac Name but sub or prd is different then duplicated and copied
#
#       --- if Enz Name's is different for a given parent pool path
#            then copy the entire Enz along with substrate/product
#       --- if same Enz Name and same sub and prd then nothing is copied
#       --- if same Enz Name but sub or prd is different then duplicated and copied
#   -- Function are copied only if destination pool to which its suppose to connect doesn't exist with function of its own
#       which is a limitation in moose that no function can be connected to same pool
#
'''
Change log
May 21 : Instead of A and B changed to S and D (source and destination), 
        - at Pool level Neutral and its objected where copied, but again at Enz and Reac level copying was done which caused duplicates which is taken care
        - If Neutral object name is changed for destination, then change made to source file which would be easy to copy else path issue will come
Oct 25 : line to load SBML file was commented, which is uncommented now and also while copying MMenz had a problem which also cleaned up
Oct 14 : absolute import with mtypes just for python3

Oct 12 : clean way of cheking the type of path provided, filepath,moose obj, moose path are taken,
        if source is empty then nothing to copy, 
        if destination was empty list is update with new object

Oct 11 : missing group are copied instead of creating one in new path which also copies Annotator info
        earlier if user asked to save the model, it was saving default to kkit format, now user need to run the command to save (if this is run in command)
        To check: When Gui is allowed to merge 2 models, need to see what happens

'''

import sys
import os
#from . import _moose as moose
import moose

from moose.chemMerge import mtypes
from moose.chemUtil.chemConnectUtil import *
from moose.chemUtil.graphUtils import *
#from moose.genesis import mooseWriteKkit

def checkFile_Obj_str(file_Obj_str):
    model = moose.element('/')
    loaded = False
    found = False
    if isinstance(file_Obj_str, str):
        if os.path.isfile(file_Obj_str) == True:
            model,loaded = loadModels(file_Obj_str)
            found = True
        elif file_Obj_str.find('/') != -1 :
            if not isinstance(file_Obj_str,moose.Neutral):
                if moose.exists(file_Obj_str):
                    model = file_Obj_str
                    loaded = True
                    found = True
        elif isinstance(file_Obj_str, moose.Neutral):
            if moose.exists(file_Obj_str.path):
                model = file_Obj_str.path
                loaded = True
                found = True
    elif isinstance(file_Obj_str, moose.Neutral):
        if moose.exists(file_Obj_str.path):
            model = file_Obj_str.path
            loaded = True
            found = True
    if not found:
        print ("%s path or filename doesnot exist. " % (file_Obj_str))
    return model,loaded

def mergeChemModel(src,des):
    """ Merges two model or the path """
    A = src
    B = des
    sfile = src
    dfile = des
    loadedA = False
    loadedB = False
    modelA = moose.element('/')
    modelB = moose.element('/')
    modelA,loadedA = checkFile_Obj_str(A)
    modelB,loadedB = checkFile_Obj_str(B)
    
    if loadedA and loadedB:
        ## yet deleteSolver is called to make sure all the moose object are off from solver
        deleteSolver(modelA)
        deleteSolver(modelB)

        global poolListina
        poolListina = {}
        grpNotcopiedyet = []
        dictComptA = dict( [ (i.name,i) for i in moose.wildcardFind(modelA+'/##[ISA=ChemCompt]') ] )
        dictComptB = dict( [ (i.name,i) for i in moose.wildcardFind(modelB+'/##[ISA=ChemCompt]') ] )
                
        poolNotcopiedyet = []
        if len(dictComptA):
            for key in list(dictComptA.keys()):
                if key not in dictComptB:
                    # if compartment name from modelB does not exist in modelA, then copy
                    copy = moose.copy(dictComptA[key],moose.element(modelB))

                    dictComptB[key]=moose.element(copy)
                else:
                    #if compartment name from modelB exist in modelA,
                    #volume is not same, then change volume of ModelB same as ModelA
                    if abs(dictComptB[key].volume - dictComptA[key].volume):
                        #hack for now
                        while (abs(dictComptB[key].volume - dictComptA[key].volume) != 0.0):
                            dictComptA[key].volume = float(dictComptB[key].volume)
                    dictComptB = dict( [ (i.name,i) for i in moose.wildcardFind(modelB+'/##[ISA=ChemCompt]') ] )
                    
                    #Mergering pool
                    poolMerge(dictComptA[key],dictComptB[key],poolNotcopiedyet)

            
            comptBdict =  comptList(modelB)

            poolListinb = {}
            poolListinb = updatePoolList(comptBdict)
            
            R_Duplicated, R_Notcopiedyet,R_Daggling = [], [], []
            E_Duplicated, E_Notcopiedyet,E_Daggling = [], [], []
            for key in list(dictComptA.keys()):
                funcExist, funcNotallowed = [], []
                funcExist,funcNotallowed = functionMerge(dictComptB,dictComptA,key)

                poolListinb = updatePoolList(dictComptB)
                R_Duplicated,R_Notcopiedyet,R_Daggling = reacMerge(dictComptA,dictComptB,key,poolListinb)

                poolListinb = updatePoolList(dictComptB)
                E_Duplicated,E_Notcopiedyet,E_Daggling = enzymeMerge(dictComptB,dictComptA,key,poolListinb)
            
            # if isinstance(src, str):
            #     if os.path.isfile(src) == True:
            #         spath, sfile = os.path.split(src)
            #     else:
            #         sfile = src
            # else:
            #     sfile = src
            # if isinstance(des, str):
            #     print " A str",des
            #     if os.path.isfile(des) == True:
            #         dpath, dfile = os.path.split(des)
            #     else:
            #         dfile = des
            # else:
            #     dfile = des
            
            print("\nThe content of %s (src) model is merged to %s (des)." %(sfile, dfile))
            # Here any error or warning during Merge is written it down
            if funcExist:
                print( "\nIn model \"%s\" pool already has connection from a function, these function from model \"%s\" is not allowed to connect to same pool,\n since no two function are allowed to connect to same pool:"%(dfile, sfile))
                for fl in list(funcExist):
                    print("\t [Pool]:  %s [Function]:  %s \n" %(str(fl.parent.name), str(fl.path)))
            if funcNotallowed:
                print( "\nThese functions is not to copied, since pool connected to function input are from different compartment:")
                for fl in list(funcNotallowed):
                    print("\t [Pool]:  %s [Function]:  %s \n" %(str(fl.parent.name), str(fl.path)))
            if R_Duplicated or E_Duplicated:
                print ("These Reaction / Enzyme are \"Duplicated\" into destination file \"%s\", due to "
                        "\n 1. If substrate / product name's are different for a give reaction/Enzyme name "
                        "\n 2. If product belongs to different compartment "
                        "\n Models have to decide to keep or delete these reaction/enzyme in %s" %(dfile, dfile))
                if E_Duplicated:
                    print("Reaction: ")
                for rd in list(R_Duplicated):
                    print ("%s " %str(rd.name))

                if E_Duplicated:
                    print ("Enzyme:")
                    for ed in list(E_Duplicated):
                        print ("%s " %str(ed.name))

            if R_Notcopiedyet or E_Notcopiedyet:

                print ("\nThese Reaction/Enzyme in model are not dagging but while copying the associated substrate or product is missing")
                if R_Notcopiedyet:
                    print("Reaction: ")
                for rd in list(R_Notcopiedyet):
                    print ("%s " %str(rd.name))
                if E_Notcopiedyet:
                    print ("Enzyme:")
                    for ed in list(E_Notcopiedyet):
                        print ("%s " %str(ed.name))

            if R_Daggling or E_Daggling:
                print ("\n Daggling reaction/enzyme are not allowed in moose, these are not merged to %s from %s" %(dfile, sfile))
                if R_Daggling:
                    print("Reaction: ")
                    for rd in list(R_Daggling):
                        print ("%s " %str(rd.name))
                if E_Daggling:
                    print ("Enzyme:")
                    for ed in list(E_Daggling):
                        print ("%s " %str(ed.name))
            
            ## Model is saved
            print ("\n ")
            print ('\nMerged model is available under moose.element(\'%s\')' %(modelB))
            print ('  From the python terminal itself \n to save the model in to genesis format use \n   >moose.mooseWriteKkit(\'%s\',\'filename.g\')' %(modelB))
            print ('  to save into SBML format \n   >moose.mooseWriteSBML(\'%s\',\'filename.xml\')' %(modelB))
            return modelB
            # savemodel = raw_input("Do you want to save the model?  \"YES\" \"NO\" ")
            # if savemodel.lower() == 'yes' or savemodel.lower() == 'y':
            #     mergeto = raw_input("Enter File name ")
            #     if mergeto and mergeto.strip():
            #         filenameto = 'merge.g'
            #     else:
            #         if str(mergeto).rfind('.') != -1:
            #             mergeto = mergeto[:str(mergeto).rfind('.')]
            #         if str(mergeto).rfind('/'):
            #             mergeto = mergeto+'merge'

            #         filenameto = mergeto+'.g'

            #     error,written = moose.mooseWriteKkit(modelB, filenameto)
            #     if written == False:
            #         print('Could not save the Model, check the files')
            #     else:
            #         if error == "":
            #             print(" \n The merged model is saved into \'%s\' " %(filenameto))
            #         else:
            #             print('Model is saved but these are not written\n %s' %(error))

            # else:
            #     print ('\nMerged model is available under moose.element(\'%s\')' %(modelB))
            #     print ('  If you are in python terminal you could save \n   >moose.mooseWriteKkit(\'%s\',\'filename.g\')' %(modelB))
            #     print ('  If you are in python terminal you could save \n   >moose.mooseWriteSBML(\'%s\',\'filename.g\')' %(modelB))
            #return modelB
        
    else:
        print ('\nSource file has no objects to copy(\'%s\')' %(modelA))
        return moose.element('/')
def functionMerge(comptA,comptB,key):
    funcNotallowed, funcExist = [], []
    comptApath = moose.element(comptA[key]).path
    comptBpath = moose.element(comptB[key]).path
    objA = moose.element(comptApath).parent.name
    objB = moose.element(comptBpath).parent.name

    #This will give us all the function which exist in modelB
    funcListinb = moose.wildcardFind(comptBpath+'/##[ISA=Function]')
    for fb in funcListinb:
        #This will give us all the pools that its connected to, for this function
        fvalueOut = moose.element(fb).neighbors['valueOut']
        for poolinB in fvalueOut:
            poolinBpath = poolinB.path
            poolinA = poolinBpath.replace(objB,objA)
            connectionexist = []
            if moose.exists(poolinA):
                #This is give us if pool which is to be connected already exist any connection
                connectionexist = moose.element(poolinA).neighbors['setN']+moose.element(poolinA).neighbors['setConc']+ moose.element(poolinA).neighbors['increment']
                if len(connectionexist) == 0:
                    #This pool in model A doesnot exist with any function
                    inputs = moose.element(fb.path+'/x').neighbors['input']
                    volumes = []
                    for ins in inputs:
                        volumes.append((findCompartment(moose.element(ins))).volume)
                    if len(set(volumes)) == 1:
                        # If all the input connected belongs to one compartment then copy
                        createFunction(fb,poolinA,objB,objA)
                    else:
                        # moose doesn't allow function's input to come from different compartment
                        funcNotallowed.append(fb)
                else:
                    #Pool in model 'A' already exist function "
                    funcExist.append(fb)
            else:
                print(" Path in model A doesn't exists %s" %(poolinA))

    return funcExist,funcNotallowed

def createFunction(fb,setpool,objB,objA):
    fapath1 = fb.path.replace(objB,objA)
    fapath = fapath1.replace('[0]','')

    if not moose.exists(fapath):
        # if fb.parent.className in ['CubeMesh','CyclMesh']:
        #   des = moose.Function('/'+objA+'/'+fb.parent.name+'/'+fb.name)
        # elif fb.parent.className in ['Pool','ZombiePool','BufPool','ZombieBufPool']:
        #   for akey in list(poolListina[findCompartment(fb).name]):
        #       if fb.parent.name == akey.name:
        #           des = moose.Function(akey.path+'/'+fb.name)
        des = moose.Function(fapath)
    else:
        des = moose.element(fapath)
    inputB = moose.element(fb.path+'/x').neighbors["input"]
    moose.connect(des, 'valueOut', moose.element(setpool),'setN' )
    inputA = []
    inputA = moose.element(fapath+'/x').neighbors["input"]
    if not inputA:
        for src in inputB:
            pool = ((src.path).replace(objB,objA)).replace('[0]','')
            numVariables = des.numVars
            expr = ""
            expr = (des.expr+'+'+'x'+str(numVariables))
            expr = expr.lstrip("0 +")
            expr = expr.replace(" ","")
            des.expr = expr
            moose.connect( pool, 'nOut', des.x[numVariables], 'input' )

def comptList(modelpath):
    comptdict = {}
    for ca in moose.wildcardFind(modelpath+'/##[ISA=ChemCompt]'):
        comptdict[ca.name] = ca
    return comptdict

def loadModels(filepath):
    """ load models into moose if file, if moosepath itself it passes back the path and
    delete solver if exist """

    modelpath = '/'
    loaded = False

    if os.path.isfile(filepath) :
        fpath, filename = os.path.split(filepath)
        # print " head and tail ",head,  " ",tail
        # modelpath = filename[filename.rfind('/'): filename.rfind('.')]
        # print "modelpath ",modelpath
        # ext = os.path.splitext(filename)[1]
        # filename = filename.strip()
        modelpath = '/'+filename[:filename.rfind('.')]
        modeltype = mtypes.getType(filepath)
        subtype = mtypes.getSubtype(filepath, modeltype)

        if subtype == 'kkit' or modeltype == "cspace":
            if moose.exists(modelpath):
                moose.delete(modelpath)
            moose.loadModel(filepath,modelpath)
            loaded = True

        elif subtype == 'sbml':
            if moose.exists(modelpath):
                moose.delete(modelpath)
            moose.mooseReadSBML(filepath,modelpath)
            loaded = True
        else:
            print("This file is not supported for mergering")
            modelpath = moose.Shell('/')

    elif moose.exists(filepath):
        modelpath = filepath
        loaded = True

    return modelpath,loaded

def deleteSolver(modelRoot):
    compts = moose.wildcardFind(modelRoot+'/##[ISA=ChemCompt]')
    for compt in compts:
        if moose.exists(compt.path+'/stoich'):
            st = moose.element(compt.path+'/stoich')
            st_ksolve = st.ksolve
            moose.delete(st)
            if moose.exists((st_ksolve).path):
                moose.delete(st_ksolve)

def poolMerge(comptS,comptD,poolNotcopiedyet):
    #Here from source file all the groups are check if exist, if doesn't exist then create those groups
    #Then for that group pools are copied
    SCmptGrp = moose.wildcardFind(comptS.path+'/#[TYPE=Neutral]')
    SCmptGrp = SCmptGrp +(moose.element(comptS.path),)
    
    DCmptGrp = moose.wildcardFind(comptD.path+'/#[TYPE=Neutral]')
    DCmptGrp = DCmptGrp +(moose.element(comptD.path),)
    
    objS = moose.element(comptS.path).parent.name
    objD = moose.element(comptD.path).parent.name
    
    for spath in SCmptGrp:
        grp_cmpt = ((spath.path).replace(objS,objD)).replace('[0]','')
        if moose.exists(grp_cmpt):
            #If path exist, but its not the Neutral obj then creating a neutral obj with _grp
            #It has happened that pool, reac, enz name might exist with the same name, which when tried to create a group
            # it silently ignored the path and object copied under that pool instead of Group
            if moose.element(grp_cmpt).className != spath.className:
                grp_cmpt = grp_cmpt+'_grp'
                moose.Neutral(grp_cmpt)
                # If group name is changed while creating in destination, then in source file the same is changed,
                # so that later path issue doens't come 
                spath.name = spath.name+'_grp'
        else:
            #Neutral obj from src if doesn't exist in destination,then create src's Neutral obj in des
            src = spath
            srcpath = (spath.parent).path
            des = srcpath.replace(objS,objD)
            moose.Neutral(moose.element(des).path+'/'+spath.name)
        
        dpath = moose.element(spath.path.replace(objS,objD))
        spoollist = moose.wildcardFind(spath.path+'/#[ISA=PoolBase]')
        dpoollist = moose.wildcardFind(dpath.path+'/#[ISA=PoolBase]')
        #check made, for a given Neutral or group if pool doesn't exist then copied
        # but some pool if enzyme cplx then there are holded untill that enzyme is copied in
        # `enzymeMerge` function        
        for spool in spoollist:
            if spool.name not in [dpool.name for dpool in dpoollist]:
                copied = copy_deleteUnlyingPoolObj(spool,dpath)
                if copied == False:
                    #hold it for later, this pool may be under enzyme, as cplx
                    poolNotcopiedyet.append(spool)
    
def copy_deleteUnlyingPoolObj(pool,path):
    # check if this pool is under compartement or under enzyme?(which is enzyme_cplx)
    # if enzyme_cplx then don't copy untill this perticular enzyme is copied
    # case: This enzyme_cplx might exist in modelA if enzyme exist then this
    # will automatically copie's the pool
    copied = False

    if pool.parent.className not in ["Enz","ZombieEnz","MMenz","ZombieMMenz"]:
         if path.className in ["Neutral","CubeMesh","CyclMesh"]:
            poolcopied = moose.copy(pool,path)
            copied = True
            # deleting function and enzyme which gets copied if exist under pool
            # This is done to ensure daggling function / enzyme not copied.
            funclist = []
            for types in ['setConc','setN','increment']:
                funclist.extend(moose.element(poolcopied).neighbors[types])

            for fl in funclist:
                moose.delete(fl)
            enzlist = moose.element(poolcopied).neighbors['reac']
            for el in list(set(enzlist)):
                moose.delete(el.path)
    return copied

def updatePoolList(comptAdict):
    for key,value in list(comptAdict.items()):
        plist = moose.wildcardFind(value.path+'/##[ISA=PoolBase]')
        poolListina[key] = plist
    return poolListina

def enzymeMerge(comptD,comptS,key,poolListind):
    war_msg = ""
    RE_Duplicated, RE_Notcopiedyet, RE_Daggling = [], [], []
    comptDpath = moose.element(comptD[key]).path
    comptSpath = moose.element(comptS[key]).path
    objD = moose.element(comptDpath).parent.name
    objS = moose.element(comptSpath).parent.name
    #nzyListina => enzyListind

    enzyListind = moose.wildcardFind(comptDpath+'/##[ISA=EnzBase]')
    enzyListins = moose.wildcardFind(comptSpath+'/##[ISA=EnzBase]')
    for es in enzyListins:
        eSsubname, eSprdname = [],[]
        eSsubname = subprdList(es,"sub")
        eSprdname = subprdList(es,"prd")
        allexists, allexistp = False, False
        allclean = False

        poolinDlist = poolListind[findCompartment(es).name]
        for pD in poolinDlist:
            if es.parent.name == pD.name:
                edpath = es.parent.path.replace(objS,objD)

                if not moose.exists(edpath+'/'+es.name):
                    #This will take care
                    #  -- If same enzparent name but different enzyme name
                    #  -- or different parent/enzyme name
                    if eSsubname and eSprdname:
                        allexists = checkexist(eSsubname,objS,objD)
                        allexistp = checkexist(eSprdname,objS,objD)
                        if allexists and allexistp:
                            enzPool = moose.element(pD.path)
                            edpath = es.parent.path.replace(objS,objD)
                            enz = moose.element(moose.copy(es,moose.element(edpath)))
                            enzPool = enz.parent
                            if es.className in ["ZombieEnz","Enz"]:
                                moose.connect(moose.element(enz),"enz",enzPool,"reac")
                            if es.className in ["ZombieMMenz","MMenz"]:
                                moose.connect(enzPool,"nOut",enz,"enzDest")
                            connectObj(enz,eSsubname,"sub",comptD,war_msg)
                            connectObj(enz,eSprdname,"prd",comptD,war_msg)
                            allclean = True
                        else:
                            # didn't find sub or prd for this Enzyme
                            RE_Notcopiedyet.append(es)
                    else:
                        #   -- it is dagging reaction
                        RE_Daggling.append(es)
                else:
                    #Same Enzyme name
                    #   -- Same substrate and product including same volume then don't copy
                    #   -- different substrate/product or if sub/prd's volume is different then DUPLICATE the Enzyme
                    allclean = False
                    ed = moose.element(es.path.replace(objS,objD))
                    eDsubname = subprdList(ed,"sub")
                    eSsubname = subprdList(es,"sub")
                    hasSamenoofsublen,hasSameS,hasSamevols = same_len_name_vol(eDsubname,eSsubname)

                    eDprdname = subprdList(ed,"prd")
                    eSprdname = subprdList(es,"prd")
                    hasSamenoofprdlen,hasSameP,hasSamevolp = same_len_name_vol(eDprdname,eSprdname)
                    if not all((hasSamenoofsublen,hasSameS,hasSamevols,hasSamenoofprdlen,hasSameP,hasSamevolp)):
                        # May be different substrate or product or volume of Sub/prd may be different,
                        # Duplicating the enzyme
                        if eSsubname and eSprdname:
                            allexists,allexistp = False,False
                            allexists = checkexist(eSsubname,objS,objD)
                            allexistp = checkexist(eSprdname,objS,objD)
                            if allexists and allexistp:
                                es.name = es.name+"_duplicated"
                                if es.className in ["ZombieEnz","Enz"]:
                                    edpath = es.parent.path.replace(objS,objD)
                                    enz = moose.copy(es,moose.element(edpath))
                                    moose.connect(enz, 'enz', edpath, 'reac' )

                                if es.className in ["ZombieMMenz","MMenz"]:
                                    edpath = es.parent.path.replace(objS,objD)
                                    enz = moose.copy(es,moose.element(edpath))
                                    enzinfo = moose.Annotator(enz.path+'/info')
                                    moose.connect(moose.element(enz).parent,"nOut",moose.element(enz),"enzDest")
                                    #moose.connect(moose.element(enz),"enz",moose.element(enz).parent,"reac")

                                connectObj(enz,eSsubname,"sub",comptD,war_msg)
                                connectObj(enz,eSprdname,"prd",comptD,war_msg)
                                RE_Duplicated.append(enz)
                                allclean = True
                            else:
                                allclean = False
                    else:
                        allclean = True

                    if not allclean:
                        # didn't find sub or prd for this enzyme
                        #   --  it may be connected Enzyme cplx
                        if eSsubname and eSprdname:
                            RE_Notcopiedyet.append(es)
                        else:
                            RE_Daggling.append(es)

    return RE_Duplicated,RE_Notcopiedyet,RE_Daggling

def reacMerge(comptS,comptD,key,poolListina):
    RE_Duplicated, RE_Notcopiedyet, RE_Daggling = [], [], []
    war_msg = ""
    comptSpath = moose.element(comptS[key]).path
    comptDpath = moose.element(comptD[key]).path
    objS = moose.element(comptSpath).parent.name
    objD = moose.element(comptDpath).parent.name
    
    reacListins = moose.wildcardFind(comptSpath+'/##[ISA=ReacBase]')
    reacListind = moose.wildcardFind(comptDpath+'/##[ISA=ReacBase]')
    
    for rs in reacListins:
        rSsubname, rSprdname = [],[]
        rSsubname = subprdList(rs,"sub")
        rSprdname = subprdList(rs,"prd")
        allexists, allexistp = False, False
        allclean = False

        if rs.name not in [rd.name for rd in reacListind]:
            # reaction name not found then copy
            # And assuming that pools are copied earlier EXPECT POOL CPLX
            # To be assured the it takes correct compartment name incase reaction sub's
            # belongs to different compt
            key = findCompartment(rs).name
            if rSsubname and rSprdname:
                allexists =  checkexist(rSsubname,objS,objD)
                allexistp =  checkexist(rSprdname,objS,objD)
                if allexists and allexistp:
                    rdpath = rs.parent.path.replace(objS,objD)
                    reac = moose.copy(rs,moose.element(rdpath))
                    connectObj(reac,rSsubname,"sub",comptD,war_msg)
                    connectObj(reac,rSprdname,"prd",comptD,war_msg)
                    allclean = True
                else:
                    # didn't find sub or prd for this reaction
                    #   --  it may be connected Enzyme cplx
                    RE_Notcopiedyet.append(rs)
            else:
                #   -- it is dagging reaction
                RE_Daggling.append(rs)
                #print ("This reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied")
                #war_msg = war_msg+"\nThis reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied"

        else:
            #Same reaction name
            #   -- Same substrate and product including same volume then don't copy
            #   -- different substrate/product or if sub/prd's volume is different then DUPLICATE the reaction
            
            allclean = False
            for rd in reacListind:
                if rs.name == rd.name:
                    rSsubname = subprdList(rs,"sub")
                    rDsubname = subprdList(rd,"sub")
                    hasSamenoofsublen,hasSameS,hasSamevols = same_len_name_vol(rSsubname,rDsubname)

                    rSprdname = subprdList(rs,"prd")
                    rDprdname = subprdList(rd,"prd")
                    hasSamenoofprdlen,hasSameP,hasSamevolp = same_len_name_vol(rSprdname,rDprdname)
                    if not all((hasSamenoofsublen,hasSameS,hasSamevols,hasSamenoofprdlen,hasSameP,hasSamevolp)):
                        # May be different substrate or product or volume of Sub/prd may be different,
                        # Duplicating the reaction
                        if rSsubname and rSprdname:
                            allexists,allexistp = False,False
                            allexists = checkexist(rSsubname,objS,objD)
                            allexistp = checkexist(rSprdname,objS,objD)
                            if allexists and allexistp:
                                rs.name = rs.name+"_duplicated"
                                #reac = moose.Reac(comptA[key].path+'/'+rb.name+"_duplicated")
                                rdpath = rs.parent.path.replace(objS,objD)
                                reac = moose.copy(rs,moose.element(rdpath))
                                connectObj(reac,rSsubname,"sub",comptD,war_msg)
                                connectObj(reac,rSprdname,"prd",comptD,war_msg)
                                RE_Duplicated.append(reac)
                                allclean = True
                            else:
                                allclean = False
                    else:
                        allclean = True

                    if not allclean:
                        # didn't find sub or prd for this reaction
                        #   --  it may be connected Enzyme cplx
                        if rSsubname and rSprdname:
                            RE_Notcopiedyet.append(rs)
                        else:
                            RE_Daggling.append(rs)
    
    return RE_Duplicated,RE_Notcopiedyet,RE_Daggling

def subprdList(reac,subprd):
    rtype = moose.element(reac).neighbors[subprd]
    rname = []
    for rs in rtype:
        rname.append(moose.element(rs))
    return rname

def same_len_name_vol(rA,rB):
    uaS = set(rA)
    ubS = set(rB)
    aS = set([uas.name for uas in uaS])
    bS = set([ubs.name for ubs in ubS])

    hassameLen = False
    hassameSP  = False
    hassamevol = False
    hassamevollist = []
    if (len(rA) == len(rB) ):
        hassameLen = True
        if  not (len (aS.union(bS) - aS.intersection(bS))):
            hassameSP = True
            if rB and rA:
                rAdict = dict( [ (i.name,i) for i in (rA) ] )
                rBdict = dict( [ (i.name,i) for i in (rB) ] )

                for key,bpath in rBdict.items():
                    apath = rAdict[key]
                    comptA = moose.element(findCompartment(apath))
                    comptB = moose.element(findCompartment(bpath))
                    if not abs(comptA.volume -comptB.volume):
                            hassamevollist.append(True)
                    else:
                            hassamevollist.append(False)

    if len(set(hassamevollist))==1:
        for x in set(hassamevollist):
            hassamevol = x

    return ( hassameLen,hassameSP,hassamevol)

def connectObj(reac,spList,spType,comptA,war_msg):
    #It should not come here unless the sub/prd is connected to enzyme cplx pool
    allclean = False
    for rsp in spList:
        for akey in list(poolListina[findCompartment(rsp).name]):
            if rsp.name == akey.name:
                if moose.exists(akey.path):
                    moose.connect(moose.element(reac), spType, moose.element(akey), 'reac', 'OneToOne')
                    allclean = True
                else:
                    #It should not come here unless the sub/prd is connected to enzyme cplx pool
                    allclean = False
    return allclean

def checkexist(spList,objB,objA):
    allexistL = []
    allexist = False
    for rsp in spList:
        found = False
        rspPath = rsp.path.replace(objB,objA)
        if moose.exists(rspPath):
            found = True
        allexistL.append(found)

    if len(set(allexistL))==1:
        for x in set(allexistL):
            allexist = x

    return allexist

def findCompartment(element):
    while not mooseIsInstance(element,["CubeMesh","CyclMesh"]):
        element = element.parent
    return element

def mooseIsInstance(element, classNames):
    return moose.element(element).__class__.__name__ in classNames


if __name__ == "__main__":

    try:
        sys.argv[1]
    except IndexError:
        print("Source filename or path not given")
        exit(0)
    else:
        src = sys.argv[1]
        if not os.path.exists(src):
            print("Filename or path does not exist %s." %(src))
        else:
            try:
                sys.argv[2]
            except IndexError:
                print("Destination filename or path not given")
                exit(0)
            else:
                des = sys.argv[2]
                if not os.path.exists(src):
                    print("Filename or path does not exist %s." %(des))
                    exit(0)
                else:
                    print ("src and des %s, %s." %(src, des))
                    mergered = mergeChemModel(src,des)

                '''
                try:
                    sys.argv[3]
                except IndexError:
                    print ("Merge to save not specified")
                    mergeto = "merge"
                else:
                    mergeto = sys.argv[3]
                    if str(mergeto).rfind('.') != -1:
                        mergeto = mergeto[:str(mergeto).rfind('.')]
                    if str(mergeto).rfind('/'):
                        mergeto = mergeto+'merge'

                    mergered = mergeChemModel(src,des,mergeto)
                '''
