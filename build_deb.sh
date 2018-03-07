#!/bin/bash -
#===============================================================================
#
#          FILE: build_deb.sh
#
#         USAGE: ./build_deb.sh
#
#   DESCRIPTION: Generate DEBIAN package.
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: Wednesday 07 March 2018 10:02:28  IST
#      REVISION:  ---
#===============================================================================

set -o nounset                                  # Treat unset variables as an error
gbp buildpackage  --git-ignore-branch --git-ignore-new -uc -us -d | tee _gbp.log
