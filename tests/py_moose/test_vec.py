# -*- coding: utf-8 -*-
import moose

def test_vec():
    foo = moose.Pool('/foo1', 500)
    bar = moose.vec('/foo1')
    assert len(bar) == 500

if __name__ == '__main__':
    test_vec()
