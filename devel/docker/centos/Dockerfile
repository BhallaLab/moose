FROM centos:7
MAINTAINER Dilawar Singh <dilawars@ncbs.res.in>

# Install dependencies.
RUN yum -y update && yum -y install epel-release && yum -y update \
      && yum -y clean all 
RUN yum install -y git cmake3 gcc gcc-c++ make \
      python3 python3-devel python3-setuptools python3-numpy \
      && yum -y clean all 
RUN yum install -y gsl-devel   && yum -y clean all
# These are required to run tests.
RUN yum install -y python3-matplotlib python3-scipy python3-numpy && yum -y clean all 
RUN yum install -y centos-release-scl && yum -y clean all
RUN yum install -y devtoolset-8 && yum -y clean all
RUN ln -s /usr/bin/cmake3 /usr/bin/cmake 
RUN ln -s /usr/bin/ctest3 /usr/bin/ctest 
WORKDIR /root

# Run docker build from project ROOT. 
COPY . moose-core
RUN python3 -m pip install matplotlib scipy sympy --user

# enable devtoolset before building.
SHELL [ "/usr/bin/scl", "enable", "devtoolset-8" ]
RUN cd moose-core && python3 setup.py build test install
CMD ["/usr/bin/python3", "-c", "'import moose;moose.test()'"]
