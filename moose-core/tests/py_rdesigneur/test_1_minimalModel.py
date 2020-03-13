# Rdesigneur tutorial converted to a test.
import moose
print("[INFO ] MOOSE version=%s, loc=%s" % (moose.version(), moose.__file__))
import rdesigneur as rd

def test_simple():
    """First test.
    >>> test_simple() # doctest: +NORMALIZE_WHITESPACE
    Rdesigneur: Elec model has 1 compartments and 0 spines on 0 compartments.
    <BLANKLINE>
    [/model[0]/elec[0]/soma[0]]
    Cm              =7.853981633975e-09
    Em              =-0.0544
    Im              =1.3194689277024895e-08
    Ra              =7639437.268410473
    Rm              =424413.1773342278
    Vm              =-0.06
    className       =ZombieCompartment
    diameter        =0.0005
    dt              =0.0
    fieldIndex      =0
    idValue         =455
    index           =0
    initVm          =-0.065
    inject          =0.0
    length          =0.0005
    name            =soma
    numData         =1
    numField        =1
    path            =/model[0]/elec[0]/soma[0]
    tick            =-2
    x               =0.0005
    x0              =0.0
    y               =0.0
    y0              =0.0
    z               =0.0
    z0              =0.0
    <BLANKLINE>
    INCOMING:
    /model[0]/elec[0]/soma ('parentMsg',) <--- /model[0]/elec ('childOut',)                                         
    OUTGOING:
    """
    rdes = rd.rdesigneur()
    rdes.buildModel()
    moose.showfields(rdes.soma)
    moose.showmsg(rdes.soma)

if __name__ == '__main__':
    import doctest
    doctest.testmod()
