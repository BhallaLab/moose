#!/bin/bash/env bash

sudo apt-get update -qq
sudo apt-get install libxml2-dev libbz2-dev wget
sudo apt-get install libhdf5-serial-dev
sudo apt-get install python-numpy python-setuptools python-networkx python-pip
sudo apt-get install cmake cdbs
sudo apt-get install python-suds python-matplotlib python-nose python-lxml
sudo apt-get install python-qt4-dev python-qt4-gl python-sip-dev libqt4-dev libopenscenegraph-dev
sudo apt-get install doxygen python-sphinx 
sudo apt-get install git-buildpackage 
sudo apt-get install libgsl-dev
sudo apt-get install -f
sudo ldconfig /usr/lib64
