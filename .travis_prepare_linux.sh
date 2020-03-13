#!/usr/bin/env bash
sudo apt-get update -qq
sudo apt-get -y install libxml2-dev libbz2-dev wget
sudo apt-get -y install libhdf5-serial-dev
sudo apt-get -y install python-numpy python-setuptools python-networkx python-pip
sudo apt-get -y install python3-numpy python3-setuptools python3-matplotlib
sudo apt-get -y install cmake cdbs
sudo apt-get -y install libgsl0-dev
sudo apt-get -y install python-suds python-matplotlib 
sudo apt-get -y install python-qt4-dev python-qt4-gl python-sip-dev libqt4-dev libopenscenegraph-dev
sudo apt-get -y install doxygen python-sphinx python-lxml
sudo apt-get -y install git-buildpackage fakeroot
sudo apt-get -y install -f
sudo ldconfig /usr/lib64
