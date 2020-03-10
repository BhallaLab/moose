# -*- coding: utf-8 -*-
from __future__ import print_function, division

################################################################
# This program is part of 'MOOSE', the
# Messaging Object Oriented Simulation Environment.
#           Copyright (C) 2015 Upinder S. Bhalla. and NCBS
# It is made available under the terms of the
# GNU Lesser General Public License version 2.1
# See the file COPYING.LIB for the full notice.
#
# rxdSpineSize.py: Builds a cell with spines and a propagating reaction
# wave. Products diffuse into the spine and cause it to get bigger.
#################################################################

# Cdc42 activation profile (spatial and time) has to be accounted,
# Tension modification
# Modification logs:
#     Model is developed by Vinu Verghese.
#     Modified by Dilawar Singh <dilawar.s.rajput@gmail.com> for testing.

import os
import numpy as np
import moose
print("[INFO] MOOSE: %s, version:%s" % (moose.__file__, moose.__version__))
import sys
import rdesigneur as rd
import time
import xml.etree.ElementTree as ET


if sys.version_info.major == 2:
    print("[INFO ] this test works with python3 only.")
    quit(0)

try:
    import moose.SBML
except Exception as e:
    print(e)
    print("libsbml-python is not found. Disabling tests.")
    quit(0)

sdir_ = os.path.dirname(os.path.realpath(__file__))
args_ = [
    '', 1e-6, 0.018, 1, 20e-12, 1e-12, 100, 102, 105, 4, 20, 10, 1.0, 0.5,
    0.05, 0.0, 0.0055, 0.1e-14
]

PI = 3.141592653
scaling_ = 10.0
RM = 1.0 / scaling_
RA = 1.0 * scaling_
CM = 0.01 * scaling_
loc1 = str(args_[6])
loc2 = str(args_[7])
loc3 = str(args_[8])
frameruntime = 1.0
diffConst = 5e-10
dendLen = 0.25e-6
comptLen = 1e-6
diffLen = 2.0e-6
dendDia = 1e-6
somaDia = 1e-6
concInit = 0.1  # 10 micromolar
spineSpacing = float(args_[1])
spineSpacingDistrib = 1e-6
spineSize = 1.0
spineSizeDistrib = 0.5
spineAngle = np.pi / 2.0
spineAngleDistrib = 0.0
numDendSegments = 100


def computeTP(t, distS, SourceC):
    print("[INFO ] Calling computeTP", end=' ....')
    sys.stdout.flush()
    conv = moose.element('/model/chem/dend/conv')
    w = len(conv.vec.n)
    h = w
    v2d = [[0.0 for x in range(w)] for y in range(h)]
    mm = moose.vec('/model/chem/dend/mesh')
    diffL = moose.element('/model/chem/dend').diffLength
    print(conv.name, conv.vec[0].nInit, 'diff lenght %s' % diffL)
    listL = [[distS, t]]
    SourceC = SourceC
    extraDiff = 0.05e-12
    for j in listL:
        for i in range(len(conv.vec.n)):
            for k in range(len(conv.vec.conc)):
                v2d[i][k] = diffL**2.0 * (
                    SourceC /
                    (4.0 * np.pi * j[1] * np.sqrt(extraDiff * extraDiff))
                ) * np.exp(-(mm[i].Coordinates[0]**2.0) /
                           (4 * extraDiff * j[1]) - (mm[k].Coordinates[0]**2) /
                           (4 * extraDiff * j[1]))
    tempRec = []
    if True:
        for i in range(len(conv.vec.n)):
            tempRec.append(v2d[i][distS])
    print("  .... done")
    sys.stdout.flush()
    return tempRec


