# *******************************************************************
#  * File:            writeSBML.py
#  * Description:
#  * Author:          HarshaRani
#  * E-mail:          hrani@ncbs.res.in
#
# ** This program is part of 'MOOSE', the
# ** Messaging Object Oriented Simulation Environment,
# ** also known as GENESIS 3 base code.
# **           copyright (C) 2003-2017 Upinder S. Bhalla. and NCBS
# Created : Friday May 27 12:19:00 2016(+0530)
# Version
# Last-Updated: Wed Jan 11 15:20:00 2017(+0530)
#           By:
# **********************************************************************/

# TODO:
#   Table should be written
#   Group's should be added
# boundary condition for buffer pool having assignment statment constant
# shd be false

import sys
import re
from collections import Counter

import moose
from moose.SBML.validation import validateModel
from moose.chemUtil.chemConnectUtil import *
from moose.chemUtil.graphUtils import *


foundLibSBML_ = False
try:
    from libsbml import *
    foundLibSBML_ = True
except Exception as e:
    pass

def mooseWriteSBML(modelpath, filename, sceneitems={}):
    global foundLibSBML_ 
    if not foundLibSBML_:
        msg = ['No python-libsbml found.'
                , 'This module can be installed by following command in terminal:'
                , '\t easy_install python-libsbml' ] 
        print( '\n'.join(msg) )
        return -2, "[FATAL] Could not save the model in to SBML file."

    sbmlDoc = SBMLDocument(3, 1)
    filepath, filenameExt = os.path.split(filename)
    if filenameExt.find('.') != -1:
        filename = filenameExt[:filenameExt.find('.')]
    else:
        filename = filenameExt

    # validatemodel
    sbmlOk = False
    global spe_constTrue
    spe_constTrue = []
    global nameList_
    nameList_ = []
    positionInfoexist = False
    xmlns = XMLNamespaces()
    xmlns.add("http://www.sbml.org/sbml/level3/version1")
    xmlns.add("http://www.moose.ncbs.res.in", "moose")
    xmlns.add("http://www.w3.org/1999/xhtml", "xhtml")
    sbmlDoc.setNamespaces(xmlns)
    cremodel_ = sbmlDoc.createModel()
    cremodel_.setId(filename)
    cremodel_.setTimeUnits("second")
    cremodel_.setExtentUnits("substance")
    cremodel_.setSubstanceUnits("substance")
    neutralNotes = ""

    specieslist = moose.wildcardFind(modelpath + '/##[ISA=PoolBase]')
    if specieslist:
        neutralPath = getGroupinfo(specieslist[0])
        if moose.exists(neutralPath.path + '/info'):
            neutralInfo = moose.element(neutralPath.path + '/info')
            neutralNotes = neutralInfo.notes
        if neutralNotes != "":
            cleanNotes = convertNotesSpecialChar(neutralNotes)
            notesString = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t" + \
                neutralNotes + "\n\t </body>"
            cremodel_.setNotes(notesString)

    srcdesConnection = {}
    if not bool(sceneitems):
        meshEntry,xmin,xmax,ymin,ymax,positionInfoexist,sitem = setupMeshObj(modelpath)
        #moose.coordinateUtil.setupItem(modelpath,srcdesConnection)
        setupItem(modelpath,srcdesConnection)
        if not positionInfoexist:
            autoCoordinates(meshEntry,srcdesConnection)
    
    writeUnits(cremodel_)
    modelAnno = writeSimulationAnnotation(modelpath)
    if modelAnno:
        cremodel_.setAnnotation(modelAnno)

    compartexist = writeCompt(modelpath, cremodel_)
    if compartexist == True:
        species = writeSpecies( modelpath,cremodel_,sbmlDoc,sceneitems)
        if species:
            writeFunc(modelpath, cremodel_)
        writeReac(modelpath, cremodel_, sceneitems)
        writeEnz(modelpath, cremodel_, sceneitems)
        consistencyMessages = ""
        SBMLok = validateModel(sbmlDoc)
        if SBMLok:
            writeTofile = filepath + "/" + filename + '.xml'
            writeSBMLToFile(sbmlDoc, writeTofile)
            return True, consistencyMessages, writeTofile
        else:
            print( "[FTAL] Errors encountered " )
            return -1, consistencyMessages
    else:
        return False, "Atleast one compartment should exist to write SBML"

def calPrime(x):
    prime = int((20 * (float(x - cmin) / float(cmax - cmin))) - 10)
    return prime

