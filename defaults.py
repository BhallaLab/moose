
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
