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
#Last-Updated: Thursday Jan 12 17:30:33 2017(+0530)
#         By: Harsha
#**********************************************************************/

# This program is used to merge models
# -- Model B is merged to modelA
#Rules are
#--In this models are mergered at group level (if exists)



import sys
import os
#from . import _moose as moose
import moose
import mtypes

from moose.chemUtil.chemConnectUtil import *
from moose.chemUtil.graphUtils import *

def mergeChemModel(A,B):
    """ Merges two model or the path """

    modelA,loadedA = loadModels(A)
    modelB,loadedB = loadModels(B)
    if not loadedA or not loadedB:
        if not loadedA:
            modelB = moose.Shell('/')
        if not loadedB:
            modelA = moose.Shell('/')
    else:
        directory, bfname = os.path.split(B)
        global grpNotcopiedyet,poolListina
        poolListina = {}
        grpNotcopiedyet = []
        dictComptA = dict( [ (i.name,i) for i in moose.wildcardFind(modelA+'/##[ISA=ChemCompt]') ] )
        dictComptB = dict( [ (i.name,i) for i in moose.wildcardFind(modelB+'/##[ISA=ChemCompt]') ] )

        poolNotcopiedyet = []

        for key in list(dictComptB.keys()):
            if key not in dictComptA:
                # if compartmentname from modelB does not exist in modelA, then copy
                copy = moose.copy(dictComptB[key],moose.element(modelA))
            else:
                #if compartmentname from modelB exist in modelA,
                #volume is not same, then change volume of ModelB same as ModelA
                if abs(dictComptA[key].volume - dictComptB[key].volume):
                    #hack for now
                    while (abs(dictComptA[key].volume - dictComptB[key].volume) != 0.0):
                        dictComptB[key].volume = float(dictComptA[key].volume)
                dictComptA = dict( [ (i.name,i) for i in moose.wildcardFind(modelA+'/##[ISA=ChemCompt]') ] )

                #Mergering pool
                poolMerge(dictComptA[key],dictComptB[key],poolNotcopiedyet)

        if grpNotcopiedyet:
            # objA = moose.element(comptA).parent.name
            # if not moose.exists(objA+'/'+comptB.name+'/'+bpath.name):
            #   print bpath
            #   moose.copy(bpath,moose.element(objA+'/'+comptB.name))
            pass

        comptAdict =  comptList(modelA)
        poolListina = {}
        poolListina = updatePoolList(comptAdict)
        funcNotallowed = []
        R_Duplicated, R_Notcopiedyet,R_Daggling = [], [], []
        E_Duplicated, E_Notcopiedyet,E_Daggling = [], [], []
        for key in list(dictComptB.keys()):
            funcNotallowed = []
            funcNotallowed = functionMerge(dictComptA,dictComptB,key)

            poolListina = updatePoolList(dictComptA)
            R_Duplicated,R_Notcopiedyet,R_Daggling = reacMerge(dictComptA,dictComptB,key,poolListina)

            poolListina = updatePoolList(dictComptA)
            E_Duplicated,E_Notcopiedyet,E_Daggling = enzymeMerge(dictComptA,dictComptB,key,poolListina)

        print("\n Model is merged to %s" %modelA)

        if funcNotallowed:
            print( "\nPool already connected to a function, this function is not to connect to same pool, since no two function are allowed to connect to same pool:")
            for fl in list(funcNotallowed):
                print("\t [Pool]:  %s [Function]:  %s \n" %(str(fl.parent.name), str(fl.path)))

        if R_Duplicated or E_Duplicated:
            print ("Reaction / Enzyme are Duplicate"
                    "\n 1. The once whoes substrate / product names are different for a give reaction name "
                    "\n 2. its compartment to which it belongs to may be is different"
                    "\n Models have to decide to keep or delete these reaction/enzyme")
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
            print ("\n Daggling reaction/enzyme are not not allowed in moose, these are not merged")
            if R_Daggling:
                print("Reaction: ")
                for rd in list(R_Daggling):
                    print ("%s " %str(rd.name))
            if E_Daggling:
                print ("Enzyme:")
                for ed in list(E_Daggling):
                    print ("%s " %str(ed.name))