def writeEnz(modelpath, cremodel_, sceneitems):
    for enz in moose.wildcardFind(modelpath + '/##[ISA=EnzBase]'):
        enzannoexist = False
        enzGpnCorCol = " "
        cleanEnzname = convertSpecialChar(enz.name)
        enzSubt = ()
        compt = ""
        notesE = ""
        if moose.exists(enz.path + '/info'):
            Anno = moose.Annotator(enz.path + '/info')
            notesE = Anno.notes
            element = moose.element(enz)
            ele = getGroupinfo(element)
            if element.className == "Neutral" or sceneitems or Anno.x or Anno.y:
                    enzannoexist = True
            if enzannoexist:
                enzAnno = "<moose:ModelAnnotation>\n"
                if ele.className == "Neutral":
                    enzGpnCorCol =  "<moose:Group>" + ele.name + "</moose:Group>\n"
                if sceneitems:
                    #Saved from GUI, then scene co-ordinates are passed
                    enzGpnCorCol = enzGpnCorCol + "<moose:xCord>" + \
                            str(sceneitems[enz]['x']) + "</moose:xCord>\n" + \
                            "<moose:yCord>" + \
                            str(sceneitems[enz]['y'])+ "</moose:yCord>\n"
                else:
                    #Saved from cmdline,genesis coordinates are kept as its
                    # SBML, cspace, python, then auto-coordinates are done
                    #and coordinates are updated in moose Annotation field
                    enzGpnCorCol = enzGpnCorCol + "<moose:xCord>" + \
                            str(Anno.x) + "</moose:xCord>\n" + \
                            "<moose:yCord>" + \
                            str(Anno.y)+ "</moose:yCord>\n"
                if Anno.color:
                    enzGpnCorCol = enzGpnCorCol + "<moose:bgColor>" + Anno.color + "</moose:bgColor>\n"
                if Anno.textColor:
                    enzGpnCorCol = enzGpnCorCol + "<moose:textColor>" + \
                        Anno.textColor + "</moose:textColor>\n"

        if (enz.className == "Enz" or enz.className == "ZombieEnz"):
            enzyme = cremodel_.createReaction()
            if notesE != "":
                cleanNotesE = convertNotesSpecialChar(notesE)
                notesStringE = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t" + \
                    cleanNotesE + "\n\t </body>"
                enzyme.setNotes(notesStringE)
            comptVec = findCompartment(moose.element(enz))
            if not isinstance(moose.element(comptVec), moose.ChemCompt):
                return -2
            else:
                compt = comptVec.name + "_" + \
                    str(comptVec.getId().value) + "_" + \
                    str(comptVec.getDataIndex()) + "_"

            enzyme.setId(str(idBeginWith(cleanEnzname +
                                         "_" +
                                         str(enz.getId().value) +
                                         "_" +
                                         str(enz.getDataIndex()) +
                                         "_" +
                                         "Complex_formation_")))
            enzyme.setName(cleanEnzname)
            enzyme.setFast(False)
            enzyme.setReversible(True)
            k1 = enz.concK1
            k2 = enz.k2
            k3 = enz.k3
            enzAnno = " "
            enzAnno = "<moose:EnzymaticReaction>\n"

            enzOut = enz.neighbors["enzOut"]

            if not enzOut:
                print(" Enzyme parent missing for ", enz.name)
            else:
                listofname(enzOut, True)
                enzSubt = enzOut
                for i in range(0, len(nameList_)):
                    enzAnno = enzAnno + "<moose:enzyme>" + \
                        (str(idBeginWith(convertSpecialChar(
                            nameList_[i])))) + "</moose:enzyme>\n"
            #noofSub,sRateLaw = getSubprd(cremodel_,True,"sub",enzSub)
            # for i in range(0,len(nameList_)):
            #    enzAnno=enzAnno+"<moose:enzyme>"+nameList_[i]+"</moose:enzyme>\n"
            #rec_order  = noofSub
            #rate_law = "k1"+"*"+sRateLaw

            enzSub = enz.neighbors["sub"]
            if not enzSub:
                print("Enzyme \"", enz.name, "\" substrate missing")
            else:
                listofname(enzSub, True)
                enzSubt += enzSub
                for i in range(0, len(nameList_)):
                    enzAnno = enzAnno + "<moose:substrates>" + \
                        nameList_[i] + "</moose:substrates>\n"
            if enzSubt:
                rate_law = "k1"
                noofSub, sRateLaw = getSubprd(cremodel_, True, "sub", enzSubt)
                #rec_order = rec_order + noofSub
                rec_order = noofSub
                rate_law = compt + " * " + rate_law + "*" + sRateLaw

            enzPrd = enz.neighbors["cplxDest"]
            if not enzPrd:
                print("Enzyme \"", enz.name, "\"product missing")
            else:
                noofPrd, sRateLaw = getSubprd(cremodel_, True, "prd", enzPrd)
                for i in range(0, len(nameList_)):
                    enzAnno = enzAnno + "<moose:product>" + \
                        nameList_[i] + "</moose:product>\n"
                rate_law = rate_law + " - " + compt + "* k2" + '*' + sRateLaw

            prd_order = noofPrd
            enzAnno = enzAnno + "<moose:groupName>" + cleanEnzname + "_" + \
                str(enz.getId().value) + "_" + \
                str(enz.getDataIndex()) + "_" + "</moose:groupName>\n"
            enzAnno = enzAnno + "<moose:stage>1</moose:stage>\n"
            if enzannoexist:
                enzAnno = enzAnno + enzGpnCorCol
            enzAnno = enzAnno + "</moose:EnzymaticReaction>"
            enzyme.setAnnotation(enzAnno)
            kl = enzyme.createKineticLaw()
            kl.setFormula(rate_law)
            kl.setNotes(
                "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n\t\t" +
                rate_law +
                "\n \t </body>")
            punit = parmUnit(prd_order - 1, cremodel_)
            printParameters(kl, "k2", k2, punit)

            unit = parmUnit(rec_order - 1, cremodel_)
            printParameters(kl, "k1", k1, unit)
            enzyme = cremodel_.createReaction()
            enzyme.setId(str(idBeginWith(cleanEnzname +
                                         "_" +
                                         str(enz.getId().value) +
                                         "_" +
                                         str(enz.getDataIndex()) +
                                         "_" +
                                         "Product_formation_")))
            enzyme.setName(cleanEnzname)
            enzyme.setFast(False)
            enzyme.setReversible(False)
            enzAnno2 = "<moose:EnzymaticReaction>"

            enzSub = enz.neighbors["cplxDest"]
            if not enzSub:
                print(" complex missing from ", enz.name)
            else:
                noofSub, sRateLaw = getSubprd(cremodel_, True, "sub", enzSub)
                for i in range(0, len(nameList_)):
                    enzAnno2 = enzAnno2 + "<moose:complex>" + \
                        nameList_[i] + "</moose:complex>\n"

            enzEnz = enz.neighbors["enzOut"]
            if not enzEnz:
                print("Enzyme parent missing for ", enz.name)
            else:
                noofEnz, sRateLaw1 = getSubprd(cremodel_, True, "prd", enzEnz)
                for i in range(0, len(nameList_)):
                    enzAnno2 = enzAnno2 + "<moose:enzyme>" + \
                        nameList_[i] + "</moose:enzyme>\n"
            enzPrd = enz.neighbors["prd"]
            if enzPrd:
                noofprd, sRateLaw2 = getSubprd(cremodel_, True, "prd", enzPrd)
            else:
                print("Enzyme \"", enz.name, "\" product missing")
            for i in range(0, len(nameList_)):
                enzAnno2 = enzAnno2 + "<moose:product>" + \
                    nameList_[i] + "</moose:product>\n"
            enzAnno2 += "<moose:groupName>" + cleanEnzname + "_" + \
                str(enz.getId().value) + "_" + \
                str(enz.getDataIndex()) + "_" + "</moose:groupName>\n"
            enzAnno2 += "<moose:stage>2</moose:stage> \n"
            if enzannoexist:
                enzAnno2 = enzAnno2 + enzGpnCorCol
            enzAnno2 += "</moose:EnzymaticReaction>"
            enzyme.setAnnotation(enzAnno2)

            enzrate_law = compt + " * k3" + '*' + sRateLaw
            kl = enzyme.createKineticLaw()
            kl.setFormula(enzrate_law)
            kl.setNotes(
                "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n\t\t" +
                enzrate_law +
                "\n \t </body>")
            unit = parmUnit(noofPrd - 1, cremodel_)
            printParameters(kl, "k3", k3, unit)

        elif(enz.className == "MMenz" or enz.className == "ZombieMMenz"):
            enzSub = enz.neighbors["sub"]
            enzPrd = enz.neighbors["prd"]
            if (len(enzSub) != 0 and len(enzPrd) != 0):
                enzCompt = findCompartment(enz)
                if not isinstance(moose.element(enzCompt), moose.ChemCompt):
                    return -2
                else:
                    compt = enzCompt.name + "_" + \
                        str(enzCompt.getId().value) + "_" + \
                        str(enzCompt.getDataIndex()) + "_"
                enzyme = cremodel_.createReaction()
                enzAnno = " "
                if notesE != "":
                    cleanNotesE = convertNotesSpecialChar(notesE)
                    notesStringE = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t" + \
                        cleanNotesE + "\n\t </body>"
                    enzyme.setNotes(notesStringE)
                enzyme.setId(str(idBeginWith(cleanEnzname +
                                             "_" +
                                             str(enz.getId().value) +
                                             "_" +
                                             str(enz.getDataIndex()) +
                                             "_")))
                enzyme.setName(cleanEnzname)
                enzyme.setFast(False)
                enzyme.setReversible(True)
                if enzannoexist:
                    enzAnno = enzAnno + enzGpnCorCol
                    enzAnno = "<moose:EnzymaticReaction>\n" + \
                        enzGpnCorCol + "</moose:EnzymaticReaction>"
                    enzyme.setAnnotation(enzAnno)
                Km = enz.Km
                kcat = enz.kcat
                enzSub = enz.neighbors["sub"]
                noofSub, sRateLawS = getSubprd(cremodel_, False, "sub", enzSub)
                #sRate_law << rate_law.str();
                # Modifier
                enzMod = enz.neighbors["enzDest"]
                noofMod, sRateLawM = getSubprd(cremodel_, False, "enz", enzMod)
                enzPrd = enz.neighbors["prd"]
                noofPrd, sRateLawP = getSubprd(cremodel_, False, "prd", enzPrd)
                kl = enzyme.createKineticLaw()
                fRate_law = "kcat *" + sRateLawS + "*" + sRateLawM + \
                    "/(" + compt + " * (" + "Km" + "+" + sRateLawS + "))"
                kl.setFormula(fRate_law)
                kl.setNotes(
                    "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n\t\t" +
                    fRate_law +
                    "\n \t </body>")
                printParameters(kl, "Km", Km, "substance")
                kcatUnit = parmUnit(0, cremodel_)
                printParameters(kl, "kcat", kcat, kcatUnit)