def pert_int(posPer, SourceC):
    print('In pert_int')
    timePer = int(args_[9])
    distS = int(args_[10])
    Ca = moose.element('/model/chem/dend/Ca')
    conv = moose.element('/model/chem/dend/conv')
    rec = moose.element('/model/chem/dend/IRSpGr/rec')
    Ca_vec = moose.vec('/model/chem/dend/Ca').conc
    tempRec = computeTP(timePer, distS, SourceC)
    print(len(tempRec))
    sys.stdout.flush()
    moose.element('/model/chem/dend/IRSpGr/rec').vec[posPer].conc += tempRec[0]
    for j in range(posPer + 1, len(Ca_vec)):
        print(j, end=' ')
        sys.stdout.flush()
        #rec.vec[j].conc=tempRec[j-posPer]
        moose.element('/model/chem/dend/IRSpGr/rec').vec[j].conc += tempRec[
            j - posPer]
        if int(posPer * 2 - j) >= 0:
            #rec.vec[posPer*2-j].conc=tempRec[j-posPer]
            moose.element('/model/chem/dend/IRSpGr/rec').vec[
                posPer * 2 - j].conc += tempRec[j - posPer]
    moose.element('/model/chem/dend/IRSpGr/rec').conc = moose.element(
        '/model/chem/dend/IRSpGr/rec').conc / 2

    print('PERTURBED')

    return distS, timePer


def makeCellProto(name):
    elec = moose.Neuron('/library/' + name)
    ecompt = []
    #soma = rd.buildCompt( elec, 'soma', dx=somaDia, dia=somaDia, x=-somaDia, RM=RM, RA=RA, CM=CM )
    dend = rd.buildCompt(elec,
                         'dend',
                         dx=dendLen,
                         dia=dendDia,
                         x=0,
                         RM=RM,
                         RA=RA,
                         CM=CM)
    #moose.connect( soma, 'axial', dend, 'raxial' )
    prev = dend
    x = dendLen
    for i in range(numDendSegments):
        compt = rd.buildCompt(elec,
                              'dend' + str(i),
                              dx=dendLen,
                              dia=dendDia,
                              x=x,
                              RM=RM,
                              RA=RA,
                              CM=CM)
        moose.connect(prev, 'axial', compt, 'raxial')
        prev = compt
        x = x + dendLen
    elec.buildSegmentTree()


def makeDendProto(name):
    dend = moose.Neuron('/library/dend')
    prev = rd.buildCompt(dend,
                         'soma',
                         RM=RM,
                         RA=RA,
                         CM=CM,
                         dia=2.0e-06,
                         x=0,
                         dx=comptLen)
    x = comptLen
    y = 0.0
    comptDia = 1e-06

    for i in range(numDendSegments):
        dx = comptLen
        dy = 0
        compt = rd.buildCompt(dend,
                              'dend' + str(i),
                              RM=RM,
                              RA=RA,
                              CM=CM,
                              x=x,
                              y=y,
                              dx=dx,
                              dy=dy,
                              dia=comptDia)
        moose.connect(prev, 'axial', compt, 'raxial')
        prev = compt
        x += dx
        y += dy


