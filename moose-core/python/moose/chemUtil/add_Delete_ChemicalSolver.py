# -*- coding: utf-8 -*-
import moose
from fixXreacs import fixXreacs

def positionCompt( compt ):
    i = 0
    while (i != len(compt)-1):
        #print "PositionCompt ", compt[i+1],compt[i+1].volume, compt[i], compt[i].volume
        compt[i+1].x1 += compt[i].x1
        compt[i+1].x0 += compt[i].x1
        i += 1

def moosedeleteChemSolver(modelRoot):
    """Delete solvers from Chemical Compartment

    """
    compts = moose.wildcardFind(modelRoot + '/##[ISA=ChemCompt]')
    for compt in compts:
        if moose.exists(compt.path + '/stoich'):
            st = moose.element(compt.path + '/stoich')
            st_ksolve = st.ksolve
            st_dsolve = st.dsolve
            
            moose.delete(st)
            if moose.exists((st_ksolve).path):
                moose.delete(st_ksolve)
                print("KSolver is deleted for modelpath %s " % modelRoot)
            if moose.exists((st_dsolve).path):
                moose.delete(st_dsolve)
                print("DSolver is deleted for modelpath %s " % modelRoot)
    '''
    compts = moose.wildcardFind(modelRoot + '/##[ISA=ChemCompt]')
    for compt in compts:
        if moose.exists(compt.path + '/stoich'):
            st = moose.element(compt.path + '/stoich')
            st_ksolve = st.ksolve
            moose.delete(st)
            if moose.exists((st_ksolve).path):
                moose.delete(st_ksolve)
                print("Solver is deleted for modelpath %s " % modelRoot)

    '''

def mooseaddChemSolver(modelRoot, solver):
    """
     Add the solvers to Chemical compartment
    """
    compt = moose.wildcardFind(modelRoot + '/##[ISA=ChemCompt]')
    if compt:
        comptinfo = moose.Annotator(moose.element(compt[0]).path + '/info')
        previousSolver = comptinfo.solver
        currentSolver = previousSolver
        if solver == "Gillespie" or solver == "gssa":
            currentSolver = "gssa"
        elif solver == "Runge Kutta" or solver == "gsl":
            currentSolver = "gsl"
        elif solver == "Exponential Euler" or solver == "ee":
            currentSolver = "ee"

        if previousSolver != currentSolver:
            # if previousSolver != currentSolver
            comptinfo.solver = currentSolver
            if (moose.exists(compt[0].path + '/stoich')):
                # "A: and stoich exists then delete the stoich add solver"
                deleteSolver(modelRoot)
                setCompartmentSolver(modelRoot, currentSolver)
                return True
            else:
                # " B: stoich doesn't exists then addSolver, this is when object is deleted which delete's the solver "
                #  " and solver is also changed, then add addsolver "
                setCompartmentSolver(modelRoot, currentSolver)
                return True
        else:
            if moose.exists(compt[0].path + '/stoich'):
                # " stoich exist, doing nothing"
                return False
            else:
                # "but stoich doesn't exist,this is when object is deleted which deletes the solver
                # " but solver are not changed, then also call addSolver"
                setCompartmentSolver(modelRoot, currentSolver)
                return True
    return False


def setCompartmentSolver(modelRoot, solver):
    comptlist = dict((c, c.volume) for c in moose.wildcardFind(modelRoot + '/##[ISA=ChemCompt]'))
    comptVol = {}
    compts = []
    vol  = [v for k,v in comptlist.items()]
    volumeSort = sorted(vol)
    for k,v in comptlist.items():
        comptVol[k]= v
    for volSor in volumeSort:
        for a,b in comptVol.items():
            if b == volSor:
                compts.append(a)
    
    #compts = [key for key, value in sorted(comptlist.items(), key=lambda (k,v): (v,k))] 
    if ( len(compts) == '0'):
        print ("Atleast one compartment is required ")
        return
    else:
        if ( len(compts) > 3 ):
            print ("Warning: setSolverOnCompt Cannot handle " ,  len(compts) , " chemical compartments\n")
            return;

        elif (len(compts) >1 ):
            positionCompt(compts)

        fixXreacs( modelRoot )

        for compt in compts:
            if solver != 'ee':
                if (solver == 'gsl') or (solver == 'Runge Kutta'):
                    ksolve = moose.Ksolve(compt.path + '/ksolve')
                if (solver == 'gssa') or (solver == 'Gillespie'):
                    ksolve = moose.Gsolve(compt.path + '/gsolve')
                
                dsolve = moose.Dsolve(compt.path+'/dsolve')
                stoich = moose.Stoich(compt.path + '/stoich')
                stoich.compartment = compt
                stoich.ksolve = ksolve
                stoich.dsolve = dsolve
                stoich.path = compt.path + "/##"
        ksolveList = moose.wildcardFind(modelRoot+'/##[ISA=Ksolve]')
        dsolveList = moose.wildcardFind(modelRoot+'/##[ISA=Dsolve]')
        stoichList = moose.wildcardFind(modelRoot+'/##[ISA=Stoich]')
        
        i = 0
        while(i < len(dsolveList)-1):
            dsolveList[i+1].buildMeshJunctions(dsolveList[i])
            i += 1
        
        print( " Solver is added to model path %s" % modelRoot )
    '''
    compts = moose.wildcardFind(modelRoot + '/##[ISA=ChemCompt]')
    for compt in compts:
        if (solver == 'gsl') or (solver == 'Runge Kutta'):
            ksolve = moose.Ksolve(compt.path + '/ksolve')
        if (solver == 'gssa') or (solver == 'Gillespie'):
            ksolve = moose.Gsolve(compt.path + '/gsolve')
        if (solver != 'ee'):
            stoich = moose.Stoich(compt.path + '/stoich')
            stoich.compartment = compt
            stoich.ksolve = ksolve
            if moose.exists(compt.path):
                stoich.path = compt.path + "/##"
    stoichList = moose.wildcardFind(modelRoot + '/##[ISA=Stoich]')
    if len(stoichList) == 2:
        stoichList[1].buildXreacs(stoichList[0])
    if len(stoichList) == 3:
        stoichList[1].buildXreacs(stoichList[0])
        stoichList[1].buildXreacs(stoichList[2])

    for i in stoichList:
        i.filterXreacs()
    print( " Solver is added to model path %s" % modelRoot )
    '''
