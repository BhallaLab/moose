import sys
import random
from . import wildcardFind, element, loadModel, ChemCompt, exists, Annotator, Pool, ZombiePool,PoolBase,CplxEnzBase,Function,ZombieFunction
import numpy as np

#Todo : To be written
#               --Notes
#               --StimulusTable

def writeKkit( modelpath, filename,sceneitems=None):
        global NA
        NA = 6.0221415e23
        global xmin,xmax,ymin,ymax
        global cord
        global multi
        xmin = ymin = 0
        xmax = ymax = 1
        multi = 50
        cord = {}
        compt = wildcardFind(modelpath+'/##[ISA=ChemCompt]')
        maxVol = estimateDefaultVol(compt)
        f = open(filename, 'w')
        writeHeader (f,maxVol)
        if (compt > 0):
                if sceneitems == None:
                        #if sceneitems is none (loaded from script) then check x,y cord exists
                        xmin,ymin,xmax,ymax,positionInfoExist = getCor(modelpath,sceneitems)
                        if not positionInfoExist:
                                #incase of SBML or cspace or python Annotator is not populated then positionInfoExist= False
                                #print " x and y cordinates doesn't exist so auto cordinates"
                                print(" auto co-ordinates needs to be applied")
                                pass
                else:
                        #This is when it comes from Gui where the objects are already layout on to scene
                        # so using thoes co-ordinates
                        xmin,ymin,xmax,ymax,positionInfoExist = getCor(modelpath,sceneitems)

                gtId_vol = writeCompartment(modelpath,compt,f)
                writePool(modelpath,f,gtId_vol)
                reacList = writeReac(modelpath,f)
                enzList = writeEnz(modelpath,f)
                writeSumtotal(modelpath,f)
                storeReacMsg(reacList,f)
                storeEnzMsg(enzList,f)
                writeGui(f)
                tgraphs = wildcardFind(modelpath+'/##[ISA=Table2]')
                if tgraphs:
                        writeplot(tgraphs,f)
                        storePlotMsgs(tgraphs,f)
                writeFooter(f)
                return True
        else:
                print("Warning: writeKkit:: No model found on " , modelpath)
                return False

def storeCplxEnzMsgs( enz, f ):
        for sub in enz.neighbors["subOut"]:
                s = "addmsg /kinetics/" + trimPath( sub ) + " /kinetics/" + trimPath(enz) + " SUBSTRATE n \n";
                s = s+ "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath( sub ) +        " REAC sA B \n";
                f.write(s)
        for prd in enz.neighbors["prd"]:
                s = "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath(prd) + " MM_PRD pA\n";
                f.write( s )
        for enzOut in enz.neighbors["enzOut"]:
                s = "addmsg /kinetics/" + trimPath( enzOut ) + " /kinetics/" + trimPath(enz) + " ENZYME n\n";
                s = s+ "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath(enzOut) + " REAC eA B\n";
                f.write( s )

def storeMMenzMsgs( enz, f):
        subList = enz.neighbors["subOut"]
        prdList = enz.neighbors["prd"]
        enzDestList = enz.neighbors["enzDest"]
        for esub in subList:
                es = "addmsg /kinetics/" + trimPath(element(esub)) + " /kinetics/" + trimPath(enz) + " SUBSTRATE n \n";
                es = es+"addmsg /kinetics/" + trimPath(enz) + " /kinetics/" + trimPath(element(esub)) + " REAC sA B \n";
                f.write(es)

        for eprd in prdList:
                es = "addmsg /kinetics/" + trimPath( enz ) + " /kinetics/" + trimPath( element(eprd)) + " MM_PRD pA \n";
                f.write(es)
        for eenzDest in enzDestList:
                enzDest = "addmsg /kinetics/" + trimPath( element(eenzDest)) + " /kinetics/" + trimPath( enz ) + " ENZYME n \n";
                f.write(enzDest)

def storeEnzMsg( enzList, f):
        for enz in enzList:
                enzClass = enz.className
                if (enzClass == "ZombieMMenz" or enzClass == "MMenz"):
                        storeMMenzMsgs(enz, f)
                else:
                        storeCplxEnzMsgs( enz, f )

