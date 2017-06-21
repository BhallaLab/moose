import moose
import rdesigneur as rd
rdes = rd.rdesigneur()
rdes.buildModel()
moose.showfields( rdes.soma )
