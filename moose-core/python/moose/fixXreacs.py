####################################################################
# fixXreacs.py
# The program is meant to take a model and reconfigure any cross-compartment
# reactions so that they are split into portions on either side coupled 
# either by diffusion, or by a reaction-driven translocation process.
#
# Copyright (C) Upinder S. Bhalla       2018
# This program is free software. It is licensed under the terms of
# GPL version 3 or later at your discretion.
# This program carries no warranty whatsoever.
####################################################################


import sys
import moose

msgSeparator = "_xMsg_"

def findCompt( elm ):
    elm = moose.element( elm )
    pa = elm.parent
    while pa.path != '/':
        if moose.Neutral(pa).isA[ 'ChemCompt' ]:
            return pa.path
        pa = pa.parent
    print( 'Error: No compartment parent found for ' + elm.path )
    return '/'

# http://stackoverflow.com/q/3844948/
def checkEqual(lst):
    return not lst or lst.count(lst[0]) == len(lst)

def findXreacs( basepath, reacType ):
    reacs = moose.wildcardFind( basepath + '/##[ISA=' + reacType + 'Base]' )
    ret = []
    for i in reacs:
        reacc = findCompt( i )
        subs = i.neighbors['subOut']
        prds = i.neighbors['prdOut']
        subc = [findCompt(j) for j in subs]
        prdc = [findCompt(j) for j in prds]

        enzc = []
        if reacType == 'Enz':
            enzc = [reacc]
        if not checkEqual( subc + prdc + enzc ):
            ret.append( [i, reacc, subs, subc, prds, prdc ])
    return ret

def removeEnzFromPool( pool ):
    kids = moose.wildcardFind( pool.path + "/#" )
    for i in kids:
        if i.isA[ 'EnzBase' ]:
            moose.delete( i )
        elif i.isA[ 'Function' ]:
            moose.delete( i )

# If a pool is not in the same compt as reac, make a proxy in the reac 
# compt, connect it up, and disconnect the one in the old compt.
def proxify( reac, reacc, direction, pool, poolc ):
    reacc_elm = moose.element( reacc )
    reac_elm = moose.element( reac )
    # Preserve the rates which were set up for the x-compt reacn
    #moose.showfield( reac )
    dupname = pool.name + '_xfer_' + moose.element(poolc).name
    #print "#############", pool, dupname, poolc
    if moose.exists( reacc + '/' + dupname ):
        duppool = moose.element( reacc + '/' + dupname )
    else:
        # This also deals with cases where the duppool is buffered.
        duppool = moose.copy(pool, reacc_elm, dupname )
    duppool.diffConst = 0   # diffusion only happens in original compt
    removeEnzFromPool( duppool )
    disconnectReactant( reac, pool, duppool )
    moose.connect( reac, direction, duppool, 'reac' )
    #moose.showfield( reac )
    #moose.showmsg( reac )

def enzProxify( enz, enzc, direction, pool, poolc ):
    if enzc == poolc:
        return
    enze = moose.element( enz )
    # kcat and k2 are indept of volume, just time^-1
    km = enze.numKm
    proxify( enz, enzc, direction, pool, poolc )
    enze.numKm = km

def reacProxify( reac, reacc, direction, pool, poolc ):
    if reacc == poolc:
        return
    reac_elm = moose.element( reac )
    kf = reac_elm.numKf
    kb = reac_elm.numKb
    proxify( reac, reacc, direction, pool, poolc )
    reac_elm.numKf = kf
    reac_elm.numKb = kb

def identifyMsg( src, srcOut, dest ):
    if src.isA[ 'ReacBase' ] or src.isA[ 'EnzBase' ]:
        if srcOut == 'subOut':
            return msgSeparator + src.path + ' sub ' + dest.path + ' reac'
        if srcOut == 'prdOut':
            return msgSeparator + src.path + ' prd ' + dest.path + ' reac'
    return ''

def disconnectReactant( reacOrEnz, reactant, duppool ):
    outMsgs = reacOrEnz.msgOut
    infoPath = duppool.path + '/info'
    if moose.exists( infoPath ):
        info = moose.element( infoPath )
    else:
        info = moose.Annotator( infoPath )

    #moose.le( reactant )
    notes = ""
    #moose.showmsg( reacOrEnz )
    for i in outMsgs:
        #print "killing msg from {} to {}\nfor {} and {}".format( reacOrEnz.path, reactant.path, i.srcFieldsOnE1[0], i.srcFieldsOnE2[0] )
        if i.e1 == reactant:
            msgStr = identifyMsg( i.e2, i.e2.srcFieldsOnE2[0], i.e1 )
            if len( msgStr ) > 0:
                notes += msgStr
                moose.delete( i )
        elif i.e2 == reactant:
            msgStr = identifyMsg( i.e1[0], i.srcFieldsOnE1[0], i.e2[0] )
            if len( msgStr ) > 0:
                notes += msgStr
                moose.delete( i )
    #print "MSGS to rebuild:", notes
    info.notes += notes

def fixXreacs( basepath ):
    xr = findXreacs( basepath, 'Reac' )
    xe = findXreacs( basepath, 'Enz' )

    for i in (xr):
        reac, reacc, subs, subc, prds, prdc = i
        for j in range( len( subs )):
            reacProxify( reac, reacc, 'sub', subs[j], subc[j] )
        for j in range( len( prds )):
            reacProxify( reac, reacc, 'prd', prds[j], prdc[j] )

    for i in (xe):
        reac, reacc, subs, subc, prds, prdc = i
        for j in range( len( subs )):
            enzProxify( reac, reacc, 'sub', subs[j], subc[j] )
        for j in range( len( prds )):
            enzProxify( reac, reacc, 'prd', prds[j], prdc[j] )

#####################################################################

def getOldRates( msgs ):
    if len( msgs ) > 1 :
        m1 = msgs[1].split( msgSeparator )[0]
        elm = moose.element( m1.split( ' ' )[0] )
        if elm.isA[ 'ReacBase' ]:
            return [elm.numKf, elm.numKb]
        elif elm.isA[ 'EnzBase' ]:
            return [elm.numKm,]
    print( "Warning: getOldRates did not have any messages" )
    return [0,]

def restoreOldRates( oldRates, msgs ):
    #print oldRates, msgs
    if len( msgs ) > 1 :
        m1 = msgs[1].split( msgSeparator )[0]
        elm = moose.element( m1.split( ' ' )[0] )
        if elm.isA[ 'ReacBase' ]:
            elm.numKf = oldRates[0]
            elm.numKb = oldRates[1]
        elif elm.isA[ 'enzBase' ]:
            elm.numKm = oldRates[0]



def restoreXreacs( basepath ):
    proxyInfo = moose.wildcardFind( basepath + "/##/#_xfer_#/info" )
    for i in proxyInfo:
        msgs = i.notes.split( msgSeparator )
        oldRates = getOldRates( msgs )
        #print( "Deleting {}".format( i.parent.path ) )
        #print msgs
        moose.delete( i.parent )
        for j in msgs[1:]:
            if len( j ) > 0:
                args = j.split( ' ' )
                assert( len( args ) == 4 )
                #moose.showfield( args[0] )
                moose.connect( args[0], args[1], args[2], args[3] )
                #print( "Reconnecting {}".format( args ) )
                #moose.showfield( args[0] )
        restoreOldRates( oldRates, msgs )