def writeEnz( modelpath,f):
        enzList = wildcardFind(modelpath+'/##[ISA=EnzBase]')
        for enz in enzList:
                x = random.randrange(0,10)
                y = random.randrange(0,10)
                textcolor = "green"
                color = "red"
                k1 = 0;
                k2 = 0;
                k3 = 0;
                nInit = 0;
                concInit = 0;
                n = 0;
                conc = 0;
                enzParent = enz.parent
                if (isinstance(enzParent.className,Pool)) or (isinstance(enzParent.className,ZombiePool)):
                        print(" raise exception enz doesn't have pool as parent")
                        return False
                else:
                        vol = enzParent.volume * NA * 1e-3;
                        isMichaelisMenten = 0;
                        enzClass = enz.className
                        if (enzClass == "ZombieMMenz" or enzClass == "MMenz"):
                                k1 = enz.numKm
                                k3 = enz.kcat
                                k2 = 4.0*k3;
                                k1 = (k2 + k3) / k1;
                                isMichaelisMenten = 1;

                        elif (enzClass == "ZombieEnz" or enzClass == "Enz"):
                                k1 = enz.k1
                                k2 = enz.k2
                                k3 = enz.k3
                                cplx = enz.neighbors['cplx'][0]
                                nInit = cplx.nInit[0];

                        xe = cord[enz]['x']
                        ye = cord[enz]['y']
                        x = ((xe-xmin)/(xmax-xmin))*multi
                        y = ((ye-ymin)/(ymax-ymin))*multi
                        #y = ((ymax-ye)/(ymax-ymin))*multi
                        einfo = enz.path+'/info'
                        if exists(einfo):
                                color = Annotator(einfo).getField('color')
                                textcolor = Annotator(einfo).getField('textColor')
                        f.write("simundump kenz /kinetics/" + trimPath(enz) + " " + str(0)+  " " +
                                        str(concInit) + " " +
                                        str(conc) + " " +
                                        str(nInit) + " " +
                                        str(n) + " " +
                                        str(vol) + " " +
                                        str(k1) + " " +
                                        str(k2) + " " +
                                        str(k3) + " " +
                                        str(0) + " " +
                                        str(isMichaelisMenten) + " " +
                                        "\"\"" + " " +
                                        str(color) + " " + str(textcolor) + " \"\"" +
                                        " " + str(x) + " " + str(y) + " "+str(0)+"\n")
        return enzList
def storeReacMsg(reacList,f):
        for reac in reacList:
                reacPath = trimPath( reac);
                sublist = reac.neighbors["subOut"]
                prdlist = reac.neighbors["prd"]
                for sub in sublist:
                        s = "addmsg /kinetics/" + trimPath( sub ) + " /kinetics/" + reacPath +  " SUBSTRATE n \n";
                        s =  s + "addmsg /kinetics/" + reacPath + " /kinetics/" + trimPath( sub ) +  " REAC A B \n";
                        f.write(s)

                for prd in prdlist:
                        s = "addmsg /kinetics/" + trimPath( prd ) + " /kinetics/" + reacPath + " PRODUCT n \n";
                        s = s + "addmsg /kinetics/" + reacPath + " /kinetics/" + trimPath( prd ) +  " REAC B A\n";
                        f.write( s)

def writeReac(modelpath,f):
        reacList = wildcardFind(modelpath+'/##[ISA=ReacBase]')
        for reac in reacList :
                color = "blue"
                textcolor = "red"
                kf = reac.numKf
                kb = reac.numKb
                xr = cord[reac]['x']
                yr = cord[reac]['y']
                x = ((xr-xmin)/(xmax-xmin))*multi
                y = ((yr-ymin)/(ymax-ymin))*multi
                #y = ((ymax-yr)/(ymax-ymin))*multi
                rinfo = reac.path+'/info'
                if exists(rinfo):
                        color = Annotator(rinfo).getField('color')
                        textcolor = Annotator(rinfo).getField('textColor')
                f.write("simundump kreac /kinetics/" + trimPath(reac) + " " +str(0) +" "+ str(kf) + " " + str(kb) + " \"\" " +
                        str(color) + " " + str(textcolor) + " " + str(x) + " " + str(y) + " 0\n")
        return reacList

def trimPath(mobj):
        original = mobj
        mobj = element(mobj)
        found = False
        while not isinstance(mobj,ChemCompt) and mobj.path != "/":
                mobj = element(mobj.parent)
                found = True
        if mobj.path == "/":
                print("compartment is not found with the given path and the path has reached root ",original)
                return
        #other than the kinetics compartment, all the othername are converted to group in Genesis which are place under /kinetics
        # Any moose object comes under /kinetics then one level down the path is taken.
        # e.g /group/poolObject or /Reac
        if found:
                if mobj.name != "kinetics":
                        splitpath = original.path[(original.path.find(mobj.name)):len(original.path)]
                else:

                        pos = original.path.find(mobj.name)
                        slash = original.path.find('/',pos+1)
                        splitpath = original.path[slash+1:len(original.path)]
                return splitpath