def makeChemProto(name='hydra'):
    moose.Neutral('/library/')
    meshName = '/library/' + name
    moose.SBML.mooseReadSBML(os.path.join(sdir_, '..', 'data', 'c_m.xml'),
                             meshName)
    Rad = moose.element('/library/hydra/dend/IRSpGr/Rad')

    I_C = moose.element(meshName + '/dend/IRSpGr/IRSp53')
    Ca = moose.element(meshName + '/dend/Ca')
    rec = moose.Pool(meshName + '/dend/IRSpGr/rec')
    conv = moose.Pool(meshName + '/dend/conv')
    Cdc42 = moose.element(meshName + '/dend/IRSpGr/Cdc42')
    Cdc42_m = moose.element(meshName + '/dend/IRSpGr/Cdc42_m')
    Cdc42_GDP = moose.element(meshName + '/dend/Cdc42_GDP')
    Rac_GTP = moose.element(meshName + '/dend/Rac_GTP')
    Rac_m = moose.element(meshName + '/dend/IRSpGr/Rac_m')
    Rac_GDP = moose.element(meshName + '/dend/Rac_GDP')
    Eps8 = moose.element(meshName + '/dend/IRSpGr/Eps8')
    I_Cact = moose.element(meshName + '/dend/IRSpGr/IRSp53_a')
    I_M = moose.element(meshName + '/dend/IRSpGr/IRSp53_m')
    Idimer = moose.element(meshName + '/dend/IRSpGr/IRSp53_dimer')
    recBAR = moose.Function(meshName + '/dend/IRSpGr/recBAR')
    gradBAR = moose.Function(meshName + '/dend/IRSpGr/gradBAR')
    Radfun = moose.Function(meshName + '/dend/IRSpGr/Radfun')
    Ipool = moose.element(meshName + '/dend/IRSpGr/Ipool')
    sort = moose.element(meshName + '/dend/IRSpGr/sort')
    detach = moose.element(meshName + '/dend/IRSpGr/detach')
    curv_IRSp53 = moose.element(meshName + '/dend/IRSpGr/curv_IRSp53')
    tube_IRSp53 = moose.element(meshName + '/dend/IRSpGr/tube_IRSp53')
    to_tube_Reac = moose.element(meshName + '/dend/IRSpGr/to_tube_Reac')
    Cdc42_breac = moose.element(meshName + '/dend/Cdc42_GTP_GDP_Reac')
    Tiam_breac = moose.element(meshName + '/dend/Tiam_Rev_Reac')
    store_tube = moose.Pool(meshName + '/dend/IRSpGr/store_tube')
    IRSp53_temp = moose.element(meshName + '/dend/IRSpGr/IRSp53_temp')
    sigma = moose.Pool(meshName + '/dend/IRSpGr/sigma')
    Kv = moose.Pool(meshName + '/dend/IRSpGr/Kv')

    dend_vasp = moose.element(meshName + '/dend/IRSpGr/VASP')

    dend_gactin = moose.element(meshName + '/dend/IRSpGr/Gactin')
    dend_factin = moose.element(meshName + '/dend/IRSpGr/Factin')
    GF_Reac = moose.element(meshName + '/dend/IRSpGr/GF_Reac')
    sort_func = moose.element(meshName + '/dend/IRSpGr/sort_func')
    breacT_fun = moose.Function(meshName + '/dend/breacT_fun')
    breacC_fun = moose.Function(meshName + '/dend/breacC_fun')
    simpleFun = moose.Function(meshName + '/dend/IRSpGr/simpleFun')
    simpleFun1 = moose.Function(meshName + '/dend/IRSpGr/simpleFun1')
    simpleFun2 = moose.Function(meshName + '/dend/IRSpGr/simpleFun2')
    Kvfun = moose.Function(meshName + '/dend/IRSpGr/Kvfun')
    recFun = moose.Function(meshName + '/dend/IRSpGr/recFun')
    simpleFun2 = moose.Function(meshName + '/dend/IRSpGr/simpleFun2')
    simpleFun3 = moose.Function(meshName + '/dend/IRSpGr/simpleFun3')
    mod_sort = moose.element(meshName + '/dend/IRSpGr/mod_sort')
    mod_detach = moose.element(meshName + '/dend/IRSpGr/mod_detach')
    to_tube_Reac = moose.element(meshName + '/dend/IRSpGr/to_tube_Reac')

    conv_fun = moose.Function(meshName + '/dend/IRSpGr/conv_fun')
    conv_fun.expr = "1*x0*(x1-1.89191155)+1.89191155"
    Kvfun.expr = "1.0"
    moose.connect(Kvfun, 'valueOut', Kv, 'setN')

    simpleFun.expr = "1.0*x0"
    simpleFun1.expr = "1.0*x0"
    simpleFun2.expr = "0.1*x0"
    simpleFun3.expr = "1.0*x0"
    breacT_fun.expr = "5*x0"
    breacC_fun.expr = "5*x0"

    moose.connect(curv_IRSp53, 'nOut', simpleFun2.x[0], 'input')
    moose.connect(simpleFun2, 'valueOut', GF_Reac, 'setNumKf')

    moose.connect(sort_func, 'nOut', simpleFun.x[0], 'input')
    moose.connect(detach, 'nOut', simpleFun1.x[0], 'input')
    moose.connect(store_tube, 'nOut', simpleFun3.x[0], 'input')

    recFun.expr = "0.0*x0+(exp(45*54*((" + str(
        args_[14]) + "/(x0*1e9))-(1/(2.0*(x0*1e9)*(x0*1e9))))))"

    Radfun.expr = "sqrt(((45+12)*4*1e-18)/(2*(x2-0.08*(ln((((1.0*x0+1.0*x1)/(2*3.14*(0.45*0.05)))*1e-6*54))))))*((3*" + str(
        float(args_[2])) + "*x3*(x0+1.0*x1)^2)/(1.+3*" + str(float(
            args_[2])) + "*x3*(x0+1.0*x1)^2))"

    mod_sort.Kf = 0.
    mod_sort.Kb = 0.
    mod_detach.Kf = 0.
    mod_detach.Kb = 0.

    moose.connect(curv_IRSp53, 'nOut', Radfun.x[0], 'input')
    moose.connect(I_M, 'nOut', Radfun.x[1], 'input')
    #moose.connect(tube_IRSp53,'nOut',Radfun.x[2],'input')
    moose.connect(sigma, 'nOut', Radfun.x[2], 'input')
    moose.connect(Kv, 'nOut', Radfun.x[3], 'input')
    moose.connect(Radfun, 'valueOut', Rad, 'setN')

    moose.connect(rec, 'nOut', conv_fun.x[0], 'input')
    moose.connect(conv, 'nOut', conv_fun.x[1], 'input')
    moose.connect(conv_fun, 'valueOut', Ca, 'setN')

    moose.connect(simpleFun, 'valueOut', mod_sort, 'setNumKf')
    moose.connect(simpleFun1, 'valueOut', mod_detach, 'setNumKf')

    moose.connect(Rad, 'nOut', recFun.x[0], 'input')
    moose.connect(recFun, 'valueOut', Ipool, 'setN')

    ### Ipool is further copied to sort to participate in the reaction tube_reac or enz (model version 2.5.3)

    I_C.concInit = 0.0012
    file_molWt = os.path.join(sdir_, '..', 'data', 'prot_wt.xml')
    den = 6.0 * 3.14 * np.cbrt(3.0 / (4.0 * 3.14 * 0.73 * 6.02 * 1e23))
    num = 1e3 * 1e6 * 1.38 * 1e-23
    co = num / den

    # Diffconsts are set here
    if int(args_[3]) == 1:
        for prot in moose.wildcardFind(meshName + '/dend/#[ISA=Pool]'):
            mol_name = ET.parse(file_molWt).find(str(prot.name))
            print('Name is %s' % mol_name, end='. ')
            molWt = mol_name.text.split()
            if float(molWt[1]) <= 0.0:
                continue
            moose.element(prot).diffConst = co * (
                300 / (200.0 * np.cbrt(float(molWt[1]) * 1e3))) * 1e-4
            print('-- D', prot.name, moose.element(prot).diffConst)

        for prot in moose.wildcardFind(meshName +
                                       '/dend/CaMKII_gr/#[ISA=Pool]'):
            mol_name = ET.parse(file_molWt).find(str(prot.name))
            print('Name is %s' % mol_name, end='. ')
            molWt = mol_name.text.split()
            if float(molWt[1]) <= 0.0:
                continue
            moose.element(prot).diffConst = co * (
                300 / (200.0 * np.cbrt(float(molWt[1]) * 1e3))) * 1e-4
            print('-- D', prot.name, moose.element(prot).diffConst)
        for prot in moose.wildcardFind(meshName + '/dend/Ras_gr/#[ISA=Pool]'):
            mol_name = ET.parse(file_molWt).find(str(prot.name))
            print('Name is %s' % mol_name, end='. ')
            molWt = mol_name.text.split()
            if float(molWt[1]) <= 0.0:
                continue
            moose.element(prot).diffConst = co * (
                300 / (200.0 * np.cbrt(float(molWt[1]) * 1e3))) * 1e-4
            print('-- D', prot.name, moose.element(prot).diffConst)

        Ca.diffConst = float(args_[4])
        print('-- D', Ca.name, Ca.diffConst)
        print('\t==================== diffConst updated')

    print('DIFFUSION FOR IRSp53 GROUP')
    I_C.diffConst = co * (300 / (8.0 * np.cbrt(120.0 * 1e3))) * 1e-4
    print(I_C.diffConst, end=' + ')
    I_Cact.diffConst = co * (300 / (8.0 * np.cbrt(208.0 * 1e3))) * 1e-4
    print(I_Cact.diffConst)
    Eps8.diffConst = co * (300 / (200.0 * np.cbrt(92.0 * 1e3))) * 1e-4
    print(Eps8.diffConst, end=' + ')
    Cdc42.diffConst = co * (300 / (8.0 * np.cbrt(22.0 * 1e3))) * 1e-4
    Cdc42_m.diffConst = co * (300 / (200.0 * np.cbrt(22.0 * 1e3))) * 1e-4
    Cdc42_GDP.diffConst = co * (300 / (8.0 * np.cbrt(22.0 * 1e3))) * 1e-4
    print(Cdc42.diffConst, end=' + ')
    Rac_GTP.diffConst = co * (300 / (8.0 * np.cbrt(22.0 * 1e3))) * 1e-4
    Rac_m.diffConst = co * (300 / (200.0 * np.cbrt(22.0 * 1e3))) * 1e-4
    Rac_GDP.diffConst = co * (300 / (8.0 * np.cbrt(22.0 * 1e3))) * 1e-4

    I_M.diffConst = 1.0 * co * (300 / (200.0 * np.cbrt(392.0 * 1e3))) * 1e-4
    print(I_M.diffConst, end=' + ')
    Idimer.diffConst = co * (300 / (8.0 * np.cbrt(120.0 * 1e3))) * 1e-4
    print(Idimer.diffConst)
    curv_IRSp53.diffConst = float(args_[17])

    conv.diffConst = 0.0
    rec.diffConst = 0.0

    dend_gactin.diffConst = 1e-13
    dend_vasp.diffConst = 1e-13

    return meshName


