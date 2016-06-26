#!/bin/bash
# Test the port file.
set -x
export PATH=/opt/local/bin:$PATH
rm -f ~/ports/science/moose/Portfile 
ln -s `pwd`/Portfile ~/ports/science/moose/Portfile
( cd ~/ports/ && portindex )
port lint --nitpick moose
sudo port uninstall moose
sudo port -vd  install moose | tee _build_port.log
