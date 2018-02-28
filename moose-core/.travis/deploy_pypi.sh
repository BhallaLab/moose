#!/bin/bash -
#===============================================================================
#
#          FILE: deploy_pypi.sh
#
#         USAGE: ./deploy_pypi.sh
#
#   DESCRIPTION:  Build docker image and deploy on PyPI.
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: Sunday 04 February 2018 11:46:32  IST
#      REVISION:  ---
#===============================================================================
set -o nounset                                  # Treat unset variables as an error

git clone https://github.com/BhallaLab/pymoose-wheels
cd pymoose-wheels && docker build -t bhallalab/wheels .
