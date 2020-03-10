# -*- coding: utf-8 -*-
"""
Test MOOSE installation with moose-examples.
"""

from __future__ import print_function, division

__author__ = "Dilawar Singh"
__copyright__ = "Copyright 2016, Dilawar Singh"
__email__ = "dilawars@ncbs.res.in"

import sys
import os
import tempfile
import re
import subprocess
import threading
import signal
import logging
import tempfile
from collections import defaultdict
import time

test_dir_ = os.path.join(tempfile.gettempdir(), 'moose-examples')
test_data_url_ = 'https://github.com/BhallaLab/moose-examples/archive/master.zip'
test_repo_url_ = 'https://github.com/BhallaLab/moose-examples'

ignored_dict_ = defaultdict(list)
test_status_ = defaultdict(list)
total_ = 0

matplotlibrc_ = '''
backend : agg
interactive : True
'''


# Handle CTRL+C
def signal_handler(signal, frame):
    print('You pressed Ctrl+C!')
    print_test_stat()
    quit(-1)


signal.signal(signal.SIGINT, signal_handler)


# Credit: https://stackoverflow.com/a/4825933/1805129
class Command(object):
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None
        self.fnull = open(os.devnull, 'w')

    def __repr__(self):
        return ' '.join(self.cmd)

    def run(self, timeout, **kwargs):
        self.process = subprocess.Popen(self.cmd, stdout=subprocess.PIPE,
                stderr=subprocess.PIPE)
        timer = threading.Timer(timeout, self.process.kill)
        try:
            timer.start()
            output, errors = self.process.communicate()
            try:
                errors = errors.decode('utf8')
            except Exception:
                pass
            logging.warn('%s %s' % (output, errors))
        finally:
            timer.cancel()
        return self.process.returncode


def init_test_dir():
    global test_dir_
    global test_url_
    if not os.path.exists(test_dir_):
        os.makedirs(test_dir_)
        logging.info("Donwloading test repository")
        subprocess.check_output(
            ['git', 'clone', '--depth=10', test_repo_url_, test_dir_])
    os.chdir(test_dir_)
    subprocess.check_output(['git', 'pull'])


def suitable_for_testing(script):
    with open(script, 'r') as f:
        txt = f.read()
        if not re.search(r'main\(\s*\)', txt):
            logging.debug('Script %s does not contain main( )' % script)
            return False, 'main( ) not found'
        if re.search(r'raw_input\(\s*\)', txt):
            logging.debug('Script %s requires user input' % script)
            return False, 'waits for user input'
    return True, 'OK'


def run_test(index, testfile, timeout, **kwargs):
    """Run a given test
    """
    global test_status_
    global total_
    pyExec = os.environ.get('PYTHON_EXECUTABLE', sys.executable)
    cmd = Command([pyExec, testfile])

    ti = time.time()
    name = os.path.basename(testfile)
    out = (name + '.' * 50)[:40]
    print('[TEST %3d/%d] %41s ' % (index, total_, out), end='')
    sys.stdout.flush()

    # Run the test.
    status = cmd.run(timeout=timeout, **kwargs)
    t = time.time() - ti
    print('% 7.2f s' % t, end='')
    sys.stdout.flush()

    # Change to directory and copy before running then test.
    cwd = os.path.dirname(testfile)
    os.chdir(cwd)
    with open(os.path.join(cwd, 'matplotlibrc'), 'w') as f:
        logging.debug('Writing matplotlibrc to %s' % cwd)
        f.write(matplotlibrc_)

    if status != 0:
        if status == -15:
            msg = '% 4d TIMEOUT' % status
            test_status_['TIMED-OUT'].append(testfile)
        else:
            msg = '% 4d FAILED' % status
            test_status_['FAILED'].append(testfile)
        print(msg)
    else:
        print('% 4d PASSED' % status)
        test_status_['PASSED'].append(testfile)

    sys.stdout.flush()


def print_test_stat():
    global test_status_
    for status in test_status_:
        print('Total %d tests %s' % (len(test_status_[status]), status))


def test_all(timeout, **kwargs):
    global test_dir_
    global total_
    scripts = []
    for d, ds, fs in os.walk(test_dir_):
        for f in fs:
            if not re.match(r'.+\.py$', f):
                continue
            filepath = os.path.join(d, f)
            isOK, msg = suitable_for_testing(filepath)
            if isOK:
                scripts.append(filepath)
            else:
                ignored_dict_[msg].append(filepath)

    for k in ignored_dict_:
        logging.debug('[INFO] Ignored due to %s' % k)
        logging.debug('\n\t'.join(ignored_dict_[k]))

    logging.info('Total %d valid tests found' % len(scripts))
    total_ = len(scripts)
    for i, s in enumerate(scripts):
        logging.info('Running test (timeout=%s) : %s' % (timeout, s))
        run_test(i, s, timeout, **kwargs)


def test(timeout=60, **kwargs):
    """Download and run tests.
    """
    print('[INFO] Running test with timeout %d sec' % timeout)
    try:
        init_test_dir()
    except Exception as e:
        print('[INFO] Failed to clone moose-examples. Error was %s' % e)
        quit()

    test_all(timeout=timeout, **kwargs)
    print_test_stat()


if __name__ == '__main__':
    test()