def writeSumtotal( modelpath,f):
        funclist = wildcardFind(modelpath+'/##[ISA=Function]')
        for func in funclist:
                funcInputs = element(func.path+'/x[0]')
                s = ""
                for funcInput in funcInputs.neighbors["input"]:
                        s = s+ "addmsg /kinetics/" + trimPath(funcInput)+ " /kinetics/" + trimPath(element(func.parent)) + " SUMTOTAL n nInit\n"
                f.write(s)

def storePlotMsgs( tgraphs,f):
        s = ""
        if tgraphs:
                for graph in tgraphs:
                        slash = graph.path.find('graphs')
                        if not slash > -1:
                                slash = graph.path.find('graph_0')
                        if slash > -1:
                                conc = graph.path.find('conc')
                                if conc > -1 :
                                        tabPath = graph.path[slash:len(graph.path)]
                                else:
                                        slash1 = graph.path.find('/',slash)
                                        tabPath = "graphs/conc1" +graph.path[slash1:len(graph.path)]

                                if len(element(graph).msgOut):
                                        poolPath = (element(graph).msgOut)[0].e2.path
                                        poolEle = element(poolPath)
                                        poolName = poolEle.name
                                        bgPath = (poolEle.path+'/info')
                                        bg = Annotator(bgPath).color
                                        s = s+"addmsg /kinetics/" + trimPath( poolEle ) + " /" + tabPath + \
                                                " PLOT Co *" + poolName + " *" + bg +"\n";
        f.write(s)

def writeplot( tgraphs,f ):
        if tgraphs:
                for graphs in tgraphs:
                        slash = graphs.path.find('graphs')
                        if not slash > -1:
                                slash = graphs.path.find('graph_0')
                        if slash > -1:
                                conc = graphs.path.find('conc')
                                if conc > -1 :
                                        tabPath = graphs.path[slash:len(graphs.path)]
                                else:
                                        slash1 = graphs.path.find('/',slash)
                                        tabPath = "graphs/conc1" +graphs.path[slash1:len(graphs.path)]

                                if len(element(graphs).msgOut):
                                        poolPath = (element(graphs).msgOut)[0].e2.path
                                        poolEle = element(poolPath)
                                        poolAnno = (poolEle.path+'/info')
                                        fg = Annotator(poolAnno).textColor
                                        f.write("simundump xplot " + tabPath + " 3 524288 \\\n" + "\"delete_plot.w <s> <d>; edit_plot.D <w>\" " + fg + " 0 0 1\n")

def writePool(modelpath,f,volIndex ):
        for p in wildcardFind(modelpath+'/##[ISA=PoolBase]'):
                slave_enable = 0
                if (p.className == "BufPool" or p.className == "ZombieBufPool"):
                        pool_children = p.children
                        if pool_children== 0:
                                slave_enable = 4
                        else:
                                for pchild in pool_children:
                                        if not(pchild.className == "ZombieFunction") and not(pchild.className == "Function"):
                                                slave_enable = 4
                                        else:
                                                slave_enable = 0
                                                break

                xp = cord[p]['x']
                yp = cord[p]['y']
                x = ((xp-xmin)/(xmax-xmin))*multi
                y = ((yp-ymin)/(ymax-ymin))*multi
                #y = ((ymax-yp)/(ymax-ymin))*multi

                pinfo = p.path+'/info'
                if exists(pinfo):
                        color = Annotator(pinfo).getField('color')
                        textcolor = Annotator(pinfo).getField('textColor')

                geometryName = volIndex[p.volume]
                volume = p.volume * NA * 1e-3
                f.write("simundump kpool /kinetics/" + trimPath(p) + " 0 " +
                        str(p.diffConst) + " " +
                        str(0) + " " +
                        str(0) + " " +
                        str(0) + " " +
                        str(p.nInit) + " " +
                        str(0) + " " + str(0) + " " +
                        str(volume)+ " " +
                        str(slave_enable) +
                        " /kinetics"+ geometryName + " " +
                        str(color) +" " + str(textcolor) + " " + str(x) + " " + str(y) + " "+ str(0)+"\n")

def getxyCord(xcord,ycord,list1,sceneitems):
        for item in list1:
                if not ( isinstance(item,Function) and isinstance(item,ZombieFunction) ):
                        if sceneitems == None:
                                objInfo = item.path+'/info'
                                xpos = xyPosition(objInfo,'x')
                                ypos = xyPosition(objInfo,'y')
                        else:
                                co = sceneitems[item]
                                xpos = co.scenePos().x()
                                ypos =-co.scenePos().y()
                        cord[item] ={ 'x': xpos,'y':ypos}
                        xcord.append(xpos)
                        ycord.append(ypos)

