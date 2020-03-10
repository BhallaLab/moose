#!/bin/bash -
#
#   DESCRIPTION:  Prepare linux build environment on travis.
#         NOTES: Always run with sudo permission.
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: 01/02/2017 10:10:02 AM

set -e -x
apt update
apt-get install -qq libxml2-dev libbz2-dev
apt-get install -qq make cmake
apt-get install -qq python-numpy python-matplotlib python-networkx python-pip
apt-get install -qq python3-numpy python3-matplotlib python3-networkx python3-pip
apt-get install -qq python-tk python3-tk
apt-get install -qq valgrind

# Gsl
apt-get install -qq libgsl0-dev || apt-get install -qq libgsl-dev

# Boost related.
apt-get install -qq liblapack-dev
apt-get install -qq libboost-all-dev

# Dependencies for NML2
apt-get install -qq python-scipy python3-scipy
apt-get install -qq python-lxml python3-lxml
apt-get install -qq python-setuptools python3-setuptools

# Install twine
python3 -m pip install twine