def functionMerge(comptA,comptB,key):
    funcNotallowed = []
    comptApath = moose.element(comptA[key]).path
    comptBpath = moose.element(comptB[key]).path
    funcListina = moose.wildcardFind(comptApath+'/##[ISA=PoolBase]')
    funcListinb = moose.wildcardFind(comptBpath+'/##[ISA=Function]')
    objA = moose.element(comptApath).parent.name
    objB = moose.element(comptBpath).parent.name
    #For function mergering pool name is taken as reference
    funcNotcopiedyet = []
    for fb in funcListinb:

        if fb.parent.className in ['ZombiePool','Pool','ZombieBufPool','BufPool']:
            objA = moose.element(comptApath).parent.name
            fbpath = fb.path
            #funcpath = fbpath[fbpath.find(findCompartment(fb).name)-1:len(fbpath)]
            funcparentB = fb.parent.path
            funcpath = fbpath.replace(objB,objA)
            funcparentA = funcparentB.replace(objB,objA)
            tt = moose.element(funcparentA).neighbors['setN']
            if tt:
                funcNotallowed.append(fb)
            else:
                if len(moose.element(fb.path+'/x').neighbors["input"]):
                    #inputB = moose.element(fb.path+'/x').neighbors["input"]
                    inputB = subprdList(moose.element(fb.path+'/x'),"input")
                    inputB_expr = fb.expr
                    if moose.exists(funcpath):
                        #inputA = moose.element(objA+funcpath+'/x').neighbors["input"]
                        inputA = subprdList(moose.element(funcpath+'/x'),"input")
                        inputA_expr = moose.element(funcpath).expr
                        hassameExpr = False
                        if inputA_expr == inputB_expr:
                            hassameExpr = True
                        hassameLen,hassameS,hassameVols = same_len_name_vol(inputA,inputB)
                        if not all((hassameLen,hassameS,hassameVols,hassameExpr)):
                            fb.name = fb.name+'_duplicatedF'
                            createFunction(fb,inputB,objB,objA)
                    else:
                        #function doesnot exist then copy
                        if len(inputB):
                            volinput = []
                            for inb in inputB:
                                volinput.append(findCompartment(moose.element(inb)).volume)
                                if len(set(volinput)) == 1:
                                    # If all the input connected belongs to one compartment then copy
                                    createFunction(fb,inputB,objB,objA)
                                else:
                                    # moose doesn't allow function input to come from different compartment
                                    funcNotallowed.append(fb)
    return funcNotallowed

def createFunction(fb,inputB,objB,objA):
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
        moose.connect(des, 'valueOut', moose.element(fapath).parent,'setN' )
        for src in inputB:
            pool = ((src.path).replace(objB,objA)).replace('[0]','')
            numVariables = des.numVars
            expr = ""
            expr = (des.expr+'+'+'x'+str(numVariables))
            expr = expr.lstrip("0 +")
            expr = expr.replace(" ","")
            des.expr = expr
            moose.connect( pool, 'nOut', des.x[numVariables], 'input' )

        #if fb.expr != des.expr:
        #    print "Function ",des, " which is duplicated from modelB, expression is different, this is tricky in moose to know what those constants are connected to "
        #    print "ModelB ", fb, fb.expr, "\nModelA ",des, des.expr

def comptList(modelpath):
    comptdict = {}
    for ca in moose.wildcardFind(modelpath+'/##[ISA=ChemCompt]'):
        comptdict[ca.name] = ca
    return comptdict

def loadModels(filename):
    """ load models into moose if file, if moosepath itself it passes back the path and
    delete solver if exist """

    modelpath = '/'
    loaded = False

    if os.path.isfile(filename) :
        modelpath = filename[filename.rfind('/'): filename.rfind('.')]
        ext = os.path.splitext(filename)[1]
        filename = filename.strip()
        modeltype = mtypes.getType(filename)
        subtype = mtypes.getSubtype(filename, modeltype)
        if subtype == 'kkit' or modeltype == "cspace":
            moose.loadModel(filename,modelpath)
            loaded = True

        elif subtype == 'sbml':
            #moose.ReadSBML()
            pass
        else:
            print("This file is not supported for mergering")
            modelpath = moose.Shell('/')
    elif moose.exists(filename):
        modelpath = filename
        loaded = True
    ## default is 'ee' solver while loading the model using moose.loadModel,
    ## yet deleteSolver is called just to be assured
    if loaded:
        deleteSolver(modelpath)

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

