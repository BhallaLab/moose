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
**           copyright (C) 2003-2017 Upinder S. Bhalla. and NCBS
Created : Thu May 12 10:19:00 2016(+0530)
Version
Last-Updated: Tue Feb 10 2017
          By:
**********************************************************************/

'''

import sys
import os.path
import collections
import moose
from moose.SBML.validation import validateModel
import re

foundLibSBML_ = False
try:
    import libsbml
    foundLibSBML_ = True
except ImportError:
    pass


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


def mooseReadSBML(filepath, loadpath, solver="ee"):
    """Load SBML model 
    """
    global foundLibSBML_
    if not foundLibSBML_:
        print('[FATAL] Module python-libsbml is not found.' 
            '\nThis module can be installed by following command in terminal:'
            '\n\t easy_install python-libsbml'
            # NOTE: Not available on most DEBIAN.
            # '\n\t apt-get install python-libsbml'. 
            )
        return None

    if not os.path.isfile(filepath):
        print('[FATAL] Model file %s is not found ' % filepath)
        return None

    with open(filepath, "r") as filep:
        filep = open(filepath, "r")
        document = libsbml.readSBML(filepath)
        tobecontinue = False
        tobecontinue = validateModel(document)
        if tobecontinue:
            level = document.getLevel()
            version = document.getVersion()
            print(("\n" + "File: " + filepath + " (Level " +
                   str(level) + ", version " + str(version) + ")"))
            model = document.getModel()
            if (model is None):
                print("No model present.")
                return moose.element('/')
            else:
                print((" model: " + str(model)))
                print(("functionDefinitions: " +
                       str(model.getNumFunctionDefinitions())))
                print(("    unitDefinitions: " +
                       str(model.getNumUnitDefinitions())))
                print(("   compartmentTypes: " +
                       str(model.getNumCompartmentTypes())))
                print(("        specieTypes: " +
                       str(model.getNumSpeciesTypes())))
                print(("       compartments: " +
                       str(model.getNumCompartments())))
                print(("            species: " +
                       str(model.getNumSpecies())))
                print(("         parameters: " +
                       str(model.getNumParameters())))
                print((" initialAssignments: " +
                       str(model.getNumInitialAssignments())))
                print(("              rules: " +
                       str(model.getNumRules())))
                print(("        constraints: " +
                       str(model.getNumConstraints())))
                print(("          reactions: " +
                       str(model.getNumReactions())))
                print(("             events: " +
                       str(model.getNumEvents())))
                print("\n")

                if (model.getNumCompartments() == 0):
                    return moose.element('/')
                else:
                    baseId = moose.Neutral(loadpath)
                    basePath = baseId
                    # All the model will be created under model as
                    # a thumbrule
                    basePath = moose.Neutral(baseId.path + '/model')
                    # Map Compartment's SBML id as key and value is
                    # list of[ Moose ID and SpatialDimensions ]
                    global comptSbmlidMooseIdMap
                    global warning
                    warning = " "
                    global msg
                    msg = " "
                    comptSbmlidMooseIdMap = {}
                    #print(("modelPath:" + basePath.path))
                    globparameterIdValue = {}
                    modelAnnotaInfo = {}
                    mapParameter(model, globparameterIdValue)
                    errorFlag = createCompartment(
                        basePath, model, comptSbmlidMooseIdMap)
                    if errorFlag:
                        specInfoMap = {}
                        errorFlag,warning = createSpecies(
                            basePath, model, comptSbmlidMooseIdMap, specInfoMap, modelAnnotaInfo)
                        #print(errorFlag,warning)
                        
                        if errorFlag:
                            errorFlag = createRules(
                                model, specInfoMap, globparameterIdValue)
                            if errorFlag:
                                errorFlag, msg = createReaction(
                                    model, specInfoMap, modelAnnotaInfo, globparameterIdValue)
                        getModelAnnotation(
                            model, baseId, basePath)
                        
                    if not errorFlag:
                        print(msg)
                        # Any time in the middle if SBML does not read then I
                        # delete everything from model level This is important
                        # as while reading in GUI the model will show up untill
                        # built which is not correct print "Deleted rest of the
                        # model"
                        moose.delete(basePath)
                        basePath = moose.Shell('/')
            return basePath
        else:
            print("Validation failed while reading the model.")
            return moose.element('/')


def setupEnzymaticReaction(enz, groupName, enzName,
                           specInfoMap, modelAnnotaInfo):
    enzPool = (modelAnnotaInfo[groupName]["enzyme"])
    enzPool = str(idBeginWith(enzPool))
    enzParent = specInfoMap[enzPool]["Mpath"]
    cplx = (modelAnnotaInfo[groupName]["complex"])
    cplx = str(idBeginWith(cplx))
    complx = moose.element(specInfoMap[cplx]["Mpath"].path)
    enzyme_ = moose.Enz(enzParent.path + '/' + enzName)
    moose.move(complx, enzyme_)
    moose.connect(enzyme_, "cplx", complx, "reac")
    moose.connect(enzyme_, "enz", enzParent, "reac")

    sublist = (modelAnnotaInfo[groupName]["substrate"])
    prdlist = (modelAnnotaInfo[groupName]["product"])

    for si in range(0, len(sublist)):
        sl = sublist[si]
        sl = str(idBeginWith(sl))
        mSId = specInfoMap[sl]["Mpath"]
        moose.connect(enzyme_, "sub", mSId, "reac")

    for pi in range(0, len(prdlist)):
        pl = prdlist[pi]
        pl = str(idBeginWith(pl))
        mPId = specInfoMap[pl]["Mpath"]
        moose.connect(enzyme_, "prd", mPId, "reac")

    if (enz.isSetNotes):
        pullnotes(enz, enzyme_)

    return enzyme_, True


def addSubPrd(reac, reName, type, reactSBMLIdMooseId, specInfoMap):
    rctMapIter = {}
    if (type == "sub"):
        noplusStoichsub = 0
        addSubinfo = collections.OrderedDict()
        for rt in range(0, reac.getNumReactants()):
            rct = reac.getReactant(rt)
            sp = rct.getSpecies()
            rctMapIter[sp] = rct.getStoichiometry()
            if rct.getStoichiometry() > 1:
                pass
                # print " stoich ",reac.name,rct.getStoichiometry()
            noplusStoichsub = noplusStoichsub + rct.getStoichiometry()
        for key, value in list(rctMapIter.items()):
            key = str(idBeginWith(key))
            src = specInfoMap[key]["Mpath"]
            des = reactSBMLIdMooseId[reName]["MooseId"]
            for s in range(0, int(value)):
                moose.connect(des, 'sub', src, 'reac', 'OneToOne')
        addSubinfo = {"nSub": noplusStoichsub}
        reactSBMLIdMooseId[reName].update(addSubinfo)

    else:
        noplusStoichprd = 0
        addPrdinfo = collections.OrderedDict()
        for rt in range(0, reac.getNumProducts()):
            rct = reac.getProduct(rt)
            sp = rct.getSpecies()
            rctMapIter[sp] = rct.getStoichiometry()
            if rct.getStoichiometry() > 1:
                pass
                # print " stoich prd",reac.name,rct.getStoichiometry()
            noplusStoichprd = noplusStoichprd + rct.getStoichiometry()

        for key, values in list(rctMapIter.items()):
            # src ReacBase
            src = reactSBMLIdMooseId[reName]["MooseId"]
            key = parentSp = str(idBeginWith(key))
            des = specInfoMap[key]["Mpath"]
            for i in range(0, int(values)):
                moose.connect(src, 'prd', des, 'reac', 'OneToOne')
        addPrdinfo = {"nPrd": noplusStoichprd}
        reactSBMLIdMooseId[reName].update(addPrdinfo)


def populatedict(annoDict, label, value):
    if label in annoDict:
        annoDict.setdefault(label, [])
        annoDict[label].update({value})
    else:
        annoDict[label] = {value}


def getModelAnnotation(obj, baseId, basepath):
    annotationNode = obj.getAnnotation()
    if annotationNode is not None:
        numchild = annotationNode.getNumChildren()
        for child_no in range(0, numchild):
            childNode = annotationNode.getChild(child_no)
            if (childNode.getPrefix() ==
                    "moose" and childNode.getName() == "ModelAnnotation"):
                num_gchildren = childNode.getNumChildren()
                for gchild_no in range(0, num_gchildren):
                    grandChildNode = childNode.getChild(gchild_no)
                    nodeName = grandChildNode.getName()
                    if (grandChildNode.getNumChildren() == 1):
                        baseinfo = moose.Annotator(baseId.path + '/info')
                        baseinfo.modeltype = "xml"
                        if nodeName == "runTime":
                            runtime = float(
                                (grandChildNode.getChild(0).toXMLString()))
                            baseinfo.runtime = runtime
                        if nodeName == "solver":
                            solver = (grandChildNode.getChild(0).toXMLString())
                            baseinfo.solver = solver
                        if(nodeName == "plots"):
                            plotValue = (
                                grandChildNode.getChild(0).toXMLString())
                            p = moose.element(baseId)
                            datapath = moose.element(baseId).path + "/data"
                            if not moose.exists(datapath):
                                datapath = moose.Neutral(baseId.path + "/data")
                                graph = moose.Neutral(
                                    datapath.path + "/graph_0")
                                plotlist = plotValue.split(";")
                                tablelistname = []
                                for plots in plotlist:
                                    plots = plots.replace(" ", "")
                                    plotorg = plots
                                    if( moose.exists(basepath.path + plotorg) and isinstance(moose.element(basepath.path+plotorg),moose.PoolBase)) :
                                        plotSId = moose.element(
                                            basepath.path + plotorg)
                                        # plotorg = convertSpecialChar(plotorg)
                                        plot2 = plots.replace('/', '_')
                                        plot3 = plot2.replace('[', '_')
                                        plotClean = plot3.replace(']', '_')
                                        plotName = plotClean + ".conc"
                                        fullPath = graph.path + '/' + \
                                            plotName.replace(" ", "")
                                        # If table exist with same name then
                                        # its not created
                                        if not fullPath in tablelistname:
                                            tab = moose.Table2(fullPath)
                                            tablelistname.append(fullPath)
                                            moose.connect(tab, "requestOut", plotSId, "getConc")


def getObjAnnotation(obj, modelAnnotationInfo):
    name = obj.getId()
    name = name.replace(" ", "_space_")
    # modelAnnotaInfo= {}
    annotateMap = {}
    if (obj.getAnnotation() is not None):
        annoNode = obj.getAnnotation()
        for ch in range(0, annoNode.getNumChildren()):
            childNode = annoNode.getChild(ch)
            if (childNode.getPrefix() == "moose" and (childNode.getName() ==
                                                      "ModelAnnotation" or childNode.getName() == "EnzymaticReaction")):
                sublist = []
                for gch in range(0, childNode.getNumChildren()):
                    grandChildNode = childNode.getChild(gch)
                    nodeName = grandChildNode.getName()
                    nodeValue = ""
                    if (grandChildNode.getNumChildren() == 1):
                        nodeValue = grandChildNode.getChild(0).toXMLString()
                    else:
                        print(
                            "Error: expected exactly ONE child of ", nodeName)

                    if nodeName == "xCord":
                        annotateMap[nodeName] = nodeValue
                    if nodeName == "yCord":
                        annotateMap[nodeName] = nodeValue
                    if nodeName == "bgColor":
                        annotateMap[nodeName] = nodeValue
                    if nodeName == "textColor":
                        annotateMap[nodeName] = nodeValue
                    if nodeName == "Group":
                        annotateMap[nodeName] = nodeValue
    return annotateMap


def getEnzAnnotation(obj, modelAnnotaInfo, rev,
                     globparameterIdValue, specInfoMap):
    name = obj.getId()
    name = name.replace(" ", "_space_")
    # modelAnnotaInfo= {}
    annotateMap = {}
    if (obj.getAnnotation() is not None):
        annoNode = obj.getAnnotation()
        for ch in range(0, annoNode.getNumChildren()):
            childNode = annoNode.getChild(ch)
            if (childNode.getPrefix() ==
                    "moose" and childNode.getName() == "EnzymaticReaction"):
                sublist = []
                for gch in range(0, childNode.getNumChildren()):
                    grandChildNode = childNode.getChild(gch)
                    nodeName = grandChildNode.getName()
                    nodeValue = ""
                    if (grandChildNode.getNumChildren() == 1):
                        nodeValue = grandChildNode.getChild(0).toXMLString()
                    else:
                        print(
                            "Error: expected exactly ONE child of ", nodeName)

                    if nodeName == "enzyme":
                        populatedict(annotateMap, "enzyme", nodeValue)

                    elif nodeName == "complex":
                        populatedict(annotateMap, "complex", nodeValue)
                    elif (nodeName == "substrates"):
                        populatedict(annotateMap, "substrates", nodeValue)
                    elif (nodeName == "product"):
                        populatedict(annotateMap, "product", nodeValue)
                    elif (nodeName == "groupName"):
                        populatedict(annotateMap, "grpName", nodeValue)
                    elif (nodeName == "stage"):
                        populatedict(annotateMap, "stage", nodeValue)
                    elif (nodeName == "Group"):
                        populatedict(annotateMap, "group", nodeValue)
                    elif (nodeName == "xCord"):
                        populatedict(annotateMap, "xCord", nodeValue)
                    elif (nodeName == "yCord"):
                        populatedict(annotateMap, "yCord", nodeValue)
    groupName = ""
    if 'grpName' in annotateMap:
        groupName = list(annotateMap["grpName"])[0]
        klaw = obj.getKineticLaw()
        mmsg = ""
        errorFlag, mmsg, k1, k2 = getKLaw(
            obj, klaw, rev, globparameterIdValue, specInfoMap)

        if 'substrates' in annotateMap:
            sublist = list(annotateMap["substrates"])
        else:
            sublist = {}
        if 'product' in annotateMap:
            prdlist = list(annotateMap["product"])
        else:
            prdlist = {}

        if list(annotateMap["stage"])[0] == '1':
            if groupName in modelAnnotaInfo:
                modelAnnotaInfo[groupName].update(
                    {"enzyme": list(annotateMap["enzyme"])[0],
                     "stage": list(annotateMap["stage"])[0],
                     "substrate": sublist,
                     "k1": k1,
                     "k2": k2
                     }
                )
            else:
                modelAnnotaInfo[groupName] = {
                    "enzyme": list(annotateMap["enzyme"])[0],
                    "stage": list(annotateMap["stage"])[0],
                    "substrate": sublist,
                    "k1": k1,
                    "k2": k2
                    #"group" : list(annotateMap["Group"])[0],
                    #"xCord" : list(annotateMap["xCord"])[0],
                    #"yCord" : list(annotateMap["yCord"]) [0]
                }

        elif list(annotateMap["stage"])[0] == '2':
            if groupName in modelAnnotaInfo:
                stage = int(modelAnnotaInfo[groupName][
                            "stage"]) + int(list(annotateMap["stage"])[0])
                modelAnnotaInfo[groupName].update(
                    {"complex": list(annotateMap["complex"])[0],
                     "product": prdlist,
                     "stage": [stage],
                     "k3": k1
                     }
                )
            else:
                modelAnnotaInfo[groupName] = {
                    "complex": list(annotateMap["complex"])[0],
                    "product": prdlist,
                    "stage": [stage],
                    "k3": k1
                }
    return(groupName)


def createReaction(model, specInfoMap, modelAnnotaInfo, globparameterIdValue):
    # print " reaction "
    # Things done for reaction
    # --Reaction is not created, if substrate and product is missing
    # --Reaction is created under first substrate's compartment if substrate not found then product
    # --Reaction is created if substrate or product is missing, but while run time in GUI atleast I have stopped
    # ToDo
    # -- I need to check here if any substance/product is if ( constant == true && bcondition == false)
    # cout <<"The species "<< name << " should not appear in reactant or product as per sbml Rules"<< endl;

    errorFlag = True
    reactSBMLIdMooseId = {}
    msg = ""
    rName = ""
    reaction_ = None

    for ritem in range(0, model.getNumReactions()):
        reactionCreated = False
        groupName = ""

        reac = model.getReaction(ritem)
        group = ""
        reacAnnoInfo = {}
        reacAnnoInfo = getObjAnnotation(reac, modelAnnotaInfo)
        if "Group" in reacAnnoInfo:
            group = reacAnnoInfo["Group"]

        if (reac.isSetId()):
            rId = reac.getId()
        if (reac.isSetName()):
            rName = reac.getName()
            rName = rName.replace(" ", "_space_")
        if not(rName):
            rName = rId
        rev = reac.getReversible()
        fast = reac.getFast()
        if (fast):
            print(
                " warning: for now fast attribute is not handled \"",
                rName,
                "\"")
        if (reac.getAnnotation() is not None):
            groupName = getEnzAnnotation(
                reac, modelAnnotaInfo, rev, globparameterIdValue, specInfoMap)

        if (groupName != "" and list(
                modelAnnotaInfo[groupName]["stage"])[0] == 3):
            reaction_, reactionCreated = setupEnzymaticReaction(
                reac, groupName, rName, specInfoMap, modelAnnotaInfo)
            reaction_.k3 = modelAnnotaInfo[groupName]['k3']
            reaction_.k2 = modelAnnotaInfo[groupName]['k2']
            reaction_.concK1 = modelAnnotaInfo[groupName]['k1']
            if reactionCreated:
                if (reac.isSetNotes):
                    pullnotes(reac, reaction_)
                    reacAnnoInfo = {}
                reacAnnoInfo = getObjAnnotation(reac, modelAnnotaInfo)
                if reacAnnoInfo:
                    if not moose.exists(reaction_.path + '/info'):
                        reacInfo = moose.Annotator(reaction_.path + '/info')
                    else:
                        reacInfo = moose.element(reaction_.path + '/info')
                    for k, v in list(reacAnnoInfo.items()):
                        if k == 'xCord':
                            reacInfo.x = float(v)
                        elif k == 'yCord':
                            reacInfo.y = float(v)
                        elif k == 'bgColor':
                            reacInfo.color = v
                        else:
                            reacInfo.textColor = v

        elif(groupName == ""):
            numRcts = reac.getNumReactants()
            numPdts = reac.getNumProducts()
            nummodifiers = reac.getNumModifiers()

            if not (numRcts and numPdts):
                print("Warning: %s" %(rName)," : Substrate or Product is missing, we will be skiping creating this reaction in MOOSE")
                reactionCreated = False
            elif (reac.getNumModifiers() > 0):
                reactionCreated, reaction_ = setupMMEnzymeReaction(
                    reac, rName, specInfoMap, reactSBMLIdMooseId, modelAnnotaInfo, model, globparameterIdValue)
            # elif (reac.getNumModifiers() > 0):
            #     reactionCreated = setupMMEnzymeReaction(reac,rName,specInfoMap,reactSBMLIdMooseId,modelAnnotaInfo,model,globparameterIdValue)
            #     reaction_ = reactSBMLIdMooseId['classical']['MooseId']
            #     reactionType = "MMEnz"
            elif (numRcts):
                # In moose, reactions compartment are decided from first Substrate compartment info
                # substrate is missing then check for product
                if (reac.getNumReactants()):
                    react = reac.getReactant(reac.getNumReactants() - 1)
                    # react = reac.getReactant(0)
                    sp = react.getSpecies()
                    sp = str(idBeginWith(sp))
                    speCompt = specInfoMap[sp]["comptId"].path
                    if group:
                        if moose.exists(speCompt+'/'+group):
                            speCompt = speCompt+'/'+group
                        else:
                            speCompt = (moose.Neutral(speCompt+'/'+group)).path
                    reaction_ = moose.Reac(speCompt + '/' + rName)
                    reactionCreated = True
                    reactSBMLIdMooseId[rName] = {
                        "MooseId": reaction_, "className ": "reaction"}
            elif (numPdts):
                # In moose, reactions compartment are decided from first Substrate compartment info
                # substrate is missing then check for product
                if (reac.getNumProducts()):
                    react = reac.getProducts(0)
                    sp = react.getSpecies()
                    sp = str(idBeginWith(sp))
                    speCompt = specInfoMap[sp]["comptId"].path
                    reaction_ = moose.Reac(speCompt + '/' + rName)
                    reactionCreated = True
                    reactSBMLIdMooseId[rId] = {
                        "MooseId": reaction_, "className": "reaction"}
            if reactionCreated:
                if (reac.isSetNotes):
                    pullnotes(reac, reaction_)
                    reacAnnoInfo = {}
                reacAnnoInfo = getObjAnnotation(reac, modelAnnotaInfo)
                if reacAnnoInfo:
                    if not moose.exists(reaction_.path + '/info'):
                        reacInfo = moose.Annotator(reaction_.path + '/info')
                    else:
                        reacInfo = moose.element(reaction_.path + '/info')
                    for k, v in list(reacAnnoInfo.items()):
                        if k == 'xCord':
                            reacInfo.x = float(v)
                        elif k == 'yCord':
                            reacInfo.y = float(v)
                        elif k == 'bgColor':
                            reacInfo.color = v
                        else:
                            reacInfo.textColor = v

                addSubPrd(reac, rName, "sub", reactSBMLIdMooseId, specInfoMap)
                addSubPrd(reac, rName, "prd", reactSBMLIdMooseId, specInfoMap)
                if reac.isSetKineticLaw():
                    klaw = reac.getKineticLaw()
                    mmsg = ""
                    errorFlag, mmsg, kfvalue, kbvalue = getKLaw(
                        model, klaw, rev, globparameterIdValue, specInfoMap)
                    if not errorFlag:
                        msg = "Error while importing reaction \"" + \
                            rName + "\"\n Error in kinetics law "
                        if mmsg != "":
                            msg = msg + mmsg
                        return(errorFlag, msg)
                    else:
                        # print " reactSBMLIdMooseId
                        # ",reactSBMLIdMooseId[rName]["nSub"], " prd
                        # ",reactSBMLIdMooseId[rName]["nPrd"]
                        if reaction_.className == "Reac":
                            subn = reactSBMLIdMooseId[rName]["nSub"]
                            prdn = reactSBMLIdMooseId[rName]["nPrd"]
                            reaction_.Kf = kfvalue  # * pow(1e-3,subn-1)
                            reaction_.Kb = kbvalue  # * pow(1e-3,prdn-1)
                        elif reaction_.className == "MMenz":
                            reaction_.kcat = kfvalue
                            reaction_.Km = kbvalue
    return (errorFlag, msg)


def getKLaw(model, klaw, rev, globparameterIdValue, specMapList):
    parmValueMap = {}
    amt_Conc = "amount"
    value = 0.0
    np = klaw. getNumParameters()
    for pi in range(0, np):
        p = klaw.getParameter(pi)
        if (p.isSetId()):
            ids = p.getId()
        if (p.isSetValue()):
            value = p.getValue()
        parmValueMap[ids] = value
    ruleMemlist = []
    flag = getMembers(klaw.getMath(), ruleMemlist)
    index = 0
    kfparm = ""
    kbparm = ""
    kfvalue = 0
    kbvalue = 0
    kfp = None
    kbp = None
    mssgstr = ""
    for i in ruleMemlist:
        if i in parmValueMap or i in globparameterIdValue:
            if index == 0:
                kfparm = i
                if i in parmValueMap:
                    kfvalue = parmValueMap[i]
                    kfp = klaw.getParameter(kfparm)
                else:
                    kfvalue = globparameterIdValue[i]
                    kfp = model.getParameter(kfparm)
            elif index == 1:
                kbparm = i
                if i in parmValueMap:
                    kbvalue = parmValueMap[i]
                    kbp = klaw.getParameter(kbparm)
                else:
                    kbvalue = globparameterIdValue[i]
                    kbp = model.getParameter(kbparm)
            index += 1

        elif not (i in specMapList or i in comptSbmlidMooseIdMap):
            mssgstr = "\"" + i + "\" is not defined "
            return (False, mssgstr)
    if kfp != "":
        # print " unit set for rate law kfp ",kfparm, " ",kfp.isSetUnits()
        if kfp.isSetUnits():
            kfud = kfp.getDerivedUnitDefinition()
            # print " kfud ",kfud
    if kbp != "":
        pass
        # print " unit set for rate law kbp ",kbparm, " ",kbp.isSetUnits()

    return (True, mssgstr, kfvalue, kbvalue)


def getMembers(node, ruleMemlist):
    if node.getType() == libsbml.AST_PLUS:
        if node.getNumChildren() == 0:
            print ("0")
            return False
        getMembers(node.getChild(0), ruleMemlist)
        for i in range(1, node.getNumChildren()):
            # addition
            getMembers(node.getChild(i), ruleMemlist)
    elif node.getType() == libsbml.AST_REAL:
        # This will be constant
        pass
    elif node.getType() == libsbml.AST_NAME:
        # This will be the ci term"
        ruleMemlist.append(node.getName())
    elif node.getType() == libsbml.AST_MINUS:
        if node.getNumChildren() == 0:
            print("0")
            return False
        else:
            lchild = node.getLeftChild()
            getMembers(lchild, ruleMemlist)
            rchild = node.getRightChild()
            getMembers(rchild, ruleMemlist)
    elif node.getType() == libsbml.AST_DIVIDE:

        if node.getNumChildren() == 0:
            print("0")
            return False
        else:
            lchild = node.getLeftChild()
            getMembers(lchild, ruleMemlist)
            rchild = node.getRightChild()
            getMembers(rchild, ruleMemlist)

    elif node.getType() == libsbml.AST_TIMES:
        if node.getNumChildren() == 0:
            print ("0")
            return False
        getMembers(node.getChild(0), ruleMemlist)
        for i in range(1, node.getNumChildren()):
            # Multiplication
            getMembers(node.getChild(i), ruleMemlist)

    elif node.getType() == libsbml.AST_FUNCTION_POWER:
        pass
    else:

        print(" this case need to be handled", node.getType())
    # if len(ruleMemlist) > 2:
    #     print "Sorry! for now MOOSE cannot handle more than 2 parameters"
 #        return True


def createRules(model, specInfoMap, globparameterIdValue):
    for r in range(0, model.getNumRules()):
        rule = model.getRule(r)
        comptvolume = []
        if (rule.isAssignment()):
            rule_variable = rule.getVariable()
            rule_variable = parentSp = str(idBeginWith(rule_variable))
            poolList = specInfoMap[rule_variable]["Mpath"].path
            poolsCompt = findCompartment(moose.element(poolList))
            if not isinstance(moose.element(poolsCompt), moose.ChemCompt):
                return -2
            else:
                if poolsCompt.name not in comptvolume:
                    comptvolume.append(poolsCompt.name)

            funcId = moose.Function(poolList + '/func')

            objclassname = moose.element(poolList).className
            if objclassname == "BufPool" or objclassname == "ZombieBufPool":
                moose.connect(funcId, 'valueOut', poolList, 'setN')
            elif objclassname == "Pool" or objclassname == "ZombiePool":
                # moose.connect( funcId, 'valueOut', poolList ,'increament' )
                moose.connect(funcId, 'valueOut', poolList, 'setN')
            elif objclassname == "Reac" or objclassname == "ZombieReac":
                moose.connect(funcId, 'valueOut', poolList, 'setNumkf')

            ruleMath = rule.getMath()
            ruleMemlist = []
            speFunXterm = {}
            getMembers(ruleMath, ruleMemlist)

            for i in ruleMemlist:

                if (i in specInfoMap):
                    i = str(idBeginWith(i))
                    specMapList = specInfoMap[i]["Mpath"]
                    poolsCompt = findCompartment(moose.element(specMapList))
                    if not isinstance(moose.element(
                            poolsCompt), moose.ChemCompt):
                        return -2
                    else:
                        if poolsCompt.name not in comptvolume:
                            comptvolume.append(poolsCompt.name)
                    numVars = funcId.numVars
                    x = funcId.path + '/x[' + str(numVars) + ']'
                    speFunXterm[i] = 'x' + str(numVars)
                    moose.connect(specMapList, 'nOut', x, 'input')
                    funcId.numVars = numVars + 1

                elif not(i in globparameterIdValue):
                    print("check the variable type ", i)
            exp = rule.getFormula()
            for mem in ruleMemlist:
                if (mem in specInfoMap):
                    #exp1 = exp.replace(mem, str(speFunXterm[mem]))
                    exp1 = re.sub(r'\b%s\b'% (mem), speFunXterm[mem], exp)
                    exp = exp1
                elif(mem in globparameterIdValue):
                    #exp1 = exp.replace(mem, str(globparameterIdValue[mem]))
                    exp1 = re.sub(r'\b%s\b'% (mem), globparameterIdValue[mem], exp)
                    exp = exp1
                else:
                    print("Math expression need to be checked")
            exp = exp.replace(" ", "")
            funcId.expr = exp.strip(" \t\n\r")
            # return True

        elif(rule.isRate()):
            print(
                "Warning : For now this \"",
                rule.getVariable(),
                "\" rate Rule is not handled in moose ")
            # return False

        elif (rule.isAlgebraic()):
            print("Warning: For now this ", rule.getVariable(),
                  " Algebraic Rule is not handled in moose")
            # return False
        if len(comptvolume) > 1:
            warning = "\nFunction ", moose.element(
                poolList).name, " has input from different compartment which is depricated in moose and running this model cause moose to crash"
    return True


def pullnotes(sbmlId, mooseId):
    if sbmlId.getNotes() is not None:
        tnodec = ((sbmlId.getNotes()).getChild(0)).getChild(0)
        notes = tnodec.getCharacters()
        notes = notes.strip(' \t\n\r')
        objPath = mooseId.path + "/info"
        if not moose.exists(objPath):
            objInfo = moose.Annotator(mooseId.path + '/info')
        else:
            objInfo = moose.element(mooseId.path + '/info')
        objInfo.notes = notes


def createSpecies(basePath, model, comptSbmlidMooseIdMap,
                  specInfoMap, modelAnnotaInfo):
    # ToDo:
    # - Need to add group name if exist in pool
    # - Notes
    # print "species "
    if not (model.getNumSpecies()):
        return (False,"number of species is zero")
    else:
        for sindex in range(0, model.getNumSpecies()):
            spe = model.getSpecies(sindex)
            group = ""
            specAnnoInfo = {}
            specAnnoInfo = getObjAnnotation(spe, modelAnnotaInfo)
            if "Group" in specAnnoInfo:
                group = specAnnoInfo["Group"]
            
            sName = None
            sId = spe.getId()
            if spe.isSetName():
                sName = spe.getName()
                sName = sName.replace(" ", "_space_")

            if spe.isSetCompartment():
                comptId = spe.getCompartment()

            if not(sName):
                sName = sId

            constant = spe.getConstant()
            boundaryCondition = spe.getBoundaryCondition()
            comptEl = comptSbmlidMooseIdMap[comptId]["MooseId"].path
            hasonlySubUnit = spe.getHasOnlySubstanceUnits()
            # "false": is {unit of amount}/{unit of size} (i.e., concentration or density).
            # "true": then the value is interpreted as having a unit of amount only.
            if group:
                if moose.exists(comptEl+'/'+group):
                    comptEl = comptEl+'/'+group
                else:
                    comptEl = (moose.Neutral(comptEl+'/'+group)).path
            if (boundaryCondition):
                poolId = moose.BufPool(comptEl + '/' + sName)
            else:
                poolId = moose.Pool(comptEl + '/' + sName)

            if (spe.isSetNotes):
                pullnotes(spe, poolId)
            
            if specAnnoInfo:
                if not moose.exists(poolId.path + '/info'):
                    poolInfo = moose.Annotator(poolId.path + '/info')
                else:
                    poolInfo = moose.element(poolId.path + '/info')

                for k, v in list(specAnnoInfo.items()):
                    if k == 'xCord':
                        poolInfo.x = float(v)
                    elif k == 'yCord':
                        poolInfo.y = float(v)
                    elif k == 'bgColor':
                        poolInfo.color = v
                    else:
                        poolInfo.textColor = v

            specInfoMap[sId] = {
                "Mpath": poolId,
                "const": constant,
                "bcondition": boundaryCondition,
                "hassubunit": hasonlySubUnit,
                "comptId": comptSbmlidMooseIdMap[comptId]["MooseId"]}
            initvalue = 0.0
            unitfactor, unitset, unittype = transformUnit(spe, hasonlySubUnit)
            if hasonlySubUnit == True:
                if spe.isSetInitialAmount():
                    initvalue = spe.getInitialAmount()
                    # populating nInit, will automatically calculate the
                    # concInit.
                    if not (unitset):
                        # if unit is not set,
                        # default unit is assumed as Mole in SBML
                        unitfactor = pow(6.0221409e23, 1)
                        unittype = "Mole"

                    initvalue = initvalue * unitfactor
                elif spe.isSetInitialConcentration():
                    initvalue = spe.getInitialConcentration()
                    print(" Since hasonlySubUnit is true and concentration is set units are not checked")
                poolId.nInit = initvalue

            elif hasonlySubUnit == False:
                # ToDo : check 00976
                if spe.isSetInitialAmount():
                    initvalue = spe.getInitialAmount()
                    # initAmount is set we need to convert to concentration
                    initvalue = initvalue / comptSbmlidMooseIdMap[comptId]["size"]

                elif spe.isSetInitialConcentration():
                    initvalue = spe.getInitialConcentration()
                if not unitset:
                    # print " unit is not set"
                    unitfactor = pow(10, -3)

                initvalue = initvalue * unitfactor
                poolId.concInit = initvalue
            else:
                nr = model.getNumRules()
                found = False
                for nrItem in range(0, nr):
                    rule = model.getRule(nrItem)
                    assignRule = rule.isAssignment()
                    if (assignRule):
                        rule_variable = rule.getVariable()
                        if (rule_variable == sId):
                            found = True
                            break
                
                if not (found):
                    print(
                        "Invalid SBML: Either initialConcentration or initialAmount must be set or it should be found in assignmentRule but non happening for ",
                        sName)
                    return (False,"Invalid SBML: Either initialConcentration or initialAmount must be set or it should be found in assignmentRule but non happening for ",sName)

    return (True," ")


def transformUnit(unitForObject, hasonlySubUnit=False):
    # print "unit
    # ",UnitDefinition.printUnits(unitForObject.getDerivedUnitDefinition())
    unitset = False
    unittype = None
    if (unitForObject.getDerivedUnitDefinition()):
        unit = (unitForObject.getDerivedUnitDefinition())
        unitnumber = int(unit.getNumUnits())
        if unitnumber > 0:
            for ui in range(0, unit.getNumUnits()):
                lvalue = 1.0
                unitType = unit.getUnit(ui)
                if(unitType.isLitre()):
                    exponent = unitType.getExponent()
                    multiplier = unitType.getMultiplier()
                    scale = unitType.getScale()
                    offset = unitType.getOffset()
                    # units for compartment is Litre but MOOSE compartment is
                    # m3
                    scale = scale - 3
                    lvalue *= pow(multiplier * pow(10.0, scale),
                                  exponent) + offset
                    unitset = True
                    unittype = "Litre"
                    return (lvalue, unitset, unittype)
                elif(unitType.isMole()):
                    exponent = unitType.getExponent()
                    multiplier = unitType.getMultiplier()
                    scale = unitType.getScale()
                    offset = unitType.getOffset()
                    # if hasOnlySubstanceUnit = True, then assuming Amount
                    if hasonlySubUnit == True:
                        lvalue *= pow(multiplier *
                                      pow(10.0, scale), exponent) + offset
                        # If SBML units are in mole then convert to number by
                        # multiplying with avogadro's number
                        lvalue = lvalue * pow(6.0221409e23, 1)
                    elif hasonlySubUnit == False:
                        # Pool units in moose is mM
                        if scale > 0:
                            lvalue *= pow(multiplier * pow(10.0,
                                                           scale - 3), exponent) + offset
                        elif scale <= 0:
                            lvalue *= pow(multiplier * pow(10.0,
                                                           scale + 3), exponent) + offset
                    unitset = True
                    unittype = "Mole"
                    return (lvalue, unitset, unittype)

                elif(unitType.isItem()):
                    exponent = unitType.getExponent()
                    multiplier = unitType.getMultiplier()
                    scale = unitType.getScale()
                    offset = unitType.getOffset()
                    # if hasOnlySubstanceUnit = True, then assuming Amount
                    if hasonlySubUnit == True:
                        # If SBML units are in Item then amount is populate as
                        # its
                        lvalue *= pow(multiplier *
                                      pow(10.0, scale), exponent) + offset
                    if hasonlySubUnit == False:
                        # hasonlySubUnit is False, which is assumed concentration,
                        # Here Item is converted to mole by dividing by
                        # avogadro and at initiavalue divided by volume"
                        lvalue *= pow(multiplier *
                                      pow(10.0, scale), exponent) + offset
                        lvalue = lvalue / pow(6.0221409e23, 1)
                    unitset = True
                    unittype = "Item"
                    return (lvalue, unitset, unittype)
        else:
            lvalue = 1.0
    return (lvalue, unitset, unittype)


def createCompartment(basePath, model, comptSbmlidMooseIdMap):
    # ToDoList : Check what should be done for the spaitialdimension is 2 or
    # 1, area or length
    if not(model.getNumCompartments()):
        return False,
    else:
        for c in range(0, model.getNumCompartments()):
            compt = model.getCompartment(c)
            # print("Compartment " + str(c) + ": "+ UnitDefinition.printUnits(compt.getDerivedUnitDefinition()))
            msize = 0.0
            unitfactor = 1.0
            sbmlCmptId = None
            name = None

            if (compt.isSetId()):
                sbmlCmptId = compt.getId()

            if (compt.isSetName()):
                name = compt.getName()
                name = name.replace(" ", "_space")

            if (compt.isSetOutside()):
                outside = compt.getOutside()

            if (compt.isSetSize()):
                msize = compt.getSize()
                if msize == 1:
                    print("Compartment size is 1")

            dimension = compt.getSpatialDimensions()
            if dimension == 3:
                unitfactor, unitset, unittype = transformUnit(compt)

            else:
                print(
                    " Currently we don't deal with spatial Dimension less than 3 and unit's area or length")
                return False

            if not(name):
                name = sbmlCmptId

            mooseCmptId = moose.CubeMesh(basePath.path + '/' + name)
            mooseCmptId.volume = (msize * unitfactor)
            comptSbmlidMooseIdMap[sbmlCmptId] = {
                "MooseId": mooseCmptId, "spatialDim": dimension, "size": msize}
    return True


def setupMMEnzymeReaction(reac, rName, specInfoMap, reactSBMLIdMooseId,
                          modelAnnotaInfo, model, globparameterIdValue):
    msg = ""
    errorFlag = ""
    numRcts = reac.getNumReactants()
    numPdts = reac.getNumProducts()
    nummodifiers = reac.getNumModifiers()
    if (nummodifiers):
        parent = reac.getModifier(0)
        parentSp = parent.getSpecies()
        parentSp = str(idBeginWith(parentSp))
        enzParent = specInfoMap[parentSp]["Mpath"]
        MMEnz = moose.MMenz(enzParent.path + '/' + rName)
        moose.connect(enzParent, "nOut", MMEnz, "enzDest")
        reactionCreated = True
        reactSBMLIdMooseId[rName] = {"MooseId": MMEnz, "className": "MMEnz"}
        if reactionCreated:
            if (reac.isSetNotes):
                pullnotes(reac, MMEnz)
                reacAnnoInfo = {}
                reacAnnoInfo = getObjAnnotation(reac, modelAnnotaInfo)
                if reacAnnoInfo:
                    if not moose.exists(MMEnz.path + '/info'):
                        reacInfo = moose.Annotator(MMEnz.path + '/info')
                    else:
                        reacInfo = moose.element(MMEnz.path + '/info')
                    for k, v in list(reacAnnoInfo.items()):
                        if k == 'xCord':
                            reacInfo.x = float(v)
                        elif k == 'yCord':
                            reacInfo.y = float(v)
                        elif k == 'bgColor':
                            reacInfo.color = v
                        else:
                            reacInfo.textColor = v
            return(reactionCreated, MMEnz)


def mapParameter(model, globparameterIdValue):
    for pm in range(0, model.getNumParameters()):
        prm = model.getParameter(pm)
        if (prm.isSetId()):
            parid = prm.getId()
        value = 0.0
        if (prm.isSetValue()):
            value = prm.getValue()
        globparameterIdValue[parid] = value


def idBeginWith(name):
    changedName = name
    if name[0].isdigit():
        changedName = "_" + name
    return changedName


def convertSpecialChar(str1):
    d = {"&": "_and", "<": "_lessthan_", ">": "_greaterthan_", "BEL": "&#176", "-": "_minus_", "'": "_prime_",
         "+": "_plus_", "*": "_star_", "/": "_slash_", "(": "_bo_", ")": "_bc_",
         "[": "_sbo_", "]": "_sbc_", ".": "_dot_", " ": "_"
         }
    for i, j in list(d.items()):
        str1 = str1.replace(i, j)
    return str1


def mooseIsInstance(element, classNames):
    return moose.element(element).__class__.__name__ in classNames


def findCompartment(element):
    while not mooseIsInstance(element, ["CubeMesh", "CyclMesh"]):
        element = element.parent
    return element

if __name__ == "__main__":
    try:
        sys.argv[1]
    except IndexError:
        print("Filename or path not given")
        exit(0)
    else:
        filepath = sys.argv[1]
        if not os.path.exists(filepath):
            print("Filename or path does not exist",filepath)
            
        else:
            try:
                sys.argv[2]
            except :
                modelpath = filepath[filepath.rfind('/'):filepath.find('.')]
            else:
                modelpath = sys.argv[2]
            read = mooseReadSBML(filepath, modelpath)
            if read:
                print(" Model read to moose path "+ modelpath)
            else:
                print(" could not read  SBML to MOOSE")