def makeModel():
    moose.Neutral('/library')
    makeCellProto('cellProto')
    makeChemProto()
    rdes = rd.rdesigneur(
        useGssa=False,
        turnOffElec=False,
        chemDt=0.0025,
        diffDt=0.0025,
        chemPlotDt=0.1,
        combineSegments=False,
        stealCellFromLibrary=True,
        diffusionLength=50e-9,
        cellProto=[['cellProto', 'elec']],
        chanProto=[
            ['make_NMDA()', 'NMDA'],
            ['make_Ca_conc()', 'Ca_conc'],
        ],
        chemProto=[['hydra', 'chem']],
        chemDistrib=[["chem", "#", "install", "1"]],
        chanDistrib=[
            ['NMDA', 'dend#', 'Gbar', '1'],
            ['Ca_conc', 'dend#', 'tau', '0.033'],
        ],
        stimList=[
            [
                'dend#', '1.', 'NMDA', 'periodicsyn',
                str(args_[13]) + '*(t>0 && t<60)+' + str(args_[13]) + '*(t>' +
                str(args_[15]) + ' && t<(' + str(args_[15]) + '+60))'
            ],
        ],
        adaptorList=[['Ca_conc', 'Ca', 'dend/conv', 'conc', 0.00008, 20]],
        benchmark=True)
    moose.seed(100)
    print("[INFO ] rdes is building model ...")
    rdes.buildModel('/model')
    print("[INFO ] Model is built...")
    moose.element('/model/chem/dend/IRSpGr/Rad').nInit = 30e-9