def poolMerge(comptA,comptB,poolNotcopiedyet):

    aCmptGrp = moose.wildcardFind(comptA.path+'/#[TYPE=Neutral]')
    aCmptGrp = aCmptGrp +(moose.element(comptA.path),)

    bCmptGrp = moose.wildcardFind(comptB.path+'/#[TYPE=Neutral]')
    bCmptGrp = bCmptGrp +(moose.element(comptB.path),)

    objA = moose.element(comptA.path).parent.name
    objB = moose.element(comptB.path).parent.name
    for bpath in bCmptGrp:
        grp_cmpt = ((bpath.path).replace(objB,objA)).replace('[0]','')
        if moose.exists(grp_cmpt) :
            if moose.element(grp_cmpt).className != bpath.className:
                grp_cmpt = grp_cmpt+'_grp'
                bpath.name = bpath.name+"_grp"
                l = moose.Neutral(grp_cmpt)
        else:
            moose.Neutral(grp_cmpt)

        apath = moose.element(bpath.path.replace(objB,objA))

        bpoollist = moose.wildcardFind(bpath.path+'/#[ISA=PoolBase]')
        apoollist = moose.wildcardFind(apath.path+'/#[ISA=PoolBase]')
        for bpool in bpoollist:
            if bpool.name not in [apool.name for apool in apoollist]:
                copied = copy_deleteUnlyingPoolObj(bpool,apath)
                if copied == False:
                    #hold it for later, this pool may be under enzyme, as cplx
                    poolNotcopiedyet.append(bpool)

def copy_deleteUnlyingPoolObj(pool,path):
    # check if this pool is under compartement or under enzyme?(which is enzyme_cplx)
    # if enzyme_cplx then don't copy untill this perticular enzyme is copied
    # case: This enzyme_cplx might exist in modelA if enzyme exist
    # which will automatically copie's the pool
    copied = False

    if pool.parent.className not in ["Enz","ZombieEnz"]:
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

