FROM ubuntu:18.04
MAINTAINER Dilawar Singh <dilawars@ncbs.res.in>

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies.
RUN apt update && apt install -y cmake gcc g++ make valgrind \
        libboost-all-dev libgsl-dev libblas-dev liblapack-dev \
        python3-pip python-pip \
        python3-numpy python-numpy python3-matplotlib python-matplotlib \
        python-tk python3-tk \
        python-setuptools python3-setuptools \
        python-dev python3-dev \
        && rm -rf /var/lib/apt/lists/*

RUN python2 -m pip --no-cache-dir install pyneuroml python-libsbml
RUN python3 -m pip --no-cache-dir install pyneuroml python-libsbml
WORKDIR /root
RUN ls -ltrah
COPY . moose-core/
RUN ls -ltarh
RUN cd moose-core && rm -rf .git && ./.ci/travis_build_linux.sh
