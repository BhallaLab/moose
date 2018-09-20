#!/bin/bash -
#===============================================================================
#
#          FILE: travis_prepare_linux.sh
#
#         USAGE: ./travis_prepare_linux.sh
#
#   DESCRIPTION:  Prepare linux build environment on travis.
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: Always run with sudo permission.
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 01/02/2017 10:10:02 AM
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
set +e  # Let installation fail in some command

apt-get install -qq libxml2-dev libbz2-dev
apt-get install -qq make cmake
apt-get install -qq python-numpy python-matplotlib python-networkx python-pip
apt-get install -qq python3-lxml python-lxml
apt-get install -qq python3-numpy python3-matplotlib python3-dev
apt-get install -qq python-pip python3-pip
apt-get install -qq libgraphviz-dev

# Gsl
apt-get install -qq libgsl0-dev || apt-get install -qq libgsl-dev

# Boost related.
apt-get install -qq liblapack-dev
apt-get install -qq libboost-all-dev

# Dependencies for NML2
apt-get install -qq python-scipy python3-scipy
apt-get install -qq python-lxml python3-lxml
apt-get install -qq python-setuptools python3-setuptools
apt-get install -qq python-tornado python3-tornado
python2 -m pip install pyNeuroML libNeuroML
python3 -m pip install pyNeuroML libNeuroML
