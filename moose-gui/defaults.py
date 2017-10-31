__credits__     =   ["Upi Lab"]
__author__      =   "Subhasis"
__license__     =   "GPL3"
__version__     =   "1.0.0"
__maintainer__  =   "Subhasis", "HarshaRani","Aviral Goel"
__email__       =   "goel.aviral@gmail.com"
__status__      =   "Development"
__updated__     =   "Jul 26 2017"

PLOT_FIELDS={
    'LeakyIaF':['Vm'],
    'Compartment':['Vm','Im'],
    'HHChannel':['Ik','Gk'],
    'ZombiePool':['n','conc'],
    'ZombieBufPool':['n','conc'],	
    'HHChannel2D':['Ik','Gk'],
    'CaConc':['Ca']
    }
FIELD_UNITS={
    'volume':'m3',
    'Kd' : 'mM',
    'Km' :'mM',
    'kcat':'s-1',
    'k1' :'1/# s',
    'k2' :'s-1',
    'k3' :'s-1',
    'Vm':'V',
    'conc':'mM',
    'concInit':'mM',
    'Rm':'ohm',
    'Ra':'ohm',
    'Cm':'F',
    'initVm':'V',
    'Im':'A',
    'Em':'V',
    'inject':'A',
    'Ek':'V',    
    'Gk':'S',
    'Gbar':'S',
    'Ik':'A',
    'Ca':'mM',
    'CaBasal':'mM',
    'tau':'s',
    'Ca_base':'mM',
    'B':'mM/A',
    'thick':'m',
    'ceiling':'mM',
    'floor':'mM'
}

OBJECT_EDIT_UNDO_LENGTH = 16