def printParameters(kl, k, kvalue, unit):
    para = kl.createParameter()
    para.setId(str(idBeginWith(k)))
    para.setValue(kvalue)
    para.setUnits(unit)


def parmUnit(rct_order, cremodel_):
    order = rct_order
    if order == 0:
        unit_stream = "per_second"
    elif order == 1:
        unit_stream = "litre_per_mmole_per_second"
    elif order == 2:
        unit_stream = "litre_per_mmole_sq_per_second"
    else:
        unit_stream = "litre_per_mmole_" + str(rct_order) + "_per_second"

    lud = cremodel_.getListOfUnitDefinitions()
    flag = False
    for i in range(0, len(lud)):
        ud = lud.get(i)
        if (ud.getId() == unit_stream):
            flag = True
            break
    if (not flag):
        unitdef = cremodel_.createUnitDefinition()
        unitdef.setId(unit_stream)
        # Create individual unit objects that will be put inside the
        # UnitDefinition .
        if order != 0:
            unit = unitdef.createUnit()
            unit.setKind(UNIT_KIND_LITRE)
            unit.setExponent(1)
            unit.setMultiplier(1)
            unit.setScale(0)
            unit = unitdef.createUnit()
            unit.setKind(UNIT_KIND_MOLE)
            unit.setExponent(-order)
            unit.setMultiplier(1)
            unit.setScale(-3)

        unit = unitdef.createUnit()
        unit.setKind(UNIT_KIND_SECOND)
        unit.setExponent(-1)
        unit.setMultiplier(1)
        unit.setScale(0)
    return unit_stream