def xyPosition(objInfo,xory):
    try:
        return(float(element(objInfo).getField(xory)))
    except ValueError:
        return (float(0))
def getCor(modelRoot,sceneitems):
        xmin = ymin = 0.0
        xmax = ymax = 1.0
        positionInfoExist = False
        xcord = ycord = []
        mollist = realist = enzlist = cplxlist = tablist = funclist = []
        meshEntryWildcard = '/##[ISA=ChemCompt]'
        if modelRoot != '/':
                meshEntryWildcard = modelRoot+meshEntryWildcard
        for meshEnt in wildcardFind(meshEntryWildcard):
                mol_cpl  = wildcardFind(meshEnt.path+'/##[ISA=PoolBase]')
                realist  = wildcardFind(meshEnt.path+'/##[ISA=ReacBase]')
                enzlist  = wildcardFind(meshEnt.path+'/##[ISA=EnzBase]')
                funclist = wildcardFind(meshEnt.path+'/##[ISA=Function]')
                tablist  = wildcardFind(meshEnt.path+'/##[ISA=StimulusTable]')
                if mol_cpl or funclist or enzlist or realist or tablist:
                        for m in mol_cpl:
                                if isinstance(element(m.parent),CplxEnzBase):
                                        cplxlist.append(m)
                                        objInfo = m.parent.path+'/info'
                                elif isinstance(element(m),PoolBase):
                                        mollist.append(m)
                                        objInfo =m.path+'/info'

                                if sceneitems == None:
                                        xx = xyPosition(objInfo,'x')
                                        yy = xyPosition(objInfo,'y')
                                else:
                                        c = sceneitems[m]
                                        xx = c.scenePos().x()
                                        yy =-c.scenePos().y()

                                cord[m] ={ 'x': xx,'y':yy}
                                xcord.append(xx)
                                ycord.append(yy)
                        getxyCord(xcord,ycord,realist,sceneitems)
                        getxyCord(xcord,ycord,enzlist,sceneitems)
                        getxyCord(xcord,ycord,funclist,sceneitems)
                        getxyCord(xcord,ycord,tablist,sceneitems)
        xmin = min(xcord)
        xmax = max(xcord)
        ymin = min(ycord)
        ymax = max(ycord)
        positionInfoExist = not(len(np.nonzero(xcord)[0]) == 0 \
                and len(np.nonzero(ycord)[0]) == 0)

        return(xmin,ymin,xmax,ymax,positionInfoExist)

def writeCompartment(modelpath,compts,f):
        index = 0
        volIndex = {}
        for compt in compts:
                if compt.name != "kinetics":
                        xgrp = xmax -random.randrange(1,10)
                        ygrp = ymin +random.randrange(1,10)
                        x = ((xgrp-xmin)/(xmax-xmin))*multi
                        #y = ((ymax-ygrp)/(ymax-ymin))*multi
                        y = ((ygrp-ymin)/(ymax-ymin))*multi
                        f.write("simundump group /kinetics/" + compt.name + " 0 " + "blue" + " " + "green"       + " x 0 0 \"\" defaultfile \\\n" )
                        f.write( "  defaultfile.g 0 0 0 " + str(x) + " " + str(y) + " 0\n")
        i = 0
        l = len(compts)
        geometry = ""
        for compt in compts:
                size = compt.volume
                ndim = compt.numDimensions
                vecIndex = l-i-1
                #print vecIndex
                i = i+1
                xgeo = xmax -random.randrange(1,10)
                ygeo = ymin +random.randrange(1,10)
                x = ((xgeo-xmin)/(xmax-xmin))*multi
                #y = ((ymax-ygeo)/(ymax-ymin))*multi
                y = ((ygeo-ymin)/(ymax-ymin))*multi
                if vecIndex > 0:
                        geometry = geometry+"simundump geometry /kinetics" +  "/geometry[" + str(vecIndex) +"] 0 " + str(size) + " " + str(ndim) + " sphere " +" \"\" white black "+ str(x) + " " +str(y) +" 0\n";
                        volIndex[size] = "/geometry["+str(vecIndex)+"]"
                else:
                        geometry = geometry+"simundump geometry /kinetics"  +  "/geometry 0 " + str(size) + " " + str(ndim) + " sphere " +" \"\" white black " + str(x) + " "+str(y)+ " 0\n";
                        volIndex[size] = "/geometry"
                f.write(geometry)
        writeGroup(modelpath,f,xmax,ymax)
        return volIndex

