#!/usr/bin/env bash
# ROOT should run this script.
VERSION=$(lsb_release -r | cut -f2)
apt-get -y update
apt-get install cmake coreutils --force-yes
apt-get -y --force-yes install python-qt4 python-pip graphviz
apt-get -y --force-yes install python-h5py python-scipy python-pygraphviz
wget -nv https://download.opensuse.org/repositories/home:moose/xUbuntu_$VERSION/Release.key -O Release.key
apt-key add - < Release.key
cat <<EOF > /etc/apt/sources.list.d/home:moose.list 
deb http://download.opensuse.org/repositories/home:/moose/xUbuntu_${VERSION}/ /
EOF
apt-get update
apt-get install python-numpy python-matplotlib python-networkx
apt-get install pymoose
python -m pip install python-libsbml --user