def getSubprd(cremodel_, mobjEnz, type, neighborslist):
    if type == "sub":
        reacSub = neighborslist
        reacSubCou = Counter(reacSub)

        # print " reacSubCou ",reacSubCou,"()",len(reacSubCou)
        noofSub = len(reacSubCou)
        rate_law = " "
        if reacSub:
            rate_law = processRateLaw(
                reacSubCou, cremodel_, noofSub, "sub", mobjEnz)
            return len(reacSub), rate_law
        else:
            return 0, rate_law
    elif type == "prd":
        reacPrd = neighborslist
        reacPrdCou = Counter(reacPrd)
        noofPrd = len(reacPrdCou)
        rate_law = " "
        if reacPrd:
            rate_law = processRateLaw(
                reacPrdCou, cremodel_, noofPrd, "prd", mobjEnz)
            return len(reacPrd), rate_law
    elif type == "enz":
        enzModifier = neighborslist
        enzModCou = Counter(enzModifier)
        noofMod = len(enzModCou)
        rate_law = " "
        if enzModifier:
            rate_law = processRateLaw(
                enzModCou, cremodel_, noofMod, "Modifier", mobjEnz)
            return len(enzModifier), rate_law


def processRateLaw(objectCount, cremodel, noofObj, type, mobjEnz):
    rate_law = ""
    nameList_[:] = []
    for value, count in objectCount.items():
        value = moose.element(value)
        nameIndex = value.name + "_" + \
            str(value.getId().value) + "_" + str(value.getDataIndex()) + "_"
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

            sbmlRef.setStoichiometry(count)
            if clean_name in spe_constTrue:
                sbmlRef.setConstant(True)
            else:
                sbmlRef.setConstant(False)
        if (count == 1):
            if rate_law == "":
                rate_law = clean_name
            else:
                rate_law = rate_law + "*" + clean_name
        else:
            if rate_law == "":
                rate_law = clean_name + "^" + str(count)
            else:
                rate_law = rate_law + "*" + clean_name + "^" + str(count)
    return(rate_law)


