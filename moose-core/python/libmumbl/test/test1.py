import moose
from IPython import embed
# Try to connect pool to species
pool = moose.Pool('/pool')
species = moose.Species('/species')
embed()
species.pool = pool
