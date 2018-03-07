#!/bin/bash -
#===============================================================================
#
#          FILE: build_rpm.sh
#
#         USAGE: ./build_rpm.sh
#
#   DESCRIPTION: Generate RPM package.
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
gbp buildpackage-rpm  --git-ignore-branch --git-ignore-new  \
    --git-spec-file=./rpm/moose.spec | tee _rpm.log
