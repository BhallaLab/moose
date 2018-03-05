#!/bin/bash -
#===============================================================================
#
#          FILE: .travis_build.sh
#
#         USAGE: ./.travis_build.sh
#
#   DESCRIPTION: 
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 03/05/2018 03:49:35 PM
#      REVISION:  ---
#===============================================================================

set -o nounset                                  # Treat unset variables as an error
set -e -x
PATH=/usr/bin:/usr/local/bin:$PATH

gbp buildpackage  --git-ignore-branch --git-ignore-new -uc -us
sudo dpkg -i ../moose*.deb

python -c 'import moose'
python -c 'import moogli'
cd __moose-core_build && ctest --output-on-failure