def listofname(reacSub, mobjEnz):
    objectCount = Counter(reacSub)
    nameList_[:] = []
    for value, count in objectCount.items():
        value = moose.element(value)
        nameIndex = value.name + "_" + \
            str(value.getId().value) + "_" + str(value.getDataIndex()) + "_"
        clean_name = convertSpecialChar(nameIndex)
        if mobjEnz == True:
            nameList_.append(clean_name)


def writeReac(modelpath, cremodel_, sceneitems):
    for reac in moose.wildcardFind(modelpath + '/##[ISA=ReacBase]'):
        reacSub = reac.neighbors["sub"]
        reacPrd = reac.neighbors["prd"]
        if (len(reacSub) != 0 and len(reacPrd) != 0):

            reaction = cremodel_.createReaction()
            reacannoexist = False
            reacGpname = " "
            cleanReacname = convertSpecialChar(reac.name)
            reaction.setId(str(idBeginWith(cleanReacname +
                                           "_" +
                                           str(reac.getId().value) +
                                           "_" +
                                           str(reac.getDataIndex()) +
                                           "_")))
            reaction.setName(cleanReacname)
            #Kf = reac.numKf
            #Kb = reac.numKb
            Kf = reac.Kf
            Kb = reac.Kb
            if Kb == 0.0:
                reaction.setReversible(False)
            else:
                reaction.setReversible(True)

            reaction.setFast(False)
            if moose.exists(reac.path + '/info'):
                Anno = moose.Annotator(reac.path + '/info')
                if Anno.notes != "":
                    cleanNotesR = convertNotesSpecialChar(Anno.notes)
                    notesStringR = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t" + \
                        cleanNotesR + "\n\t </body>"
                    reaction.setNotes(notesStringR)
                element = moose.element(reac)
                ele = getGroupinfo(element)
                if element.className == "Neutral" or sceneitems or Anno.x or Anno.y:
                    reacannoexist = True
                if reacannoexist:
                    reacAnno = "<moose:ModelAnnotation>\n"
                    if ele.className == "Neutral":
                        reacAnno = reacAnno + "<moose:Group>" + ele.name + "</moose:Group>\n"
                    if sceneitems:
                        #Saved from GUI, then scene co-ordinates are passed
                        reacAnno = reacAnno + "<moose:xCord>" + \
                                str(sceneitems[reac]['x']) + "</moose:xCord>\n" + \
                                "<moose:yCord>" + \
                                str(sceneitems[reac]['y'])+ "</moose:yCord>\n"
                    else:
                        #Saved from cmdline,genesis coordinates are kept as its
                        # SBML, cspace, python, then auto-coordinates are done
                        #and coordinates are updated in moose Annotation field
                        reacAnno = reacAnno + "<moose:xCord>" + \
                                str(Anno.x) + "</moose:xCord>\n" + \
                                "<moose:yCord>" + \
                                str(Anno.y)+ "</moose:yCord>\n"
                    if Anno.color:
                        reacAnno = reacAnno + "<moose:bgColor>" + Anno.color + "</moose:bgColor>\n"
                    if Anno.textColor:
                        reacAnno = reacAnno + "<moose:textColor>" + \
                            Anno.textColor + "</moose:textColor>\n"
                    reacAnno = reacAnno + "</moose:ModelAnnotation>"
                    reaction.setAnnotation(reacAnno)

            kl_s, sRL, pRL, compt = "", "", "", ""

            if not reacSub and not reacPrd:
                print(" Reaction ", reac.name, "missing substrate and product")
            else:
                kfl = reaction.createKineticLaw()
                if reacSub:
                    noofSub, sRateLaw = getSubprd(
                        cremodel_, False, "sub", reacSub)
                    if noofSub:
                        comptVec = findCompartment(moose.element(reacSub[0]))
                        if not isinstance(moose.element(
                                comptVec), moose.ChemCompt):
                            return -2
                        else:
                            compt = comptVec.name + "_" + \
                                str(comptVec.getId().value) + "_" + \
                                str(comptVec.getDataIndex()) + "_"
                        cleanReacname = cleanReacname + "_" + \
                            str(reac.getId().value) + "_" + \
                            str(reac.getDataIndex()) + "_"
                        kfparm = idBeginWith(cleanReacname) + "_" + "Kf"
                        sRL = compt + " * " + \
                            idBeginWith(cleanReacname) + "_Kf * " + sRateLaw
                        unit = parmUnit(noofSub - 1, cremodel_)
                        printParameters(kfl, kfparm, Kf, unit)
                        #kl_s = compt+"(" +sRL
                        kl_s = sRL
                    else:
                        print(reac.name + " has no substrate")
                        return -2
                else:
                    print(" Substrate missing for reaction ", reac.name)

                if reacPrd:
                    noofPrd, pRateLaw = getSubprd(
                        cremodel_, False, "prd", reacPrd)
                    if noofPrd:
                        if Kb:
                            kbparm = idBeginWith(cleanReacname) + "_" + "Kb"
                            pRL = compt + " * " + \
                                idBeginWith(cleanReacname) + \
                                "_Kb * " + pRateLaw
                            unit = parmUnit(noofPrd - 1, cremodel_)
                            printParameters(kfl, kbparm, Kb, unit)
                            #kl_s = kl_s+ "- "+pRL+")"
                            kl_s = kl_s + "-" + pRL
                    else:
                        print(reac.name + " has no product")
                        return -2
                else:
                    print(" Product missing for reaction ", reac.name)
            kfl.setFormula(kl_s)
            kfl.setNotes(
                "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n\t\t" +
                kl_s +
                "\n \t </body>")
            kfl.setFormula(kl_s)
        else:
            print(" Reaction ", reac.name, "missing substrate and product")


