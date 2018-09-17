#!/bin/bash - 
#===============================================================================
#
#          FILE: pull_subtree.sh
# 
#         USAGE: ./pull_subtree.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 05/18/2016 01:11:59 PM
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
set -e
set -x

if [[ `pwd` == *"/moose" ]]; then

    git subtree pull --prefix moose-core \
        https://github.com/BhallaLab/moose-core chhennapoda --squash
    git subtree pull --prefix moose-examples \
        https://github.com/BhallaLab/moose-examples chhennapoda --squash 
    git subtree pull --prefix moose-gui \
        https://github.com/BhallaLab/moose-gui chhennapoda --squash

else
    echo "Run this script from top-level git directory."
fi
