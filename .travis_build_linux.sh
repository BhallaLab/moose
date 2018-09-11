#!/usr/bin/env bash
wget http://ftp.debian.org/debian/pool/main/g/gsl/libgsl0ldbl_1.16+dfsg-2_amd64.deb -O gsl_1.16.deb
sudo dpkg -i gsl_1.16.deb
wget http://ftp.debian.org/debian/pool/main/g/gsl/libgsl0-dev_1.16+dfsg-2_amd64.deb -O gsl_1.16-dev.deb
sudo dpkg -i gsl_1.16-dev.deb
sudo apt-get install -f
sudo ldconfig /usr/lib64
