# -*- coding: utf-8 -*-
from __future__ import print_function
try:
    from future_builtins import zip
except ImportError:
    pass
from . import moose as _moose

_tick = 8
_base = '/_utils'
_path = _base + '/y{0}'
_counter = 0
_plots = []

_moose.Neutral( _base )

_defaultFields = {
	_moose.Compartment : 'Vm',
	_moose.ZombieCompartment : 'Vm',

	_moose.HHChannel: 'Gk',
	_moose.ZombieHHChannel: 'Gk',

	_moose.HHChannel2D: 'Gk',

	_moose.SynChan: 'Gk',

	_moose.CaConc: 'Ca',
	_moose.ZombieCaConc: 'Ca',

	_moose.Pool: 'conc',
	_moose.ZombiePool: 'conc',
	_moose.ZPool: 'conc',

	_moose.BufPool: 'conc',
	_moose.ZombieBufPool: 'conc',
	_moose.ZBufPool: 'conc',

	_moose.FuncPool: 'conc',
	_moose.ZombieFuncPool: 'conc',
	_moose.ZFuncPool: 'conc',
}

def _defaultField( obj ):
	return _defaultFields[ type( obj ) ]

def setDt( dt ):
	'''-----------
	Description
	-----------
	Sets time-step for recording values.

	---------
	Arguments
	---------
	dt: Time-step for recording values.

	-------
	Returns
	-------
	Nothing.'''
	_moose.setClock( _tick, dt )

class SetupError( Exception ):
	pass

def _time( npoints = None ):
	import numpy

	if npoints is None:
		try:
			npoints = len( _plots[ 0 ].vec )
		except IndexError:
			raise SetupError(
				'List of time-points cannot be constructed because '
				'no plots have been set up yet.'
			)

	begin   = 0.0
	end     = _moose.Clock( '/clock' ).currentTime

	return numpy.linspace( begin, end, npoints )

class _Plot( _moose.Table ):
	def __init__( self, path, obj, field, label ):
		_moose.Table.__init__( self, path )

		self._table = _moose.Table( path )

		self.obj    = obj
		self.field  = field
		self.label  = label

	@property
	def values( self ):
		return self._table.vec

	@property
	def size( self ):
		return len( self.values )

	@property
	def time( self ):
		return _time( self.size )

	def __iter__( self ):
		return iter( self.values )

def record( obj, field = None, label = None ):
	'''
	'''
	global _counter

	# Checking if object is an iterable like list or a tuple, but not a string.
	if hasattr( obj, '__iter__' ):
		return [ record( o, field, label ) for o in obj ]

	if isinstance( obj, str ):
		obj = _moose.element( obj )

	if field is None:
		field = _defaultField( obj )

	path = _path.format( _counter )
	_counter += 1

	p = _Plot( path, obj, field, label )
	_plots.append( p )

	_moose.connect( p, "requestData", obj, 'get_' + field )
	_moose.useClock( _tick, path, "process" )

	return p

def _label( plot, labelFormat = '{path}.{field}' ):
	# Over-ride label format if label has been given explicitly.
	if plot.label:
		labelFormat = plot.label

	return labelFormat.format(
		path  = plot.obj.path,
		name  = plot.obj.name,
		field = plot.field
	)

def _selectedPlots( selected ):
	if selected is None:
		# Returning a copy of this list, instead of reference. The returned
		# list will be manipulated later.
		return _plots[ : ]
	elif isinstance( selected, _Plot ):
		return [ selected ]
	else:
		return selected

def saveCSV(
		fileName,
		selected = None,
		delimiter = '\t',
		header = True,
		headerCommentCharacter = '#',
		labelFormat = '{path}.{field}',
		timeCol = True,
		timeHeader = 'Time',
		fileMode = 'w' ):
	'''
	'''
	import csv
	plots = _selectedPlots( selected )

	if header:
		header = []

		if timeCol:
			header.append( timeHeader )

		for plot in plots:
			header.append( _label( plot, labelFormat ) )

		header[ 0 ] = headerCommentCharacter + header[ 0 ]

	if timeCol:
		plots.insert( 0, _time() )

	with open( fileName, fileMode ) as fout:
		writer = csv.writer( fout, delimiter = delimiter )

		if header:
			writer.writerow( header )

		writer.writerows( list(zip( *plots )) )

def saveXPLOT(
		fileName,
		selected = None,
		labelFormat = '{path}.{field}',
		fileMode = 'w' ):
	'''
	'''
	plots = _selectedPlots( selected )

	with open( fileName, fileMode ) as fout:
		write = lambda line: fout.write( line + '\n' )

		for ( i, plot ) in enumerate( plots ):
			label = '/plotname ' + _label( plot, labelFormat )

			if i > 0:
				write( '' )
			write( '/newplot' )
			write( label )

			for value in plot:
				write( str( value ) )

def show(
		selected = None,
		combine = True,
		labelFormat = '{path}.{field}',
		xLabel = 'Time (s)',
		yLabel = '{field}' ):
	'''
	'''
	try:
		from matplotlib import pyplot as plt
	except ImportError:
		print("Warning: recording.show(): Cannot find 'matplotlib'. Not showing plots.")
		return

	plots = _selectedPlots( selected )

	if combine:
		plt.figure()

	for plot in plots:
		if not combine:
			plt.figure()

		print(_label(plot))
		plt.plot( plot.time, plot.values, label = _label( plot ) )

	plt.legend()
	plt.show()

def HDF5():
	pass