def enzymeMerge(comptA,comptB,key,poolListina):
    war_msg = ""
    RE_Duplicated, RE_Notcopiedyet, RE_Daggling = [], [], []
    comptApath = moose.element(comptA[key]).path
    comptBpath = moose.element(comptB[key]).path
    objA = moose.element(comptApath).parent.name
    objB = moose.element(comptBpath).parent.name
    enzyListina = moose.wildcardFind(comptApath+'/##[ISA=EnzBase]')
    enzyListinb = moose.wildcardFind(comptBpath+'/##[ISA=EnzBase]')
    for eb in enzyListinb:
        eBsubname, eBprdname = [],[]
        eBsubname = subprdList(eb,"sub")
        eBprdname = subprdList(eb,"prd")
        allexists, allexistp = False, False
        allclean = False

        poolinAlist = poolListina[findCompartment(eb).name]
        for pA in poolinAlist:
            if eb.parent.name == pA.name:
                eapath = eb.parent.path.replace(objB,objA)

                if not moose.exists(eapath+'/'+eb.name):
                    #This will take care
                    #  -- If same enzparent name but different enzyme name
                    #  -- or different parent/enzyme name
                    if eBsubname and eBprdname:
                        allexists = checkexist(eBsubname,objB,objA)
                        allexistp = checkexist(eBprdname,objB,objA)
                        if allexists and allexistp:
                            enzPool = moose.element(pA.path)
                            eapath = eb.parent.path.replace(objB,objA)
                            enz = moose.element(moose.copy(eb,moose.element(eapath)))
                            enzPool = enz.parent
                            if eb.className in ["ZombieEnz","Enz"]:
                                moose.connect(moose.element(enz),"enz",enzPool,"reac")
                            if eb.className in ["ZombieMMenz","MMenz"]:
                                moose.connect(enzPool,"nOut",enz,"enzDest")
                            connectObj(enz,eBsubname,"sub",comptA,war_msg)
                            connectObj(enz,eBprdname,"prd",comptA,war_msg)
                            allclean = True
                        else:
                            # didn't find sub or prd for this Enzyme
                            #print ("didn't find sub or prd for this reaction" )
                            RE_Notcopiedyet.append(eb)
                    else:
                        #   -- it is dagging reaction
                        RE_Daggling.append(eb)
                        #print ("This reaction \""+eb.path+"\" has no substrate/product daggling reaction are not copied")
                        #war_msg = war_msg+"\nThis reaction \""+eb.path+"\" has no substrate/product daggling reaction are not copied"
                else:
                    #Same Enzyme name
                    #   -- Same substrate and product including same volume then don't copy
                    #   -- different substrate/product or if sub/prd's volume is different then DUPLICATE the Enzyme
                    allclean = False
                    #ea = moose.element('/'+obj+'/'+enzcompartment.name+'/'+enzparent.name+'/'+eb.name)
                    #ea = moose.element(pA.path+'/'+eb.name)
                    ea = moose.element(eb.path.replace(objB,objA))
                    eAsubname = subprdList(ea,"sub")
                    eBsubname = subprdList(eb,"sub")
                    hasSamenoofsublen,hasSameS,hasSamevols = same_len_name_vol(eAsubname,eBsubname)

                    eAprdname = subprdList(ea,"prd")
                    eBprdname = subprdList(eb,"prd")
                    hasSamenoofprdlen,hasSameP,hasSamevolp = same_len_name_vol(eAprdname,eBprdname)
                    if not all((hasSamenoofsublen,hasSameS,hasSamevols,hasSamenoofprdlen,hasSameP,hasSamevolp)):
                        # May be different substrate or product or volume of Sub/prd may be different,
                        # Duplicating the enzyme
                        if eBsubname and eBprdname:
                            allexists,allexistp = False,False
                            allexists = checkexist(eBsubname,objB,objA)
                            allexistp = checkexist(eBprdname,objB,objA)
                            if allexists and allexistp:
                                eb.name = eb.name+"_duplicated"
                                if eb.className in ["ZombieEnz","Enz"]:
                                    eapath = eb.parent.path.replace(objB,objA)
                                    enz = moose.copy(eb,moose.element(eapath))
                                    moose.connect(enz, 'enz', eapath, 'reac' )

                                if eb.className in ["ZombieMMenz","MMenz"]:
                                    eapath = eb.parent.path.replace(objB,objA)
                                    enz = moose.copy(eb.name,moose.element(eapath))
                                    enzinfo = moose.Annotator(enz.path+'/info')
                                    moose.connect(moose.element(enz).parent,"nOut",moose.element(enz),"enzDest")
                                    #moose.connect(moose.element(enz),"enz",moose.element(enz).parent,"reac")

                                #moose.connect( cplxItem, 'reac', enz, 'cplx' )
                                connectObj(enz,eBsubname,"sub",comptA,war_msg)
                                connectObj(enz,eBprdname,"prd",comptA,war_msg)
                                RE_Duplicated.append(enz)
                                allclean = True
                            else:
                                allclean = False
                    else:
                        allclean = True

                    if not allclean:
                        # didn't find sub or prd for this enzyme
                        #   --  it may be connected Enzyme cplx
                        if eBsubname and eBprdname:
                            RE_Notcopiedyet.append(eb)
                            #print ("This Enzyme \""+eb.path+"\" has no substrate/product must be connect to cplx")
                            #war_msg = war_msg+ "\nThis Enzyme \""+rb.path+"\" has no substrate/product must be connect to cplx"
                        else:
                            RE_Daggling.append(eb)
                            #print ("This enzyme \""+eb.path+"\" has no substrate/product daggling reaction are not copied")
                            #war_msg = war_msg+"\nThis reaction \""+eb.path+"\" has no substrate/product daggling reaction are not copied"
    return RE_Duplicated,RE_Notcopiedyet,RE_Daggling

