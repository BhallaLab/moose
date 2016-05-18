# config.py ---
#
# Filename: config.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Sat Feb 13 16:07:56 2010 (+0530)
# Version:
# Last-Updated: Wed Nov 12 19:10:08 2014 (+0530)
#           By: Subhasis Ray
#     Update #: 369
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Provides keys for accessing per-user settings.
# Provides initialization of several per-user variables for MooseGUI.
# As part of initialization, creates `~/.moose` and `~/moose`
# directories.
#
#

# Change log:
#
# 2012-09-22 13:49:36 (+0530) Subha: cleaned up the initialization
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:

import os
import sys
import tempfile
import logging
import errno
from PyQt4.Qt import Qt
from PyQt4 import QtGui, QtCore

TEMPDIR = tempfile.gettempdir()
KEY_FIRSTTIME = 'firsttime'
# KEY_STATE_FILE = 'statefile'

KEY_UNDO_LENGTH = 'main/undolength'
KEY_WINDOW_GEOMETRY = 'main/geometry'
KEY_WINDOW_LAYOUT = 'main/layout'
KEY_RUNTIME_AUTOHIDE = 'main/rtautohide'
KEY_DEMOS_DIR = 'main/demosdir'
KEY_DOCS_DIR = 'main/docsdir'
KEY_HOME_DIR = 'main/homedir'
KEY_ICON_DIR = 'main/icondir'
KEY_COLORMAP_DIR = 'main/colormapdir'
KEY_BIOMODEL_DIR = 'main/biomodelsdir'
KEY_LOCAL_DEMOS_DIR = 'main/localdemosdir'
KEY_MOOSE_LOCAL_DIR = 'main/localdir'
KEY_NUMPTHREADS = 'main/numpthreads'
KEY_LOCAL_BUILD = 'main/localbuild'
KEY_LAST_PLUGIN = 'main/lastplugin'

# These are the keys for mode specific configuration.
# Ideally the default values should be provided by the plugin.
# We use the QSettings to remember the last values
KEY_KINETICS_SIMDT = 'kinetics/simdt'
KEY_KINETICS_PLOTDT = 'kinetics/plotdt'
KEY_ELECTRICAL_SIMDT = 'electrical/simdt'
KEY_ELECTRICAL_PLOTDT = 'electrical/plotdt'
KEY_SIMTIME = 'main/simtime'

QT_VERSION = str(QtCore.QT_VERSION_STR).split('.')
QT_MAJOR_VERSION = int(QT_VERSION[0])
QT_MINOR_VERSION = int(QT_VERSION[1])

MOOSE_DOC_URL = 'http://moose.ncbs.res.in/builtins_classes/moose_builtins.html'
MOOSE_REPORT_BUG_URL = 'http://sourceforge.net/tracker/?func=add&group_id=165660&atid=836272'
MOOSE_GUI_BUG_URL  = 'https://github.com/BhallaLab/moose-gui/issues'
MOOSE_CORE_BUG_URL = 'https://github.com/BhallaLab/moose-core/issues'
MOOSE_DEMOS_DIR = '/usr/share/moose/Demos'
MOOSE_DOCS_DIR =  '/usr/share/doc/moose'
MOOSE_GUI_DIR = os.path.dirname(os.path.abspath(__file__))
MOOSE_PLUGIN_DIR = os.path.join(MOOSE_GUI_DIR, 'plugins')
NEUROKIT_PLUGIN_DIR = os.path.join(MOOSE_GUI_DIR, 'plugins/NeuroKit')
# MOOSE_CFG_DIR = os.path.join(os.environ['HOME'], '.moose')
MOOSE_LOCAL_DIR = os.path.join(os.environ['HOME'], 'moose')
MOOSE_NUMPTHREADS = '1'

MOOSE_ABOUT_FILE = os.path.join(MOOSE_GUI_DIR, 'about.html')
MOOSE_UNDO_LENGTH = 128 # Arbitrary undo length
LOCAL_BUILD = False

sys.path.append(os.path.join(MOOSE_PLUGIN_DIR))

