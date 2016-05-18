#!/usr/bin/env python

"""types.py: Implement some data-types.

Last modified: Tue Jan 07, 2014  10:58AM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

class DoubleDict:
    '''This is a double dict. It maps keys to values and values to keys.
    '''
    def __init__(self):
        self.k = dict()
        self.v = dict()

    def __str__(self):
        txt = []
        for k in self.k:
            if k == self.v[self.k.get(k, None)]:
                txt.append(u'{} <-> {}'.format(k, self.k[k]))
            else:
                raise UserWarning, "Duplicate Entries: {} {}".format(
                        self.k
                        , self.v
                        )
        return '{' + ', '.join(txt) + '}'



    def insert(self, key, value):
        if self.k.get(key, 0) != value:
            self.k[key] = value
            self.v[value] = key
        else:
            self.v[value] = key

    def insertUniqueVal(self, key):
        self.k[key] = self.k.get(key, self.k.__len__() + 1)
        self.v[self.k[key]] = key
        assert len(set(self.k.values())) == len(self.k.values()), self.k.values()

    def get(self, key, dictId=-1, default=None):
        if dictId == 0:  # Fetch from key dict k
            return self.k.get(key, default)
        elif dictId == 1: # Fetch from key dict v
            return self.v.get(key, default)

        # Else query both
        if self.k.get(key, None) is not None:
            return self.k[key]
        elif self.v.get(key, None) is not None:
            return self.v[key]
        else:
            return default
    
    def get0(self, key, default=None):
        return self.get(key, 0, default)

    def get1(self, key, default=None):
        return self.get(key, 1, default)


if __name__ == "__main__":
    a = DoubleDict()
    b = ['Ka', 'Ca', 'Na', 'Ca', 'Fe']
    for s in b:
        a.insertUniqueVal(s)

    print(a)