def writeFunc(modelpath, cremodel_):
    funcs = moose.wildcardFind(modelpath + '/##[ISA=Function]')
    # if func:
    for func in funcs:
        if func:
            if func.parent.className == "CubeMesh" or func.parent.className == "CyclMesh":
                funcEle = moose.element(
                    moose.element(func).neighbors["valueOut"][0])
                funcEle1 = moose.element(funcEle)
                fName = idBeginWith(convertSpecialChar(
                    funcEle.name + "_" + str(funcEle.getId().value) + "_" + str(funcEle.getDataIndex()) + "_"))
                expr = moose.element(func).expr

            else:
                fName = idBeginWith(convertSpecialChar(func.parent.name +
                                                       "_" +
                                                       str(func.parent.getId().value) +
                                                       "_" +
                                                       str(func.parent.getDataIndex()) +
                                                       "_"))
                item = func.path + '/x[0]'
                sumtot = moose.element(item).neighbors["input"]
                expr = moose.element(func).expr
                for i in range(0, len(sumtot)):
                    v = "x" + str(i)
                    if v in expr:
                        z = str(idBeginWith(str(convertSpecialChar(sumtot[i].name + "_" + str(moose.element(
                            sumtot[i]).getId().value) + "_" + str(moose.element(sumtot[i]).getDataIndex())) + "_")))
                        expr = expr.replace(v, z)

            rule = cremodel_.createAssignmentRule()
            rule.setVariable(fName)
            rule.setFormula(expr)


def convertNotesSpecialChar(str1):
    d = {"&": "_and", "<": "_lessthan_", ">": "_greaterthan_", "BEL": "&#176"}
    for i, j in d.items():
        str1 = str1.replace(i, j)
    # stripping \t \n \r and space from begining and end of string
    str1 = str1.strip(' \t\n\r')
    return str1


