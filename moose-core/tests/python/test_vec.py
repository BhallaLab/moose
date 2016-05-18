import moose
foo = moose.Pool('/foo1', 500)
bar = moose.vec('/foo1')
assert len(bar) == 500