def writeGroup(modelpath,f,xmax,ymax):
        ignore = ["graphs","moregraphs","geometry","groups","conc1","conc2","conc3","conc4"]
        for g in wildcardFind(modelpath+'/##[TYPE=Neutral]'):
                if not g.name in ignore:
                        if trimPath(g) != None:
                                xgrp1 = xmax - random.randrange(1,10)
                                ygrp1 = ymin + random.randrange(1,10)
                                x = ((xgrp1-xmin)/(xmax-xmin))*multi
                                #y = ((ymax-ygrp1)/(ymax-ymin))*multi
                                y = ((ygrp1-ymin)/(ymax-ymin))*multi
                                f.write("simundump group /kinetics/" + trimPath(g) + " 0 " +    "blue" + " " + "green"   + " x 0 0 \"\" defaultfile \\\n")
                                f.write("  defaultfile.g 0 0 0 " + str(x) + " " + str(y) + " 0\n")

def writeHeader(f,maxVol):
        simdt = 0.001
        plotdt = 0.1
        rawtime = 100
        maxtime = 100
        defaultVol = maxVol
        f.write("//genesis\n"
                        "// kkit Version 11 flat dumpfile\n\n"
                        "// Saved on " + str(rawtime)+"\n"
                        "include kkit {argv 1}\n"
                        "FASTDT = " + str(simdt)+"\n"
                        "SIMDT = " +str(simdt)+"\n"
                        "CONTROLDT = " +str(plotdt)+"\n"
                        "PLOTDT = " +str(plotdt)+"\n"
                        "MAXTIME = " +str(maxtime)+"\n"
                        "TRANSIENT_TIME = 2"+"\n"
                        "VARIABLE_DT_FLAG = 0"+"\n"
                        "DEFAULT_VOL = " +str(defaultVol)+"\n"
                        "VERSION = 11.0 \n"
                        "setfield /file/modpath value ~/scripts/modules\n"
                        "kparms\n\n"
                        )
        f.write( "//genesis\n"
                        "initdump -version 3 -ignoreorphans 1\n"
                        "simobjdump table input output alloced step_mode stepsize x y z\n"
                        "simobjdump xtree path script namemode sizescale\n"
                        "simobjdump xcoredraw xmin xmax ymin ymax\n"
                        "simobjdump xtext editable\n"
                        "simobjdump xgraph xmin xmax ymin ymax overlay\n"
                        "simobjdump xplot pixflags script fg ysquish do_slope wy\n"
                        "simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \\\n"
                                "  link savename file version md5sum mod_save_flag x y z\n"
                        "simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \\\n"
                                "  geomname xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \\\n"
                                "  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z\n"
                        "simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \\\n"
                                "  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z\n"
                        "simobjdump xtab input output alloced step_mode stepsize notes editfunc \\\n"
                                "  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z\n"
                        "simobjdump kchan perm gmax Vm is_active use_nernst notewriteReacs xtree_fg_req \\\n"
                                "  xtree_textfg_req x y z\n"
                        "simobjdump transport input output alloced step_mode stepsize dt delay clock \\\n"
                                "  kf xtree_fg_req xtree_textfg_req x y z\n"
                        "simobjdump proto x y z\n"
                        )

def estimateDefaultVol(compts):
        maxVol = 0
        vol = []
        for compt in compts:
                vol.append(compt.volume)
        if len(vol) > 0:
                return max(vol)
        return maxVol

def writeGui( f ):
        f.write("simundump xgraph /graphs/conc1 0 0 99 0.001 0.999 0\n"
        "simundump xgraph /graphs/conc2 0 0 100 0 1 0\n"
        "simundump xgraph /moregraphs/conc3 0 0 100 0 1 0\n"
        "simundump xgraph /moregraphs/conc4 0 0 100 0 1 0\n"
        "simundump xcoredraw /edit/draw 0 -6 4 -2 6\n"
        "simundump xtree /edit/draw/tree 0 \\\n"
        "  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \"edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>\" auto 0.6\n"
        "simundump xtext /file/notes 0 1\n")

def writeFooter( f ):
        f.write( "\nenddump\n" +
           "complete_loading\n")

if __name__ == "__main__":
        import sys

        filename = sys.argv[1]
        modelpath = filename[0:filename.find('.')]
        loadModel('/home/harsha/genesis_files/gfile/'+filename,'/'+modelpath,"gsl")
        output = '/home/harsha/Desktop/moose2genesis/moosefolder_cmd__sep2_'+filename
        written = writeKkit('/'+modelpath,output)
        if written:
                print(" file written to ",output)
        else:
                print(" could be written to kkit format")