def getGroupinfo(element):
    #   Note: At this time I am assuming that if group exist (incase of Genesis)
    #   1. for 'pool' its between compartment and pool, /modelpath/Compartment/Group/pool
    #   2. for 'enzComplx' in case of ExpilcityEnz its would be, /modelpath/Compartment/Group/Pool/Enz/Pool_cplx
    #   For these cases I have checked, but subgroup may exist then this bit of code need to cleanup further down
    #   if /modelpath/Compartment/Group/Group1/Pool, then I check and get Group1
    #   And /modelpath is also a NeutralObject,I stop till I find Compartment

    while not mooseIsInstance(element, ["Neutral"]) and not mooseIsInstance(
            element, ["CubeMesh", "CyclMesh"]):
        element = element.parent
    return element


def mooseIsInstance(element, classNames):
    return moose.element(element).__class__.__name__ in classNames


def findCompartment(element):
    while not mooseIsInstance(element, ["CubeMesh", "CyclMesh"]):
        element = element.parent
    return element


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
    for i, j in d.items():
        str1 = str1.replace(i, j)
    return str1


def writeSpecies(modelpath, cremodel_, sbmlDoc, sceneitems):
    # getting all the species
    for spe in moose.wildcardFind(modelpath + '/##[ISA=PoolBase]'):
        sName = convertSpecialChar(spe.name)
        comptVec = findCompartment(spe)
        speciannoexist = False
        speciGpname = ""

        if not isinstance(moose.element(comptVec), moose.ChemCompt):
            return -2
        else:
            compt = comptVec.name + "_" + \
                str(comptVec.getId().value) + "_" + \
                str(comptVec.getDataIndex()) + "_"
            s1 = cremodel_.createSpecies()
            spename = sName + "_" + \
                str(spe.getId().value) + "_" + str(spe.getDataIndex()) + "_"
            spename = str(idBeginWith(spename))
            s1.setId(spename)

            if spename.find(
                    "cplx") != -1 and isinstance(moose.element(spe.parent), moose.EnzBase):
                enz = spe.parent
                if (moose.element(enz.parent), moose.PoolBase):
                    # print " found a cplx name ",spe.parent,
                    # moose.element(spe.parent).parent
                    enzname = enz.name
                    enzPool = (enz.parent).name
                    sName = convertSpecialChar(
                        enzPool + "_" + enzname + "_" + sName)

            s1.setName(sName)
            # s1.setInitialAmount(spe.nInit)
            s1.setInitialConcentration(spe.concInit)
            s1.setCompartment(compt)
            #  Setting BoundaryCondition and constant as per this rule for BufPool
            #  -constanst  -boundaryCondition  -has assignment/rate Rule  -can be part of sub/prd
            #   false           true              yes                       yes
            #   true            true               no                       yes
            if spe.className == "BufPool" or spe.className == "ZombieBufPool":
                # BoundaryCondition is made for buff pool
                s1.setBoundaryCondition(True)

                if moose.exists(spe.path + '/func'):
                    bpf = moose.element(spe.path)
                    for fp in bpf.children:
                        if fp.className == "Function" or fp.className == "ZombieFunction":
                            if len(moose.element(
                                    fp.path + '/x').neighbors["input"]) > 0:
                                s1.setConstant(False)
                            else:
                                # if function exist but sumtotal object doesn't
                                # exist
                                spe_constTrue.append(spename)
                                s1.setConstant(True)
                else:
                    spe_constTrue.append(spename)
                    s1.setConstant(True)
            else:
                # if not bufpool then Pool, then
                s1.setBoundaryCondition(False)
                s1.setConstant(False)
            s1.setUnits("substance")
            s1.setHasOnlySubstanceUnits(False)
            if moose.exists(spe.path + '/info'):
                Anno = moose.Annotator(spe.path + '/info')
                if Anno.notes != "":
                    cleanNotesS = convertNotesSpecialChar(Anno.notes)
                    notesStringS = "<body xmlns=\"http://www.w3.org/1999/xhtml\">\n \t \t" + \
                        cleanNotesS + "\n\t </body>"
                    s1.setNotes(notesStringS)


                element = moose.element(spe)
                ele = getGroupinfo(element)
                if element.className == "Neutral" or Anno.color or Anno.textColor or sceneitems or Anno.x or Anno.y:
                    speciannoexist = True
                if speciannoexist:
                    speciAnno = "<moose:ModelAnnotation>\n"
                    if ele.className == "Neutral":
                        speciAnno = speciAnno + "<moose:Group>" + ele.name + "</moose:Group>\n"
                    if sceneitems:
                        #Saved from GUI, then scene co-ordinates are passed
                        speciAnno = speciAnno + "<moose:xCord>" + \
                                str(sceneitems[spe]['x']) + "</moose:xCord>\n" + \
                                "<moose:yCord>" + \
                                str(sceneitems[spe]['y'])+ "</moose:yCord>\n"
                    else:
                        #Saved from cmdline,genesis coordinates are kept as its
                        # SBML, cspace, python, then auto-coordinates are done
                        #and coordinates are updated in moose Annotation field
                        speciAnno = speciAnno + "<moose:xCord>" + \
                                str(Anno.x) + "</moose:xCord>\n" + \
                                "<moose:yCord>" + \
                                str(Anno.y)+ "</moose:yCord>\n"
                    if Anno.color:
                        speciAnno = speciAnno + "<moose:bgColor>" + Anno.color + "</moose:bgColor>\n"
                    if Anno.textColor:
                        speciAnno = speciAnno + "<moose:textColor>" + \
                            Anno.textColor + "</moose:textColor>\n"
                    speciAnno = speciAnno + "</moose:ModelAnnotation>"
                    s1.setAnnotation(speciAnno)
    return True


