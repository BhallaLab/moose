#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function
from subprocess import check_output as run
from datetime import datetime
from itertools import groupby
from operator import itemgetter
import re
import magic

def authors(filename):
    log = run(['git', 'log', '--follow',
              '--date=short','--format=%aN%x09%ad', filename],
              universal_newlines=True)
    for line in log.splitlines():
        author, date = line.split('\t')
        if author != 'fix-copyright.py':
            yield author, datetime.strptime(date, '%Y-%m-%d')

def new_copyright(filename, previous):
    def f():
        au = list(authors(filename))
        alldates = list(map(itemgetter(1), au))
        aup = sorted(au + [(a, None) for a in previous], key=itemgetter(0))
        for author, records in groupby(aup, itemgetter(0)):
            dates = [_f for _f in map(itemgetter(1), records) if _f]
            if not dates: dates = alldates
            start = min(dates)
            end = max(dates)
            fmt = '{0}' if start.year == end.year else '{0}-{1}'
            line = 'Copyright ' + fmt.format(start.year, end.year) + ' ' + author
            key = (start, author)
            yield key, line
    return list(map(itemgetter(1), sorted(f())))

def fix_copyright(filename):
    # Find copyright block in original file
    prefix = set()
    names = []
    lines = []
    with open(filename, 'r') as f:
        content = list(f)
    for i, line in enumerate(content[:15]):
        m = re.match(r'^(?P<prefix>\W*)(\(c\))?\s*?copyright\s*(\(c\))?\s+\d{4}(\s*-\s*\d{4})?\s+(?P<name>.+?)\s*$', line, re.IGNORECASE)
        if m:
            d = m.groupdict()
            prefix.add(d['prefix'])
            lines.append(i)
            names.append(d['name'].strip())
    if len(prefix) != 1:
        print('Not found:', filename)
        return
    prefix = list(prefix)[0]

    print(filename)
    new = iter(new_copyright(filename, names))
    with open(filename, 'w') as f:
        for i, line in enumerate(content):
            if i in lines:
                for repl in new:
                    print(prefix + repl, file=f)
            else:
                print(line, end=' ', file=f)
    pass

def all_files():
    ls = run(['git', 'ls-files'], universal_newlines=True)
    for filename in ls.splitlines():
        if magic.from_file(filename, mime=True).split('/')[0] == 'text':
            yield filename

for f in all_files():
    fix_copyright(f)
