import moose
import numpy as np
import matplotlib.pyplot as plt
import chan_proto

from params import *

def linoid(x,param):
    #plt.figure()
    #plt.plot(x,(param[2]+np.exp((V+param[3])/param[4])))
    den = (param[2]+np.exp((V+param[3])/param[4]))
    nom = (param[0]+param[1]*V)
    return nom/den

def add_difshell(comp,i,shell_thickness,shell_radius):
    new_name = comp.path.split('[')[0]+'/'+comp.name+'/'+difshell_name+str(i)
    dif = moose.DifShell(new_name)


    #dif.C = Ca_initial
    dif.Ceq = Ca_basal
    dif.D = dca
    dif.valence = 2
    dif.leak = 0
    dif.shapeMode = 0
    dif.length = comp.length
    dif.diameter = 2*shell_radius
    dif.thickness = shell_thickness
   
    return dif

def add_difbuffer_to_dishell(comp,i,j,shell_thickness,shell_radius):
    new_name = comp.path.split('[')[0]+'/'+comp.name+'/'+difshell_name+str(i)+'_'+difbuff_name+str(j)
    buf = moose.DifBuffer(new_name)
    buf.bTot = btotal
    buf.shapeMode = 0
    buf.length = comp.length
    buf.diameter = 2*shell_radius
    buf.thickness = shell_thickness
    buf.kf = kf
    buf.kb = kb
    buf.D = d
    return buf

def add_difshells_and_buffers(comp):
    
    if difshell_no < 1:
        return [], []
    
    difshell= []
    shell_thickness = dend.diameter/difshell_no/2.
    difbuffer = []
    for i in range(difshell_no):
        shell_radius = comp.diameter/2-i*shell_thickness
        dif = add_difshell(comp,i,shell_thickness,shell_radius)
        difshell.append(dif)
       
        if i > 0 :
            moose.connect(difshell[i-1],"outerDifSourceOut",difshell[i],"fluxFromOut")
            moose.connect(difshell[i],"innerDifSourceOut",difshell[i-1],"fluxFromIn")
            
        if difbuff_no > 0:
            difbuffer.append([])
            for j in range(difbuff_no):
                buf = add_difbuffer_to_dishell(comp,i,j,shell_thickness,shell_radius)
                difbuffer[i].append(buf)
                moose.connect(difshell[i],"concentrationOut",buf,"concentration")
                moose.connect(buf,"reactionOut",difshell[i],"reaction")
                if i>0:
                    moose.connect(difbuffer[i-1][j],"outerDifSourceOut",difbuffer[i][j],"fluxFromOut")
                    moose.connect(difbuffer[i][j],"innerDifSourceOut",difbuffer[i-1][j],"fluxFromIn")
    
    return difshell,difbuffer

def addOneChan(chanpath,gbar,comp):
    
    SA = np.pi * comp.length*comp.diameter
    proto = moose.element('/library/'+chanpath)
    chan = moose.copy(proto, comp, chanpath)
    chan.Gbar = gbar * SA
    #If we are using GHK AND it is a calcium channel, connect it to GHK
    moose.connect(comp,'VmOut',chan,'Vm')
    moose.connect(chan,"channelOut",comp,"handleChannel")
    return chan


if __name__ == '__main__':
    
    for tick in range(0, 7):
        moose.setClock(tick, dt)
    moose.setClock(8,0.005) #set output clock
    model = moose.Neutral('/model')
    dend = moose.Compartment('/model/dend')
    pulse = moose.PulseGen('/model/pulse')
    data = moose.Neutral('/data')
    vmtab = moose.Table('/data/dend_Vm')
    gktab = moose.Table('/data/CaT_Gk')
    iktab = moose.Table('/data/CaT_Ik')

    dend.Cm = Cm
    dend.Rm = Rm
    dend.Em = Em
    dend.initVm = Vm_0
    dend.diameter = dend_diameter
    dend.length = dend_length
    
    pulse.delay[0] = 8.
    pulse.width[0] = 500e-3
    pulse.level[0] = inject
    pulse.delay[1] = 1e9

    m = moose.connect(pulse, 'output', dend, 'injectMsg')
    
    moose.connect(vmtab, 'requestOut', dend, 'getVm')

    chan_proto.chanlib()
    chan = addOneChan('CaT',gbar,dend)

    
    moose.connect(gktab, 'requestOut', chan, 'getGk')
    moose.connect(iktab, 'requestOut', chan, 'getIk')
    diftab = []
    buftab = []
    difs, difb = add_difshells_and_buffers(dend)
    if pumps:
        pump = moose.MMPump('/model/dend/pump')
        pump.Vmax = kcat
        pump.Kd = km
        moose.connect(pump,"PumpOut",difs[0],"mmPump")
    if difs:
        moose.connect(chan,"IkOut",difs[0],"influx")
        moose.connect(difs[0],'concentrationOut',chan,'concen')
        
    for i,dif in enumerate(difs):
        res_dif = moose.Table('/data/'+difshell_name+str(i))
        diftab.append(res_dif)
        moose.connect(diftab[i],'requestOut',dif,'getC')
        if (difbuff_no):
            buftab.append([])
            for j,buf in enumerate(difb[i]):
                res_buf =  moose.Table('/data/'+difshell_name+str(i)+'_'+difbuff_name+str(j))
                buftab[i].append(res_buf)
                moose.connect(buftab[i][j],'requestOut',buf,'getBBound')
    
   
    moose.reinit()
    if not gbar:
        for i,dif in enumerate(difs):
            if i == 0:
                dif.C = Ca_initial
            else:
                dif.C = 0
            for j,dbuf in enumerate(difb[i]):
                dbuf.bFree = dbuf.bTot
    
    moose.start(t_stop)
   
    t = np.linspace(0, t_stop, len(vmtab.vector))
    fname = 'moose_results_difshell_no_'+str(difshell_no)+'_difbuffer_no_'+str(difbuff_no)+'_pump_'+str(pumps)+'_gbar_'+str(gbar)+'.txt'
    print fname
    header = 'time Vm Ik Gk'
    number  = 4+difshell_no*(difbuff_no+1)
    res = np.zeros((len(t),number))
    res[:,0] = t
    res[:,1] = vmtab.vector
    res[:,2] = iktab.vector
    res[:,3] = gktab.vector
 
    for i in range(difshell_no):
        header += ' difshell_'+str(i)
        res[:,4+i*(difbuff_no+1)] =  diftab[i].vector
        if (difbuff_no):
            for j,buf in enumerate(buftab[i]):
                res[:,4+i*(difbuff_no+1)+j+1] =  buf.vector
                header += ' difshell_'+str(i)+'_difbuff_'+str(j)
    np.savetxt(fname,res,header=header, comments='')



    

    # plt.show()