def writeCompt(modelpath, cremodel_):
    # getting all the compartments
    compts = moose.wildcardFind(modelpath + '/##[ISA=ChemCompt]')
    for compt in compts:
        comptName = convertSpecialChar(compt.name)
        # converting m3 to litre
        size = compt.volume * pow(10, 3)
        ndim = compt.numDimensions
        c1 = cremodel_.createCompartment()
        c1.setId(str(idBeginWith(comptName +
                                 "_" +
                                 str(compt.getId().value) +
                                 "_" +
                                 str(compt.getDataIndex()) +
                                 "_")))
        c1.setName(comptName)
        c1.setConstant(True)
        c1.setSize(size)
        c1.setSpatialDimensions(ndim)
        c1.setUnits('volume')
    if compts:
        return True
    else:
        return False
# write Simulation runtime,simdt,plotdt
def writeSimulationAnnotation(modelpath):
    modelAnno = ""
    plots = ""
    if moose.exists(modelpath + '/info'):
        mooseclock = moose.Clock('/clock')
        modelAnno = "<moose:ModelAnnotation>\n"
        modelAnnotation = moose.element(modelpath + '/info')
        modelAnno = modelAnno + "<moose:runTime> " + \
            str(modelAnnotation.runtime) + " </moose:runTime>\n"
        modelAnno = modelAnno + "<moose:solver> " + \
            modelAnnotation.solver + " </moose:solver>\n"
        modelAnno = modelAnno + "<moose:simdt>" + \
            str(mooseclock.dts[12]) + " </moose:simdt>\n"
        modelAnno = modelAnno + "<moose:plotdt> " + \
            str(mooseclock.dts[18]) + " </moose:plotdt>\n"
        plots = ""
        graphs = moose.wildcardFind(modelpath + "/##[TYPE=Table2]")
        for gphs in range(0, len(graphs)):
            gpath = graphs[gphs].neighbors['requestOut']
            if len(gpath) != 0:
                q = moose.element(gpath[0])
                ori = q.path
                name = convertSpecialChar(q.name)
                graphSpefound = False
                while not(isinstance(moose.element(q), moose.CubeMesh)):
                    q = q.parent
                    graphSpefound = True
                if graphSpefound:
                    if not plots:
                        plots = ori[ori.find(q.name)-1:len(ori)]
                        #plots = '/' + q.name + '/' + name

                    else:
                        plots = plots + "; "+ori[ori.find(q.name)-1:len(ori)]
                        #plots = plots + "; /" + q.name + '/' + name
        if plots != " ":
            modelAnno = modelAnno + "<moose:plots> " + plots + "</moose:plots>\n"
        modelAnno = modelAnno + "</moose:ModelAnnotation>"
    return modelAnno

def writeUnits(cremodel_):
    unitVol = cremodel_.createUnitDefinition()
    unitVol.setId("volume")
    unit = unitVol.createUnit()
    unit.setKind(UNIT_KIND_LITRE)
    unit.setMultiplier(1.0)
    unit.setExponent(1.0)
    unit.setScale(0)

    unitSub = cremodel_.createUnitDefinition()
    unitSub.setId("substance")
    unit = unitSub.createUnit()
    unit.setKind(UNIT_KIND_MOLE)
    unit.setMultiplier(1)
    unit.setExponent(1.0)
    unit.setScale(-3)

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
    
            moose.loadModel(filepath, modelpath, "gsl")

            written, c, writtentofile = mooseWriteSBML(modelpath, filepath)
            if written:
                print(" File written to ", writtentofile)
            else:
                print(" could not write model to SBML file")
