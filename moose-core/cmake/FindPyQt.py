# -*- coding: utf-8 -*-
#
#   Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#        * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#        * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#        * Neither the name of the  Simon Edwards <simon@simonzone.com> nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#    THIS SOFTWARE IS PROVIDED BY Simon Edwards <simon@simonzone.com> ''AS IS'' AND ANY
#    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#    DISCLAIMED. IN NO EVENT SHALL Simon Edwards <simon@simonzone.com> BE LIABLE FOR ANY
#    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# FindPyQt.py
# Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

try:
    import PyQt4.pyqtconfig
    pyqtcfg = PyQt4.pyqtconfig.Configuration()
    pyqt_sip_flags = pyqtcfg.pyqt_sip_flags
    print("pyqt_version:%06.0x" % pyqtcfg.pyqt_version)
    print("pyqt_version_num:%d" % pyqtcfg.pyqt_version)
    print("pyqt_version_str:%s" % pyqtcfg.pyqt_version_str)
except ImportError:
    # PyQt built with configure-ng.py has no pyqtconfig module
    from PyQt4.QtCore import PYQT_CONFIGURATION, PYQT_VERSION, PYQT_VERSION_STR
    pyqtcfg = None
    pyqt_sip_flags = PYQT_CONFIGURATION["sip_flags"]
    print("pyqt_version:%06.0x" % PYQT_VERSION)
    print("pyqt_version_num:%d" % PYQT_VERSION)
    print("pyqt_version_str:%s" % PYQT_VERSION_STR)

pyqt_version_tag = ""
in_t = False
for item in pyqt_sip_flags.split(' '):
    if item=="-t":
        in_t = True
    elif in_t:
        if item.startswith("Qt_4"):
            pyqt_version_tag = item
    else:
        in_t = False
print("pyqt_version_tag:%s" % pyqt_version_tag)

if pyqtcfg:
    print("pyqt_mod_dir:%s" % pyqtcfg.pyqt_mod_dir)
    print("pyqt_sip_dir:%s" % pyqtcfg.pyqt_sip_dir)
    print("pyqt_sip_flags:%s" % pyqtcfg.pyqt_sip_flags)
    print("pyqt_bin_dir:%s" % pyqtcfg.pyqt_bin_dir)
else:
    import sipconfig
    sipcfg = sipconfig.Configuration()
    print("pyqt_mod_dir:%s" % sipcfg.default_mod_dir)
    print("pyqt_sip_dir:%s" % sipcfg.default_sip_dir)
    print("pyqt_sip_flags:%s" % pyqt_sip_flags)
    print("pyqt_bin_dir:%s" % sipcfg.default_bin_dir)