class MooseSetting(dict):
    """
    dict-like access to QSettings.

    This subclass of dict wraps a QSettings object and lets one set
    and get values as Python strings rather than QVariant.

    This is supposed to be a singleton in the whole application (all
    QSettings are with same parameters).
    """
    _instance = None
    def __new__(cls, *args, **kwargs):
        # This is designed to check if the class has been
        # instantiated, if so, returns the single instance, otherwise
        # creates it.
        if cls._instance is None:
            cls._instance = super(MooseSetting, cls).__new__(cls, *args, **kwargs)
            firsttime, errs = init_dirs()
            for e in errs:
                print e
            QtCore.QCoreApplication.setOrganizationName('NCBS')
            QtCore.QCoreApplication.setOrganizationDomain('ncbs.res.in')
            QtCore.QCoreApplication.setApplicationName('MOOSE')
            cls._instance.qsettings = QtCore.QSettings()
            # If this is the first time, then set some defaults
            cls._instance.qsettings.setValue(KEY_COLORMAP_DIR, os.path.join(MOOSE_GUI_DIR, 'colormaps'))
            cls._instance.qsettings.setValue(KEY_BIOMODEL_DIR, os.path.join(MOOSE_GUI_DIR, 'bioModels'))
            cls._instance.qsettings.setValue(KEY_ICON_DIR, os.path.join(MOOSE_GUI_DIR, 'icons'))
            cls._instance.qsettings.setValue(KEY_NUMPTHREADS, '1')
            cls._instance.qsettings.setValue(KEY_UNDO_LENGTH, str(MOOSE_UNDO_LENGTH))
            # These are to be checked at every run
            cls._instance.qsettings.setValue(KEY_HOME_DIR, os.environ['HOME'])
            cls._instance.qsettings.setValue(KEY_DEMOS_DIR, MOOSE_DEMOS_DIR)
            cls._instance.qsettings.setValue(KEY_LOCAL_DEMOS_DIR, os.path.join(MOOSE_LOCAL_DIR, 'Demos/'))
            cls._instance.qsettings.setValue(KEY_DOCS_DIR, MOOSE_DOCS_DIR)
            cls._instance.qsettings.setValue(KEY_MOOSE_LOCAL_DIR, MOOSE_LOCAL_DIR)
            cls._instance.qsettings.setValue(KEY_LOCAL_BUILD, LOCAL_BUILD)
            os.environ['NUMPTHREADS'] = str(cls._instance.qsettings.value(KEY_NUMPTHREADS).toString())
        return cls._instance

    def __init__(self, *args, **kwargs):
        super(MooseSetting, self).__init__(self, *args, **kwargs)

    def __iter__(self):
        return (str(key) for key in self.qsettings.allKeys())

    def __setitem__(self, key, value):
        if isinstance(key, str):
            self.qsettings.setValue(key, value)
        else:
            raise TypeError('Expect only strings as keys')

    def __getitem__(self, key):
        return str(self.qsettings.value(key).toString())

    def keys(self):
        return [str(key) for key in self.qsettings.allKeys()]

    def values(self):
        return [str(self.qsettings.value(key).toString()) for key in self.qsettings.allKeys()]

    def itervalues(self):
        return (str(self.qsettings.value(key).toString()) for key in self.qsettings.allKeys())

def init_dirs():
    """Check if there is a `.moose` directory in user's home
    directory. If not, we assume this to be the first run of MOOSE.
    Then we try to create the `~/.moose` directory and `~/moose`
    directory.
    """
    firsttime = False
    global MOOSE_DEMOS_DIR
    global MOOSE_LOCAL_DIR
    # global MOOSE_CFG_DIR
    global MOOSE_DOCS_DIR
    global LOCAL_BUILD
    # If we have a Makefile above GUI directory, then this must be a
    # locally built version
    LOCAL_BUILD = os.access(os.path.join(MOOSE_GUI_DIR, '../Makefile'), os.R_OK)
    errors = []
    '''
    moose_cfg_dir = os.path.join(os.environ['HOME'], '.moose')
    if not os.path.exists(moose_cfg_dir):
        firsttime = True
        # try:
        #     # os.mkdir(moose_cfg_dir)
        #     # # MOOSE_CFG_DIR = moose_cfg_dir
        #     # print 'Created moose configuration directory:', moose_cfg_dir
        # except OSError, e:
        #     errors.append(e)
        #     print e
    if LOCAL_BUILD:
        MOOSE_LOCAL_DIR = os.path.normpath(os.path.join(MOOSE_GUI_DIR, '..'))
        MOOSE_DEMOS_DIR = os.path.join(MOOSE_LOCAL_DIR, 'Demos')
        MOOSE_DOCS_DIR = os.path.join(MOOSE_LOCAL_DIR, 'Docs/user/py/_build/html/')
    else:
        MOOSE_LOCAL_DIR = os.path.join(os.environ['HOME'], 'moose')
        if not os.path.exists(MOOSE_LOCAL_DIR):
            try:
                os.mkdir(MOOSE_LOCAL_DIR)
                print 'Created local moose directory:', MOOSE_LOCAL_DIR
            except OSError, e:
                errors.append(e)
                print e
    if not os.access(MOOSE_DOCS_DIR, os.R_OK + os.X_OK):
        print "Could not access Demos directory: %s" % (MOOSE_DOCS_DIR)
        errors.append(OSError(errno.EACCES, 'Cannot access %s' % (MOOSE_DOCS_DIR)))
    '''
    return firsttime, errors

settings = MooseSetting()

# LOG_FILENAME = os.path.join(TEMPDIR, 'moose.log')
LOG_LEVEL = logging.ERROR
# logging.basicConfig(filename=LOG_FILENAME, level=LOG_LEVEL, filemode='w', format='%(asctime)s %(levelname)s %(name)s %(filename)s %(funcName)s: %(lineno)d: %(message)s')
logging.basicConfig(stream=sys.stdout, level=LOG_LEVEL, filemode='w', format='%(asctime)s %(levelname)s %(name)s %(filename)s %(funcName)s: %(lineno)d: %(message)s')
LOGGER = logging.getLogger('moose')
BENCHMARK_LOGGER = logging.getLogger('moose.benchmark')
BENCHMARK_LOGGER.setLevel(logging.INFO)

#
# config.py ends here