def reacMerge(comptA,comptB,key,poolListina):
    RE_Duplicated, RE_Notcopiedyet, RE_Daggling = [], [], []
    war_msg = ""
    comptApath = moose.element(comptA[key]).path
    comptBpath = moose.element(comptB[key]).path
    objA = moose.element(comptApath).parent.name
    objB = moose.element(comptBpath).parent.name
    reacListina = moose.wildcardFind(comptApath+'/##[ISA=ReacBase]')
    reacListinb = moose.wildcardFind(comptBpath+'/##[ISA=ReacBase]')
    for rb in reacListinb:
        rBsubname, rBprdname = [],[]
        rBsubname = subprdList(rb,"sub")
        rBprdname = subprdList(rb,"prd")
        allexists, allexistp = False, False
        allclean = False

        if rb.name not in [ra.name for ra in reacListina]:
            # reaction name not found then copy
            # And assuming that pools are copied earlier EXPECT POOL CPLX
            #To be assured the it takes correct compartment name incase reaction sub's
            #belongs to different compt
            key = findCompartment(rb).name
            if rBsubname and rBprdname:
                allexists =  checkexist(rBsubname,objB,objA)
                allexistp = checkexist(rBprdname,objB,objA)
                if allexists and allexistp:
                    rapath = rb.parent.path.replace(objB,objA)
                    reac = moose.copy(rb,moose.element(rapath))
                    connectObj(reac,rBsubname,"sub",comptA,war_msg)
                    connectObj(reac,rBprdname,"prd",comptA,war_msg)
                    allclean = True
                else:
                    # didn't find sub or prd for this reaction
                    #   --  it may be connected Enzyme cplx
                    RE_Notcopiedyet.append(rb)
            else:
                #   -- it is dagging reaction
                RE_Daggling.append(rb)
                #print ("This reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied")
                #war_msg = war_msg+"\nThis reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied"

        else:
            #Same reaction name
            #   -- Same substrate and product including same volume then don't copy
            #   -- different substrate/product or if sub/prd's volume is different then DUPLICATE the reaction
            allclean = False
            for ra in reacListina:
                if rb.name == ra.name:
                    rAsubname = subprdList(ra,"sub")
                    rBsubname = subprdList(rb,"sub")
                    hasSamenoofsublen,hasSameS,hasSamevols = same_len_name_vol(rAsubname,rBsubname)

                    rAprdname = subprdList(ra,"prd")
                    rBprdname = subprdList(rb,"prd")
                    hasSamenoofprdlen,hasSameP,hasSamevolp = same_len_name_vol(rAprdname,rBprdname)
                    if not all((hasSamenoofsublen,hasSameS,hasSamevols,hasSamenoofprdlen,hasSameP,hasSamevolp)):
                        # May be different substrate or product or volume of Sub/prd may be different,
                        # Duplicating the reaction
                        if rBsubname and rBprdname:
                            allexists,allexistp = False,False
                            allexists = checkexist(rBsubname,objB,objA)
                            allexistp = checkexist(rBprdname,objB,objA)
                            if allexists and allexistp:
                                rb.name = rb.name+"_duplicated"
                                #reac = moose.Reac(comptA[key].path+'/'+rb.name+"_duplicated")
                                rapath = rb.parent.path.replace(objB,objA)
                                reac = moose.copy(rb,moose.element(rapath))
                                connectObj(reac,rBsubname,"sub",comptA,war_msg)
                                connectObj(reac,rBprdname,"prd",comptA,war_msg)
                                RE_Duplicated.append(reac)
                                allclean = True
                            else:
                                allclean = False
                    else:
                        allclean = True

                    if not allclean:
                        # didn't find sub or prd for this reaction
                        #   --  it may be connected Enzyme cplx
                        if rBsubname and rBprdname:
                            RE_Notcopiedyet.append(rb)
                            #print ("This reaction \""+rb.path+"\" has no substrate/product must be connect to cplx")
                            #war_msg = war_msg+ "\nThis reaction \""+rb.path+"\" has no substrate/product must be connect to cplx"
                        else:
                            RE_Daggling.append(rb)
                            #print ("This reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied")
                            #war_msg = war_msg+"\nThis reaction \""+rb.path+"\" has no substrate/product daggling reaction are not copied"

    return RE_Duplicated,RE_Notcopiedyet,RE_Daggling

def subprdList(reac,subprd):
    #print  "Reac ",reac
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
                    #print ("This pool \""+rsp.name+"\" doesnot exists in this "+comptName+" compartment to connect to this reaction \""+reac.name+"\"")
                    #war_msg = war_msg+ "This pool \""+rsp.name+"\" doesnot exists in this "+comptName+" compartment to connect to this reaction \""+reac.name+"\""
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

    modelA = '/home/harsha/genesis_files/gfile/acc92.g'
    modelB = '/home/harsha/genesis_files/gfile/acc50.g'
    mergered = mergeChemModel(modelA,modelB)