def makePlot(name, srcVec, field):
    tab = moose.Table2('/graphs/' + name + 'Tab', len(srcVec)).vec
    for i in zip(srcVec, tab):
        moose.connect(i[1], 'requestOut', i[0], field)
    return tab


def main():
    """
    This illustrates the use of rdesigneur to build a simple dendrite with
    spines, and then to resize them using spine fields. These are the
    fields that would be changed dynamically in a simulation with reactions
    that affect spine geometry.
    In this simulation there is a propagating reaction wave using a
    highly abstracted equation, whose product diffuses into the spines and
    makes them bigger.
    """
    t0 = time.time()
    makeModel()
    print("[INFO ] Took %f sec." % (time.time() - t0))
    sys.stdout.flush()
    for i in range(11, 18):
        moose.setClock(i, 0.0025)
    moose.setClock(18, 0.0025)
    moose.element('/model/elec')
    moose.Neutral('/graphs')
    Cdcinit = moose.vec('/model/chem/dend/IRSpGr/Cdc42')
    Tiaminit = moose.vec('/model/chem/dend/Tiam1')
    Cdcinit.concInit = 0.0
    Tiaminit.concInit = 0.0

    makePlot('dend_curv_IRSp53',
             moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53'), 'getN')
    makePlot('sorting', moose.vec('/model/chem/dend/IRSpGr/sort'), 'getN')
    makePlot('Ca', moose.vec('/model/chem/dend/Ca'), 'getN')
    makePlot('CaM', moose.vec('/model/chem/dend/CaM'), 'getConc')
    makePlot('CaM_GEF', moose.vec('/model/chem/dend/Ras_gr/CaM_GEF'),
             'getConc')
    makePlot('Ca4CaM', moose.vec('/model/chem/dend/CaM_Ca4'), 'getConc')
    makePlot('Cdc42', moose.vec('/model/chem/dend/IRSpGr/Cdc42'), 'getN')
    makePlot('RacGTP', moose.vec('/model/chem/dend/Rac_GTP'), 'getN')
    makePlot('Tiam', moose.vec('/model/chem/dend/Tiam1'), 'getN')
    mypyrun = moose.PyRun('/model/mypyrun')
    mypyrun.initString = """count=0"""
    mypyrun.runString = """count=count+1
radv=moose.vec('/model/chem/dend/IRSpGr/Rad').n
radm=radv[10]
output=radm
IRvec=moose.vec( '/model/chem/dend/IRSpGr/IRSp53' ).n
curv_IRSp53=moose.element( '/model/chem/dend/IRSpGr/curv_IRSp53')
Cdcvec=moose.vec( '/model/chem/dend/IRSpGr/Cdc42' ).n
if count==1:
    Cdcinit=moose.vec( '/model/chem/dend/IRSpGr/Cdc42' ).nInit
    pos=np.unravel_index(np.argmax(Cdcinit), Cdcinit.shape)
    pos=pos[0]
curv_IRSp53vec=moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53').n
sigmaVec=moose.vec('/model/chem/dend/IRSpGr/sigma').n
IRSp53_mvec=moose.vec('/model/chem/dend/IRSpGr/IRSp53_m').n
Ipoolvec=moose.vec('/model/chem/dend/IRSpGr/Ipool').n
Kv=moose.vec('/model/chem/dend/IRSpGr/Kv').n
sort_func=moose.vec('/model/chem/dend/IRSpGr/sort').n
for gri in range(0,len(Cdcvec)):
   sort_func[gri]=1.0*((3*float(args_[2])*Kv[gri]*(1.0*curv_IRSp53vec[gri]+1.0*IRSp53_mvec[gri])**2)/(1.0+3.0*float(args_[2])*Kv[gri]*(1.0*curv_IRSp53vec[gri]+1.0*IRSp53_mvec[gri])**2))*sort_func[gri]
if count==1:
   maxCIR=[]
   maxIR=[]
   max_curv=[]
for gri in range(0,len(Cdcvec)):
   sigmaVec[gri]=(0.3-0.25*np.exp(-0.015*count*0.0025))*((float(args_[2])*3*curv_IRSp53vec[gri]**2)/(1+float(args_[2])*3*curv_IRSp53vec[gri]**2))+0.0055
   s_m=1-np.exp(-sigmaVec[gri]/0.3)
   sort_func[gri]=sort_func[gri]-1.6*s_m*sort_func[gri]
det_vec=moose.vec('/model/chem/dend/IRSpGr/detach').n
for gri in range(0,len(Cdcvec)):
   s_m=1-np.exp(-sigmaVec[gri]/0.3)
   det_vec[gri]=det_vec[gri]+1.6*s_m*det_vec[gri]
moose.vec('/model/chem/dend/IRSpGr/detach').n=det_vec   
temp_sort=moose.vec('/model/chem/dend/IRSpGr/sort_func').n
pref_curv=0.055
radmax=max(radv)
if count%100==0:
  print("Sum of IRSp53s")
  print(sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_dimer').n)+sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_a').n)+sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_m').n)+sum(moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53').n),sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_m').n),sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_a').n),sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_dimer').n),sum(moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53').n))
  sys.stdout.flush()
"""
    outputTab = moose.Table('/model/output')
    moose.connect(mypyrun, 'output', outputTab, 'input')
    moose.setClock(30, 0.0025)
    moose.setClock(10, 0.0025)
    moose.setClock(15, 0.0025)
    moose.setClock(16, 0.0025)
    mypyrun.mode = 1

    k = moose.wildcardFind('/##[TYPE=Ksolve]')[0]
    k.numThreads = 4
    print('   .... done making plots.')
    print("[INFO ] Doing moose.reinit ...")
    sys.stdout.flush()
    moose.reinit()
    print("     ... DONE REINIT")
    sys.stdout.flush()
    print('\n TEST PROPAGATING WAVES \n')
    recavec = []
    loc1_vox = (int(loc1) * dendLen) / 50e-9
    loc2_vox = (int(loc2) * dendLen) / 50e-9
    loc3_vox = (int(loc3) * dendLen) / 50e-9

    pert_int(int(loc1_vox), int(args_[11]))

    temp = moose.vec('/model/chem/dend/IRSpGr/rec').n
    recavec.append(moose.vec('/model/chem/dend/IRSpGr/rec').n)
    gap = int(loc2_vox) - int(loc1_vox)
    gap_p = int(loc3_vox) - int(loc1_vox)
    recavec.append([
        0.0 + float(args_[12]) * temp[i - gap_p]
        for i in range(0, len(moose.vec('/model/chem/dend/IRSpGr/rec').n))
    ])
    moose.vec('/model/chem/dend/IRSpGr/rec').n = [
        temp[i - gap]
        for i in range(0, len(moose.vec('/model/chem/dend/IRSpGr/rec').n))
    ]
    recavec.append(moose.vec('/model/chem/dend/IRSpGr/rec').n)
    moose.vec('/model/chem/dend/IRSpGr/rec').n = [
        temp[i - gap]
        for i in range(0, len(moose.vec('/model/chem/dend/IRSpGr/rec').n))
    ]
    moose.vec('/model/chem/dend/IRSpGr/rec'
              ).n = moose.vec('/model/chem/dend/IRSpGr/rec').n + temp
    recavec.append(moose.vec('/model/chem/dend/IRSpGr/rec').n)
    if float(args_[15]) == 0.0:
        moose.vec('/model/chem/dend/IRSpGr/rec').n = recavec[3]
    else:
        moose.vec('/model/chem/dend/IRSpGr/rec').n = recavec[0]

    allres = []
    t0 = time.time()
    for i in range(3):
        print("[INFO ] Simulation step %d/3" % (1 + i))
        moose.start(0.5)
        res = sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_dimer').n) + sum(
            moose.vec('/model/chem/dend/IRSpGr/IRSp53_a').n
        ) + sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_m').n) + sum(
            moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53').n), sum(
                moose.vec('/model/chem/dend/IRSpGr/IRSp53_m').n
            ), sum(moose.vec('/model/chem/dend/IRSpGr/IRSp53_a').n), sum(
                moose.vec('/model/chem/dend/IRSpGr/IRSp53_dimer').n), sum(
                    moose.vec('/model/chem/dend/IRSpGr/curv_IRSp53').n)
        allres.append(res)

    print("== Time taken %g s" % (time.time() - t0))

    # These values are created by GSL based solver. Boost solvers generates
    # slightly different values.
    gslExpected = [[1.43312300e+04, 3.59179272e-10, 7.90030831e-09, 1.43312300e+04, 0.00000000e+00],
        [1.43312300e+04, 5.85016305e-08, 6.01850896e-07, 1.43312300e+04, 0.00000000e+00],
        [1.43312300e+04, 1.88782106e-01, 1.78215231e+00, 1.43292591e+04, 0.00000000e+00]]
    gslExpected = np.array(gslExpected)

    allres = np.array(allres)
    # Compare the last value. When using Boost based solvers these numbers can
    # differ a bit but not more than 1%ii.
    assert np.allclose(gslExpected, allres, rtol=1e-2, atol=1e-3), (allres, gslExpected)
    print('done')

if __name__ == '__main__':
    main()
